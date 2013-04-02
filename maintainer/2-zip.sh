#!/bin/sh -x

DATE=$(date "+%Y%m%d")
BASE=/tmp/traceset
NAME=lttng-traceset-$DATE

time tar -C $BASE -cjf $NAME.tar.bz2 $NAME/

