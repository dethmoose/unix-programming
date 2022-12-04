# Assignment 2

New courses begin 2022-10-31.
Assignment 2 second deadline: 2022-12-05.

## Todo list

### Grade D

* Separate machines: make sure output files are generated correctly
  - Remove use of folder clients_results/

* Test every server feature

### Higher Grade

* Fix bug where daemon server cannot handle `kmeans` command without any options from client (probably only happens when at least 2 concurrent clients)

* Investigate timeout for POLL, required? (muxbasic)
  * Since the server only should terminate with ctrl+C we could probably skip the timeout?
  * Rewrite to be less copypaste.... Or cite source

* Muxscale

### Extra
* Clean up string operations (util functions)
