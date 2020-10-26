#include "lib/common.h"
#include "message_object.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        err_sys("usage: pingpong_client <IPaddress>");
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

    char recv_line[MAXLINE + 1];
    int n;

    fd_set readmask;
    fd_set allreads;

    struct timeval tv;
    int heartbeats = 0;

    tv.tv_sec = KEEP_ALIVE_TIME;
    tv.tv_usec = 0;

    MessageObject messageObject;

    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);
    for (;;)
    {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, &tv);
        if (rc < 0)
        {
            err_sys("select failed");
        }

        // 超时返回0
        if (rc == 0)
        {
            if (++heartbeats > KEEP_ALIVE_PROBETIMES)
            {
                err_sys("connection dead\n");
            }
            printf("sending heartbeat #%d\n", heartbeats);
            messageObject.type = htonl(MSG_PING);
            rc = send(socket_fd, (char *)&messageObject, sizeof(MessageObject), 0);
            if (rc < 0)
            {
                err_sys("send failure");
            }
            tv.tv_sec = KEEP_ALIVE_INTERVAL;
            continue;
        }

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
            printf("received heartbeat, make heartbeats to 0 \n");
            heartbeats = 0;
            tv.tv_sec = KEEP_ALIVE_TIME;
        }
    }

    return 0;
}
