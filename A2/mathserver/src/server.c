#include <stdio.h>
#include <stdlib.h>

int connect(int port_no);
void init(int argc, char **argv);
char *receive_command(char *msg);

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
void main(int argc, char **argv)
{
    // while (1) {}
    init(argc, argv);
    printf("Listening for clients...\n");
    connect(0);
    char *solution = receive_command("-");
    printf("Sending solution: %s\n", solution);
}

// Set values according to command line flags
void init(int argc, char **argv)
{
    // set port_no, daemon, strategy if needed
    if (daemon)
    {
        char *pid = "-";
        printf("Daemon process ID: %s\n", pid);
    }
}

// Connect with a client
int connect(int port_no)
{
    char *client = "-";
    printf("Connected with client %s\n", client);
    return 0;
}

// Receive command from client and execute
char *receive_command(char *msg)
{
    char *fname = "-", *client = "-";
    printf("Client %s commanded:  %s\n", client, msg);
    return fname;
}
