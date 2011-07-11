#!/bin/sh

if [ -z "$1" ]; then
    echo "missing command argument"
    exit 1
fi

cmd="$@"

# Absolute path to this script. /home/user/bin/foo.sh
SCRIPT=$(readlink -f $0)
# Absolute path this script is in. /home/user/bin
SCRIPTPATH=$(dirname $SCRIPT)

ID=$(ifconfig eth0 | head -n 1 | awk {'print $5'} | sed s/://g)

name="cmd"
dir="$SCRIPTPATH/../../traces/trace-$name-$ID"
sudo rm -rf $dir
sudo lttctl -o channel.all.bufnum=8 -C -w $dir $name
echo "executing $cmd..."
$cmd
sleep 0.1
echo "return code: $?" 
sudo lttctl -D $name

