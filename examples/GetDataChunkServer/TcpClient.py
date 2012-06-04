#! /usr/bin/env python

from socket import *
import struct
import DataChunk_pb2

HOST = 'localhost'
PORT = 29995
BUFSIZ = 1024
ADDR = (HOST, PORT)

def get_package(message):
    package_header = ""
    message_name = message.DESCRIPTOR.name + "\x00"
    message_name_length = len(message_name)
    encoded_message = message.SerializeToString()
    
    package = struct.pack('!I', 10)
    package += "123456789\x00"
    
    for i in range(length - 10 - 4):
        package += "a"
    return package

tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR)

while True:
    num = raw_input('> ')
    data_chunk = DataChunk_pb2.GetSimpleDataChunkRequest()
    data_chunk.num = int(num)
    data = struct.pack('!I', long(num))
    data += get_package(int(num))
    print "length: %d" % len(data)
    if not data:
        break
    tcpCliSock.send('%s' % data)
    num = raw_input('continue...')
tcpCliSock.close()
