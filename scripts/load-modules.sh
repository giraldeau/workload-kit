#!/bin/sh

for i in ltt-trace-control ltt-marker-control ltt-tracer ltt-relay ltt-kprobes ltt-userspace-event ltt-statedump ipc-trace kernel-trace mm-trace net-trace fs-trace jbd2-trace syscall-trace trap-trace block-trace rcu-trace net-extended-trace; do modprobe $i; done
