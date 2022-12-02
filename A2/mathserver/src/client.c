#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 256
#define PATH_SIZE 1024

// Flags and default values.
int ip_f = 0, port = -1;
char *ip = "";

// Forward declarations
int usage();
void send_file(int sd, char filename[]);
void recv_file(int sd, char filename[]);
void read_options(int argc, char *argv[]);
void parse_command(int sd, char command[]);

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage(); // Exits program
    read_options(argc, argv);

    if (port == -1)
    {
        printf("Error: No port assigned\n");
        usage(); // Exits program
    }

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
            perror("Error sending command.");
            exit(EXIT_FAILURE);
        }

        // Temp, save copy of command before overwriting with filename
        // strncpy(tmp, command, BUF_SIZE);

        // Recieve results filename from server.
        recv(sd, res_filename, sizeof(res_filename), 0);
        printf("Received the solution: %s\n", res_filename);
        char filename[PATH_SIZE] = "../client_results/";
        strncat(filename, res_filename, PATH_SIZE - strlen(filename));

        // Check if -f flag is set in kmeans command, for input file
        if (strncmp(command, "kmeans", 6) == 0)
        {
            parse_command(sd, command);
        }

        // Receive results data
        recv_file(sd, filename);
    }
}

/*
 * Parse kmeans command for the "-f" flag.
 * If it is set, send data file to server.
 */
void parse_command(int sd, char command[])
{
    char *ptr = strtok(command, " ");
    while (ptr != NULL)
    {
        if (strcmp(ptr, "-f") == 0)
        {
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
 * Send data file for kmeans to server.
 */
void send_file(int sd, char filename[])
{
    // Open file
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Error opening file.");
        exit(EXIT_FAILURE);
    }
    // Send file size to recieve to socket.
    char file_size[255];

    struct stat file_stat;
    if (fstat(fileno(fp), &file_stat) < 0)
    {
        perror("Error reading file size.");
        exit(EXIT_FAILURE);
    }

    snprintf(file_size, 255, "%d", file_stat.st_size);

    if ((send(sd, file_size, sizeof(file_size), 0)) == -1)
    {
        perror("Error sending file size.");
        exit(EXIT_FAILURE);
    }

    char output[BUF_SIZE] = "";
    memset(output, 0, BUF_SIZE);
    while (fgets(output, sizeof(output), fp) != NULL)
    {
        if ((send(sd, output, strlen(output), 0)) == -1)
        {
            perror("Error sending file.");
            exit(EXIT_FAILURE);
        }
        memset(output, 0, BUF_SIZE);
    }

    fclose(fp);
}

/*
 * Receive solution file from server.
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

    // Open file with append. "a" functions as O_CREAT | O_WRONLY | O_APPEND
    FILE *fp = fopen(filename, "w"); // "w"
    if (fp == NULL)
    {
        printf("Error opening file.\n");
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
}

void read_options(int argc, char *argv[])
{
    char *prog;
    prog = *argv;

    int i = 1;
    for (i; i < argc; i++)
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
                    break;
                default:
                    printf("%s: ignored option: -%s\n", prog, argv[i]);
                    printf("HELP: try %s -h \n\n", prog);
                    break;
            }
        }
    }
}

int usage()
{
    printf("\nUsage: client [-p port]\n");
    printf("              [-ip address]\n");
    printf("              [-h]          help\n");
    exit(EXIT_SUCCESS);
}
