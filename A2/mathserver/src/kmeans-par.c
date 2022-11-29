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

pthread_barrier_t barrier;
struct threadArgs
{
    unsigned int id;
    unsigned int iter;
};

typedef struct point
{
    float x;     // The x-coordinate of the point
    float y;     // The y-coordinate of the point
    int cluster; // The cluster that the point belongs to
} point;

int N = 0;                   // number of entries in the data
int k = 9;                   // number of centroids
point data[MAX_POINTS];      // Data coordinates
point cluster[MAX_CLUSTERS]; // The coordinates of each cluster center (also called centroid)

int count[MAX_CLUSTERS] = {0}; // Array to keep track of the number of points in each cluster
point temp[MAX_CLUSTERS] = {{0.0}};

char default_filename[22] = "./src/kmeans-data.txt";
char *filename = default_filename;
char default_results_path[41] = "./../computed_results/kmeans-results.txt";
char *results_path = default_results_path;
bool somechange = false;

void *child(void *params);

void read_data()
{
    char line[256];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }

    // Initialize points from the data file
    while (fgets(line, sizeof(line), fp))
    {
        if (!isspace(line[0])) // Lines cannot start with whitespace
        {
            char data_buf[256] = {0};
            strncpy(data_buf, line, strlen(line) - 2);         // Copy everything except trailing '\n\0'
            sscanf(data_buf, "%f %f", &data[N].x, &data[N].y); // Save to data array
            data[N].cluster = -1;                              // Initialize the cluster number to -1
            N++;
        }
    }
    if (N > MAX_POINTS)
    {
        N = MAX_POINTS;
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
                case 'f':
                    --argc;
                    filename = *++argv;
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
                    results_path = *++argv;
                    break;

                default:
                    printf("%s: ignored option: -%s\n", prog, *argv);
                    printf("\nUsage: kmeans\n");
                    printf("                [-f filename]    input data file\n");
                    printf("                [-k clusters]    number of clusters\n");
                    // printf("                [-p path]        path and filename for results\n");
                    break;
            }
        }
    }
    return 0;
}

int get_closest_centroid(int id)
{
    // Find the nearest centroid
    int nearest_cluster = -1;
    double xdist, ydist, dist, min_dist;
    min_dist = dist = INT_MAX;

    for (int c = 0; c < k; c++)
    { // For each centroid
        // Calculate the square of the Euclidean distance between that centroid and the point
        xdist = data[id].x - cluster[c].x;
        ydist = data[id].y - cluster[c].y;
        dist = xdist * xdist + ydist * ydist; // The square of Euclidean distance
        if (dist <= min_dist)
        {
            min_dist = dist;
            nearest_cluster = c;
        }
    }
    return nearest_cluster;
}

// TODO: O(N*k), could try parallelize only here and keep the rest as the sequential version
void assign_cluster_to_point(int id)
{
    int old_cluster = -1, new_cluster = -1;

    old_cluster = data[id].cluster;
    new_cluster = get_closest_centroid(id);
    data[id].cluster = new_cluster; // Assign a cluster to the point
    if (old_cluster != new_cluster)
    {
        somechange = true;
    }
}

void update_cluster_centers(int id)
{
    // Reset count and temp (only one thread)
    if (id == 0)
    {
        memset(count, 0, sizeof(count));
        for (int b = 0; b < MAX_CLUSTERS; b++)
        {
            temp[b].x = 0.0;
            temp[b].y = 0.0;
            temp[b].cluster = 0;
        }
    }
    pthread_barrier_wait(&barrier);

    int c = data[id].cluster;
    count[c]++;
    temp[c].x += data[id].x;
    temp[c].y += data[id].y;
    pthread_barrier_wait(&barrier);

    // Only one thread needs to iterate over the clusters
    if (id == 0)
    {
        for (int i = 0; i < k; i++)
        {
            cluster[i].x = temp[i].x / count[i];
            cluster[i].y = temp[i].y / count[i];
        }
    }
}

int kmeans()
{
    // printf("kmeans (k=%d, N=%d, inputfile='%s', resultfile='%s')\n", k, N, filename, results_path); // debug

    // N threads
    pthread_barrier_init(&barrier, NULL, N);
    pthread_t *children;     // dynamic array of child threads
    struct threadArgs *args; // argument buffer

    children = malloc(N * sizeof(pthread_t));     // allocate array of handles
    args = malloc(N * sizeof(struct threadArgs)); // args vector

    for (int i = 0; i < N; i++)
    {
        args[i].id = i;
        args[i].iter = 0;
        pthread_create(&(children[i]),    // our handle for the child
                       NULL,              // attributes of the child
                       child,             // the function it should run
                       (void *)&args[i]); // args to that function
    }

    // wait for all threads to complete
    for (int j = 0; j < N; j++)
    {
        pthread_join(children[j], NULL);
    }

    // calculate iterations
    int iter = 0;
    for (int k = 0; k < N; k++)
    {
        if (args[k].iter > iter)
        {
            iter = args[k].iter;
        }
    }
    printf("Number of iterations taken = %d\n", iter);
    printf("Computed cluster numbers successfully!\n");

    free(args);
    free(children);
    pthread_barrier_destroy(&barrier);
    return 0;
}

void write_results()
{
    FILE *fp = fopen(results_path, "w");
    if (fp == NULL)
    {
        perror("Cannot write to file");
        exit(EXIT_FAILURE);
    }
    else
    {
        for (int i = 0; i < N; i++)
        {
            fprintf(fp, "%.2f %.2f %d\n", data[i].x, data[i].y, data[i].cluster);
        }
    }
    printf("Wrote the results to a file!\n");
}

int main(int argc, char *argv[])
{
    read_options(argc, argv);
    read_data();
    kmeans();
    write_results();
    return 0;
}

// TODO: rename to kmeans again
// create threads, call assign_cluster_to_point for every thread
// does not need to share somechange between threads
// Parallelizing only the part of the algo that has the worst time complexity,
// keeping the O(1) parts as is, no need to parallelize them
void *child(void *params)
{
    struct threadArgs *args = (struct threadArgs *)params;
    int id = args->id;

    do
    {
        pthread_barrier_wait(&barrier);
        // Set global somechange value to false before starting iteration
        if (id == 0)
        {
            somechange = false;
        }
        pthread_barrier_wait(&barrier);

        args->iter++; // Keep track of number of iterations
        assign_cluster_to_point(id);
        pthread_barrier_wait(&barrier);

        update_cluster_centers(id);
        pthread_barrier_wait(&barrier); // Wait for all to read somechange before resetting it
    } while (somechange);
}
