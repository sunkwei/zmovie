#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#ifdef WIN32
#   include <cc++/thread.h>
using namespace ost;
#else
#   include <sys/time.h>
#endif
#include "cJSON.h"

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

inline void log_init()
{
    FILE *fp = fopen("cal.log", "w");
    if (fp)
        fclose(fp);
}

inline void log(const char *fmt, ...)
{
    va_list arg;
    char buf[1024];

    va_start(arg, fmt);
    vsnprintf(buf, sizeof(buf), fmt, arg);
    va_end(arg);

    fprintf(stderr, "%s", buf);

    FILE *fp = fopen("cal.log", "at");
    if (fp) {
        fprintf(fp, "%s", buf);
        fclose(fp);
    }
}

class TimeUsed
{
    double begin_, max_;
    std::string info_;

public:
    TimeUsed(const char *info, double max_duration = 0.500)
    {
        if (info) info_ = info;
        else info_ = "NONE: ";

        max_ = max_duration;
        begin_ = util_now();
    }

    bool is_timeout() const
    {
        return util_now() - begin_ >= max_;
    }

    ~TimeUsed()
    {
        double now = util_now();
        if (now - begin_ > max_) {
            log("WARNING: TIMEOUT：'%s', using %.3f (%.3f)\n", info_.c_str(), now-begin_, max_);
        }
    }
};

// -------------- 构造更方便些的 json 解析树.
struct util_TypedObject
{
    int type;	// 0: bv
    // 1: nv
    // 2: sv
    // 3: av
    // 4: ob
    // 5: null
    struct {
        bool bv;	// True, False
        double nv;	// number
        std::string sv;	// string
        std::vector<util_TypedObject> av;	// array
        std::map<std::string, util_TypedObject> ov; // object
    } data;
};

static void build_array(const cJSON *json, std::vector<util_TypedObject> &array);
static void build_object(const cJSON *json, std::map<std::string, util_TypedObject> &obj);

static std::string build_one(const cJSON *json, util_TypedObject &to)
{
    switch (json->type) {
    case 0:	// cJSON_False
    case 1:	// cJSON_True
        to.type = 0;
        to.data.bv = (json->type == cJSON_True);
        break;

    case 2: // cJSON_NULL
        to.type = 5;
        break;

    case 3: // cJSON_Number
        to.type = 1;
        to.data.nv = json->valuedouble;
        break;

    case 4: // cJSON_String
        to.type = 2;
        to.data.sv = json->valuestring;
        break;

    case 5: // cJSON_array
        to.type = 3;
        build_array(json->child, to.data.av);
        break;

    case 6: // cJSON_object
        to.type = 4;
        build_object(json->child, to.data.ov);
        break;
    }

    if (json->string)
        return json->string;
    else
        return "";	// 无名对象 ..
}

static util_TypedObject util_build_object_from_json_string(const char *json_str)
{
    util_TypedObject to = { 5 };	// null ..

    cJSON *json = cJSON_Parse(json_str);
    if (json) {
        build_one(json, to);
        cJSON_Delete(json);
    }

    return to;
}

static void build_array(const cJSON *json, std::vector<util_TypedObject> &array)
{
    while (json) {
        util_TypedObject to;
        build_one(json, to);
        array.push_back(to);
        json = json->next;
    }
}

static void build_object(const cJSON *json, std::map<std::string, util_TypedObject> &objs)
{
    while (json) {
        util_TypedObject to;
        std::string name = build_one(json, to);
        objs[name] = to;
        json = json->next;
    }
}

#endif // UTILS_H
