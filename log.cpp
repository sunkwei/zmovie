#include "log.h"
#include <stdio.h>
#include "dialoglogger.h"

Log::Log()
{

}

DialogLogger *_logger = 0;

void Log::log(int level, const char *fmt, ...)
{
    char buf[4096];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    if (_logger) {
        _logger->log(level, buf);
    }
    else {
        fprintf(stderr, "(%d): %s", level, buf);
    }
}
