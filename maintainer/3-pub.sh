#!/bin/sh -x

SRV=fgiraldeau@secretaire.dorsal.polymtl.ca
SRC=/tmp/traceset/ready/
DST=public_html/traceset/

echo rsync -av $SRC $SRV:$DST

for tag in kernel-full kernel-full-nosys kernel-basic kernel-basic-nosys ust-and-kernel ust-only; do
	F=$(find $SRC -maxdepth 1 -type f -name "*$tag*" | sort | tail -n1)
	if [ -n "$F" ]; then
		LATEST=$(basename $F)
		echo ssh $SRV "cd $DST; ln -sf $LATEST lttng-traceset-$tag-current.tar.bz2"
	fi
done
