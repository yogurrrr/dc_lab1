#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <pthread.h>

#define EPS 1e-6
#define GRAVCONSTANT 6.6743e-11
#define TIMESTEPS 100
#define DT 0.05

typedef struct
{
    double x, y;
} vector;

typedef struct
{
    int first_body, last_body;
} thread_args;

double *masses;
vector *positions, *velocities, *accelerations;
int bodies, thread_count;

vector addVectors(vector a, vector b)
{
    vector c = {a.x + b.x, a.y + b.y};
    return c;
}

vector scaleVector(double b, vector a)
{
    vector c = {b * a.x, b * a.y};
    return c;
}

vector subtractVectors(vector a, vector b)
{
    vector c = {a.x - b.x, a.y - b.y};
    return c;
}

double mod(vector a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

void initiateSystem(char *fileName)
{
    FILE *fp = fopen(fileName, "r");
    fscanf(fp, "%d", &bodies);

    masses = (double *)malloc(bodies * sizeof(double));
    positions = (vector *)malloc(bodies * sizeof(vector));
    velocities = (vector *)malloc(bodies * sizeof(vector));
    accelerations = (vector *)malloc(bodies * sizeof(vector));

    for (int i = 0; i < bodies; ++i)
    {
        fscanf(fp, "%lf", &masses[i]);
        fscanf(fp, "%lf%lf", &positions[i].x, &positions[i].y);
        fscanf(fp, "%lf%lf", &velocities[i].x, &velocities[i].y);
    }

    fclose(fp);
}

void resolveCollisions()
{
    int i, j;

    for (i = 0; i < bodies - 1; ++i)
        for (j = i + 1; j < bodies; ++j)
        {
            if (fabs(positions[i].x - positions[j].x) < EPS && fabs(positions[i].y - positions[j].y) < EPS)
            {
                vector temp = velocities[i];
                velocities[i] = velocities[j];
                velocities[j] = temp;
            }
        }
}

void computeAccelerations(int first_body, int last_body)
{
    int i, j;

    for (i = first_body; i < last_body; ++i)
    {
        accelerations[i].x = 0;
        accelerations[i].y = 0;
        for (j = 0; j < bodies; ++j)
        {
            if (i != j)
            {
                accelerations[i] = addVectors(accelerations[i], scaleVector(GRAVCONSTANT * masses[j] / pow(mod(subtractVectors(positions[i], positions[j])), 3), subtractVectors(positions[j], positions[i])));
            }
        }
    }
}

void computeVelocities(int first_body, int last_body)
{
    int i;

    for (i = first_body; i < last_body; ++i)
        velocities[i] = addVectors(velocities[i], scaleVector(DT, accelerations[i]));
}

void computePositions(int first_body, int last_body)
{
    int i;

    for (i = first_body; i < last_body; ++i)
        positions[i] = addVectors(positions[i], scaleVector(DT, velocities[i]));
}

void *routine(void *arg)
{
    thread_args *args = (thread_args *)arg;
    computeAccelerations(args->first_body, args->last_body);
    computePositions(args->first_body, args->last_body);
    computeVelocities(args->first_body, args->last_body);
    return 0;
}

int main(int argC, char *argV[])
{
    if (argC != 3)
    {
        printf("Usage: %s <input_file> <thread_count>", argV[0]);
        return -1;
    }

    initiateSystem(argV[1]);
    thread_count = atoi(argV[2]);
    thread_args thread_info[thread_count];
    pthread_t threads[thread_count];

    int amount = bodies / thread_count;
    for (int i = 0; i < thread_count; ++i)
    {
        thread_info[i].first_body = amount * i;
        thread_info[i].last_body = amount * (i + 1);
    }
    thread_info[thread_count - 1].last_body = bodies;

    FILE *fp = fopen("output.csv", "w");;
    fprintf(fp, "t, ");
    for (int i = 0; i < bodies; ++i)
        fprintf(fp, "x%d, y%d, ", i + 1, i + 1);
    fprintf(fp, "\n");

    for (int i = 0; i < TIMESTEPS; ++i)
    {
        for (int j = 0; j < thread_count; ++j)
            pthread_create(&threads[j], NULL, routine, &thread_info[j]);

        for (int j = 0; j < thread_count; ++j)
            pthread_join(threads[j], NULL);

        resolveCollisions();

        fprintf(fp, "%d, ", i + 1);
        for (int j = 0; j < bodies; ++j)
            fprintf(fp, "%lf, %lf, ", positions[j].x, positions[j].y);
        fprintf(fp, "\n");
    }

    fclose(fp);
    free(masses);
    free(positions);
    free(velocities);
    free(accelerations);
    return 0;
}
