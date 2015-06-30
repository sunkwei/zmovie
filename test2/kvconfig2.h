#ifndef KVCONFIG_H
#define KVCONFIG_H

#include <stdio.h>
#include <string>
#include <map>
#include <QQuickItem>
#include <QString>
#include <curl/curl.h>

// 加载/保存 key=value 配置文件 ...
class KVConfig2 : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString fname READ fname WRITE setFname)

    std::string fname_, fsession_name_;
    typedef std::map<std::string, std::string> KVS;
    KVS kvs_;
    bool local_;

public:
    KVConfig2(const char *fname);
    KVConfig2();

    const char *get_value(const char *key, const char *def = 0);
    void set_value(const char *key, const char *value);
    void set_value(const char *key, int value);
    void set_value(const char *key, double value);

    int save_as(const char *fname = 0);

    QString fname();
    void setFname(const QString &fname);
    Q_INVOKABLE QString get(const QString &key);
    Q_INVOKABLE void set(const QString &key, const QString &val);
    Q_INVOKABLE int save();
    Q_INVOKABLE int reload();

private:
    void load(const char *fname);
    int load_from_target(const char *url);
    int save_to_target(const char *url);
};

#endif // KVCONFIG_H
