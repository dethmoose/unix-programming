#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>

// TODO zombie processes
// TODO Handle arguments for kmeans and matinv
// TODO Send output file to client (or just filename and data and then client creates file)

// Default values
int port = -1;
int d = 0;
char *strat = "fork";

int usage();
void read_options(int argc, char *argv[]);
char *validate_command(char command[]);

// Filenames for output files, incrementing id
int client_num;
client_num = 0;
// int solution_num = 0;
// kmeans_client<i>_soln<j>.txt
// matinv_client<i>_soln<j>.txt

int main(int argc, char *argv[])
{
    if (argc < 2)
        usage();
    read_options(argc, argv);
    // printf("Port number: %d, daemon: %d, strategy: %s\n", port, d, strat);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
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
            // int thispid = getpid();
            // printf("pid: %d\n", thispid);
            int solution_num;
            solution_num = 0;
            while (1)
            {
                solution_num++;
                char msg[255];
                int err;
                err = recv(client_socket, msg, sizeof(msg), 0);
                if (err == 0 || err == -1)
                {
                    printf("Closing socket\n");
                    close(client_socket);
                    exit(0);
                }

                // char *cmd = validate_command(msg); // TODO validate function
                char cmd[7];
                snprintf(cmd, sizeof(cmd), "%.6s", msg); // 6 first chars

                if (strlen(msg) < 6 || (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0))
                {
                    char error[] = "Error! Valid commands: 'matinv' or 'kmeans'";
                    send(client_socket, error, sizeof(error), 0);
                    close(client_socket);
                    exit(0);
                }

                // exec command, get filename of solution
                // kmeans(*, client_num, solution_num);

                char path[257] = "./";
                strcat(path, msg);
                int status = system(path); // execute e.g."./matinv"

                // returned -1 even when successful?
                // if (status == -1)
                // {
                //     send(client_socket, "error", 6, 0);
                //     close(client_socket);
                //     exit(0);
                // }

                // TODO: get output from matinv/kmeans as a string, add it to `fname`
                // so that the solution is sent in the form of "filename\nfiledata" to client,
                // then client can create a file with that filename and data under computed_results

                char fname[255];
                sprintf(fname, "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
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

// could not get this to work as a function
char *validate_command(char command[])
{
    char cmd[7];
    snprintf(cmd, sizeof(cmd), "%.6s", command); // 6 first chars of command
    printf("First word: %s\n", cmd);

    if (strlen(command) < 6 || (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0))
    {
        printf("Valid commands: 'matinv' or 'kmeans'\n");
        return "";
    }
    printf("validate_command: returning '%s'\n", cmd);
    return cmd;
}
