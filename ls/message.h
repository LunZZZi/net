#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include "lib/common.h"

typedef struct {
    u_int32_t message_length;
    u_int32_t message_type;
    char buf[1024];
} MessageObject;

#define MSG_CD          1
#define MSG_PWD         2
#define MSG_LS          3
#define MSG_RECV_CD     101
#define MSG_RECV_PWD    102
#define MSG_RECV_LS     103

void write_message(int, MessageObject*);
size_t read_message(int, MessageObject*);
void log_message(MessageObject*);

#endif
