#!/bin/bash

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=$(dirname $SCRIPT)
macfile="$SCRIPTPATH/mac"

if [ ! -e "$macfile" ]; then
    ranmac="DE:AD:BE:EF"
    for x in $(seq 1 2); do
        num=$(echo "$RANDOM$RANDOM" | cut -n -c -2)
        ranmac=$ranmac:$num
    done
    echo $ranmac > $macfile
fi

exec sudo kvm -m 128 -net nic,macaddr=$(cat $macfile) -net tap -smp 1 -drive file=$SCRIPTPATH/hda.qcow2 "$@"
