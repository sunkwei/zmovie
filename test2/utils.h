#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
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

inline void save_pcm(const void *data, int len)
{
    const char *fname = "test.pcm";
    FILE *fp = fopen(fname, "ab");
    if (fp) {
        fwrite(data, 1, len, fp);
        fclose(fp);
    }
}

#endif // UTILS_H
