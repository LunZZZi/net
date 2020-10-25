#include "lib/common.h"

// ---- writen start
ssize_t writen(int fd, const void *vptr, size_t n)
{
    size_t nleft;
    ssize_t nwrite;

    char *ptr = (char *)vptr;
    nleft = n;
    while (nleft > 0)
    {
        if ((nwrite = write(fd, ptr, nleft)) <= 0)
        {
            if (nwrite < 0 && errno == EINTR)
                nwrite = 0;
        }
        else
        {
            return -1;
        }
        nleft -= nwrite;
        ptr += nwrite;
    }
    return n;
}
// ---- writen end

void str_echo(int sockfd)
{
    ssize_t n;
    char buf[MAXLINE];

again:
    while ((n = read(sockfd, buf, MAXLINE)) > 0)
        writen(sockfd, buf, n);

    if (n < 0 && errno == EINTR)
        goto again;
    else if (n < 0)
        err_sys("str_echo: read error\n");
}

int main(int argc, char **argv)
{
    int listenfd, connfd;
    int err;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    err = bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    assert_ret(err, "bind error");

    err = listen(listenfd, LISTENQ);
    assert_ret(err, "listen error");

    printf("listening on port %d...\n", SERV_PORT);

    clilen = sizeof(cliaddr);
    connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
    printf("accept fd %d\n", connfd);

    close(listenfd);
    str_echo(connfd);
    close(connfd);
}
