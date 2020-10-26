#include "lib/common.h"
#include "message_object.h"

static int count;

int main(int argc, char const *argv[])
{
    if (argc != 2) {
        err_sys("usage: pingpong_server <sleepingtime>");
    }

    int sleepingTime = atoi(argv[1]);

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

    int connfd;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    if ((connfd = accept(listenfd, (struct sockaddr *)&client_addr, &client_len)) < 0)
    {
        assert_ret(errno, "bind failed");
    }

    MessageObject message;
    count = 0;

    for (;;)
    {
        int n = read(connfd, (char *) &message, MAXLINE);
        if (n < 0)
        {
            err_sys("error read");
        }
        else if (n == 0)
        {
            printf("client closed \n");
            exit(0);
        }

        printf("received %d bytes\n", n);
        count++;

        switch (ntohl(message.type))
        {
        case MSG_TYPE1:
            printf("process MSG_TYPE1 \n");
            break;
        
        case MSG_PING: {
            MessageObject pong_message;
            pong_message.type = MSG_PONG;
            sleep(sleepingTime);
            ssize_t rc = send(connfd, (char *) &pong_message, sizeof(pong_message), 0);
            if (rc < 0) {
                err_sys("send failure");
            }
            break;
        }

        default:
            assert_ret(message.type, "unknown message type");
            break;
        }
    }
    return 0;
}
