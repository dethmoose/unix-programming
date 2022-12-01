// #include <sys/resource.h>
// #include <sys/socket.h>
// #include <sys/time.h>
// #include <syslog.h>
// #include <netinet/in.h>
// #include <errno.h>
// #include <sys/ioctl.h>
// #include <sys/poll.h>
// #include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/file_util.h"
#include "../include/server_util.h"

// Default values
int d = 0, port = -1;
enum Strategy strat = FORK;

// Declaring the command globally because most likely all of the functions will use this.
char cwd[PATH_SIZE];

// Forward declarations
void usage();
void read_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    read_options(argc, argv);
    if (port == -1)
    {
        printf("Error: No port assigned.\n");
        usage();
        exit(EXIT_FAILURE);
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
        run_with_fork(port, cwd);
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

void read_options(int argc, char *argv[])
{
    char *prog, *value;
    prog = *argv;
    if (argc < 2)
    {
        usage();
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++)
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
                exit(EXIT_SUCCESS);
                break;

            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                printf("HELP: try %s -h \n\n", prog);
                break;
            }
        }
    }
}

void usage()
{
    printf("\nUsage: server [-p port]\n");
    printf("              [-d]            run as daemon\n");
    printf("              [-s strategy]   specify the request handling strategy (fork/muxbasic/muxscale)\n");
    printf("              [-h]            help\n");
}
