#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <syslog.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/poll.h>

#define EXIT_NOT_IMPLEMENTED 3 /* Failing exit status for features not implemented */
#define BUF_SIZE 256
#define PATH_SIZE 1024

// Default values
int d = 0, port = -1;
enum Strategy{FORK, MUXBASIC, MUXSCALE};
enum Strategy strat = FORK; // Fork strategy as default.

// Forward declarations
int usage();
int parse_command(int sd, char command[]);
void read_options(int argc, char *argv[]);
void recv_file(int sd, char filename[]);
void send_file(FILE *fp, int sd);
void kmeans_run(int sd, char command[], size_t size);
void matinv_run(int sd, char command[]);
void run_with_fork();
void run_with_muxbasic();
void run_with_muxscale();
void run_as_daemon(const char *process_name);

// Filenames for output files, incrementing id
int client_num = 0,
    solution_num = 0;

// Declaring the command globally because most likely all of the functions will use this.
char cwd[PATH_SIZE] = {0}; // char command[PATH_SIZE] = {0};

int main(int argc, char *argv[])
{
    read_options(argc, argv);
    if (port == -1)
    {
        printf("Error: No port assigned.\n");
        usage(); // Exits program
    }

    // Mostly useful becuse daemonizing the process will change working dir to root.
    // Need to know the path to mathserver.
    getcwd(cwd, sizeof(cwd));

    if (d)
    {
        run_as_daemon("server");
    }

    // Clean up generated folders at exit?
    // atexit(cleanup_results);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);

    switch (strat) 
    {
        case FORK:
            run_with_fork();
            break;
        case MUXBASIC:
            run_with_muxbasic();
            break;
        case MUXSCALE:
            run_with_muxscale();
            break;
    }

    return 0;
}

/*
 * Receive data file for kmeans from client.
 * Save it in computed_results/client<client_num>/.
 */
void recv_file(int sd, char filename[])
{
    // Open file with append. "a" functions as O_CREAT | O_WRONLY | O_APPEND
    FILE *fp = fopen(filename, "a");
    if (fp == NULL)
    {
        printf("Error opening file.\n");
        exit(EXIT_FAILURE);
    }

    char recvbuf[BUF_SIZE] = {0};
    memset(recvbuf, 0, BUF_SIZE);
    int recv_bytes; // How many bytes are recieved by call to recv().
    while (1)
    {
        if ((recv_bytes = recv(sd, recvbuf, sizeof(recvbuf), 0)) == -1)
        {
            perror("Error recieving file.");
        }
        else if (strstr(recvbuf, "\nOutput End\n") != NULL) { break; }
        else
        {
            // Writes recv_bytes number of bytes from recvbuf to file.
            fwrite(recvbuf, sizeof(char), recv_bytes, fp);
        }
    }
    fclose(fp);
}

/*
 * Send file to client.
 */
void send_file(FILE* fp, int sd)
{
    char output[BUF_SIZE] = "";
    memset(output, 0, BUF_SIZE);
    while (fgets(output, BUF_SIZE, fp) != NULL)
    {
        if ((send(sd, output, strlen(output), 0)) == -1)
        {
            perror("Error sending file.");
            break;
        }
        memset(output, 0, BUF_SIZE);
    }
    if ((send(sd, "\nOutput End\n", strlen("\nOutput End\n"), 0)) == -1)
    {
        if (errno == EPIPE)
            close(sd);
        else 
            perror("Error sending FIN command");
        exit(EXIT_FAILURE);
    }
}

/*
 * Parse kmeans command for the "-f" flag.
 * If it is set, return 1.
 */
int parse_command(int sd, char command[])
{
    char *ptr = strtok(command, " ");
    while (ptr != NULL)
    {
        if (strcmp(ptr, "-f") == 0)
        {
            return 1;
        }
        ptr = strtok(NULL, " ");
    }
    return 0;
}

void kmeans_run(int sd, char command[], size_t size)
{
    // Path to directory for client results
    char path[PATH_SIZE];
    strncpy(path, cwd, sizeof(path));
    strncat(path, "/../computed_results/", strlen(path) - sizeof(path));
    char client[10];
    snprintf(client, sizeof(client), "client%d", client_num);
    strncat(path, client, strlen(path) - sizeof(path));

    // Create client dir if not exist
    struct stat st = {0};
    if (stat(path, &st) == -1)
    {
        // umask(0000);
        mkdir(path, 0777);
    }

    // TODO: Works without -f, with -f program gets stuck, input file empty

    // Get input file
    int inp = parse_command(sd, command);
    if (inp == 1)
    {
        char input_path[PATH_SIZE];
        snprintf(input_path, sizeof(input_path), "%s/input.txt", path);
        printf("Receiving input file for kmeans: %s\n", input_path); // debug
        recv_file(sd, input_path);
        strncat(command, " -f ", size - strlen(command));
        strncat(command, input_path, size - strlen(command));
    }

    // Concat path with results filename
    char solution_str[20];
    snprintf(solution_str, sizeof(solution_str), "/%d.txt", solution_num);
    strncat(path, solution_str, strlen(path) - sizeof(path));
    strncat(command, " -p ", size - strlen(command));
    strncat(command, path, size - strlen(command));

    // Execute kmeans
    FILE *fp = popen(command, "r");
    pclose(fp); // pclose will block until the process opened by popen terminates.

    // Open results and send data to client.
    FILE *result = fopen(path, "r");
    if (result == NULL)
    {
        perror("Error opening file.");
        exit(EXIT_FAILURE);
    }
    send_file(result, sd);
    fclose(result);
}

void matinv_run(int sd, char command[])
{
    // I just realized that giving popen direct user input is super unsafe, but this isn't a course on infosec so... Don't abuse?
    FILE *fp = popen(command, "r");
    send_file(fp, sd);
    pclose(fp);

}

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
        exit(EXIT_FAILURE);
    }

    /* STEP 2a: Fork a child process */
    if ((pid = fork()) < 0)
    {
        perror(process_name);
        exit(EXIT_FAILURE);
    }
    else if (pid != 0)
    { /* STEP 2b: Exit the parent process */
        exit(EXIT_SUCCESS);
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
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0)
    {
        perror("Can't fork");
        exit(EXIT_FAILURE);
    }
    else if (pid != 0) /* parent */
        exit(EXIT_SUCCESS);

    /* Change the current working directory to the root so
     * we won't prevent file systems from being unmounted. */
    if (chdir("/") < 0)
    {
        perror("Can't change to /");
        exit(EXIT_FAILURE);
    }
    printf("%d\n", getpid());

    /* Close all open file descriptors */
    // printf("limit: %ld\n", rl.rlim_max);
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
        exit(EXIT_FAILURE);
    }
}

void run_with_fork()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        printf("Socket creation failed.\n");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
    {
        printf("Socket bind failed.\n");
        exit(EXIT_FAILURE);
    }

    if ((listen(server_socket, 1)) != 0)
    {
        printf("Listen to socket failed.\n");
        exit(EXIT_FAILURE);
    }
    printf("Listening for clients...\n");

    int client_socket;
    while (client_socket = accept(server_socket, NULL, NULL), client_socket)
    {
        client_num++;
        pid_t pid = fork();

        if (pid == 0) // Child process
        {
            printf("Connected with client %d\n", client_num);
            solution_num = 0;

            while (1)
            {
                solution_num++;
                char msg[BUF_SIZE];
                int err = recv(client_socket, msg, sizeof(msg), 0);
                if (err == 0 || err == -1)
                {
                    printf("Closing socket\n");
                    close(client_socket);
                    exit(EXIT_SUCCESS);
                }

                printf("Client %d commanded: %s\n", client_num, msg);

                char cmd[7]; // "kmeans" or "matinv"
                snprintf(cmd, sizeof(cmd), "%.6s", msg); // 6 first chars (7? Add space?)

                if (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0) // Add space after? "matinvv"
                {
                    char error[] = "Error! Valid commands: 'matinv' or 'kmeans'";
                    send(client_socket, error, sizeof(error), 0);
                    close(client_socket);
                    exit(EXIT_FAILURE);
                }

                // Generate filename
                char data[30];
                snprintf(data, sizeof(data), "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
                printf("Sending solution: %s\n", data);
                // Why +1?
                send(client_socket, data, strlen(data) + 1, 0); // Send solution filename to client

                // Build command string
                char delimiter = '/';
                char command[PATH_SIZE];
                strncpy(command, cwd, sizeof(command));
                strncat(command, &delimiter, 1);
                strcat(command, msg);

                // Run kmeans_run or matinv_run based on msg.
                if (strcmp(cmd, "kmeans") == 0)
                {
                    kmeans_run(client_socket, command, sizeof(command));
                }
                else if (strcmp(cmd, "matinv") == 0)
                {
                    matinv_run(client_socket, command);
                }

            }
        }
    }
}

// TODO: For grade B, "-s muxbasic"
// https://www.ibm.com/docs/en/i/7.2?topic=designs-using-poll-instead-select
void run_with_muxbasic()
{
    printf("Muxbasic not implemented\n");
    exit(EXIT_NOT_IMPLEMENTED);
    /*
    int len, rc, on = 1;
    int listen_sock = -1;
    int new_sock = -1;
    int desc_ready, end_server = 0, compress_array = 0;
    int close_conn;
    char buffer[80];

    struct sockaddr_in addr;

    int timeout;

    struct pollfd fds[200];

    int nfds = 1, current_size = 0, i, j;

    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0){
        perror("Socket failed.\n");
        exit(EXIT_FAILURE);
    }

    // Sets socket to be reusable
    rc = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
    if (rc < 0) {
        perror("Set socket options failed.\n");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    rc = ioctl(listen_sock, FIONBIO, (char *) &on);
    if (rc < 0) {
        perror("IO control failed.\n");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    rc = bind(listen_sock, (struct sockaddr *) &addr, sizeof(addr));
    if (rc < 0) {
        perror("Bind failed.\n");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    // Listen to descriptor and set queue to 16.
    rc = listen(listen_sock, 16);
    if (rc < 0) {
        perror("Listen failed.");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    memset(fds, 0, sizeof(fds));

    fds[0].fd = listen_sock;
    fds[0].events = POLLIN;

    // Do we need timeout???
    timeout = (5 * 60 * 1000);

    do
    {
        printf("Polling for client...\n");

        rc = poll(fds, nfds, timeout);

        if (rc < 0) {
            perror("Poll failed.");
            break;
        }

        if (rc == 0) {
            printf("Poll timed out. \n");
            break;
        }

        current_size = nfds;
        for (i = 0; i < current_size; i++) {
            if (fds[i].revents == 0) {
                continue;
            }
            if (fds[i].revents != POLLIN) {
                printf("Error, revents.\n");
                end_server = 1;
                break;
            }
            if (fds[i].fd == listen_sock) {
                do
                {
                    new_sock = accept(listen_sock, NULL, NULL);

                    // This error-catch should exclude EWOULDBLOCK since
                    // if this error is encountered we've accepted all of the descriptors in queue.
                    // Any other error should cause the server to exit.
                    if (new_sock < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("Accept failed.");
                            end_server = 1;
                        }
                        break;
                    }
                    printf("New connection.\n");
                    fds[nfds].fd = new_sock;
                    fds[nfds].events = POLLIN;
                    nfds++;
                } while (new_sock != -1);
            }
            else {
                // If we're in this else statement, it means this is not the listening socket.
                printf("This descriptor is readable: %d\n", fds[i].fd);
                close_conn = 0;
                solution_num = 0;

                do
                {
                    // Here we do the work we want to perform.
                // ###############################
                    solution_num++;
                    char msg[255];
                    rc = recv(fds[i].fd, msg, sizeof(msg), 0);
                    if (rc < 0) {
                        if (errno != EWOULDBLOCK) {
                            perror("Recv failed. Client closed.");
                            close_conn = 1;
                        }
                        break;
                    }

                    printf("Client %d commanded: %s\n", client_num, msg);

                    char cmd[7];
                    snprintf(cmd, sizeof(cmd), "%.6s", msg); // 6 first chars

                    // Generate filename
                    char data[30];
                    snprintf(data, sizeof(data), "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
                    printf("Sending solution: %s\n", data);
                    send(fds[i].fd, data, strlen(data) + 1, 0);

                    // Append file separator to path.
                    char delimiter = '/';

                    char command[1024];
                    strncpy(command, cwd, sizeof(command));
                    strncat(command, &delimiter, 1);
                    strcat(command, msg);

                    // Here, either run kmeans_run or matinv_run based on msg.
                    if (strcmp(cmd, "kmeans") == 0)
                    {
                        kmeans_run(fds[i].fd, command);
                    }
                    else if (strcmp(cmd, "matinv") == 0)
                    {
                        matinv_run(fds[i].fd, command);
                    }

                // ###############################
                } while (1);

                if (close_conn) {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = 1;
                }
            }
        }

        if (compress_array) {
            compress_array = 0;
            for (i = 0; i < nfds; i++) {
                if (fds[i].fd == -1)
                {
                    for (j = i; j < nfds; j++)
                    {
                        fds[j].fd = fds[j+1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }
    } while (end_server == 0);

    for (i = 0; i < nfds; i++) {
        if (fds[i].fd >= 0)
        {
            close(fds[i].fd);
        }
    }
    */
}

// TODO: For grade A, "-s muxscale"
void run_with_muxscale()
{
    printf("Muxscale not implemented\n");
    exit(EXIT_NOT_IMPLEMENTED);
}

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
                    d = 1;
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
                        printf("%s: ignored option: -s %s\n", prog, argv[i]);
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
    printf("              [-d]            run as daemon\n");
    printf("              [-s strategy]   specify the request handling strategy (fork/muxbasic/muxscale)\n");
    printf("              [-h]            help\n");
    exit(EXIT_SUCCESS);
}
