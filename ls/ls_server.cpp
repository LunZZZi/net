#include "lib/common.h"
#include "message.h"

static int count;
static MessageObject message;

static void sig_pipe(int signo)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

size_t read_message(int fd, char *buffer, size_t length, u_int32_t* msg_type) {
    u_int32_t msg_length;
    int rc;

    rc = readn(fd, (char *) &msg_length, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    msg_length = ntohl(msg_length);

    rc = readn(fd, (char *) msg_type, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    *msg_type = ntohl(*msg_type);

    if (msg_length > length) {
        return -1;
    }

    if (msg_length) {
        rc = readn(fd, buffer, msg_length);
        if (rc != msg_length)
            return rc < 0 ? -1 : 0;
    }
    return rc;
}

int main(int argc, char const *argv[])
{
    int listenfd;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(SERV_PORT);

    // ??
    int on = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int rt1 = bind(listenfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (rt1 < 0)
    {
        assert_ret(errno, "bind failed ");
    }

    int rt2 = listen(listenfd, LISTENQ);
    if (rt2 < 0)
    {
        assert_ret(errno, "listen failed ");
    }

    signal(SIGINT, sig_pipe);
    signal(SIGPIPE, sig_pipe);

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0)
    {
        assert_ret(errno, "bind failed");
    }

    char buf[128];
    count = 0;

    while (1) {
        int n = read_message(connfd, buf, sizeof(buf), &message.message_type);
        if (n < 0) {
            err_sys("error read message");
        } else if (n == 0) {
            err_sys("client closed");
        }
        buf[n] = 0;
        printf("received %d bytes: %s\n", n, buf);
        count++;

        printf("message_type %d\n", message.message_type);
        if (message.message_type == MSG_PWD) {
            getcwd(message.buf, 1024);
            printf("cwd: %s\n", message.buf);
            size_t len = strlen(message.buf);
            message.message_type = htonl(MSG_PWD);
            message.message_length = htonl(len);
            writeMessage(connfd, &message, len);
        }
    }

    return 0;
}
