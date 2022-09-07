#!/bin/sh

# Print message then exit 
error() { echo $1; exit; }

# Validate number of arguments
if [ $# -ne 1 ]; then
    error "1 argument needed (file to compress)"
fi

filename=$1
best_tool="-"

# Run commands in parallel
gzip --keep -q $filename & 
P1=$!
bzip2 --keep -q $filename & 
P2=$!
p7zip --keep $filename >/dev/null & 
P3=$!
lzop -q $filename &
P4=$!
# wait for all processes to finish before continuing
wait $P1 $P2 $P3 $P4 

# Which is the smallest file
s_size=$(du -b $filename | awk '{print $1}')
s_filename=$filename
for file in $(find ${filename}.*)
do  
    temp=$(du -b $file | awk '{print $1}')
    if [ $temp -lt $s_size ]; then
        s_size=$temp
        s_filename=$file
        best_tool=$(echo $file | sed 's/.*\.//')
    fi
done
# Remove all compressed files except for the smallest file (including original file).
if [ $s_filename = $filename ]; then
    echo "Original file is the smallest."
    rm ${filename}.*
else
    for file in $(find ${filename}*)
    do
        if [ $file != "$filename.$best_tool" ]; then
            rm $file
        fi
    done
fi

# Print result
echo "Most compression obtained with $best_tool. Compressed file is $s_filename"