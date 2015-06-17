#ifndef UTILS_H
#define UTILS_H

#ifdef WIN32
#   include <cc++/thread.h>
using namespace ost;
#else
#   include <sys/time.h>
#endif

/** 返回秒 */
inline double util_now()
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec+tv.tv_usec/1000000.0;
}

#endif // UTILS_H
