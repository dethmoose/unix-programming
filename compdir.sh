#!/bin/bash

# Check if given dir exists
# Ensure user is in parent dir of the given dir
# Save tar archive file in parent dir of the given dir
# Check if user has write permission on parent dir
# Warn before archiving a large dir
# 'du' and 'awk'

error() { 
    if [ $# -eq 1 ]; then
        echo $1 
    fi
    exit 1
}

if  [ $# != 1 ] 
then
    error "No arguments given! Give a directory as input" # says this for too many args as well
fi
NAME=$1

# Verify dir exists
if [ ! $(find $NAME -type d) ]; then # -path ?
    # Prevent find from printing error messages in terminal
    error "Cannot find directory $NAME"
fi

# Check write permission parent dir
if [ ! -w "${PWD}" ]; then 
    error "Missing write permission on ${PWD}"
fi

# TODO Check file size

# Archive dir
tar -czf $NAME.tgz $NAME
# Debug print
echo "done"