#ifndef LOG_H
#define LOG_H

#include "stdarg.h"

#define LOG_DEBUG_TYPE 0
#define LOG_MSG_TYPE   1
#define LOG_WARN_TYPE  2
#define LOG_ERR_TYPE   3

void net_log(int severity, const char *msg);
void net_logx(int severity, const char *errstr, const char *fmt, va_list ap);
void net_msgx(const char *fmt, ...);
void net_debugx(const char *fmt, ...);


#define LOG_MSG(msg) net_log(LOG_MSG_TYPE, msg)
#define LOG_ERR(msg) net_log(LOG_ERR_TYPE, msg)

#endif
