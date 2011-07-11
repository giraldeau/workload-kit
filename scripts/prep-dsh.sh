#!/bin/sh

cat /dev/null > members2
for i in $(cat members); do
    echo root@$i >> members2
done

