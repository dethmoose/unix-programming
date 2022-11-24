all:
	rm -f client server matinv kmeans
	gcc -w -O2 ./src/client.c -o client
	gcc -w -O2 ./src/server.c -o server
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans 
# TODO Undersök pthread v lpthread, får en massa undefined references med lpthread
tests:
	rm -f seq_kmeans seq_matinv matinv kmeans
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans 
	gcc -w -O2 ./src/matrix_inverse.c -o seq_matinv
	gcc -w -O2 ./src/kmeans.c -o seq_kmeans
	
client:
	gcc -w -O2 ./src/client.c -o client 

server:
	gcc -w -O2 ./src/server.c -o server

matinv-par:
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv

kmeans-par: 
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans

matinv:
	gcc -w -O2 ./src/matrix_inverse.c -o seq_matinv

kmeans: 
	gcc -w -O2 ./src/kmeans.c -o seq_kmeans

clean:
	rm -f client kmeans matinv server 
# ./../computed_results/kmeans-results.txt
