#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

// int connect(char *ip_addr, int port_no);
// int command();
// void help_text();
// char *send(char *command);

// Command line options to handle:
// -p port
// -ip address

void main(int argc, char *argv[])
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = atoi(argv[1]);
    server_address.sin_addr.s_addr = INADDR_ANY;

    int connect_status = connect(sock, (struct sockaddr*)&server_address, sizeof(server_address));

    if (connect_status == -1) {
        printf("Error\n");
    } 
    else {
        char strData[255];
        recv(sock, strData, sizeof(strData), 0);

        printf("message: %s\n", strData);
    }
    // while (1) {}

    // command();
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
