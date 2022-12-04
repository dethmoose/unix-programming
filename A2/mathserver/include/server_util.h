/* Process util functions for server.c */

#ifndef SERVER_UTIL_H
#define SERVER_UTIL_H

#include <stdio.h> // enum

/* Failing exit status for features not implemented */
#define EXIT_NOT_IMPLEMENTED 3

enum Strategy
{
    FORK,
    MUXBASIC,
    MUXSCALE
};

/* Functions */

void run_with_fork(int port, char cwd[]);
void run_with_muxbasic();
void run_with_muxscale();
void run_as_daemon(const char *process_name);
void matinv_run(int sd, char command[], char cwd[], int client_num, int solution_num);
void kmeans_run(int sd, char command[], char cwd[], int client_num, int solution_num);

#endif // SERVER_UTIL_H
