#!/bin/bash

if [ "$(id -u)" != "0" ]; then
    echo "must be root"
    exit 1
fi

#CLUSTER_DIR=/srv/ltt/cluster
#mkdir -p $CLUSTER_DIR
#nc -k -l 172.16.0.1 1234 > $CLUSTER_DIR/members &
#NC_PID=$!
#echo $NC_PID > ltt.pid

cat /dev/null > ltt.pid

for i in $(ls -1d ltt*/); do
    $i/run.sh & 
    echo $! >> ltt.pid
    sleep 1
done

