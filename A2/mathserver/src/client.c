#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

// Default values
int port = -1;
char *ip = "";

int usage();
void read_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    if (argc < 2) // ip address optional?
        usage();
    read_options(argc, argv);
    // printf("Port number: %d, ip address: %s\n", port, ip);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connect_status = connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_status == -1)
    {
        printf("Error: cannot connect\n");
        exit(1);
    }

    while (1)
    {
        char strData[255];
        printf("Enter a command for the server: ");
        fgets(strData, 255, stdin);
        strData[strlen(strData) - 1] = '\0';
        send(server_socket, strData, sizeof(strData), 0);

        recv(server_socket, strData, sizeof(strData), 0);
        // parse filename, file content
        printf("Received the solution: %s\n", strData); // TODO only print filename

        // create file
        char create_file[281] = "touch ../computed_results/";
        strcat(create_file, strData);
        // TODO: fill file with data
        // int status = system(create_file);
    }
    exit(0);
}

void read_options(int argc, char *argv[])
{
    char *prog;
    prog = *argv;

    while (++argv, --argc > 0)
    {
        if (**argv == '-')
        {
            switch (*++*argv)
            {
            case 'i':
                --argc;
                ip = *++argv;
                break;

            case 'p':
                --argc;
                port = atoi(*++argv);
                break;

            case 'h':
            case 'u':
                usage();
                break;

            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
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
    exit(1);
}
