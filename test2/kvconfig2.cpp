#include "kvconfig2.h"
#include <string>
#include <stdio.h>
#include "cJSON.h"
#include <sstream>
#include <vector>
#include <map>
#include <assert.h>
#include "utils.h"
#include <sys/types.h>
#include <sys/stat.h>

extern KVConfig2 *_cfg;

KVConfig2::KVConfig2()
{
    local_ = true;
    setFname(".noname.config"); // 缺省 ...

    _cfg = this;    // Fuck ..
}

KVConfig2::KVConfig2(const char *fname)
{
    _cfg = this;    // Fuck ..

    fname_ = fname;
    fsession_name_ = fname_ + ".session";

    reload();
}

static bool isspace_any(int c)
{
    return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}

static char *strim(char *p)
{
    char *q = p;
    while (*q && (isspace_any(*q))) q++;
    char *tail = q + strlen(q);
    while (--tail > q && isspace_any(*tail)) {
        *tail = 0;
    }

    return q;
}

void KVConfig2::load(const char *fname)
{
    FILE *fp = fopen(fname, "r");
    if (fp) {
        while (!feof(fp)) {
            char line[1024], key[64], value[1024];
            char *p = fgets(line, sizeof(line), fp);
            if (p) {
                p = strim(p);
                if (*p == '#' || *p == 0) {
                    continue;
                }

                if (sscanf(p, "%63[^=] = %1023[^\r\n]", key, value) == 2) {
                    char *k = strim(key), *v = strim(value);
                    kvs_[k] = v;
                }
            }
        }

        fclose(fp);
    }
}

const char *KVConfig2::get_value(const char *key, const char *def)
{
    KVS::const_iterator itf = kvs_.find(key);
    if (itf == kvs_.end()) {
        return def;
    }
    else {
        return itf->second.c_str();
    }
}

void KVConfig2::set_value(const char *key, const char *value)
{
    kvs_[key] = value;
}

void KVConfig2::set_value(const char *key, double value)
{
    char buf[32];
    sprintf(buf, "%f", value);
    set_value(key, buf);
}

void KVConfig2::set_value(const char *key, int value)
{
    char buf[16];
    sprintf(buf, "%d", value);
    set_value(key, buf);
}

int KVConfig2::save_as(const char *fname)
{
    if (!fname) {
        fname = fsession_name_.c_str();
    }

    FILE *fp = fopen(fname, "w");
    if (!fp) {
        return -1;
    }

    KVS::const_iterator it;
    for (it = kvs_.begin(); it != kvs_.end(); ++it) {
        fprintf(fp, "%s=%s\n", it->first.c_str(), it->second.c_str());
    }

    fclose(fp);
    return 0;
}

QString KVConfig2::fname()
{
    return fname_.c_str();
}

int KVConfig2::reload()
{
    kvs_.clear();

    if (!strncmp(fname_.c_str(), "http:", 5)) {
        local_ = false;
    }
    else {
        local_ = true;
    }

    if (local_) {
        load(fname_.c_str());
        load(fsession_name_.c_str());

        return 0;
    }
    else {
        return load_from_target(fname_.c_str());
    }
}

void KVConfig2::setFname(const QString &fname)
{
    fname_ = fname.toStdString();
    fsession_name_ = fname_ + ".session";
}

QString KVConfig2::get(const QString &key)
{
    return get_value(key.toStdString().c_str(), "");
}

void KVConfig2::set(const QString &key, const QString &val)
{
    set_value(key.toStdString().c_str(), val.toStdString().c_str());
}

int KVConfig2::save()
{
    if (local_) {
        save_as(fsession_name_.c_str());
        return 0;
    }
    else {
        return save_to_target(fname_.c_str());
    }
}

struct MemoryStruct {
    char *memory;
    size_t size;
};

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

// trim from start
static inline std::string &ltrim(std::string &s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s)
{
    return ltrim(rtrim(s));
}

struct Headers
{
    std::vector<std::string> headers;	// 需要自己根据 ":" 分割 ...

    struct {
        std::string version;
        int code;
        std::string reason;
    } start_line;

    std::map<std::string, std::vector<std::string> > h;

    /// 从 headers 中解析，保存到 start_line 和 h 中
    void parse()
    {
        start_line.version.clear();
        start_line.code = -1;
        start_line.reason.clear();

        h.clear();

        for (std::vector<std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
            parse_line(it->c_str());
        }
    }

    int parse_line(const char *s)
    {
        char key[64], value[256];
        if (sscanf(s, "%63[^:] : %255[^\r\n]", key, value) == 2) {
            std::string skey(key), svalue(value);
            skey = trim(skey), svalue = trim(svalue);
            std::map<std::string, std::vector<std::string> >::iterator itf = h.find(skey);
            if (itf == h.end()) {
                std::vector<std::string> vs;
                vs.push_back(svalue);
                h[skey] = vs;
            }
            else {
                itf->second.push_back(svalue);
            }

            return 1;
        }
        else if (strstr(s, "HTTP") == s) {
            /// 八成是 start_line
            if (sscanf(s, "%s %d %255[^\r\n]", key, &start_line.code, value) == 3) {
                start_line.version = key;
                start_line.reason = value;

                return 0;
            }
        }

        return -1;
    }
};

static size_t cb_headers(void *p, size_t size, size_t n, void *opaque)
{
    Headers *h = (Headers*)opaque;
    h->headers.push_back(std::string((char*)p, size*n));
    return size*n;
}

static size_t cb_write(void *body, size_t size, size_t n, void *opaque)
{
    size_t len = size * n;
    MemoryStruct *ms = (MemoryStruct*)opaque;
    ms->memory = (char*)realloc(ms->memory, ms->size + len + 1); // 方便保存 "\0"
    if (!ms->memory) {
        fprintf(stderr, "ERR:[kvconfig]: realloc err ?????? size=%u\n", ms->size+len);
        return 0;
    }

    memcpy(ms->memory+ms->size, body, len);
    ms->size += len;
    ms->memory[ms->size] = 0;	// 这样为了方便字符串处理

    return len;
}

int KVConfig2::load_from_target(const char *turl)
{
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "ERR:[kvconfig]: curl_easy_init fault, url=%s\n", turl);
        return -1;
    }

    char url[1024];
    snprintf(url, sizeof(url), "%s/get_cfg", turl);
    curl_easy_setopt(curl, CURLOPT_URL, url);

    MemoryStruct ms = { 0, 0 };
    Headers headers;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ms);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-kvconfig/1.9");
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, cb_headers);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &headers);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

    int rc = -1;
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "ERR:[kvconfig]: curl_easy_perform fault, url=%s\n", turl);
    }
    else {
        headers.parse();

        cJSON *json = cJSON_Parse(ms.memory);
        if (json) {
            /** json 格式为：
                    {
                        "key1": "value1",
                        "key2": "value2",
                         ...
                    }
             */
            util_TypedObject to;
            std::string name = build_one(json, to);

            assert(to.type == 4);
            for (std::map<std::string, util_TypedObject>::const_iterator it = to.data.ov.begin(); it != to.data.ov.end(); ++it) {
                if (it->second.type == 2) {
                    // String
                    if (it->first != "_dummy_") {
                        kvs_[it->first] = it->second.data.sv;
                    }
                }
            }

            cJSON_Delete(json);
            rc = 0;
        }
    }

    curl_easy_cleanup(curl);
    if (ms.memory) {
        free(ms.memory);
    }

    return rc;
}

static size_t cb_read(void *p, size_t size, size_t n, void *opaque)
{
    MemoryStruct *ms = (MemoryStruct*)opaque;
    size_t bytes = size*n;
    if (bytes > ms->size) {
        bytes = ms->size;
    }

    memcpy(p, ms->memory, bytes);
    ms->memory += bytes;
    ms->size -= bytes;

    return bytes;
}

int KVConfig2::save_to_target(const char *turl)
{
    int rc = -1;
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "ERR:[kvconfig]: curl_easy_init fault, url=%s\n", turl);
        return -1;
    }

    std::stringstream ss;
    ss << "{";
    for (KVS::const_iterator it = kvs_.begin(); it != kvs_.end(); ++it) {
        if (it->first == "_dummy_") {
            continue;
        }
        ss << '"' << it->first << '"' << ':' << '"' << it->second << '"' << ',';
    }
    ss << " \"_dummy_\":\"\" }";

    std::string str = ss.str();

    curl_slist *headers = 0;
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=gbk");

    MemoryStruct msr = { (char*)str.data(), str.size() };
    MemoryStruct msw = { 0, 0 };

    char url[1024];
    snprintf(url, sizeof(url), "%s/save", turl);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    //curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    //curl_easy_setopt(curl, CURLOPT_READFUNCTION, cb_read);
    //curl_easy_setopt(curl, CURLOPT_READDATA, &msr);
    //curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, msr.size);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, str.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &msw);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "ERR:[kvconfig] curl_easy_perform PUT fault, url=%s\n", turl);
        save_as(".cached.config.session"); //
    }
    else {
        cJSON *json = cJSON_Parse(msw.memory);
        if (json) {
            util_TypedObject to;
            std::string name = build_one(json, to);
            if (to.type == 4 && to.data.ov["result"].data.sv == "ok") {
                rc = 0;
            }
            else {
                rc = -2;
            }
            cJSON_Delete(json);
        }
    }

    curl_slist_free_all(headers);
    if (msw.memory) {
        free(msw.memory);
    }
    curl_easy_cleanup(curl);

    return rc;
}
