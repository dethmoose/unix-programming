#!/usr/bin/bash

run_silent() {
    $1 $2 $3 $4 $5 >/dev/null
}
make -s tests

echo -e "Parallell matinv:"
time run_silent ./matinv -P 0          # -n 2048

echo -e "\nSequential matinv:"
time run_silent ./matinv-seq -P 0      # -n 2048

echo -e "\nParallell kmeans:"
time run_silent ./kmeans

echo -e "\nSequential kmeans:"
time run_silent ./kmeans-seq
