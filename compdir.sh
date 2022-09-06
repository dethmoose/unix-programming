#!/bin/sh

# Check if given dir exists
# Ensure user is in parent dir of the given dir
# Save tar archive file in parent dir of the given dir
# Check if user has write permission on parent dir
# Warn before archiving a large dir
# 'du' and 'awk'

error() { 
    echo $1
    exit 1
}

if  [ $# -ne 1 ]; then
    error "Invalid number of arguments. Requires name of directory as the singular argument."
fi
NAME=$1

# Verify dir exists
if [ ! $(find $NAME -type d 2>/dev/null) ]; then # -path ?
    # 2>/dev/null redirects stderr to /dev/null because we do not want find to print errors.
    error "Cannot find directory $NAME"
fi

# Ensure user is in parent dir of the given dir
# realpath prints the resolved path. 
# dirname strips the last component of the filename, to give the path to the parent directory.
PARENT="$(dirname "$(realpath $NAME)")"

# comparing PARENT to the pwd ensures we're running the script from the parent dir of the target dir.
if [ "$PARENT" != "$(pwd)" ]; then
    error "Not in the parent directory of target."
fi

# Check write permission parent dir
if [ ! -w "${PWD}" ]; then 
    error "Missing write permission on ${PWD}"
fi

# Check file size
SIZE=$(du -b $NAME | awk '{ print $1 }')

if [ "$SIZE" -gt 512000000 ]; then
    error "Directory larger than 512Mb"
fi

# Archive dir
tar -czf $NAME.tgz $NAME 

# Debug print
echo "Done"