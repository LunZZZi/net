#ifndef __MESSAGE_OBJECT_H__
#define __MESSAGE_OBJECT_H__

typedef struct {
    u_int32_t type;
    char data[1024];
} MessageObject;

#define MSG_PING    1
#define MSG_PONG    2
#define MSG_TYPE1   4
#define MSG_TYPE2   8

#define KEEP_ALIVE_TIME        10
#define KEEP_ALIVE_INTERVAL     3
#define KEEP_ALIVE_PROBETIMES   3

#endif
