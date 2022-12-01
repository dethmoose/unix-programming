/* File util functions for client.c and server.c */

#ifndef FILE_UTIL_H
#define FILE_UTIL_H

/* Constant sizes */

#define PATH_SIZE 1024
#define BUF_SIZE 256

/* Functions */

void recv_file(int sd, char filename[]);
void send_file(int sd, char filename[]);
void parse_command(int sd, char command[]);
int has_f_flag(char command[]);

#endif // FILE_UTIL_H
