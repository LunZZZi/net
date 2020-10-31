#include "lib/common.h"

#define FD_INIT_SIZE 128

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else 
        return c;
}

struct Buffer {
    int connect_fd;
    char buffer[MAXLINE];
    size_t writeIndex;
    size_t readIndex;
    int readable;
};

Buffer *alloc_Buffer() {
    Buffer *buffer = (Buffer *)malloc(sizeof(Buffer));
    if (!buffer)
        return NULL;
    buffer->connect_fd = 0;
    buffer->writeIndex = buffer->readIndex = buffer->readIndex = 0;
    return buffer;
}

void free_Buffer(Buffer *buffer) {
    free(buffer);
}

/**
 * 返回-1=错误或结束，返回0=可读
 */
int onSocketRead(int fd, Buffer* buffer) {
    char buf[1024];
    int i;
    ssize_t result;
    while (1) {
        // returns in non-blocking ??
        result = recv(fd, buf, sizeof(buf), 0);
        if (result < 0)
            break;
        
        for (i = 0; i < result; ++i) {
            if (buffer->writeIndex < sizeof(buffer->buffer))
                buffer->buffer[buffer->writeIndex++] = rot13_char(buf[i]);
            if (buf[i] == '\n')
                buffer->readable = 1;
        }
    }

    if (result < 0) {
        if (errno == EAGAIN)
            return 0;
        return -1;
    }

    return 0;
}

/**
 * 0=未结束或者写入结束，-1表示出错 
 */
int onSocketWrite(int fd, Buffer* buffer) {
    while (buffer->readIndex < buffer->writeIndex) {
        // returns in non-blocking ???
        ssize_t result = send(fd, buffer->buffer + buffer->readIndex, 
                              buffer->writeIndex - buffer->readIndex, 
                              0);
        if (result < 0) {
            if (errno == EAGAIN)
                return 0;
            return -1;
        }

        printf("sent %zu bytes\n", result);
        printf("buffer Read: %zu, Write: %zu\n", buffer->readIndex, buffer->writeIndex);
        buffer->readIndex += result;
    }

    if (buffer->readIndex == buffer->writeIndex)
        buffer->readIndex = buffer->writeIndex = 0;
    
    buffer->readable = 0;

    return 0;
}

int main(int argc, char const *argv[])
{
    int listen_fd;
    int i, maxfd;

    Buffer* buffer[FD_INIT_SIZE];
    for ( i = 0; i < FD_INIT_SIZE; i++)
    {
        buffer[i] = alloc_Buffer();
    }

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0)
    {
        assert_ret(errno, "bind failed ");
    }

    int rt2 = listen(listen_fd, LISTENQ);
    if (rt2 < 0)
    {
        assert_ret(errno, "listen failed ");
    }

    fd_set readset, writeset, exset;

    while (1) {
        maxfd = listen_fd;

        FD_ZERO(&readset);
        FD_ZERO(&writeset);
        FD_ZERO(&exset);

        FD_SET(listen_fd, &readset);

        for (i = 0; i < FD_INIT_SIZE; ++i) {
            if (buffer[i]->connect_fd > 0) {
                if (buffer[i]->connect_fd > maxfd)
                    maxfd = buffer[i]->connect_fd;
                FD_SET(buffer[i]->connect_fd, &readset);
                if (buffer[i]->readable) {
                    FD_SET(buffer[i]->connect_fd, &writeset);
                }
            }
        }
        
        if (select(maxfd + 1, &readset, &writeset, &exset, NULL) < 0) {
            err_sys("select error");
        }

        if (FD_ISSET(listen_fd, &readset)) {
            printf("listening socket readable\n");
            sleep(5);
            struct sockaddr_in ss;
            socklen_t slen = sizeof(ss);
            int fd = accept(listen_fd, (struct sockaddr *)&ss, &slen);
            if (fd < 0) {
                err_sys("accept failed");
            } else if (fd > FD_INIT_SIZE) {
                err_sys("too many connections");
            } else {
                // nonblocking
                fcntl(fd, F_SETFL, O_NONBLOCK);
                if (buffer[fd]->connect_fd == 0) {
                    buffer[fd]->connect_fd = fd;
                } else {
                    err_sys("too many connections");
                }
            }
        }

        for (i = 0; i < maxfd + 1; ++i) {
            int r = 0;
            if (i == listen_fd)
                continue;
            
            if (FD_ISSET(i, &readset)) {
                r = onSocketRead(i, buffer[i]);
            }
            if (r == 0 && FD_ISSET(i, &writeset)) {
                r = onSocketWrite(i, buffer[i]);
            }
            if (r) {
                buffer[i]->connect_fd = 0;
                close(i);
            }
        }
    }
    
    return 0;
}
