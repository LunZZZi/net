#include "common.h"

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

// ---- readline start
static int read_cnt;
static char *read_ptr;
static char read_buf[MAXLINE];

static ssize_t
my_read(int fd, char *ptr)
{
    if (read_cnt <= 0)
    {
    again:
        if ((read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0)
        {
            if (errno == EINTR)
                goto again;
            return -1;
        }
        else if (read_cnt == 0)
        {
            return 0;
        }
        read_ptr = read_buf;
    }
    read_cnt--;
    *ptr = *read_ptr++;
    return 1;
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;
    ptr = (char *)vptr;
    for (n = 1; n < maxlen; n++)
    {
        if ((rc = my_read(fd, &c)) == 1)
        {
            *ptr++ = c;
            if (c == '\n')
                break; /* newline is stored, like fgets() */
        }
        else if (rc == 0)
        {
            *ptr = 0;
            return n - 1; /* EOF */
        }
        else
        {
            return -1; /* error */
        }
    }
    *ptr = 0; /* null terminate like fgets() */
    return n;
}
// ---- readline end

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
