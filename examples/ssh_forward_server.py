#! /usr/bin/env python
from SocketServer import (TCPServer as TCP,
                          StreamRequestHandler as SRH)
import struct
from sys import stdout

HOST = ''
PORT = 8999
ADDR = (HOST, PORT)

class MyRequestHandler(SRH):
    def handle(self):
        print '...connected from:', self.client_address
        s = self.rfile.read(1024) # size of int
        for i in range(len(s) / 4):
            num = struct.unpack('!I', s[i * 4 : (i + 1) * 4])[0]
            print num
        
if __name__ == '__main__':
    tcpServ = TCP(ADDR, MyRequestHandler)
    stdout.write("port:%5d" % PORT)
    stdout.flush()
    # print 'waiting for connection...'
    tcpServ.serve_forever()
