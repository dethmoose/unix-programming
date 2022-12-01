all:
	rm -f client server matinv kmeans
	gcc -w -O2 ./src/client.c ./src/file_util.c -o client
	gcc -w -O2 ./src/server.c ./src/file_util.c ./src/server_util.c -o server
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans

tests:
	rm -f kmeans-seq matinv-seq matinv kmeans
	gcc -w -O2 -pthread ./src/matinv-par.c -o matinv
	gcc -w -O2 -pthread ./src/kmeans-par.c -o kmeans 
	gcc -w -O2 ./src/matrix_inverse.c -o matinv-seq
	gcc -w -O2 ./src/kmeans.c -o kmeans-seq
	
client:
	gcc -w -O2 ./src/client.c ./src/file_util.c -o client 

server:
	gcc -w -O2 ./src/server.c ./src/file_util.c ./src/server_util.c -o server

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
