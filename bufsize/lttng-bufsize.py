#!/usr/bin/python
# do experiment for buffer size

import subprocess
import time

# size power of two
subbuf_size_default=18
subbuf_size_min=12
subbuf_size_max=22

num_subbuf_default=4
num_subbuf_min=0
num_subbuf_max=6

LTTNG="lttng"
PREFIX="buf"

def cmd_stub(cmds):
    print str(cmds)
    #subprocess.call(cmds)

def do_trace(subbuf_size, num_subbuf):
    print "do_trace %s %s" % (subbuf_size, num_subbuf)
    session_name = "%s-%d-%d" % (PREFIX, subbuf_size, num_subbuf)
    subbuf_size_str = str(subbuf_size)
    num_subbuf_str = str(num_subbuf)
    cmd_stub([LTTNG,"create", session_name])    
    cmd_stub([LTTNG,"enable-channel", "chan", "--subbuf-size",
        subbuf_size_str, "--num-subbuf", num_subbuf_str, "--kernel"])
    cmd_stub([LTTNG,"enable-event", "-a", "-k"])
    cmd_stub([LTTNG,"start"])
    time.sleep(1)    
    cmd_stub([LTTNG,"stop"])
    cmd_stub([LTTNG,"destroy", session_name])

def do_experiment():
    for s in range(subbuf_size_min, subbuf_size_max+1):
        byte_size = 1 << s
        for n in range(num_subbuf_min, num_subbuf_max + 1):
            num_subbuf = 1 << n
            do_trace(byte_size, num_subbuf)

if __name__=="__main__":
    do_experiment()
    #do_trace(4096, 4)

