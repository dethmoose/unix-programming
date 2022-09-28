#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>
#include <pthread.h>

#define MAX_POINTS 4096
#define MAX_CLUSTERS 32

pthread_barrier_t barrier;
struct threadArgs
{
    unsigned int id;
};

typedef struct point
{
    float x;     // The x-coordinate of the point
    float y;     // The y-coordinate of the point
    int cluster; // The cluster that the point belongs to
} point;

int THREADS = 10;
int N = 0;                   // number of entries in the data
int k = 0;                   // number of centroids
point data[MAX_POINTS];      // Data coordinates
point cluster[MAX_CLUSTERS]; // The coordinates of each cluster center (also called centroid)

void *child(void *params);

void read_data()
{
    N = 1797;
    k = 9;

    // TODO: paths to filenames correct?
    FILE *fp = fopen("./src/kmeans-data.txt", "r");
    if (fp == NULL)
    {
        perror("read_data: Cannot open the file kmeans-data.txt");
        exit(EXIT_FAILURE);
    }

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

int get_closest_centroid(int i, int k)
{
    // find the nearest centroid
    int nearest_cluster = -1;
    double xdist, ydist, dist, min_dist;
    min_dist = dist = INT_MAX;
    for (int c = 0; c < k; c++)
    { // For each centroid
        // Calculate the square of the Euclidean distance between that centroid and the point
        xdist = data[i].x - cluster[c].x;
        ydist = data[i].y - cluster[c].y;
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
    bool something_changed = false;
    int old_cluster = -1, new_cluster = -1;
    old_cluster = data[id].cluster;
    new_cluster = get_closest_centroid(id, k);
    data[id].cluster = new_cluster; // Assign a cluster to the point 'id'
    if (old_cluster != new_cluster)
    {
        something_changed = true;
    }
    
    return something_changed;
}

void update_cluster_centers()
{
    // Update the cluster centers
    int c = 0;
    int count[MAX_CLUSTERS] = {0}; // Array to keep track of the number of points in each cluster
    point temp[MAX_CLUSTERS] = {{0.0}};

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

// TODO parallelize
int kmeans(int k)
{
    bool somechange;
    int iter = 0;
    do
    {
        iter++; // Keep track of number of iterations
        somechange = assign_cluster_to_point();
        update_cluster_centers();
        // pthread_barrier_wait(&barrier);
    } while (somechange);
    printf("Number of iterations taken = %d\n", iter);
    printf("Computed cluster numbers successfully!\n");
    return 0;
}

void write_results()
{
    FILE *fp = fopen("./../computed_results/kmeans-results.txt", "w");
    if (fp == NULL)
    {
        perror("write_results: Cannot open the file kmeans-results.txt");
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

int main()
{
    read_data();

    // N threads probably not needed, N is number of data points
    pthread_barrier_init(&barrier, NULL, THREADS);
    pthread_t *children;     // dynamic array of child threads
    struct threadArgs *args; // argument buffer

    children = malloc(THREADS * sizeof(pthread_t)); // allocate array of handles
    args = malloc(THREADS * sizeof(struct threadArgs)); // args vector

    for (int i = 0; i < THREADS; i++)
    {
        args[i].id = i;
        pthread_create(&(children[i]),    // our handle for the child
                       NULL,              // attributes of the child
                       child,             // the function it should run
                       (void *)&args[i]); // args to that function
    }
     // kmeans(k);

    // wait for all threads to complete
    for (int j = 0; j < THREADS; j++)
    {
        pthread_join(children[j], NULL);
    }
    write_results();

    pthread_barrier_destroy(&barrier);
    free(args);
    free(children);
    return 0;
    
}

void *child(void *params)
{
    struct threadArgs *args = (struct threadArgs *)params;
    int id = args->id;

    // do something
    // pthread_barrier_wait(&barrier);
}
