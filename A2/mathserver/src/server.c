#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>

void validate_args(int argc, char *argv[]);
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
int main(int argc, char *argv[])
{
    while (1) // only runs once
    {
        validate_args(argc, argv);
        int server_socket = socket(AF_INET, SOCK_STREAM, 0);

        // char msg[255] = "This message is from the server\n";
        // char msg[255];
        // strcpy(msg, argv[2]);

        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_port = atoi(argv[1]);
        server_address.sin_addr.s_addr = INADDR_ANY;

        bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
        listen(server_socket, 4096);
        printf("Listening for clients...\n");

        int client_socket;
        while (client_socket = accept(server_socket, NULL, NULL))
        {
            while (1)
            {
                char msg[255];
                recv(client_socket, msg, sizeof(msg), 0);
                send(client_socket, msg, sizeof(msg), 0);
                // Instead of send here, could be a call to fork
                // close(client_socket);
            }
        }
        // when client exits, server exits
    }
    return 0;

    // init(argc, argv);
    // connect(0);
    // char *solution = receive_command("-");
    // printf("Sending solution: %s\n", solution);
}

void validate_args(int argc, char *argv[])
{
    // printf("Validating args (%d): ", argc);
    // for (int i = 0; i < argc; i++)
    // {
    //     printf("'%s' ", argv[i]);
    // }
    // printf("\n");
    if (argc < 2)
    {
        printf("Error\n");
        exit(1);
    }
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
