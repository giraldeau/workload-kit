#!/bin/sh

i=0
while true; do
	modprobe lttng-uevent
	rmmod lttng-uevent
	echo "reload $i"
	i=$(expr $i + 1)
done
