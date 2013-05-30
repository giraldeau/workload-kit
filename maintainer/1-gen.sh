#!/bin/sh

DATE=$(date "+%Y%m%d")
BASE_DIR="/usr/local/share/workload-kit/scripts"

OUT=/tmp/traceset/lttng-traceset-$DATE/
rm -rf $OUT
mkdir -p $OUT

gen_trace() {
	EXTRA_ARGS=$1
	SCRIPT_DIR=$2
	LOG=$OUT/lttng-simple.log
	time lttng-simple $EXTRA_ARGS \
		-b $SCRIPT_DIR -o $OUT \
		-l $LOG >> $LOG
}

gen_trace "-k -s" 	$BASE_DIR/kernel
gen_trace "-u" 		$BASE_DIR/ust
gen_trace "-u -k -s" 	$BASE_DIR/ust
