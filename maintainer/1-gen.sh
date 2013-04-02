#!/bin/sh

DATE=$(date "+%Y%m%d")
OUT=/tmp/traceset/lttng-traceset-$DATE/
rm -rf $OUT
mkdir -p $OUT
BASE_DIR="/usr/local/share/workload-kit/scripts"

gen_trace() {
	EXTRA_ARGS=$1
	SCRIPT_DIR=$2
	time lttng-simple $EXTRA_ARGS \
		-b $SCRIPT_DIR -o $OUT \
		-l $OUT/lttng-simple.log >> $OUT/lttng-simple.log
}

gen_trace "-k" $BASE_DIR/kernel
gen_trace "" $BASE_DIR/ust
