#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

// TODO zombie processes
// TODO Handle arguments for kmeans and matinv
// TODO Execute kmeans and matinv
// TODO Send output file to client

void validate_args(int argc, char *argv[]);

// Filenames for output files, incrementing id
int client_num;
client_num = 0;
// int solution_num = 0;
// kmeans_client<i>_soln<j>.txt
// matinv_client<i>_soln<j>.txt

// Server should handle concurrent clients
int main(int argc, char *argv[])
{
    validate_args(argc, argv);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = atoi(argv[1]);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 1);
    printf("Listening for clients...\n");

    int client_socket;

    while (client_socket = accept(server_socket, NULL, NULL), client_socket)
    {
        client_num++;
        pid_t pid = fork();
        if (pid == 0) // child process
        {
            int thispid = getpid();
            printf("%d\n", thispid);
            int solution_num;
            solution_num = 0;
            while(1) {
                solution_num++;
                char msg[255];
                int err;
                err = recv(client_socket, msg, sizeof(msg), 0);
                if (err == 0) {
                    printf("Closing socket\n");
                    close(client_socket);
                    exit(0);
                } 
                // exec command, get filename of solution
                int status = system("ls");
                // kmeans(*, client_num, solution_num);
                char fname[255];
                sprintf(fname, "%s_client%d_soln%d.txt", msg, client_num, solution_num); // TODO remove newline later
                send(client_socket, fname, sizeof(fname), 0);
            }
        }
    }
    return 0;
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

    // Default values
    int port_no = -1;
    int d = 1;
    char *strategy = "fork";

    // Command line options to handle:
    // -h           (Print help text)
    // -p port      (Listen to port number port.)
    // -d           (Run as a daemon instead of as a normal program.)
    // -s strategy  (Specify the request handling strategy : fork, muxbasic, or muxscale)
    // TODO read daemon, ip, port
}
