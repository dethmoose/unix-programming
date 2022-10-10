#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>

#define MAX_POINTS 4096
#define MAX_CLUSTERS 32

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
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

int N = 1797;                // number of entries in the data
int k = 9;                   // number of centroids
point data[MAX_POINTS];      // Data coordinates
point cluster[MAX_CLUSTERS]; // The coordinates of each cluster center (also called centroid)
char *filename = "./src/kmeans-data.txt";
bool somechange = false;

void *child(void *params);

void read_data()
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("Cannot open file");
        exit(EXIT_FAILURE);
    }
    // TODO: N depends on number of entries in the input data file

    // Initialize points from the data file
    // float temp = 0.0;
    for (int i = 0; i < N; i++)
    {
        fscanf(fp, "%f %f", &data[i].x, &data[i].y);
        data[i].cluster = -1; // Initialize the cluster number to -1
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
                // TODO temp N value for "./src/kmeans-data-50.txt"
                N = 50;
                break;

            case 'k':
                --argc;
                k = atoi(*++argv);
                break;

            default:
                printf("%s: ignored option: -%s\n", prog, *argv);
                break;
            }
        }
    }
    return 0;
}

int get_closest_centroid(int id, int k)
{
    // printf("get_closest_centroid %d\n", id);
    // find the nearest centroid
    int nearest_cluster = -1;
    double xdist, ydist, dist, min_dist;
    min_dist = dist = INT_MAX;

    for (int c = 0; c < k; c++)
    { // For each centroid
        // Calculate the square of the Euclidean distance between that centroid and the point
        xdist = data[id].x - cluster[c].x;
        ydist = data[id].y - cluster[c].y;
        dist = xdist * xdist + ydist * ydist; // The square of Euclidean distance
        // printf("%.2lf \n", dist);
        if (dist <= min_dist)
        {
            min_dist = dist;
            nearest_cluster = c;
        }
    }
    // printf("-----------\n");
    return nearest_cluster;
}

bool assign_cluster_to_point(int id)
{
    // printf("assign_cluster_to_point %d\n", id);
    bool something_changed = false;
    int old_cluster = -1, new_cluster = -1;

    old_cluster = data[id].cluster;
    new_cluster = get_closest_centroid(id, k);
    data[id].cluster = new_cluster; // Assign a cluster to the point
    if (old_cluster != new_cluster)
    {
        printf("assign_cluster_to_point %d, something changed\n", id);
        something_changed = true;
    }

    return something_changed;
}

void update_cluster_centers(int id)
{
    // printf("update_cluster_centers %d\n", id);
    // Update the cluster centers
    int c = 0;
    int count[MAX_CLUSTERS] = {0}; // Array to keep track of the number of points in each cluster
    point temp[MAX_CLUSTERS] = {{0.0}};

    c = data[id].cluster;
    count[c]++;
    temp[c].x += data[id].x;
    temp[c].y += data[id].y;

    pthread_barrier_wait(&barrier);

    // only one thread updates clusters
    if (id == 0)
    {
        for (int i = 0; i < k; i++)
        {
            cluster[i].x = temp[i].x / count[i];
            cluster[i].y = temp[i].y / count[i];
        }
    }
}

void write_results()
{
    FILE *fp = fopen("./../computed_results/kmeans-results.txt", "w");
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
    printf("kmeans (k=%d, N=%d, file=%s)\n", k, N, filename);

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

    write_results();

    // print number of iterations
    int iter = 0;
    for (int k = 0; k < N; k++)
    {
        if (args[k].iter > iter)
        {
            iter = args[k].iter;
        }
    }
    printf("Number of iterations taken = %d\n", iter);
    // printf("Computed cluster numbers successfully!\n");

    free(args);
    free(children);
    pthread_mutex_destroy(&lock);
    pthread_barrier_destroy(&barrier);

    return 0;
}

void *child(void *params)
{
    struct threadArgs *args = (struct threadArgs *)params;
    int id = args->id;
    args->iter = 1;
    return NULL;
}
