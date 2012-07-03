#!/bin/sh

while true; do
	modprobe lttng-uevent
	rmmod lttng-uevent
done
