#include <stdio.h>
#include <stdlib.h>

int connect(char *ip_addr, int port_no);
int command();
void help_text();
char *send(char *command);

// Command line options to handle:
// -p port
// -ip address

void main(int argc, char **argv)
{
    // while (1) {}
    connect("", 0);
    command();
}

// Connect to server
int connect(char *ip_addr, int port_no)
{
    printf("Connected to server\n");
    return 0;
}

// Take user input, send message, present solution
int command()
{
    char *solution = "-", *msg = "-";

    printf("Enter a command for the server: ");
    printf("\n");
    solution = send(msg);
    printf("Recieved the solution: %s\n", solution);
    return 0;
}

// Send a command to server
char *send(char *msg)
{
    return "-";
}
