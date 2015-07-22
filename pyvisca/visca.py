#!/usr/bin/python
# coding: utf-8
#
# @file: visca.py
# @date: 2015-07-22
# @brief:
# @detail: 需要 easy_install pyserial
#
#################################################################

import serial

class Visca:
    ''' 实现常用的 visca 协议

        每个命令返回：
            -1：一般错误
            -2：写串口错误
            -3：读串口超时

        注意：
            1. addr 从 1 开始，到 cam_cnt()
            2. get_pos() 返回格式为 (errcode, x, y)
            3. get_zoom() 返回格式为 (errcode, zoom)

    '''

    def open(self, name):
        ''' 打开串口，失败返回 <0
            成功则返回链接的云台数目
        '''
        self.__serial_name = name
        try:
            self._serial = serial.Serial(name, 9600)
            self._serial.timeout = 10
            self._cam_cnt = self.set_address()
            return self._cam_cnt
        except Exception as e:
            print e
            return -1


    def close(self):
        try:
            self._serial.close()
        except:
            pass


    def cam_cnt(self):
        ''' 返回云台数目 '''
        return self._cam_cnt


    def set_address(self):
        ''' set address，返回云台数目 '''
        req = bytearray([0x88, 0x30, 0x01, 0xff])
        if self.__send(req) < 0:
            return -2
        try:
            res = self.__get_packet()
            return res[2] - 1
        except:
            return -3


    def stop(self, addr):
        req = bytearray([0x80 | addr, 0x01, 0x06, 0x01, \
                0x00, 0x00, 0x03, 0x03, 0xff])
        return self.__req_with_complete(req)


    def left(self, addr, speed):
        req = bytearray([0x80|addr, 0x01, 0x06, 0x01, speed, speed, \
                0x01, 0x03, 0xff])
        return self.__req_with_complete(req)


    def right(self, addr, speed):
        req = bytearray([0x80|addr, 1, 6, 1, speed, speed, 2, 3, 0xff])
        return self.__req_with_complete(req)


    def up(self, addr, speed):
        req = bytearray([0x80|addr, 1, 6, 1, speed, speed, 3, 1, 0xff])
        return self.__req_with_complete(req)


    def down(self, addr, speed):
        req = bytearray([0x80|addr, 1, 6, 1, speed, speed, 3, 2, 0xff])
        return self.__req_with_complete(req)


    def home(self, addr):
        req = bytearray([0x80|addr, 1, 6, 4, 0xff])
        return self.__req_with_complete(req)


    def preset_call(self, addr, idx):
        ''' 预制位 ..'''
        req = bytearray([0x80|addr, 1, 4, 0x3f, 2, idx, 0xff])
        return self.__req_with_complete(req)


    def preset_clr(self, addr, idx):
        req = bytearray([0x80|addr, 1, 4, 0x3f, 0, idx, 0xff])
        return self.__req_with_complete(req)


    def preset_save(self, addr, idx):
        req = bytearray([0x80|addr, 1, 4, 0x3f, 1, idx, 0xff])
        return self.__req_with_complete(req)


    def zoom_stop(self, addr):
        req = bytearray([0x80|addr, 1, 4, 7, 0, 0xff])
        return self.__req_with_complete(req)


    def zoom_wide(self, addr, speed):
        req = bytearray([0x80|addr, 1, 4, 7, 0x30|speed, 0xff])
        return self.__req_with_complete(req)


    def zoom_tele(self, addr, speed):
        req = bytearray([0x80|addr, 1, 4, 7, 0x20|speed, 0xff])
        return self.__req_with_complete(req)


    def set_pos(self, addr, x, y, sx, sy):
        req = bytearray([0x80|addr, 0x01, 0x06, 0x02, sx, sy, \
                0, 0, 0, 0, \
                0, 0, 0, 0, \
                0xff])
        if x < 0:
            x = 65536 + x
        if y < 0:
            y = 65536 + y
        
        req[6] = (x & 0xf000) >> 12
        req[7] = (x & 0x0f00) >> 8
        req[8] = (x & 0x00f0) >> 4
        req[9] = (x & 0x000f) >> 0

        req[10] = (y & 0xf000) >> 12
        req[11] = (y & 0x0f00) >> 8
        req[12] = (y & 0x00f0) >> 4
        req[13] = (y & 0x000f) >> 0

        return self.__req_with_ack(req)


    def set_rpos(self, addr, x, y, sx, sy):
        req = bytearray([0x80|addr, 0x01, 0x06, 0x03, sx, sy, \
                0, 0, 0, 0, \
                0, 0, 0, 0, \
                0xff])
        if x < 0:
            x = 65536 + x
        if y < 0:
            y = 65536 + y
        
        req[6] = (x & 0xf000) >> 12
        req[7] = (x & 0x0f00) >> 8
        req[8] = (x & 0x00f0) >> 4
        req[9] = (x & 0x000f) >> 0

        req[10] = (y & 0xf000) >> 12
        req[11] = (y & 0x0f00) >> 8
        req[12] = (y & 0x00f0) >> 4
        req[13] = (y & 0x000f) >> 0

        return self.__req_with_ack(req)
    

    def set_zoom(self, addr, z):
        req = bytearray([0x80|addr, 1, 4, 0x47,
                (z & 0xf000) >> 12,
                (z & 0x0f00) >> 8,
                (z & 0x00f0) >> 4,
                (z & 0x000f) >> 0,
                0xff])
        return self.__req_with_ack(req)


    def get_pos(self, addr):
        ''' 返回 (err, x, y) 表示当前云台位置 '''
        req = bytearray([0x80|addr, 0x09, 0x06, 0x12, 0xff])
        (err, res) = self.__req_with_res(req)
        if err < 0:
            return (err, 0, 0)

        x = res[2] << 12 | res[3] << 8 | res[4] << 4 | res[5]
        y = res[6] << 12 | res[7] << 8 | res[8] << 4 | res[9]

        if x > 0x8000:
            x = x - 65536
        if y > 0x8000:
            y = y - 65536

        return (0, x, y)


    def get_zoom(self, addr):
        ''' 返回 (err, zoom) '''
        req = bytearray([0x80|addr, 9, 4, 0x47, 0xff])
        (err, res) = self.__req_with_res(req)
        if err < 0:
            return (err, 0)

        z = res[2] << 12 | res[3] << 8 | res[4] << 4 | res[5]

        if z > 0x8000:
            z = z - 65536

        return (0, z)


########################################################################

    def __req_with_res(self, req):
        ''' 发送 req，返回 reply (err|len, reply)
        '''
        if self.__send(req) < 0:
            return (-2, None)
        try:
            res = self.__get_reply()
            return (len(res), res)
        except:
            return (-3, None)


    def __req_with_ack(self, req):
        ''' 发送 req，等待收到 ack，但不等待 complete '''
        if self.__send(req) < 0:
            return -2
        try:
            self.__get_packet()
            return 0
        except:
            return -3


    def __req_with_complete(self, req):
        ''' 发送 req，等待 complete '''
        if self.__send(req) < 0:
            return -2
        try:
            self.__get_complete()
            return 0
        except:
            return -3


    def __send(self, req):
        try:
            return self._serial.write(req)
        except:
            return -1


    def __get_reply(self):
        res = self.__get_packet()
        while not self.__is_reply(res):
            res = self.__get_packet()
        return res


    def __get_complete(self):
        res = self.__get_packet()
        while self.__is_ack(res):
            res = self.__get_packet()
        return res

        
    def __is_reply(self, res):
        ''' 检查是否为 information return '''
        return res[1] == 0x50


    def __is_ack(self, res):
        ''' 检查得到的 packet 是否为 ack '''
        return res[1] & 0xf0 == 0x40


    def __errcode(self, res):
        ''' 返回错误编号 '''
        if res[1] == 0x60:
            return res[2]
        else:
            return 0


    def __get_packet(self):
        ''' 读取，直到收到 \xff
        '''
        buf = bytearray([])
        c = self._serial.read()
        while c != '\xff':
            buf.append(c)
            c = self._serial.read()

        buf.append(c)
        return buf


    def __dump_packet(self, packet):
        import binascii
        print binascii.hexlify(packet)
##====================================================================##


if __name__ == '__main__':
    import time, sys

    name = '/dev/ttyS6'
    addr = 1

    visca = Visca()
  
    cnt = visca.open(name)
    if cnt < 0:
        print 'open err'
        sys.exit()
    else:
        print 'There are', cnt, 'cams'

    visca.home(addr)

    # left, right, up, down, stop
    rc = visca.left(addr, 3)
    time.sleep(2)
    rc = visca.up(addr, 1)
    time.sleep(2)
    rc = visca.right(addr, 3)
    time.sleep(2)
    rc = visca.down(addr, 1)

    rc = visca.get_pos(addr)
    print 'curr pos:', rc


    visca.set_pos(addr, 400, 400, 36, 36)
    rc = visca.get_pos(addr)
    print 'get_pos: ret:', rc

    rc = visca.set_zoom(addr, 3000)
    print 'set_zoom: ret:', rc

    rc = visca.get_zoom(addr)
    print 'get_zoom: ret:', rc

    visca.close()

# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4

