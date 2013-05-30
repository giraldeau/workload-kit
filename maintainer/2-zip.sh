#!/bin/sh -x

DATE=$(date "+%Y%m%d")
BASE=/tmp/traceset
OUT=lttng-traceset-$DATE/

cd $BASE
time tar -C $BASE -cjf lttng-traceset-$DATE.tar.bz2 $OUT/
cd -
