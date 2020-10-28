#include "lib/common.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        err_sys("usage: echo2_client <IPaddress>");
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

    struct {
        u_int32_t message_length;
        u_int32_t message_type;
        char buf[128];
    } message;

    int n;
    
    while (fgets(message.buf, sizeof(message.buf), stdin) != NULL)
    {
        n = strlen(message.buf);
        message.message_length = htonl(n);
        message.message_type = htonl(1);
        size_t send_size = sizeof(message.message_type) 
            + sizeof(message.message_length)
            + n;
        if (send(socket_fd, (char *) &message, send_size, 0) < 0)
            err_sys("send failure");
    }

    return 0;
}
