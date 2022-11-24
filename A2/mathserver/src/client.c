#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 256
#define PATH_SIZE 1024

// TODO: kmeans send data to server with -f filename option

// Flags and default values.
int ip_f = 0, port = -1;
char *ip = "";

// Forward declarations
int usage();
void send_file(FILE *fp, int sd);
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
        char strData[BUF_SIZE];
        printf("Enter a command for the server: ");
        fgets(strData, BUF_SIZE, stdin);
        strData[strlen(strData) - 1] = '\0'; // Remove newline from command

        // Check if the strData from input is either a kmeans or matinv command.
        int intitial_chars = (strlen(strData) > 6) ? 7 : 6;
        if (strncmp(strData, "matinv ", intitial_chars) != 0 &&
            strncmp(strData, "kmeans ", intitial_chars) != 0)
        {
            printf("Valid commands: 'matinv' or 'kmeans'\n");
            exit(EXIT_FAILURE);
        }

        // Send command to server
        // Why sometimes sending strlen chars and sometimes strlen+1 chars?
        if ((send(sd, strData, strlen(strData) + 1, 0)) == -1)
        {
            perror("Error sending command.");
            exit(EXIT_FAILURE);
        }

        // Recieve results filename from server.
        recv(sd, strData, sizeof(strData), 0);
        printf("Received the solution: %s\n", strData);
        char filename[60] = "../client_results/";
        strcat(filename, strData);

        // Check if -f flag is set in kmeans command, for input file
        if (strncmp(strData, "kmeans", 6) == 0)
        {
            parse_command(sd, strData);
        }
        // Receive results data
        recv_file(sd, filename);

        // Move results filename to here?
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
            FILE *fp = fopen(ptr, "r");
            if (fp == NULL)
            {
                perror("Cannot open file");
                exit(EXIT_FAILURE);
            }
            send_file(fp, sd);
            fclose(fp);
            break;
        }
        ptr = strtok(NULL, " ");
    }
}

/*
 * Send data file for kmeans to server.
 */
void send_file(FILE *fp, int sd)
{
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

    if ((send(sd, "\nOutput End\n", strlen("\nOutput End\n"), 0)) == -1)
    {
        if (errno == EPIPE)
            close(sd);
        else
            perror("Error sending FIN command");
        exit(EXIT_FAILURE);
    }
    printf("Sent file to sever\n");
}

/*
 * Receive solution file from server.
 */
void recv_file(int sd, char filename[])
{
    // Open file with append. "a" functions as O_CREAT | O_WRONLY | O_APPEND
    FILE *fp = fopen(filename, "a"); // "w"
    if (fp == NULL)
    {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    
    int recv_bytes; // How many bytes are recieved by call to recv().
    char recvbuf[BUF_SIZE] = "";
    memset(recvbuf, 0, BUF_SIZE);
    while (1)
    {
        if ((recv_bytes = recv(sd, recvbuf, sizeof(recvbuf), 0)) == -1)
        {
            perror("Error recieving file.");
            exit(EXIT_FAILURE);
        }
        else if (strstr(recvbuf, "\nOutput End\n") != NULL) { break; }
        else
        {
            // Writes recv_bytes number of bytes from recvbuf to file.
            fwrite(recvbuf, sizeof(char), recv_bytes, fp);
        }
    }
    fclose(fp);
    printf("Received file from server\n");
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
