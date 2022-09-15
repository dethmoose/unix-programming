#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

// int connect(int port_no);
// void init(int argc, char **argv);
// char *receive_command(char *msg);

// Default values
int port_no = -1;
int daemon = 1;
char *strategy = "-";

// Command line options to handle:
// -h           (Print help text)
// -p port      (Listen to port number port.)
// -d           (Run as a daemon instead of as a normal program.)
// -s strategy  (Specify the request handling strategy : fork, muxbasic, or muxscale)

// Server should handle concurrent clients
void main(int argc, char *argv[])
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // char msg[255] = "This message is from the server\n";
    char msg[255];
    strcpy(msg, argv[2]);
    
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = atoi(argv[1]);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    listen(server_socket, 1);
    printf("Listening for clients...\n");
    int client_socket;
    while (client_socket = accept(server_socket, NULL, NULL)) {
        printf("Sending message to client");
        // Action
        send(client_socket, msg, sizeof(msg), 0);
        // Instead of send here, could be a call to fork
        
        close(client_socket);
    }

    return 0;
    // while (1) {}
    // init(argc, argv);
    // connect(0);
    // char *solution = receive_command("-");
    // printf("Sending solution: %s\n", solution);
}

// // Set values according to command line flags
// void init(int argc, char **argv)
// {
//     // set port_no, daemon, strategy if needed
//     if (daemon)
//     {
//         char *pid = "-";
//         printf("Daemon process ID: %s\n", pid);
//     }
// }

// // Connect with a client
// int connect(int port_no)
// {
//     char *client = "-";
//     printf("Connected with client %s\n", client);
//     return 0;
// }

// // Receive command from client and execute
// char *receive_command(char *msg)
// {
//     char *fname = "-", *client = "-";
//     printf("Client %s commanded:  %s\n", client, msg);
//     return fname;
// }
