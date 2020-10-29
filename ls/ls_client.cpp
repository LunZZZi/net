#include "lib/common.h"
#include "message.h"

MessageObject message;

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
    
    while (fgets(message.buf, sizeof(message.buf), stdin) != NULL)
    {
        if (strncmp(message.buf, "pwd", 3) == 0) {
            type = MSG_PWD;
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
                log_message(&message);
                assert_ret(ret, "read error");
                printf("cwd: %s\n", message.buf);
                break;
            default:
                printf("unknown type %d\n", type);
        }
    }

    return 0;
}
