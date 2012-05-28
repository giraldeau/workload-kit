#!/bin/sh -x

# Release script: makes new tarballs of the current directory and push them to
# server. Updates md5sums accordingly.

# vars
PKG_NAME="workload-kit"
USER=fgiraldeau
SERVER=secretaire.dorsal.polymtl.ca
CONNEX=${USER}@${SERVER}
PKG_PATH=public_html/${PKG_NAME}/

# do it
make dist
rsync -av ${PKG_NAME}-*.tar.bz2 ${PKG_NAME}-*.tar.gz  ${CONNEX}:${PKG_PATH}
ssh ${CONNEX} ./update_md5sums.sh ${PKG_PATH}

echo "Done."

echo "http://$SERVER/~$USER/$PKG_NAME/"
