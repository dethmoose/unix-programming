#!/usr/bin/bash
make tests
# echo "Parallell matinv"
# time ./matinv -n 2048 -P 0
# echo "Sequential matinv"
# time ./seq_matinv -n 2048 -P 0
echo "Parallell kmeans"
time ./kmeans 
echo "Sequential kmeans"
time ./seq_kmeans 
