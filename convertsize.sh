#!/bin/sh

usage() { echo "Usage: $0 [integer][B|KB|MB|GB]"; exit 1; }

# Validate number of arguments
if  [ $# != 1 ]; then
    usage
fi

RESULT=$(echo $1 | grep -o -e '\-\?[0-9]*')
UNIT=$(echo $1 | grep -o '[A-Z]*')

if [ ! $RESULT ] || [ "$RESULT" = "-" ]; then 
    echo "Error: no number"
    usage 
elif [ $RESULT -lt 0 ]; then
    echo "Error: integer must be non-negative"
    usage 
fi

# Convert
# TODO Is there a way to perform the same but with less repetition?
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
	*)
		echo "Error: unkown unit '$UNIT'"
		usage
	;;
esac

# Print result
echo "Bytes = ${b}"
echo "Kilobytes = ${kb}"
echo "Megabytes = ${mb}"
echo "Gigabytes = ${gb}"
