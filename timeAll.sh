#!/bin/bash

dirlevels="environment/levels/"
dirlevels="environment/levels-1/"

exec 3<> test.log
for filename in "$dirlevels"*.lvl; do  
    echo $(basename $filename)
    echo $(readlink -f $filename)
    >&3 echo $(basename $filename)
    result=$( { time ./environment/client/main < $filename 1>&3 2>&3; } 2>&3 )
done

exec 3>&-

