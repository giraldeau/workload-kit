#!/bin/sh

DATE=$(date "+%Y%m%d")
BASE_DIR="/usr/local/share/workload-kit/scripts"

gen_trace() {
	TAG=$1
	EXTRA_ARGS=$2
	SCRIPT_DIR=$3
	OUT=/tmp/traceset/lttng-traceset-$TAG-$DATE/
	rm -rf $OUT
	mkdir -p $OUT
	time lttng-simple $EXTRA_ARGS \
		-b $SCRIPT_DIR -o $OUT \
		-l $OUT/lttng-simple.log >> $OUT/lttng-simple.log
}

EVLIST_BASIC=$BASE_DIR/profiles/basic.list

gen_trace kernel-full 		"-k -s" $BASE_DIR/kernel
gen_trace kernel-full-nosys 	"-k" $BASE_DIR/kernel
gen_trace kernel-basic 		"-k -s -e $EVLIST_BASIC" $BASE_DIR/kernel
gen_trace kernel-basic-nosys 	"-k -e $EVLIST_BASIC" $BASE_DIR/kernel
gen_trace ust-and-kernel	"-s" $BASE_DIR/ust
gen_trace ust			"-u" $BASE_DIR/ust
