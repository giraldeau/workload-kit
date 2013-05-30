#!/bin/bash -x

# Release script: makes new tarballs of the current directory and push them to
# server. Updates md5sums accordingly.

# vars
PKG=workload-kit
VER=$(cat configure.ac | grep workload-kit | awk {'print $2'} | cut -d\) -f1)
TGZ=$PKG-$VER.tar.gz
BZ2=$PKG-$VER.tar.bz2
USER=fgiraldeau
HOST=secretaire.dorsal.polymtl.ca
HTML=public_html
CONN=$USER@$HOST
PKG_PATH=$HTML/$PKG/

#echo $TGZ $BZ2 $CONN $PKG_PATH
#exit 0
# do it
#git clean -f -d
./bootstrap
./configure
make ChangeLog
make dist

if [ ! -f ${TGZ} ]; then
	echo "ERROR: ${TGZ} not found"
	exit 1
fi
rsync -av ${TGZ} ${BZ2}  ${CONN}:${PKG_PATH}
ssh ${CONN} ./update_md5sums.sh ${PKG_PATH}


echo "Done."

echo "http://$HOST/~$USER/$PKG/"
