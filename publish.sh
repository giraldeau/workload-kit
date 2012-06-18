#!/bin/bash -x

# Release script: makes new tarballs of the current directory and push them to
# server. Updates md5sums accordingly.

# vars
source vars.sh

# do it
#git clean -f -d
./bootstrap
./configure
make dist

if [ ! -f ${TGZ} ]; then
	echo "ERROR: ${TGZ} not found"
	exit 1
fi
rsync -av ${TGZ} ${BZ2}  ${CONNEX}:${PKG_PATH}
ssh ${CONNEX} ./update_md5sums.sh ${PKG_PATH}


echo "Done."

echo "http://$SERVER/~$USER/$PKG_NAME/"
