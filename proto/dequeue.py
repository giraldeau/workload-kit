#!/usr/bin/python

import sys
import os
import socket
import time
import string
from scapy.all import IP, sendp
import struct
import threading

is_running = True
sock = "nfqueue.sock"
delay = 0.1
pkts = []

def clean_sock():
    if (os.path.exists(sock)):
        os.unlink(sock)

def enqueue_pkt(data):
    pkts.append(data)
    
def recv_pkt(conn):
    d1 = conn.recv(4)
    print len(d1)
    size = struct.unpack("I", d1)[0]
    data = ""
    total = 0
    while(1):
        d2 = conn.recv(size)
        if not d2: break
        total = len(d2)
        data = data + d2
    if (total != size):
        raise RuntimeError("recv data doesn't match size")
    enqueue_pkt(data)
    
def run_server():
    clean_sock()
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(sock)
    s.listen(1)
    global is_running
    while(is_running):
        conn, addr = s.accept()
        recv_pkt(conn)

def sender_daemon():
    global is_running
    while(is_running):
        if len(pkts) > 0:
            try:
                sendp(IP(pkts.pop(0)))
            except Exception, e:
                print "sendp error: " + str(e)
        time.sleep(delay)

def flush_pkts():
    for p in pkts:
        sendp(p)

def main():
    t1 = threading.Thread(target=sender_daemon)
    t1.daemon = True
    t1.start()
    
    t2 = threading.Thread(target=run_server)
    t2.daemon = True
    t2.start()
    
    try:
        while(1):
            time.sleep(1)
    except KeyboardInterrupt:
        pass

    flush_pkts()
    print "exitting"
    
if __name__ == "__main__":
    main()