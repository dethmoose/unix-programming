#!/usr/bin/bash

make -s tests
run_silent() {
    echo -e $1 $2 $3 $4 $5 $6 $7 $8 $9  # Print command.
    $2 $3 $4 $5 $6 $7 $8 $9 >/dev/null  # Execute & redirect output to /dev/null.
}



# Comparing time and results parallel vs sequential

# N = 1797
time run_silent "Parallel kmeans:"       ./kmeans     -f "./src/kmeans-data.txt" -p "../computed_results/kmeans-results-par.txt"
time run_silent "\n\nSequential kmeans:" ./kmeans-seq -f "./src/kmeans-data.txt" -p "../computed_results/kmeans-results-seq.txt"
diff "../computed_results/kmeans-results-par.txt" "../computed_results/kmeans-results-seq.txt" >/dev/null

if [ $? == 1 ]
then
    echo "Wrong results..."
fi

# N > 3500000
time run_silent "\n\nParallel kmeans:"   ./kmeans     -f "./src/kmeans-data-big.txt" -p "../computed_results/kmeans-results-par.txt"
time run_silent "\n\nSequential kmeans:" ./kmeans-seq -f "./src/kmeans-data-big.txt" -p "../computed_results/kmeans-results-seq.txt"
diff "../computed_results/kmeans-results-par.txt" "../computed_results/kmeans-results-seq.txt" >/dev/null

if [ $? == 1 ]
then
    echo "Wrong results..."
fi

# Clean up test files
rm -f "../computed_results/kmeans-results-par.txt" "../computed_results/kmeans-results-seq.txt"
