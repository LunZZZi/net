#include <dirent.h>
#include "lib/common.h"
#include "message.h"

static int count;
static MessageObject message;
static char path_buff[MAXLINE];

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

    DIR *dp;
    struct dirent *dirp;
    int start = 0;
    int ret;

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
            switch (message.message_type) {
                case MSG_PWD:
                    getcwd(message.buf, 1024);
                    printf("cwd: %s\n", message.buf);
                    message.message_type = (MSG_RECV_PWD);
                    write_message(connfd, &message);
                    break;
                case MSG_LS:
                    message.message_type = MSG_RECV_LS;
                    getcwd(path_buff, 1024);
                    start = 0;
                    if ((dp = opendir(path_buff)) == NULL) {
                        strcpy(message.buf, "bad directory");
                    } else {
                        message.buf[0] = 0;
                        while ((dirp = readdir(dp)) != NULL) {
                            if (strcmp(dirp->d_name, ".") == 0 ||
                                strcmp(dirp->d_name, "..") == 0)
                                continue;
                            if (start) strncat(message.buf, "\t", 1024);
                            strncat(message.buf, dirp->d_name, 1024);
                            start++;
                        }
                        closedir(dp);
                    }
                    write_message(connfd, &message);
                    break;
                case MSG_CD:
                    message.message_type = MSG_RECV_CD;
                    printf("cd \"%s\"\n", message.buf);
                    ret = chdir(message.buf);
                    printf("cd return %d\n", ret);
                    write_message(connfd, &message);
                    break;
                default:
                    break;
            }
        }
    }

    return 0;
}
