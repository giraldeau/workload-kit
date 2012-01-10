#!/usr/bin/python

import signal, time
import sys

interval = 1.0
ticks = 0

def alarm_handler(signo, frame):
    global ticks
    print "alarm ", ticks
    ticks = ticks + 1
    sys.exit(0)

def main():
    signal.signal(signal.SIGALRM, alarm_handler)
    signal.setitimer(signal.ITIMER_REAL, interval, 0)
    while 1:
        time.sleep(1)
        pass

if __name__ == "__main__":
    main()