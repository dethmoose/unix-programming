#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>

// TODO: kmeans send data to server with -f filename option

// Flags and default values.
int port = 1337;
int ip_f = 0;
char *ip = "";

int usage();
void read_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage();
    read_options(argc, argv);

    if (port == -1) 
    {
        printf("Error: No port assigned\n");
        usage();
    }

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (ip_f)
        server_address.sin_addr.s_addr = inet_addr(ip);
    else
        server_address.sin_addr.s_addr = INADDR_ANY;

    int connect_status = connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_status == -1)
    {
        printf("Error: cannot connect\n");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        char strData[255];
        printf("Enter a command for the server: ");
        fgets(strData, 255, stdin);
        strData[strlen(strData) - 1] = '\0'; // Remove newline from command 

        /* TODO:
        Check here if the strData from input is either a kmeans or matinv command.
        Reason is that the server sends an error message, but the client is actually 
        waiting to recieve the information for the file to be created, and instead of a
        proper filename, it instead opens a file called "Error: ..."

        Read strData and parse command before sending.
        */


       
        if ((send(server_socket, strData, strlen(strData) + 1, 0)) == -1)
        {
            perror("Error sending command.");
        }

        // If -f flag is set, open filename and send data to server.

        // Recieve filename from server.
        recv(server_socket, strData, sizeof(strData), 0);
        printf("Received the solution: %s\n", strData);
        char filename[60] = "../computed_results/";
        strcat(filename, strData);

        recv_file(server_socket, filename);
    }
    exit(EXIT_SUCCESS);
}

void parse_command(int sd, char command[])
{
    char *ptr = strtok(command, " ");
    while (ptr != NULL)
    {
        if (strcmp(ptr, "-f") == 0){
            ptr = strtok(NULL, " ");
            send_file(sd, ptr); 
            break;
        }
        ptr = strtok(NULL, " ");
    }
}

void send_file(FILE* fp, int sd)
{
    char output[255] = "";
    memset(output, 0, sizeof(output));
    while (fgets(output, sizeof(output), fp) != NULL)
    {
        if ((send(sd, output, strlen(output), 0)) == -1)
        {
            perror("Error sending file.\n");
            break;
        }
        memset(output, 0, sizeof(output));
    }
}
 
void recv_file(int sd, char filename[])
{
    // Open file with append. "a" functions as if calling open with O_CREAT | O_WRONLY | O_APPEND
    FILE *fp = fopen(filename, "a");
        if (fp == NULL)
        {
            printf("Error opening file.\n");
            exit(EXIT_FAILURE);
        }

        char recvbuf[255] = "";
        memset(recvbuf, 0, sizeof(recvbuf));
        int recv_bytes; // How many bytes are recieved by call to recv().
        while (1)
        {
            if ((recv_bytes = recv(sd, recvbuf, sizeof(recvbuf), 0)) == -1)
            {
                perror("Error recieving file.");
            }
            else if (strstr(recvbuf, "\nOutput End\n") != NULL) { break; }
            else
            {
                // Writes recv_bytes number of bytes from recvbuf to file.
                fwrite(recvbuf, sizeof(char), recv_bytes, fp);
            }
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
    exit(EXIT_FAILURE);
}

