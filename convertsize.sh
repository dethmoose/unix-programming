#!/bin/bash

usage() { echo "Usage: $0 [integer][B|KB|MB|GB]"; exit 1; }

if  [ $# != 1 ] 
then
    usage
fi

RESULT=$(echo $1 | grep -o '[0-9]*')
UNIT=$(echo $1 | grep -o '[A-Z]*')
echo $UNIT
echo $(expr $RESULT )

# Is there a way to perform the same but with less repetition?
case $UNIT in
    B)
        b=$RESULT
        kb=$(echo "scale=4;${b}/1024;" | bc -l)
        mb=$(echo "scale=4;${kb}/1024;" | bc -l)
        gb=$(echo "scale=4;${mb}/1024;" | bc -l)
    ;;
    KB)
        kb=$RESULT
        b=$(echo "${kb}*1024" | bc)
        mb=$(echo "scale=4;${kb}/1024;" | bc -l)
        gb=$(echo "scale=4;${mb}/1024;" | bc -l)
        
    ;;
    MB)
        mb=$RESULT
        gb=$(echo "scale=4;${mb}/1024;" | bc -l)
        kb=$(echo "${mb}*1024" | bc)
        b=$(echo "${kb}*1024" | bc)
    ;;
    GB)
        gb=$RESULT
        mb=$(echo "scale=4;${gb}*1024;" | bc -l)
        kb=$(echo "${mb}*1024" | bc)
        b=$(echo "${kb}*1024" | bc)
    ;;
esac

echo "Bytes = ${b}"
echo "Kilobytes = ${kb}"
echo "Megabytes = ${mb}"
echo "Gigabytes = ${gb}"