#!/bin/sh -x

DATE=$(date "+%Y%m%d")
TAR=lttng-traceset-$DATE.tar.bz2

scp $TAR fgiraldeau@secretaire.dorsal.polymtl.ca:public_html/traceset/
ssh fgiraldeau@secretaire.dorsal.polymtl.ca "cd public_html/traceset/; ln -sf $TAR lttng-traceset-current.tar.bz2"
