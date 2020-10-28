#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include "lib/common.h"

typedef struct {
    u_int32_t message_length;
    u_int32_t message_type;
    char buf[1024];
} MessageObject;

#define MSG_CD  1
#define MSG_PWD 2

void writeMessage(int, MessageObject*, size_t);
void logMessage(MessageObject*);

#endif