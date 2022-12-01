/***************************************************************************
 *
 * Parallel version of Kmeans
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <pthread.h>

#define MAX_POINTS 4096
#define MAX_CLUSTERS 32
// #define THREADS 4

// pthread_barrier_init(&barrier, NULL, N);
pthread_barrier_t barrier;
struct threadArgs
{
    unsigned int i;
    int old_cluster;
    int new_cluster;
};

typedef struct point
{
    float x;     // The x-coordinate of the point
    float y;     // The y-coordinate of the point
    int cluster; // The cluster that the point belongs to
} point;

int N = 0;                   // Number of entries in the data
int k = 9;                   // Number of centroids
point data[MAX_POINTS];      // Data coordinates
point cluster[MAX_CLUSTERS]; // The coordinates of each cluster center (also called centroid)

// File paths
char default_results_path[41] = "./../computed_results/kmeans-results.txt";
char default_input_path[22] = "./src/kmeans-data.txt";
char *results_path = default_results_path;
char *input_path = default_input_path;

// Forward declarations
void kmeans();
void read_data();
void write_results();
void update_cluster_centers();
bool assign_clusters_to_points();
void get_closest_centroid(void *params);
void read_options(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    read_options(argc, argv);
    read_data();
    kmeans();
    write_results();
    return 0;
}

// Read command line arguments
void read_options(int argc, char *argv[])
{
    char *prog;
    prog = *argv;

    while (++argv, --argc > 0)
        if (**argv == '-')
            switch (*++*argv)
            {
            case 'f':
                --argc;
                input_path = *++argv;
                break;

            case 'k':
                --argc;
                int value = atoi(*++argv);
                if (value > MAX_CLUSTERS)
                {
                    k = MAX_CLUSTERS;
                }
                else if (value < 1)
                {
                    k = 1;
                }
                else
                {
                    k = value;
                }
                break;

            case 'p':
                --argc;
                // Where the server wants to save the results
                results_path = *++argv;
                break;

            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                printf("\nUsage: kmeans\n");
                printf("                [-f filename]    input data file\n");
                printf("                [-k clusters]    number of clusters\n");
                break;
            }
}

// Read data from input file and intialize centroids
void read_data()
{
    char line[256];
    FILE *fp;
    if ((fp = fopen(input_path, "r")) == NULL)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    // Initialize points from the data file
    while (fgets(line, sizeof(line), fp))
    {
        if (!isspace(line[0]) && N < MAX_POINTS) // Lines cannot start with whitespace
        {
            char data_buf[256] = {0};
            strncpy(data_buf, line, strlen(line) - 2);         // Copy everything except trailing '\n\0'
            sscanf(data_buf, "%f %f", &data[N].x, &data[N].y); // Save to data array
            data[N].cluster = -1;                              // Initialize the cluster number to -1
            N++;
        }
    }
    printf("Read the problem data!\n");

    // Initialize centroids randomly
    srand(0); // Setting 0 as the random number generation seed
    for (int i = 0; i < k; i++)
    {
        int r = rand() % N;
        cluster[i].x = data[r].x;
        cluster[i].y = data[r].y;
    }
    fclose(fp);
}

// Kmeans algorithm
void kmeans()
{
    int iter = 0;
    bool somechange = false;
    do
    {
        iter++; // Keep track of number of iterations
        somechange = assign_clusters_to_points();
        update_cluster_centers();
    } while (somechange);
    printf("Number of iterations taken = %d\n", iter);
    printf("Computed cluster numbers successfully!\n");
}

bool assign_clusters_to_points()
{
    pthread_t *children;     // Dynamic array of child threads
    struct threadArgs *args; // Argument buffer
    bool something_changed = false;

    children = malloc(N * sizeof(pthread_t));     // Allocate array of handles
    args = malloc(N * sizeof(struct threadArgs)); // Args vector

    // Let threads find closest centroids
    for (int i = 0; i < N; i++)
    {
        args[i].i = i;
        args[i].old_cluster = data[i].cluster;
        args[i].new_cluster = -1;
        pthread_create(&(children[i]),       // Our handle for the child
                       NULL,                 // Attributes of the child
                       get_closest_centroid, // The function it should run
                       (void *)&args[i]);    // Args to that function
    }

    // Wait for all threads to complete
    for (int j = 0; j < N; j++)
    {
        pthread_join(children[j], NULL);
    }
    free(children);

    // Read thread args to update something_changed
    for (int k = 0; k < N; k++)
    {
        if (args[k].old_cluster != args[k].new_cluster)
        {
            something_changed = true;
        }
    }
    free(args);
    return something_changed;
}

// Parallelized function
void get_closest_centroid(void *params)
{
    struct threadArgs *args = (struct threadArgs *)params;
    int i = args->i, new_cluster = args->new_cluster;

    // Find the nearest centroid
    int nearest_cluster = -1;
    double xdist, ydist, dist, min_dist;
    min_dist = dist = INT_MAX;

    for (int c = 0; c < k; c++)
    { // For each centroid
        // Calculate the square of the Euclidean distance between that centroid and the point
        xdist = data[i].x - cluster[c].x;
        ydist = data[i].y - cluster[c].y;
        dist = xdist * xdist + ydist * ydist; // The square of Euclidean distance
        if (dist <= min_dist)
        {
            min_dist = dist;
            nearest_cluster = c;
        }
    }

    // Save "return value" to thread args and to data array
    args->new_cluster = data[i].cluster = nearest_cluster;
}

void update_cluster_centers()
{
    int c;
    int count[MAX_CLUSTERS] = {0}; // Array to keep track of the number of points in each cluster
    point temp[MAX_CLUSTERS] = {0.0};

    for (int i = 0; i < N; i++)
    {
        c = data[i].cluster;
        count[c]++;
        temp[c].x += data[i].x;
        temp[c].y += data[i].y;
    }

    for (int i = 0; i < k; i++)
    {
        cluster[i].x = temp[i].x / count[i];
        cluster[i].y = temp[i].y / count[i];
    }
}

void write_results()
{
    FILE *fp;
    if ((fp = fopen(results_path, "w")) == NULL)
    {
        perror("Cannot write to file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < N; i++)
    {
        fprintf(fp, "%.2f %.2f %d\n", data[i].x, data[i].y, data[i].cluster);
    }
    printf("Wrote the results to a file!\n");
}
