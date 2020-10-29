#include "lib/common.h"
#include "message.h"

#define DEBUG 0

MessageObject message;
char path_buff[1024];

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        err_sys("usage: ls_client <IPaddress>");
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

    int type;
    
    while (fgets(path_buff, sizeof(path_buff), stdin) != NULL)
    {
        if (strncmp(path_buff, "pwd", 3) == 0) {
            type = MSG_PWD;
        } else if (strncmp(path_buff, "ls", 2) == 0) {
            type = MSG_LS;
        } else if (strncmp("cd ", path_buff, 3 ) == 0) {
            int i;
            for (i = 3; path_buff[i] != 0; i++) {
                if (path_buff[i] != ' ') {
                    break;
                }
            }
            strncpy(message.buf, path_buff + i, 1024);
            size_t length = strlen(message.buf);
            // remove \n
            message.buf[length - 1] = 0;
            type = MSG_CD;
        } else {
            type = 0;
        }

        size_t send_size;
        int ret;
        switch (type)
        {
            case MSG_PWD:
                message.message_type = type;
                write_message(socket_fd, &message);
                ret = read_message(socket_fd, &message);
            #if DEBUG
                log_message(&message);
            #endif
                assert_ret(ret, "read error");
                printf("cwd: %s\n", message.buf);
                break;
            case MSG_LS:
                message.message_type = type;
                write_message(socket_fd, &message);
                ret = read_message(socket_fd, &message);
            #if DEBUG
                log_message(&message);
            #endif
                assert_ret(ret, "read_error");
                printf("%s\n", message.buf);
                break;
            case MSG_CD:
                message.message_type = type;
                // get path_buff
                write_message(socket_fd, &message);
                ret = read_message(socket_fd, &message);
            #if DEBUG
                log_message(&message);
            #endif
                assert_ret(ret, "read_error");
                printf("cd success\n");
                break;
            default:
                printf("unknown type %d\n", type);
        }
    }

    return 0;
}
