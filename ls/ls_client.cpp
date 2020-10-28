#include "lib/common.h"
#include "message.h"

MessageObject message;

size_t read_message(int fd, char *buffer, size_t length, u_int32_t* msg_type) {
    u_int32_t msg_length;
    int rc;

    rc = readn(fd, (char *) &msg_length, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    msg_length = ntohl(msg_length);

    rc = readn(fd, (char *) msg_type, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    *msg_type = ntohl(*msg_type);

    if (msg_length > length) {
        return -1;
    }

    if (msg_length) {
        rc = readn(fd, buffer, msg_length);
        if (rc != msg_length)
            return rc < 0 ? -1 : 0;
    }
    return rc;
}

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

    int len, type;
    
    while (fgets(message.buf, sizeof(message.buf), stdin) != NULL)
    {
        if (strncmp(message.buf, "pwd", 3) == 0) {
            type = MSG_PWD;
            len = 0;
        } else {
            type = 0;
        }

        size_t send_size;
        int ret;
        switch (type)
        {
            case MSG_PWD:
                message.message_length = htonl(len);
                message.message_type = htonl(type);
                writeMessage(socket_fd, &message, 0);
                ret = read_message(socket_fd, message.buf, 1024, &message.message_type);
                assert_ret(ret, "read error");
                printf("cwd: %s\n", message.buf);
                break;
            default:
                printf("unknown type %d\n", type);
        }
    }

    return 0;
}
