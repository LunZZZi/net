#include "message.h"

void writeMessage(int sockfd, MessageObject* message, size_t len) {
    size_t send_size = sizeof(message->message_type) + sizeof(message->message_length) + len;
    if (send(sockfd, (char *)message, send_size, 0) < 0)
        err_sys("send failure");
}
