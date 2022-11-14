# Assignment 2

New courses begin 2022-10-31.
Assignment 2 second deadline: 2022-12-05.

## Todo list

### Grade D

* Send input data from client to server for kmeans.
  * Create function `send_file` and `recv_file`. 

* Save results to file in directory named after client_num, read from file to send to client.
  * Requires modification to kmeans and matinv programs to save file, takes argument from server.

* Validate function in server `run_with_fork` for more readability

* Separate machines: make sure output files are generated correctly

* Handle arguments for kmeans and matinv such as -f
  * If -f is supplied, client should start sending file, and server should start recieving file.
  * Server should use this file to run program with.

* Does Matinv print flag make a difference?

* Test if server crashes if the client exits after not being able to open the result file (client.c line 90)

* Maybe write better, more readable variable names in client/server

### Higher Grade

* Investigate timeout for POLL, required? (muxbasic)
  * Rewrite to be less copypaste....

* Muxscale
