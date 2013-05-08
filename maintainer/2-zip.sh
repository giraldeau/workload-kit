#!/bin/sh -x

DATE=$(date "+%Y%m%d")
BASE=/tmp/traceset

cd $BASE
for dir in $(find . -maxdepth 1 -type d -name "*traceset*"); do
	NAME=$(basename $dir)
	time tar -C $BASE -cjf $NAME.tar.bz2 $NAME/
done

mkdir -p ready
for tar in $(find . -maxdepth 1 -type f -name "*.tar.bz2"); do
	mv $tar ready/
done

cd -
