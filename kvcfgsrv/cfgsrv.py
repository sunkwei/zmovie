# coding:utf-8
import tornado.httpserver
import tornado.ioloop
import tornado.web
import json
import kvs
from tornado.options import define, options
import os, sys
#import stdlib

define("port", default=8888, help="run on the given port", type = int)


class ConfigHandler(tornado.web.RequestHandler):
    def get(self, fname, method):
        self.__func(fname, method);

    def put(self, fname, method):
        self.__func(fname, method);    

    def __get_cfg(self, fname):
        ret = kvs.get_cfg(fname)
        jret = json.dumps(ret)
        self.set_header('Content-Type', 'application/json; charset=utf-8')
        self.set_header("Cache-control", "no-cache")
        self.write(jret)


    def __getValueByKey(self, fname):
        # FIXME
        kvs = cu.fn_config(fname, "get_kvs", self.request.arguments)
        if kvs == {}:
            ret = ''
        else:
            k = self.request.arguments.keys()[0]
            ret = kvs[k]
        self.set_header("Content-type", "text/plain")
        self.set_header("Cache-control", "no-cache")
        self.write(ret)
    

    def __setValueByKey(self, fname):
        # FIXME
        cu.fn_config(fname, 'alter', self.request.arguments)
        jret = json.dumps(ret)
        self.set_header('Content-Type', 'application/json')
        self.set_header('Cache-control', 'no-cache')
        self.write(jret)


    def __save(self, fname):
        env_path = os.environ.get('image_trace')
        # body must application/json
        dic = json.loads(self.request.body)
        if kvs.save(fname, dic) < 0:
            ret = { 'result': 'err', 'info':'' }
        else:
            ret = { 'result': 'ok', 'info': '' }
        self.set_header('Content-Type', 'application/json')
        jret = json.dumps(ret)
        self.set_header("Cache-control", "no-cache")
        self.write(jret)

    def __func(self, fname, method):
        func_table = {
            "get_cfg": self.__get_cfg,
            "getValueByKey": self.__getValueByKey,
            "setValueByKey": self.__setValueByKey,
            "save": self.__save
        }
        if method in func_table:
            func_table[method](fname)
        elif method == 'restart':
            self.set_header('Content-Type', 'application/json')
            self.set_header("Cache-control", "no-cache")
#            stdlib.send_udp_data("\x09\x01", "127.0.0.1")    
            ret = {"result":"", "info": ""}
            jret = json.dumps(ret)
            self.write(jret)
        else:
            self.set_header("Cache-control", "no-cache")
            self.render(method)

def main():
    tornado.options.parse_command_line()
    application = tornado.web.Application([
        (r'/config/([^/]*)/([^/]*)', ConfigHandler)
    ])
    http_server = tornado.httpserver.HTTPServer(application)
    http_server.listen(options.port)
    tornado.ioloop.IOLoop.instance().start()

if __name__ == "__main__":
    main()
