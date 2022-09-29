#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

// TODO popen() instead of system()
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
int execute_command(char command[], char program[], char data[], int size_d);

// Filenames for output files, incrementing id
int client_num = 0, solution_num = 0;
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
            printf("Connected with client %d\n", client_num);
            // int thispid = getpid();
            // printf("pid: %d\n", thispid);
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

                printf("Client %d commanded: %s\n", client_num, msg);

                char cmd[7];
                // cmd = validate_command(msg); // TODO validate function

                // validate command
                snprintf(cmd, sizeof(cmd), "%.6s", msg); // 6 first chars

                if (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0)
                {
                    char error[] = "Error! Valid commands: 'matinv' or 'kmeans'";
                    send(client_socket, error, sizeof(error), 0);
                    close(client_socket);
                    exit(0);
                }

                char data[30];

                char command[30] = "./";
                strcat(command, cmd);

                char output[255] = "";
                memset(output, 0, sizeof(output));

                FILE* fp = popen(command, "r");

                while (fgets(output, sizeof(output), fp) != NULL) 
                {
                    printf(output);
                    if ((send(client_socket, output, strlen(output), 0)) == -1) {
                        printf(errno);
                    }
                    memset(output, 0, sizeof(output));
                }
                pclose(fp);

                if ((send(client_socket, "\nOutput End\n", sizeof("\nOutput End\n"), 0)) == -1) {
                    printf(errno);
                }


                snprintf(data, sizeof(data), "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
                printf("Sending solution: %s\n", data);
                send(client_socket, data, strlen(data) + 1, 0);
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

    if (!(strat == "fork" || strat == "muxbasic" || strat == "muxscale"))
    {
        printf("Ignoring -s %s\n", strat);
        strat = "fork";
    }

    if (!(d == 0 || d == 1))
    {
        printf("Ignoring -d %d\n", d);
        d = 0;
    }
}

int usage()
{
    printf("\nUsage: server [-p port]\n");
    printf("              [-d]            run as daemon (0/1)\n");
    printf("              [-s strategy]   specify the request handling strategy (fork/muxbasic/muxscale)\n");
    printf("              [-h]            help\n");
    exit(1);
}

// could not get this to work as a function
char *validate_command(char command[])
{
    char cmd[7];
    sprintf(cmd, 7, "%.6s", command); // 6 first chars of command

    if (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0)
    {
        printf("Valid commands: 'matinv' or 'kmeans'\n"); // TODO should be shown to client instead
        return "";
    }
    return cmd;
}
