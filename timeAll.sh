#!/bin/bash
#
# LINUX
#
# ./timeAll.sh path-to-levels timeout-in-seconds logfile-to-write-to
# e.g.:
# cd into the project folder (dashen)
# ./timeAll.sh ./environment/levels-1/ 2 test.log 
#
#   preparation or in case something does not work:
# chmod +x timeAll.sh
# chmod +x timeout3
# make clean && make
#
# have fun, tested and working :)

dirlevels="$1"
executiontime="$2"
logfilename="$3"

if [ -f $logfilename ] ; then
    rm $logfilename
fi

exec 3<> $logfilename
for filename in "$dirlevels"*.lvl; do  
    #echo "LVL" $(basename $filename)
    echo $(readlink -f $filename)
    >&3 echo "LVL" $(basename $filename)
    result=$( { time ./timeout3 -t $executiontime ./environment/client/main < $filename 1>&3 2>&3; } 2>&3 )
done

exec 3>&-

grep -oe 'LVL.*' -e 'Length.*' -e 'user.*' test.log

