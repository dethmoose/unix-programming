#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

// TODO Handle arguments for kmeans and matinv
// TODO Send output file to client (or just filename and data and then client creates file)

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

    if (argc < 2)
        usage();
    read_options(argc, argv);
    
    if (d) {
        // TODO
        printf("Running as daemon\n");
        printf("PID: %d\n", getpid());
    }

    printf("Strategy: %d\n", strat);
    // printf("Port number: %d, daemon: %d, strategy: %s\n", port, d, strat);

    signal(SIGPIPE, SIG_IGN);
    signal(SIGCHLD, SIG_IGN);
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        printf("Socket creation failed.\n");
        exit(1);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if ((bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        printf("Socket bind failed.\n");
        exit(1);
    }

    if ((listen(server_socket, 1)) != 0){
        printf("Listen to socket failed.\n");
        exit(1);
    }
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

                // Generate filename
                char data[30];
                snprintf(data, sizeof(data), "%s_client%d_soln%d.txt", cmd, client_num, solution_num);
                printf("Sending solution: %s\n", data);
                send(client_socket, data, strlen(data) + 1, 0);

                
                char command[30] = "./";
                strcat(command, cmd);

                // Start program. Different functions for kmeans and matinv?
                // Might be necessary to get the results from kmeans? matinv prints to stdout.
                char output[255] = "";
                memset(output, 0, sizeof(output));

                FILE* fp = popen(command, "r");
                while (fgets(output, sizeof(output), fp) != NULL) 
                {
                    printf(output);
                    if ((send(client_socket, output, strlen(output), 0)) == -1) {
                        perror("Error sending command output.\n");
                    }
                    memset(output, 0, sizeof(output));
                }
                pclose(fp);

                if ((send(client_socket, "\nOutput End\n", strlen("\nOutput End\n"), 0)) == -1) {
                    printf(errno);
                }


                
            }
        }
    }
    return 0;
}

// TODO: For grade C 
// void run_as_daemon(){

// }

// void run_with_fork(){

// }

// TODO: For grade B, "-s muxbasic"
// void run_with_muxbasic(){

// }

// TODO: For grade A, "-s muxscale"
// void run_with_muxscale() {

// }

void read_options(int argc, char *argv[])
{
    char *prog;
    prog = *argv;

    int i = 1;
    for (i; i < argc; i++)
    {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) 
            {
            case 'd':
                d = 1;
                break;
            case 'p':
                port = atoi(argv[++i]);
                break;
            case 's':
                if (strcmp(argv[i+1], "fork") == 0){
                    strat = FORK;
                }
                else if (strcmp(argv[i+1], "muxbasic") == 0){
                    strat = MUXBASIC;
                }
                else if (strcmp(argv[i+1], "muxscale") == 0){
                    strat = MUXSCALE;
                }
                else {
                    printf("%s: ignored option: -%s\n", prog, argv[i]);
                }
                i++;
                break;
            case 'h':
            case 'u':
                usage();
                break;

            default:
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
