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

// Default values
int port = -1;
int d = 0;
char *strat = "fork";

int usage();
void read_options(int argc, char *argv[]);

// Filenames for output files, incrementing id
int client_num;
client_num = 0;
// int solution_num = 0;
// kmeans_client<i>_soln<j>.txt
// matinv_client<i>_soln<j>.txt

// Server should handle concurrent clients
int main(int argc, char *argv[])
{
    if (argc < 2)
        usage();
    read_options(argc, argv);
    // printf("Port number: %d, daemon: %d, strategy: %s", port, d, strat);

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
            while (1)
            {
                solution_num++;
                char msg[255];
                int err;
                err = recv(client_socket, msg, sizeof(msg), 0);
                if (err == 0)
                {
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
            case 'd':
                --argc;
                d = atoi(*++argv);
                break;

            case 'p':
                --argc;
                port = atoi(*++argv);
                break;

            case 's':
                --argc;
                strat = *++argv;
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
    printf("\nUsage: server [-p port]\n");
    printf("              [-d]            run as daemon \n");
    printf("              [-s strategy]   specify the request handling strategy : fork/muxbasic/muxscale \n");
    printf("              [-h]            help \n");
    exit(1);
}
