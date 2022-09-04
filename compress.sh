#!/bin/bash

# Print message then exit 
error() { echo $1; exit; }

# Compress with different tools, return name of best tool
compress() {
    local best_tool="-"

    # Run commands in parallel
    gzip --keep $1 & 
    P1=$!
    bzip2 --keep $1 & 
    P2=$!
    p7zip --keep $1 & 
    P3=$!
    lzop $1 &
    P4=$!
    wait $P1 $P2 $P3 $P4 # wait for all processes to finish before continuing

    # Retain smallest file, remove the others
    o_size=$(wc -c $1 | awk '{print $1}')
    g_size=$(wc -c $1.gz | awk '{print $1}')
    b_size=$(wc -c $1.bz2 | awk '{print $1}')
    p_size=$(wc -c $1.7z | awk '{print $1}')
    l_size=$(wc -c $1.lzo | awk '{print $1}') 
    # TODO compare sizes
    
    # Debug printing sizes temporarily
    echo "[$o_size, $g_size, $b_size, $p_size, $l_size]" # $best_tool
}

get_suffix() {
    case $1 in
        gzip)
            echo "gz"
            ;;
        bzip2)
            echo "bz2"
            ;;
        p7zip)
            echo "7z"
            ;;
        lzop)
            echo "lzo"
            ;;
        *)
            echo "-"
            ;;
    esac
}

# Validate number of arguments
if [ $# -ne 1 ]; then
    error "1 argument needed (file to compress)"
fi
filename=$1

# Compress
best_tool=$(compress $filename)
suffix=$(get_suffix $best_tool)

# Remove original file
# rm $filename

# Print result
echo "Most compression obtained with $best_tool. Compressed file is $filename.$suffix"