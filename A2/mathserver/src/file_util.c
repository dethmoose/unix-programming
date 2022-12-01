#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "../include/file_util.h"

/*
 * Receive file from socket `sd`. Save it as `filename`.
 */
void recv_file(int sd, char filename[])
{
    int file_size = 0;
    char recvbuf[BUF_SIZE] = {0};
    int recv_bytes; // How many bytes are recieved by call to recv().

    if ((recv_bytes = recv(sd, recvbuf, sizeof(recvbuf), 0)) == -1)
    {
        perror("Error recieving file.");
        exit(EXIT_FAILURE);
    }

    file_size = atoi(recvbuf);

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("Error opening file");
        // TODO: close socket before exit?
        exit(EXIT_FAILURE);
    }

    memset(recvbuf, 0, BUF_SIZE);

    int remain = file_size;
    int recieving = 1;
    while ((remain > 0) && ((recv_bytes = recv(sd, recvbuf, sizeof(recvbuf), 0)) > 0))
    {
        // Writes recv_bytes number of bytes from recvbuf to file.
        fwrite(recvbuf, sizeof(char), recv_bytes, fp);
        remain -= recv_bytes;
    }
    fclose(fp);
};


/*
 * Send file `filename` to socket `sd`.
 */
void send_file(int sd, char filename[])
{
    // Open file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("send_file: Error opening file");
        exit(EXIT_FAILURE);
    }

    // Send file size to recieve to socket.
    char file_size[BUF_SIZE];
    struct stat file_stat;
    if (fstat(fileno(fp), &file_stat) < 0)
    {
        perror("Error reading file size");
        exit(EXIT_FAILURE);
    }

    snprintf(file_size, BUF_SIZE, "%d", file_stat.st_size);
    if ((send(sd, file_size, sizeof(file_size), 0)) == -1)
    {
        perror("Error sending file size");
        exit(EXIT_FAILURE);
    }

    // Send file data.
    char output[BUF_SIZE] = "";
    memset(output, 0, BUF_SIZE);
    while (fgets(output, sizeof(output), fp) != NULL)
    {
        if ((send(sd, output, strlen(output), 0)) == -1)
        {
            perror("Error sending file");
            exit(EXIT_FAILURE);
        }
        memset(output, 0, BUF_SIZE);
    }
    fclose(fp);
}

/*
 * Parse command for the "-f" flag. If it is set, send the file to socket `sd`.
 */
void parse_command(int sd, char command[])
{
    char *ptr = strtok(command, " ");
    while (ptr != NULL)
    {
        if (strcmp(ptr, "-f") == 0)
        {
            // The filename is the next argument
            ptr = strtok(NULL, " ");

            // Check to make sure there actually follows another argument after "-f"
            if (ptr == NULL)
            {
                printf("Ignored option: -f\n");
                break;
            }
            send_file(sd, ptr);
            break;
        }
        ptr = strtok(NULL, " ");
    }
}

/*
 * Parse command for the "-f" flag. If it is set, return 1.
 */
int has_f_flag(char command[])
{
    char *ptr = strtok(command, " ");
    while (ptr != NULL)
    {
        if (strcmp(ptr, "-f") == 0)
        {
            return 1;
        }
        ptr = strtok(NULL, " ");
    }
    return 0;
}
