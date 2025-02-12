#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_cli(int argc, char *argv[], int *n_agents, int *n_variables, int *n_iterations, double *lower_bound, double *upper_bound, double *f, double *l, double *R, double *M, double *scale)
{
    // Set default values
    *n_agents = 80;
    *n_variables = 3;
    *n_iterations = 100;
    *lower_bound = -100.0;
    *upper_bound = 100.0;
    *f = 2.0;     // Exploration control parameter
    *l = 1.5;     // Exploitation control parameter
    *R = 0.5;     // Huddle radius
    *M = 2.0;     // Movement parameter
    *scale = 1.0; // Scale factor

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
            printf("\t-s    --scale          Scale factor (default: %f)\n", *scale);
            printf("\t-h,   --help           Display this help message\n");
            exit(0);
        }
    }

    // Read command line arguments
    int i = 1;
    while (i < argc - 1)
    {
        if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--penguins") == 0)
        {
            *n_agents = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--variables") == 0)
        {
            *n_variables = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0)
        {
            printf("Found iterations\n");
            *n_iterations = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-lb") == 0 || strcmp(argv[i], "--lower_bound") == 0)
        {
            *lower_bound = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-ub") == 0 || strcmp(argv[i], "--upper_bound") == 0)
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
        else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--radius") == 0)
        {
            *R = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-M") == 0)
        {
            *M = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scale") == 0)
        {
            *scale = atof(argv[i + 1]);
        }
        i += 2;
    }
}
