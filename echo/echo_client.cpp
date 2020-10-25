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

void str_cli(FILE *fp, int sockfd)
{
    char sendline[MAXLINE], recvline[MAXLINE];
    while (fgets(sendline, MAXLINE, fp) != NULL)
    {
        writen(sockfd, sendline, strlen(sendline));

        if (readline(sockfd, recvline, MAXLINE) == 0)
        {
            printf("str_cli: server terminated prematurely\n");
            exit(-1);
        }

        fputs(recvline, stdout);
    }
}

int main(int argc, char const *argv[])
{
    int sockfd;
    struct sockaddr_in servaddr;
    if (argc != 2)
        err_sys("usage: echo_client <IPaddress>");
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    // The inet_pton() function convers a presentation format address (string) to network format
    // (usually a struct in_addr).
    inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

    int err = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    assert_ret(err, "connect error");

    str_cli(stdin, sockfd);
    
    return 0;
}
