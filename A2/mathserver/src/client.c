#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

void validate_args(int argc, char *argv[]);
// int connect(char *ip_addr, int port_no);
// int command();
// void help_text();
// char *send(char *command);

// Command line options to handle:
// -p port
// -ip address

int main(int argc, char *argv[])
{
    validate_args(argc, argv);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = atoi(argv[1]);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connect_status = connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connect_status == -1)
    {
        printf("Error\n");
        exit(1);
    }

    int running = 1;
    while (running)
    {
        char strData[255];
        printf("Enter a command for the server: ");
        fgets(strData, 255, stdin);
        send(server_socket, strData, sizeof(strData), 0);

        recv(server_socket, strData, sizeof(strData), 0);
        printf("Received the solution: %s", strData);
    }
    exit(0);
}

void validate_args(int argc, char *argv[])
{
    if (argc < 2 || argc > 5)
    {
        printf("Error\n");
        exit(1);
    }
}

// // Connect to server
// int connect(char *ip_addr, int port_no)
// {
//     printf("Connected to server\n");
//     return 0;
// }

// // Take user input, send message, present solution
// int command()
// {
//     char *solution = "-", *msg = "-";

//     printf("Enter a command for the server: ");
//     printf("\n");
//     solution = send(msg);
//     printf("Recieved the solution: %s\n", solution);
//     return 0;
// }

// // Send a command to server
// char *send(char *msg)
// {
//     return "-";
// }
