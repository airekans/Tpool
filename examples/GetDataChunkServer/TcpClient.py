#! /usr/bin/env python

from socket import *
import struct
import DataChunk_pb2

HOST = 'localhost'
PORT = 29995
BUFSIZ = 1024
ADDR = (HOST, PORT)

def get_package(message):
    message_name = message.DESCRIPTOR.full_name + "\0"

    print "message old len: %d" % len(message.DESCRIPTOR.name)
    print len(message_name)
    
    message_name_length = len(message_name)
    encoded_message = message.SerializeToString()
    package_length = 4 + message_name_length + len(encoded_message)
    package = struct.pack('!II', package_length, message_name_length)
    package += message_name + encoded_message

    print "message len: %d" % len(encoded_message)

    return package
    
tcpCliSock = socket(AF_INET, SOCK_STREAM)
tcpCliSock.connect(ADDR)

while True:
    try:
        num = int(raw_input('> '))
        data_chunk = DataChunk_pb2.GetSimpleDataChunkRequest()
        data_chunk.num = num
        package = get_package(data_chunk)
        print "length: %d" % len(package)
        if not package:
            break
        tcpCliSock.send(package)
        num = raw_input('continue...')
    except EOFError:
        break
tcpCliSock.close()
