#!/usr/bin/python
# coding: utf-8
# 
# @file: kvs.py
# @date: 2015-07-09
# @brief:
# @detail:
#
#################################################################

from kvcfg import kvcfg

__kvss = {}

def __getcfg(fname):
    if fname in __kvss:
        return __kvss[fname]
    else:
        cfg = kvcfg(fname)
        __kvss[fname] = cfg
        return cfg
        

def save(fname, dic):
    cfg = __getcfg(fname)
    cfg.clr() # FIXME: 
    for k in dic:
        cfg.set(k, dic[k])
    return cfg.save()


def get_cfg(fname):
    cfg = __getcfg(fname)
    kvs = {}
    for k in cfg.keys():
        kvs[k] = cfg.get(k)
    return kvs


def getValueByKey(fname, k):
    cfg = __getcfg(fname)
    return cfg.get(k, '')


def getValuesByKeys(fname, key_list):
    cfg = __getcfg(fname)
    kvs = {}
    for k in key_list:
        kvs[k] = cfg.get(k, '')
    return kvs


def setValueByKey(fname, k, v):
    cfg = __getcfg(fname)
    cfg.set(k, v)


def setValuesByKeys(fname, kvs):
    cfg = __getcfg(fname)
    for k in kvs:
        cfg.set(k, kvs[k])


if __name__ == '__main__':
    import os
    try:
        os.remove('not_exist.config')
    except:
        pass

    try:
        os.remove('not_exist.config.session')
    except:
        pass

    print getValueByKey('test.config', 'cnt')
    print getValueByKey('not_exist.config', 'key')
    print setValueByKey('not_exist.config', 'key', 'A Key Value Testing     ')
    print getValueByKey('not_exist.config', 'key')
    print save('not_exist.config', {'a':'1', 'b':'2', 'c':'3'})


# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

