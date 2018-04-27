#!/bin/bash

dirlevels="environment/levels/"

for filename in "$dirlevels"*.lvl; do  
    echo $(basename $filename)
    echo $(readlink -f $filename)
done
