#!/bin/bash

dirlevels="environment/levels-1/"

exec 3<> test.log
for filename in "$dirlevels"*.lvl; do  
    echo "current level" $(basename $filename)
    echo $(readlink -f $filename)
    >&3 echo "current level" $(basename $filename)
    result=$( { time ./timeout3 -t 2 ./environment/client/main < $filename 1>&3 2>&3; } 2>&3 )
done

exec 3>&-

