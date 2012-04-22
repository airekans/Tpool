#!/usr/bin/env python

import socket
import sys

def parse_address(addr):
    if ':' not in addr:
        host = '127.0.0.1'
        port = addr
    else:
        host, port = addr.split(':', 1)

    if not port.isdigit():
        parser.error('Ports must be integers.')

    return host, int(port)

def main(address):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(address)

    daytime = ''
    while True:
        data = sock.recv(1024)
        if not data:
            sock.close()
            break
        else:
            daytime += data

    print "received daytime: ", daytime

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print "Please input server address"
        sys.exit(1)

    main(parse_address(sys.argv[1]))

