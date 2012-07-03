#!/bin/bash 

function reload_modules() {
	sudo rmmod lttng-uevent
	sudo modprobe lttng-uevent
}

function run_once() {
	lttng create test
	lttng enable-event lttng_uevent -k
	lttng start
	./test-uevent
	lttng stop
	lttng destroy
}

reload_modules

for i in $(seq 1 1); do
	run_once
done
