#include "lib/common.h"
#include "message.h"

static int count;
static MessageObject message;

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

    while (true) {
        if ((connfd = accept(listenfd, 
                             (struct sockaddr *)&client_addr, 
                             &client_len)) < 0)
        {
            assert_ret(errno, "bind failed");
        }

        count = 0;

        while (1) {
            int n = read_message(connfd, &message);
            if (n < 0) {
                err_sys("error read message");
            } else if (n == 0) {
                printf("client closed\n");
                break;
            }
            count++;

            log_message(&message);
            if (message.message_type == MSG_PWD) {
                getcwd(message.buf, 1024);
                printf("cwd: %s\n", message.buf);
                message.message_type = (MSG_RECV_PWD);
                write_message(connfd, &message);
            }
        }
    }

    return 0;
}
