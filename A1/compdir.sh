#!/bin/sh

error() { echo $1; exit 1; }

if  [ $# -ne 1 ]; then
    error "Invalid number of arguments. Requires name of directory as the singular argument."
fi
NAME=$1

# Verify dir exists
if [ ! $(find $NAME -type d 2>/dev/null) ]; then
    # 2>/dev/null redirects stderr to /dev/null because we do not want find to print errors.
    error "Cannot find directory $NAME"
fi

# Ensure user is in parent dir of the given dir
# realpath prints the resolved path. 
# dirname strips the last component of the filename, to give the path to the parent directory.
PARENT="$(dirname "$(realpath $NAME)")"

# comparing PARENT to the pwd ensures we're running the script from the parent dir of the target dir.
if [ "$PARENT" != "$(pwd)" ]; then
    error "Not in the parent directory of target. You must specify a subdirectory."
fi

# Check write permission parent dir
if [ ! -w "${PWD}" ]; then 
    error "Cannot write the compressed file to the current directory, missing write permission on ${PWD}"
fi

# Check file size
SIZE=$(du -b $NAME | awk '{ print $1 }')

if [ "$SIZE" -gt 512000000 ]; then
    while [ "$PROCEED" != "y" ]
    do
        echo "Warning: the directory is bigger than 512 MB. Proceed? [y/n]"
        read PROCEED
        if [ "$PROCEED" = "n" ]; then
            exit
        fi
    done
fi

# Archive dir
tar -czf $NAME.tgz $NAME 
echo "Directory $NAME archived as $NAME.tgz"
