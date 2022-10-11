#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <syslog.h> // LOG_CONS, LOG_DAEMON, LOG_ERR
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

// TODO Handle arguments for kmeans and matinv

// TODO result files are empty when running as daemon

// Default values
int port = 1337;
int d = 0;
enum Strategy{FORK, MUXBASIC, MUXSCALE};
enum Strategy strat = FORK; // Fork strategy as default.

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
    read_options(argc, argv);
    // printf("Strategy: %d\n", strat);
    // printf("Port number: %d, daemon: %d, strategy: %d\n", port, d, strat);

    if (d)
    {
        printf("Running as daemon\n");
        run_as_daemon("server");
    }

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
    {
        printf("Socket bind failed.\n");
        exit(1);
    }

    if ((listen(server_socket, 1)) != 0)
    {
        printf("Listen to socket failed.\n");
        exit(1);
    }
    printf("Listening for clients...\n");

    int client_socket;
    while (client_socket = accept(server_socket, NULL, NULL), client_socket)
    {
        client_num++;
        pid_t pid = fork();
        // printf("PID: %d\n", getpid());

        if (pid == 0) // child process
        {
            printf("Connected with client %d\n", client_num);
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
                // start recieving client data for kmeans. save as filename kmeans-results_clientnum_solnum.txt

                // validate command
                snprintf(cmd, sizeof(cmd), "%.6s", msg); // 6 first chars

                if (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0)
                {
                    char error[] = "Error! Valid commands: 'matinv' or 'kmeans'";
                    send(client_socket, error, sizeof(error), 0);
                    close(client_socket);
                    exit(0);
                }

                // Generate filename
                char data[30];
                snprintf(data, sizeof(data), "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
                printf("Sending solution: %s\n", data);
                send(client_socket, data, strlen(data) + 1, 0);

                char command[255] = "./";
                strcat(command, msg);

                // Start program.
                // Might be necessary to get the results from kmeans? matinv prints to stdout.
                char output[255] = "";
                memset(output, 0, sizeof(output));

                FILE *fp = popen(command, "r");
                while (fgets(output, sizeof(output), fp) != NULL)
                {
                    // printf(output);
                    if ((send(client_socket, output, strlen(output), 0)) == -1)
                    {
                        perror("Error sending command output.\n");
                    }
                    memset(output, 0, sizeof(output));
                }
                pclose(fp);

                if ((send(client_socket, "\nOutput End\n", strlen("\nOutput End\n"), 0)) == -1)
                {
                    printf(errno);
                }
            }
        }
    }
    return 0;
}

// void run_with_fork()
// {

// }

// Code by Advanced Programming in the UNIX® Environment: Second Edition - Stevens & Rago
void run_as_daemon(const char *process_name)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    struct sigaction sa;

    /* STEP 1: Clear file creation mask */
    umask(0);

    /* Get maximum number of file descriptors */
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        perror(process_name);
        exit(1);
    }

    /* STEP 2a: Fork a child process */
    if ((pid = fork()) < 0)
    {
        perror(process_name);
        exit(1);
    }
    else if (pid != 0)
    { /* STEP 2b: Exit the parent process */
        exit(0);
    }
    /* STEP 3: Become a session leader to lose controlling TTY
     * The child process executes this! */
    setsid();

    /* Ensure future opens won't allocate controlling TTYs */
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0)
    {
        perror("Can't ignore SIGHUP");
        exit(1);
    }

    if ((pid = fork()) < 0)
    {
        perror("Can't fork");
        exit(1);
    }
    else if (pid != 0) /* parent */
        exit(0);

    /* Change the current working directory to the root so
     * we won't prevent file systems from being unmounted. */
    if (chdir("/") < 0)
    {
        perror("Can't change to /");
        exit(1);
    }

    /* Close all open file descriptors */
    printf("limit: %ld\n", rl.rlim_max);
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);

    /* Attach file descriptors 0, 1, and 2 to /dev/null */
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    /* Initialize the log file. */
    openlog(process_name, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2)
    {
        syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
               fd0, fd1, fd2);
        exit(1);
    }
}

// TODO: For grade B, "-s muxbasic"
// void run_with_muxbasic(){

// }

// TODO: For grade A, "-s muxscale"
// void run_with_muxscale() {

// }

void read_options(int argc, char *argv[])
{
    if (argc < 2)
        usage();

    char *prog, *value;
    prog = *argv;

    int i = 1;
    for (i; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'd':
                value = argv[++i];
                if (strcmp(value, "1") == 0)
                {
                    d = 1;
                }
                break;
            case 'p':
                port = atoi(argv[++i]);
                break;
            case 's':
                value = argv[++i];
                if (strcmp(value, "fork") == 0)
                {
                    strat = FORK;
                }
                else if (strcmp(value, "muxbasic") == 0)
                {
                    strat = MUXBASIC;
                }
                else if (strcmp(value, "muxscale") == 0)
                {
                    strat = MUXSCALE;
                }
                else
                {
                    printf("%s: ignored option: -%s\n", prog, argv[i]);
                }
                i++;
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
