# TODO Undersök pthread v lpthread, får en massa undefined references med lpthread

all:
	rm -f client server matinv kmeans
	gcc -w -O2 ./src/client.c -o client
	gcc -w -O2 ./src/server.c -o server
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans

tests:
	rm -f seq_kmeans seq_matinv matinv kmeans
	gcc -w -O2 ./src/matinv-par.c -o matinv -lpthread
	gcc -w -O2 ./src/kmeans-par.c -o kmeans -lpthread 
	gcc -w -O2 ./src/matrix_inverse.c -o matinv-seq
	gcc -w -O2 ./src/kmeans.c -o kmeans-seq
	
client:
	gcc -w -O2 ./src/client.c -o client 

server:
	gcc -w -O2 ./src/server.c -o server

matinv: # parallel
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv

kmeans: # parallel
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans

matinv-seq:
	gcc -w -O2 ./src/matrix_inverse.c -o matinv-seq

kmeans-seq: 
	gcc -w -O2 ./src/kmeans.c -o kmeans-seq

clean:
	rm -f client kmeans matinv matinv-seq server kmeans-seq
	rm -f -r ./../computed_results/* ./../client_results/*
