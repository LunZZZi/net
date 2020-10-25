#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <errno.h>
#include <arpa/inet.h>

ssize_t readline(int fd, void *vptr, size_t maxlen);

#define SERV_PORT   9877
#define MAXLINE     200     /* max text line length */
#define	LISTENQ		1024	/* 2nd argument to listen() */

#define assert_ret(no, msg) \
    do {\
        if (no < 0) {\
            err_sys(msg);\
        }\
    } while (false)

#define err_sys(msg)\
    do {\
        printf(msg "\n");\
        exit(-1);\
    } while (false)

#endif
