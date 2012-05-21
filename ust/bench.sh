#!/bin/bash -x

lttng create test
#lttng enable-channel chan --subbuf-size 8192 -u
lttng enable-channel chan --subbuf-size 131072 -u
lttng enable-event -a -u
lttng start
sleep 0.1
./functrace 10 &
PID=$!
wait $PID
lttng stop
lttng view
lttng destroy

