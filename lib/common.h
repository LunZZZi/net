#ifndef __COMMON_H__
#define __COMMON_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/time.h>       /* timeval */
#include <poll.h>
#include <fcntl.h>          /* for nonblocking */
#include <netinet/in.h>
#include <strings.h>
#include <unistd.h>
#include <stdlib.h>
#include <cstdio>
#include <errno.h>
#include <arpa/inet.h>
#include <stdint.h>

ssize_t readline(int fd, void *vptr, size_t maxlen);
size_t readn(int fd, void *buffer, size_t length);
ssize_t writen(int fd, const void *vptr, size_t n);

#define SERV_PORT   9877
#define MAXLINE     4096     /* max text line length */
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
