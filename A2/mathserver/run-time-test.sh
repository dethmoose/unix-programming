#!/usr/bin/bash

make -s tests
run_silent() {
    echo -e $1 $2 $3 $4 $5 $6  # Print command.
    $2 $3 $4 $5 $6 >/dev/null  # Execute & redirect output to /dev/null.
}

# time run_silent "Parallell matinv:"      ./matinv     -P 0 -n 400
# time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 400
# time run_silent "\n\nParallell kmeans:"  ./kmeans
# time run_silent "\n\nSequential kmeans:" ./kmeans-seq


time run_silent "\nParallell Kmeans"    ./kmeans -f ./src/kmeans-data-10.txt
time run_silent "\nParallell Kmeans"    ./kmeans -f ./src/kmeans-data-50.txt
time run_silent "\nParallell Kmeans"    ./kmeans -f ./src/kmeans-data-300.txt
time run_silent "\nParallell Kmeans"    ./kmeans
time run_silent "\nParallell Kmeans"    ./kmeans-seq