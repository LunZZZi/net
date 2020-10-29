#include "message.h"
#include "cstring"

void write_message(int sockfd, MessageObject* message) {
    size_t len = 0;
    switch (message->message_type) {
        case MSG_CD:
        case MSG_PWD:
        case MSG_LS:
        case MSG_RECV_CD:
            len = 0;
            break;
        case MSG_RECV_LS:
        case MSG_RECV_PWD:
            len = strlen(message->buf);
            break;
        default:
            break;
    }

    size_t send_size = sizeof(message->message_type) 
        + sizeof(message->message_length) 
        + len;

    message->message_length = htonl(len);
    message->message_type = htonl(message->message_type);
    if (writen(sockfd, (char *)message, send_size) < 0)
        err_sys("send failure");
}

size_t read_message(int fd, MessageObject* message) {
    int rc;

    rc = readn(fd, (char *) &message->message_length, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    message->message_length = ntohl(message->message_length);

    rc = readn(fd, (char *) &message->message_type, sizeof(u_int32_t));
    if (rc != sizeof(u_int32_t))
        return rc < 0 ? -1 : 0;
    message->message_type = ntohl(message->message_type);

    if (message->message_length > sizeof(message->buf)) {
        return -1;
    }

    if (message->message_length) {
        rc = readn(fd, message->buf, message->message_length);
        if (rc != message->message_length)
            return rc < 0 ? -1 : 0;
        else message->buf[rc] = 0;
    }
    return rc;
}

void log_message(MessageObject* message) {
    printf("[%3d] [L%d] ", message->message_type, message->message_length);
    if (message->message_type) {
        printf("content:%s", message->buf);
    }
    printf("\n");
}