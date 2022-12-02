#!/usr/bin/bash

make -s tests
run_silent() {
    echo -e $1 $2 $3 $4 $5 $6  # Print command.
    $2 $3 $4 $5 $6 >/dev/null  # Execute & redirect output to /dev/null.
}



# Comparing results parallel vs sequential

# ./matinv     -n 2
# ./matinv-seq -n 2

# ./matinv     -n 3
# ./matinv-seq -n 3

# ./matinv     -n 4
# ./matinv-seq -n 4

# # Default n=5
# ./matinv
# ./matinv-seq

# ./matinv     -n 6
# ./matinv-seq -n 6

# ./matinv     -n 7
# ./matinv-seq -n 7



# Comparing time  parallel vs sequential (THREADS = 32)

time run_silent "Parallel matinv:"       ./matinv     -P 0 -n 100
time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 100

time run_silent "\n\nParallel matinv:"   ./matinv     -P 0 -n 200
time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 200

time run_silent "\n\nParallel matinv:"   ./matinv     -P 0 -n 400
time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 400

time run_silent "\n\nParallel matinv:"   ./matinv     -P 0 -n 1087
time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 1087

time run_silent "\n\nParallel matinv:"   ./matinv     -P 0 -n 2951
time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 2951

time run_silent "\n\nParallel matinv:"   ./matinv     -P 0 -n 4011
time run_silent "\n\nSequential matinv:" ./matinv-seq -P 0 -n 4011
