#!/bin/bash

usage() { echo "No arguments given! Give a directory as input"; exit 1; }

if  [ $# != 1 ] 
then
    usage
fi