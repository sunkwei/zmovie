#ifndef LOG_H
#define LOG_H

class Log
{
public:
    Log();

    static void log(int level, const char *fmt, ...);
};

#define FAULT 0
#define ERROR 1
#define WARNING 2
#define INFO 3
#define DEBUG 4

#endif // LOG_H
