#!/usr/bin/python
# coding: utf-8
# 
# @file: kvcfg.py
# @date: 2015-07-09
# @brief:
# @detail:
#
#################################################################

class kvcfg:
    ''' load from fname, and then overwrite by content from fname+'.session'
        save to fname+'.session
    '''
    def __init__(self, fname):
        self._kvs = {}
        self._fname = fname
        self._kvs.update(self._load(fname))
        self._kvs.update(self._load(fname + ".session"))


    def get(self, key, defval = ''):
        if key in self._kvs:
            return self._kvs[key]
        else:
            return defval

    def clr(self):
        self._kvs.clear()

    def remove(self, key):
        return self._kvs.pop(key, '')


    def set(self, key, val):
        self._kvs[key] = val


    def save(self, fname = None):
        ''' -1 for error
        '''
        if not fname:
            fname = self._fname + ".session"
        try:
            with open(fname, 'w') as f:
                for k in self._kvs:
                    f.write(k + '=' + self._kvs[k] + '\n')
        except:
            return -1
        else:
            return 0


    def keys(self):
        ''' return keys list, just for debug ... '''
        return self._kvs.keys()


    def _load(self, fname):
        ''' -1 for error
        '''
        kvs = {}
        try:
            with open(fname, 'r') as f:
                for line in f:
                    line = line.strip()
                    if len(line) < 3 or line[0] == '#':
                        continue
                    kv = line.split('=', 1)
                    if len(kv) == 2:
                        kvs[kv[0]] = kv[1]
        except Exception as e:
            pass # just ignore all ...
        return kvs


if __name__ == '__main__':
    cfg = kvcfg('./test.config')
    for k in cfg.keys():
        print k, '=' , cfg.get(k)

    cnt = int(cfg.get('cnt', '0'))
    cnt += 1
    cfg.set('cnt', str(cnt));
    if cfg.save() < 0:
        print 'ERR save ...'
    else:
        print 'ok..'


# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

