#!/usr/bin/python

import sys
import signal
import time
import socket
import nfqueue
from scapy.all import IP, TCP, sendp
import threading
import struct

interval = 0.5
ticks = 0

pkts = []
timestamp = 0.0
is_running = True

sock = "nfqueue.sock"

def setup_nfqueue(fct, num):
    q = nfqueue.queue()
    q.open()
    q.unbind(socket.AF_INET)
    q.bind(socket.AF_INET)
    
    q.set_callback(fct)
    q.create_queue(num)
    return q

def get_socket():
    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    try:
        s.connect(sock)
    except socket.error, e:
        print "error with socket " + str(e)
        s.close()
        s = None
    return s

def test_conn():
    s = get_socket()
    if s is None:
        return
    data = "testing"
    s.send(struct.pack("I", len(data)))
    s.send(data)
    s.close()

def teardown_nfqueue(q):
    q.unbind(socket.AF_INET)
    q.close()

def packet_handler(i, payload):
    data = payload.get_data()
    size = len(data)
    print "enqueue"
    payload.set_verdict(nfqueue.NF_DROP)
    s = get_socket()
    s.send(struct.pack("I", size))
    s.send(data)
    s.close()
    
def packet_handler_simple(i, payload):
    payload.set_verdict(nfqueue.NF_ACCEPT)
    time.sleep(0.1)

def enqueue_loop():
    print "start enqueue"
    q = setup_nfqueue(packet_handler_simple, 13)
    print "before try_run"
    q.try_run()
    print "Exitting..."
    teardown_nfqueue(q)

if __name__ == "__main__":
    #test_conn()
    try:
        enqueue_loop()
    except KeyboardInterrupt, e:
        pass
    print "Exitting..."
