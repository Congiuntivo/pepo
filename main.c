#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "space.h"
#include "epo.h"
#include "utils.h"

// Example fitness function: Sphere function
double sphere_function(double *position, int n_variables);

// Read command line arguments
void read_cli(int argc, char *argv[], int *n_agents, int *n_variables, int *n_iterations, double *lower_bound, double *upper_bound, double *f, double *l, double *R, double *M);

int main(int argc, char *argv[])
{
    int n_agents, n_variables, n_iterations;
    double lower_bound, upper_bound, f, l, R, M;

    // Read command line arguments
    read_cli(argc, argv, &n_agents, &n_variables, &n_iterations, &lower_bound, &upper_bound, &f, &l, &R, &M);
    
    srand(time(NULL)); // Set random seed



    // Initialize search space
    Space space;
    init_space(&space, n_agents, n_variables, lower_bound, upper_bound);

    // Initialize EPO
    EPO epo;
    init_epo(&epo, R, M, f, l, n_iterations);

    // Optimization loop
    for (int iteration = 1; iteration <= n_iterations; iteration++)
    {
        // Update fitness and find the best agent
        update_best_agent(&space, sphere_function);

        // Perform EPO update
        epo_update(&epo, &space);

        // Print progress
        printf("Iteration %d: Best Fitness = %.6f\n", iteration, space.best_agent.fitness);
    }

    // Free allocated memory
    free_space(&space);

    return 0;
}

// Example fitness function: Sphere function
double sphere_function(double *position, int n_variables)
{
    double fitness = 0.0;
    for (int i = 0; i < n_variables; i++)
    {
        fitness += position[i] * position[i];
    }
    return fitness;
}

void read_cli(int argc, char *argv[], int *n_agents, int *n_variables, int *n_iterations, double *lower_bound, double *upper_bound, double *f, double *l, double *R, double *M)
{
    // Set default values
    *n_agents = 80;
    *n_variables = 3;
    *n_iterations = 1000;
    *lower_bound = -1000.0;
    *upper_bound = 1000.0;
    *f = 2.0; // Exploration control parameter
    *l = 1.5; // Exploitation control parameter
    *R = 0.5; // Huddle radius
    *M = 2.0; // Movement parameter

    if (argc >= 2)
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)
        {
            printf("Usage: %s [-p N] [-v N] [-i N] [-lb N] [-ub N] [-f N] [-l N] [-R N] [-M N]\n", argv[0]);
            printf("Options:\n");
            printf("\t-p,   --penguins       Number of penguins (default: %d)\n", *n_agents);
            printf("\t-v,   --variables      Number of variables (default: %d)\n", *n_variables);
            printf("\t-i,   --iterations     Number of iterations (default: %d)\n", *n_iterations);
            printf("\t-lb,  --lower_bound    Lower bound of search space (default: %f)\n", *lower_bound);
            printf("\t-ub,  --upper_bound    Upper bound of search space (default: %f)\n", *upper_bound);
            printf("\t-f                     Exploration control parameter (default: %f)\n", *f);
            printf("\t-l                     Exploitation control parameter (default: %f)\n", *l);
            printf("\t-R,   --radius         Huddle radius (default: %f)\n", *R);
            printf("\t-M                     Movement parameter (default: %f)\n", *M);
            exit(0);
        }
    }

    // Read command line arguments
    int i = 1;
    while (i < argc - 1)
    {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--penguins"))
        {
            *n_agents = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--variables"))
        {
            *n_variables = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations"))
        {
            *n_iterations = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-lb") == 0 || strcmp(argv[i], "--lower_bound"))
        {
            *lower_bound = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-ub") == 0 || strcmp(argv[i], "--upper_bound"))
        {
            *upper_bound = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            *f = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            *l = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--radius"))
        {
            *R = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-M") == 0)
        {
            *M = atof(argv[i + 1]);
        }
        i += 2;
    }
}