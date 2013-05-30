#!/bin/sh -x

DATE=$(date "+%Y%m%d")

TAR=lttng-traceset-$DATE.tar.bz2
SRV=fgiraldeau@secretaire.dorsal.polymtl.ca
SRC=/tmp/traceset
DST=public_html/traceset

rsync -av $SRC/$TAR $SRV:$DST/$TAR
ssh $SRV "cd $DST; ln -sf $TAR lttng-traceset-current.tar.bz2"
