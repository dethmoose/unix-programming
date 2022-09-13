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
bzip2 --keep -q $filename & 
p7zip --keep $filename >/dev/null & 
lzop -q $filename &
wait # wait for all processes to finish before continuing

# Find the smallest file
s_size=$(du -b $filename | awk '{print $1}')
s_filename=$filename
# echo "$filename , size $s_size"

for file in $(find ${filename}.*)
do  
    temp=$(du -b $file | awk '{print $1}')
    # echo "$file , size $temp"
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

    # Print result
    echo "Most compression obtained with $best_tool. Compressed file is $s_filename"
fi
