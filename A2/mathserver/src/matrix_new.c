/***************************************************************************
 *
 * Sequential version of Matrix Inverse
 * An adapted version of the code by Håkan Grahn
 *
 ***************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_SIZE 4096

typedef double matrix[MAX_SIZE][MAX_SIZE];
pthread_barrier_t barrier;
struct threadArgs
{
    unsigned int id;
    int multiplier;
    int row;
};

int N, PRINT, maxnum; // matrix size, print switch, max number of element
char *Init;           // matrix init type
matrix A;             // matrix A
matrix I = {{0.0}};   // the A inverse matrix, which will be initialized to the identity matrix

// forward declarations
void find_inverse(void);
void init_matrix(void);
void print_matrix(matrix M, char name[]);
void init_default(void);
void *multiply_columns(void *params);
int read_options(int, char *[]);

int main(int argc, char *argv[])
{
    if (PRINT == 1)
    {
        printf("Matrix Inverse\n");
    }
    // int i, timestart, timeend, iter;

    init_default();
    read_options(argc, argv);
    init_matrix();
    find_inverse();

    if (PRINT == 1)
    {
        // print_matrix(A, "End: Input");
        print_matrix(I, "Inversed");
    }
}

void find_inverse()
{
    int row, col, p; // 'p' stands for pivot (numbered from 0 to N-1)
    double pivalue;  // pivot value

    pthread_barrier_init(&barrier, NULL, N);
    pthread_t *children;     // dynamic array of child threads
    struct threadArgs *args; // argument buffer

    // Bringing the matrix A to the identity form
    for (p = 0; p < N; p++)
    { // Outer loop
        pivalue = A[p][p];
        for (col = 0; col < N; col++)
        {
            A[p][col] = A[p][col] / pivalue; // Division step on A
            I[p][col] = I[p][col] / pivalue; // Division step on I
        }
        assert(A[p][p] == 1.0);

        

        double multiplier;
        for (row = 0; row < N; row++)
        {
            multiplier = A[row][p];
            if (row != p) // Perform elimination on all except the current pivot row
            {
                // Create N threads
                children = malloc(N * sizeof(pthread_t));     // allocate array of handles
                args = malloc(N * sizeof(struct threadArgs)); // args vector

                for (int i = 0; i < N; i++)
                {
                    args[i].id = i;
                    args[i].multiplier = multiplier;
                    args[i].row = row;
                    pthread_create(&(children[i]),    // our handle for the child
                                   NULL,              // attributes of the child
                                   multiply_columns,  // the function it should run
                                   (void *)&args[i]); // args to that function
                }

                // Wait for all threads to complete
                for (int j = 0; j < N; j++)
                {
                    pthread_join(children[j], NULL);
                }
                pthread_barrier_destroy(&barrier);
                free(args);
                free(children);
                
            }
        }
    }
}

void *multiply_columns(void *params)
{
    struct threadArgs *args = (struct threadArgs *)params;
    int col = args->id;
    int multiplier = args->multiplier;
    int row = args->row;

    A[row][col] = A[row][col] - A[col][col] * multiplier; // Elimination step on A
    I[row][col] = I[row][col] - I[col][col] * multiplier; // Elimination step on I
    
    pthread_barrier_wait(&barrier);
    assert(A[row][col] == 0.0);
}

void init_matrix()
{
    int row, col;

    // Set the diagonal elements of the inverse matrix to 1.0
    // So that you get an identity matrix to begin with
    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
        {
            if (row == col)
                I[row][col] = 1.0;
        }
    }

    printf("\nsize      = %dx%d ", N, N);
    printf("\nmaxnum    = %d \n", maxnum);
    printf("Init	  = %s \n", Init);
    printf("Initializing matrix...");

    if (strcmp(Init, "rand") == 0)
    {
        for (row = 0; row < N; row++)
        {
            for (col = 0; col < N; col++)
            {
                if (row == col) // diagonal dominance
                    A[row][col] = (double)(rand() % maxnum) + 5.0;
                else
                    A[row][col] = (double)(rand() % maxnum) + 1.0;
            }
        }
    }

    if (strcmp(Init, "fast") == 0)
    {
        for (row = 0; row < N; row++)
        {
            for (col = 0; col < N; col++)
            {
                if (row == col) // diagonal dominance
                    A[row][col] = 5.0;
                else
                    A[row][col] = 2.0;
            }
        }
    }

    printf("done \n\n");
    if (PRINT == 1)
    {
        print_matrix(A, "Begin: Input");
        // print_matrix(I, "Begin: Inverse");
    }
}

void print_matrix(matrix M, char name[])
{
    int row, col;

    printf("%s Matrix:\n", name);
    for (row = 0; row < N; row++)
    {
        for (col = 0; col < N; col++)
            printf(" %5.2f", M[row][col]);
        printf("\n");
    }
    printf("\n\n");
}

// init default values
void init_default()
{
    N = 5;
    Init = "fast";
    maxnum = 15.0;
    PRINT = 1;
}

// read arguments
int read_options(int argc, char *argv[])
{
    char *prog;
    prog = *argv;

    while (++argv, --argc > 0)
    {
        if (**argv == '-')
        {
            switch (*++*argv)
            {
            case 'n':
                --argc;
                N = atoi(*++argv);
                break;
            case 'h':
                printf("\nHELP: try matinv -u \n\n");
                exit(0);
                break;
            case 'u':
                printf("\nUsage: matinv [-n problemsize]\n");
                printf("           [-D] show default values \n");
                printf("           [-h] help \n");
                printf("           [-I init_type] fast/rand \n");
                printf("           [-m maxnum] max random no \n");
                printf("           [-P print_switch] 0/1 \n");
                exit(0);
                break;
            case 'D':
                printf("\nDefault:  n         = %d ", N);
                printf("\n          Init      = rand");
                printf("\n          maxnum    = 5 ");
                printf("\n          P         = 0 \n\n");
                exit(0);
                break;
            case 'I':
                --argc;
                Init = *++argv;
                break;
            case 'm':
                --argc;
                maxnum = atoi(*++argv);
                break;
            case 'P':
                --argc;
                PRINT = atoi(*++argv);
                break;
            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                printf("HELP: try %s -u \n\n", prog);
                break;
            }
        }
    }
    return 0;
}
