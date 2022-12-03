#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "../include/file_util.h"

// Flags and default values.
int ip_f = 0, port = -1;
char *ip = "";

// Forward declarations
void usage();
void read_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    read_options(argc, argv);

    // Create a socket for connecting with server
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("Socket creation failed.");
        exit(EXIT_FAILURE);
    }

    // IP address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (ip_f)
        server_address.sin_addr.s_addr = inet_addr(ip);
    else
        server_address.sin_addr.s_addr = INADDR_ANY;

    if (connect(sd, (struct sockaddr *)&server_address, sizeof(server_address)) == -1)
    {
        perror("Cannot connect");
        exit(EXIT_FAILURE);
    }

    // Start communication with server
    while (1)
    {
        // Take user input
        char command[BUF_SIZE], res_filename[BUF_SIZE];
        memset(command, 0, BUF_SIZE);
        memset(res_filename, 0, BUF_SIZE);
        printf("Enter a command for the server: ");
        fgets(command, BUF_SIZE, stdin);
        command[strlen(command) - 1] = '\0'; // Remove newline from command

        // Check if the command from input is either a kmeans or matinv command.
        int intitial_chars = (strlen(command) > 6) ? 7 : 6;
        if (strncmp(command, "matinv ", intitial_chars) != 0 &&
            strncmp(command, "kmeans ", intitial_chars) != 0)
        {
            printf("Valid commands: 'matinv' or 'kmeans'\n");
            continue;
        }

        // Send command to server
        // Why sometimes sending strlen chars and sometimes strlen+1 chars?
        if ((send(sd, command, strlen(command) + 1, 0)) == -1)
        {
            perror("Error sending command");
            exit(EXIT_FAILURE);
        }

        // Recieve results filename from server.
        recv(sd, res_filename, sizeof(res_filename), 0);
        printf("Received the solution: %s\n", res_filename);
        char filename[PATH_SIZE] = "../client_results/";
        strncat(filename, res_filename, PATH_SIZE - strlen(filename));

        // Check if -f flag is set in kmeans command, send input file if so
        if (strncmp(command, "kmeans", 6) == 0)
        {
            parse_command(sd, command);
        }

        // Receive results data
        recv_file(sd, filename);
    }
}

void read_options(int argc, char *argv[])
{
    char *prog;
    prog = *argv;
    if (argc < 2)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'i':
                ip = argv[++i];
                ip_f = 1;
                break;
            case 'p':
                port = atoi(argv[++i]);
                break;
            case 'h':
            case 'u':
                usage();
                exit(EXIT_SUCCESS);
                break;
            default:
                printf("%s: ignored option: -%s\n", prog, argv[i]);
                printf("HELP: try %s -h \n\n", prog);
                break;
            }
        }
    }

    if (port == -1)
    {
        printf("Error: No port assigned\n");
        usage();
        exit(EXIT_FAILURE);
    }
}

void usage()
{
    printf("\nUsage: client [-p port]\n");
    printf("              [-ip address]\n");
    printf("              [-h]          help\n");
}
