#!/bin/bash
#
# HOW TO USE
#
# ./timeAll.sh path-to-levels timeout-in-seconds logfile-to-write-to
# e.g.:
# cd into the project folder (dashen)
# ./timeAll.sh ./environment/competition_levels17/ 15 benchmark.log
#
# The final output - benchmark in table like structure - is written
# in a file with current local timestamp.
#
#   preparation or in case something does not work:
# chmod +x timeAll.sh
# chmod +x timeout3
# chmod +x makePretty.py
# make clean && make
#
# have fun, tested and working (LINUX) :)

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

tmpfile="tmp.log"
grep -oe 'LVL.*' -e 'Length.*' -e 'user.*' $logfilename > $tmpfile

curdate=$(date '+%Y%m%d_%H%M_%S')".txt"
echo $curdate
python3.6 makePretty.py > $curdate 

if [ -f $tmpfile ] ; then
    rm $tmpfile
fi
