#!/bin/bash -x

lttng create test
#lttng enable-channel chan --subbuf-size 8192 -u
lttng enable-channel chan --subbuf-size 16384 -u
./sample &
PID=$!
sleep 0.1
lttng enable-event -a -u
lttng start
wait $PID
lttng stop
lttng destroy

