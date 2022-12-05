/*
 * Util functions for a mathserver computing matrix inverse and kmeans clustering,
 * with process strategies (e.g. forking), and/or running as daemon.
 */

#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include "../include/server_util.h"
#include "../include/file_util.h"

/*
 * Execute kmeans
 */
void kmeans_run(int sd, char command[], char cwd[], int client_num, int solution_num)
{
    // Path to directory for client results
    char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "%s/../computed_results/client%d/", cwd, client_num);

    // Create client dir if not exist
    struct stat st = {0};
    if (stat(path, &st) == -1)
    {
        mkdir(path, 0777);
    }

    // Get input file if necessary
    int inp = has_f_flag(command);
    if (inp == 1)
    {
        char input_path[PATH_SIZE];
        snprintf(input_path, PATH_SIZE, "%s/input.txt", path);
        recv_file(sd, input_path);
        strncat(command, " -f ", PATH_SIZE - strlen(command));
        strncat(command, input_path, PATH_SIZE - strlen(command));
    }

    // Concat path with results filename
    char solution_str[20];
    snprintf(solution_str, sizeof(solution_str), "/%d.txt", solution_num);
    strncat(path, solution_str, PATH_SIZE - strlen(path));

    // Concat path command to the command.
    strncat(command, " -p ", PATH_SIZE - strlen(command));
    strncat(command, path, PATH_SIZE - strlen(command));

    // Execute kmeans
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("Cannot start program");
        exit(EXIT_FAILURE);
    }
    pclose(fp); // pclose will block until the process opened by popen terminates.
    send_file(sd, path);
}

/*
 * Execute matinv
 */
void matinv_run(int sd, char command[], char cwd[], int client_num, int solution_num)
{
    // Path to directory for client results
    char path[PATH_SIZE];
    snprintf(path, PATH_SIZE, "%s/../computed_results/client%d/", cwd, client_num);

    // Create client dir if not exist
    struct stat st = {0};
    if (stat(path, &st) == -1)
    {
        mkdir(path, 0777);
    }

    // Concat path with results filename
    char solution_str[20];
    snprintf(solution_str, sizeof(solution_str), "/%d.txt", solution_num);
    strncat(path, solution_str, PATH_SIZE - strlen(path));
    strncat(command, " -p ", PATH_SIZE - strlen(command));
    strncat(command, path, PATH_SIZE - strlen(command));

    // Execute matinv
    FILE *fp = popen(command, "r");
    if (fp == NULL)
    {
        perror("Cannot start program");
        exit(EXIT_FAILURE);
    }

    // Save generated output to a results file
    FILE *result_fp = fopen(path, "w");
    if (result_fp == NULL)
    {
        perror("Error creating matinv results file");
        exit(EXIT_FAILURE);
    }

    // Send results file to client
    char buf[BUF_SIZE];
    while (fgets(buf, BUF_SIZE, fp) != NULL)
    {
        fprintf(result_fp, "%s", buf);
    }
    fclose(result_fp);
    pclose(fp);
    send_file(sd, path);
}

/*
 * Run process in the background.
 * Source: Advanced Programming in the UNIX® Environment: Second Edition - Stevens & Rago
 */
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

/*
 * Handle concurrent clients by forking the server process.
 */
void run_with_fork(int port, char cwd[])
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))) != 0)
    {
        perror("Socket bind failed");
        exit(EXIT_FAILURE);
    }

    if ((listen(server_socket, 1)) != 0)
    {
        perror("Listen to socket failed.");
        exit(EXIT_FAILURE);
    }
    printf("Listening for clients...\n");

    int client_num = 0, solution_num = 0;
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
                if (err < 1)
                {
                    printf("Closing socket\n");
                    close(client_socket);
                    exit(EXIT_SUCCESS);
                }

                char cmd[7];                             // "kmeans" or "matinv"
                snprintf(cmd, sizeof(cmd), "%.6s", msg); // TODO: 6 first chars (7? Include next char? What if "matinvv"?)
                printf("Client %d commanded: %s\n", client_num, msg);

                if (strcmp(cmd, "matinv") != 0 && strcmp(cmd, "kmeans") != 0)
                {
                    // Send error message to client
                    char error[] = "Error! Valid commands: 'matinv' or 'kmeans'";
                    send(client_socket, error, sizeof(error), 0);
                    close(client_socket);
                    exit(EXIT_FAILURE);
                }

                // Generate solution filename
                char data[30];
                snprintf(data, sizeof(data), "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
                printf("Sending solution: %s\n", data);
                send(client_socket, data, strlen(data), 0); // Send solution filename to client 

                // Build command string
                char delimiter = '/';
                char command[PATH_SIZE];
                strncpy(command, cwd, sizeof(command));
                strncat(command, &delimiter, 1);
                strcat(command, msg);

                // Run kmeans_run or matinv_run based on msg.
                if (strcmp(cmd, "kmeans") == 0)
                {
                    kmeans_run(client_socket, command, cwd, client_num, solution_num);
                }
                else if (strcmp(cmd, "matinv") == 0)
                {
                    matinv_run(client_socket, command, cwd, client_num, solution_num);
                }
            }
        }
    }
}

// TODO: For grade B, "-s muxbasic"
// Source: IBM, <https://www.ibm.com/docs/en/i/7.2?topic=designs-using-poll-instead-select>
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
    if (listen_sock < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Sets socket to be reusable
    rc = setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
    if (rc < 0)
    {
        perror("Set socket options failed.\n");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    // Set to nonblocking
    rc = ioctl(listen_sock, FIONBIO, (char *)&on);
    if (rc < 0)
    {
        perror("IO control failed");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    rc = bind(listen_sock, (struct sockaddr *)&addr, sizeof(addr));
    if (rc < 0)
    {
        perror("Bind failed");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    // Listen to descriptor and set queue to 16.
    rc = listen(listen_sock, 16);
    if (rc < 0)
    {
        perror("Listen failed");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    memset(fds, 0, sizeof(fds));
    fds[0].fd = listen_sock;
    fds[0].events = POLLIN;

    // The program will end if no activity for 5 minutes
    timeout = (5 * 60 * 1000); // TODO: Do we need timeout??? Probably not

    do
    {
        printf("Polling for client...\n");

        rc = poll(fds, nfds, timeout);

        if (rc < 0)
        {
            perror("Poll failed");
            break;
        }

        if (rc == 0)
        {
            printf("Poll timed out.\n");
            break;
        }

        current_size = nfds;
        for (i = 0; i < current_size; i++)
        {
            if (fds[i].revents == 0)
            {
                continue;
            }
            if (fds[i].revents != POLLIN)
            {
                printf("Error, revents.\n");
                end_server = 1;
                break;
            }
            if (fds[i].fd == listen_sock)
            {
                do
                {
                    new_sock = accept(listen_sock, NULL, NULL);

                    // This error-catch should exclude EWOULDBLOCK since if this error
                    // is encountered we've accepted all of the descriptors in queue.
                    // Any other error should cause the server to exit.
                    if (new_sock < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
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
            else
            {
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
                    if (rc < 0)
                    {
                        if (errno != EWOULDBLOCK)
                        {
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

                if (close_conn)
                {
                    close(fds[i].fd);
                    fds[i].fd = -1;
                    compress_array = 1;
                }
            }
        }

        if (compress_array)
        {
            compress_array = 0;
            for (i = 0; i < nfds; i++)
            {
                if (fds[i].fd == -1)
                {
                    for (j = i; j < nfds; j++)
                    {
                        fds[j].fd = fds[j + 1].fd;
                    }
                    i--;
                    nfds--;
                }
            }
        }
    } while (end_server == 0);

    // Clean up open sockets
    for (i = 0; i < nfds; i++)
    {
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
