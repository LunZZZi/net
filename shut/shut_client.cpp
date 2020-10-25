#include "lib/common.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        err_sys("usage: shut_client <IPaddress>");
    }

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (struct sockaddr *)&server_addr, server_len);
    assert_ret(connect_rt, "connect failed");

    char send_line[MAXLINE], recv_line[MAXLINE];
    int n;

    fd_set readmask;
    fd_set allreads;

    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);
    for (;;)
    {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if (rc <= 0)
            err_sys("select failed");

        // socket上有数据可读
        if (FD_ISSET(socket_fd, &readmask))
        {
            n = read(socket_fd, recv_line, MAXLINE);
            if (n < 0)
            {
                assert_ret(errno, "read error");
            }
            else if (n == 0)
            {
                printf("server terminated\n");
                exit(0);
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        // 标准输入上有数据可读
        if (FD_ISSET(STDIN_FILENO, &readmask))
        {
            if (fgets(send_line, MAXLINE, stdin) != NULL)
            {
                if (strncmp(send_line, "shutdown", 8) == 0)
                {
                    FD_CLR(0, &allreads);
                    if (shutdown(socket_fd, SHUT_WR))
                    {
                        assert_ret(errno, "shutdown failed");
                    }
                }
                else if (strncmp(send_line, "close", 5) == 0)
                {
                    FD_CLR(0, &allreads);
                    if (close(socket_fd))
                    {
                        assert_ret(errno, "close failed");
                    }
                    sleep(6);
                    exit(0);
                }
                else
                {
                    int i = strlen(send_line);
                    if (send_line[i - 1] == '\n')
                    {
                        send_line[i - 1] = 0;
                    }
                    printf("now sending %s\n", send_line);
                    size_t rt = write(socket_fd, send_line, strlen(send_line));
                    if (rt < 0)
                    {
                        assert_ret(errno, "write failed");
                    }
                    printf("send bytes: %zu \n", rt);
                }
            }
        }
    }

    return 0;
}
