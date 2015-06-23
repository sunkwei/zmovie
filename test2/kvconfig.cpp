#include "kvconfig.h"

KVConfig::KVConfig()
{

}

KVConfig::KVConfig(const char *fname)
{
    fname_ = fname;
    fsession_name_ = fname_ + ".session";

    load(fname_.c_str());
    load(fsession_name_.c_str());
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

void KVConfig::load(const char *fname)
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

const char *KVConfig::get_value(const char *key, const char *def)
{
    KVS::const_iterator itf = kvs_.find(key);
    if (itf == kvs_.end()) {
        return def;
    }
    else {
        return itf->second.c_str();
    }
}

void KVConfig::set_value(const char *key, const char *value)
{
    kvs_[key] = value;
}

void KVConfig::set_value(const char *key, double value)
{
    char buf[32];
    sprintf(buf, "%f", value);
    set_value(key, buf);
}

void KVConfig::set_value(const char *key, int value)
{
    char buf[16];
    sprintf(buf, "%d", value);
    set_value(key, buf);
}

int KVConfig::save_as(const char *fname)
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

QString KVConfig::fname()
{
    return fname_.c_str();
}

void KVConfig::setFname(const QString &fname)
{
    fname_ = fname.toStdString();
    fsession_name_ = fname_ + ".session";

    kvs_.clear();

    load(fname_.c_str());
    load(fsession_name_.c_str());
}

QString KVConfig::get(const QString &key)
{
    return get_value(key.toStdString().c_str(), "");
}

void KVConfig::set(const QString &key, const QString &val)
{
    set_value(key.toStdString().c_str(), val.toStdString().c_str());
}

void KVConfig::save()
{
    save_as(fsession_name_.c_str());
}
