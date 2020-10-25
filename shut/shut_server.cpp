#include "lib/common.h"

static int count;

static void sig_pipe(int signo)
{
    printf("\nreceived %d datagrams\n", count);
    exit(0);
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

    char message[MAXLINE];
    count = 0;

    for (;;)
    {
        int n = read(connfd, message, MAXLINE);
        if (n < 0)
        {
            assert_ret(errno, "error read");
        }
        else if (n == 0)
        {
            printf("server closed \n");
            exit(0);
        }
        message[n] = 0;
        printf("received %d bytes: %s\n", n, message);
        count++;

        char send_line[MAXLINE];
        sprintf(send_line, "Hi, %s", message);

        sleep(5);

        int write_nc = send(connfd, send_line, strlen(send_line), 0);
        printf("send bytes: %d \n", write_nc);
        if (write_nc < 0)
        {
            assert_ret(errno, "error write");
        }
    }
    return 0;
}
