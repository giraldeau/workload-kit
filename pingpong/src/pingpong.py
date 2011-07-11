#!/usr/bin/python
'''
Created on 2011-01-06

@author: francis
'''
from threading import Thread, Event
import time
import socket
import SocketServer
import threading
import sys, os
from optparse import OptionParser

class BaseWorker(Thread):
    def __init__(self):
        Thread.__init__(self)
        self.socket = None

    def set_socket(self, socket):
        self.socket = socket

    def send_msg(self, msg):
        totalsent = 0
        msg_len = len(msg)
        while(totalsent < msg_len):
            sent = self.socket.send(msg[totalsent:])
            if sent == 0:
                raise RuntimeError("socket connection broken")
            totalsent = totalsent + sent
    
    def recv_msg(self):
        rep = ""
        totalrecv = 1
        while(totalrecv > 0):
            chunk = self.socket.recv(1024)
            totalrecv = len(chunk)
            rep = rep + chunk        
        return rep

class ClientWorker(BaseWorker):
    
    def run(self):
        msg = "ping"
        self.send_msg(msg)
        rep = self.recv_msg()
        print "ClientWorker: sent " + msg + " recv " + rep
        self.socket.close()

class ClientThread(Thread):
    def __init__(self, port):
        Thread.__init__(self)
        self.stop_event = Event()
        self.servers = []
        self.port = port
    
    def set_servers(self, servers):
        self.servers = servers    
    
    def ping_servers(self):
        workers_dic = {}
        for server in self.servers:
            workers_dic[server] = ClientWorker()
            
        for server in self.servers:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                s.connect((server, self.port))
            except IOError, e:
                print "error: can't connect to " + server
                continue
            worker = workers_dic[server] 
            worker.set_socket(s)
            worker.start()

        for server in self.servers:
            worker = workers_dic[server]
            if (worker.is_alive()):
                worker.join()
            
    def run(self):
        while(not self.stop_event.isSet()):
            self.ping_servers()
            time.sleep(1);
    
    def stop(self):
        self.stop_event.set()
        Thread.join(self, 0.5)
        
class ThreadedTCPRequestHandler(SocketServer.BaseRequestHandler):
    
    def handle(self):
        data = self.request.recv(1024)
        cur_thread = threading.currentThread()
        response = "%s: %s" % (cur_thread.getName(), "pong")
        self.request.send(response)

class ThreadedTCPServer(SocketServer.ThreadingMixIn, SocketServer.TCPServer):
    def __init__(self, host, handler):
        SocketServer.TCPServer.allow_reuse_address = 1
        SocketServer.TCPServer.__init__(self, host, handler)

def main(port, timeout, servers):
    print "starting"
    
    HOST, PORT = "0.0.0.0", int(port)

    try:
        server = ThreadedTCPServer((HOST, PORT), ThreadedTCPRequestHandler)
    except Exception, e:
        print "error starting server: " + str(e) 
        return
    
    ip, port = server.server_address

    # Start a thread with the server -- that thread will then start one
    # more thread for each request
    server_thread = threading.Thread(target=server.serve_forever)
    # Exit the server thread when the main thread terminates
    server_thread.setDaemon(True)
    server_thread.start()
    print "Server loop running in thread:", server_thread.getName()

    ct = ClientThread(port)
    ct.set_servers(servers)
    ct.start()
    
    start_time = time.time()
    try:
        while(time.time() < start_time + timeout):
            time.sleep(0.1)
    except KeyboardInterrupt, e:
        print "\ntear down"
    
    ct.stop()

    server.shutdown()
    
    print "exiting"
    
    
if __name__ == '__main__':
    
    parser = OptionParser()
    parser.add_option("-f", "--file", dest="filename",
                  help="hosts input FILE", metavar="FILE")
    parser.add_option("-p", "--port", dest="port", 
                  help="port number PORT", metavar="PORT", default="8765")
    parser.add_option("-t", "--timeout", dest="timeout", 
                  help="timeout SEC", metavar="SEC", default="5")

    (options, args) = parser.parse_args()

    servers = args
    if ((options.filename is not None) and (os.path.isfile(options.filename))):
        f = open(options.filename, 'r')
        for host in f.readlines():
            servers.append(host.strip())

    main(options.port, float(options.timeout), servers)
