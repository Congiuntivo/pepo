#include "cli.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set_default_parameters(Parameters *params)
{
    params->n_agents = 80;
    params->n_variables = 3;
    params->n_iterations = 100;
    params->lower_bound = -100.0;
    params->upper_bound = 100.0;
    params->f = 2.0;
    params->l = 1.5;
    params->R = 0.5;
    params->M = 2.0;
    params->scale = 1.0;
}

void print_help(const char *program_name, const Parameters *params)
{
    printf("Usage: %s [-p N] [-v N] [-i N] [-lb N] [-ub N] [-f N] [-l N] [-R N] [-M N] [-s N]\n", program_name);
    printf("Options:\n");
    printf("\t-p,   --penguins       Number of penguins (default: %d)\n", params->n_agents);
    printf("\t-v,   --variables      Number of variables (default: %d)\n", params->n_variables);
    printf("\t-i,   --iterations     Number of iterations (default: %d)\n", params->n_iterations);
    printf("\t-lb,  --lower_bound    Lower bound of search space (default: %f)\n", params->lower_bound);
    printf("\t-ub,  --upper_bound    Upper bound of search space (default: %f)\n", params->upper_bound);
    printf("\t-f                     Exploration control parameter (default: %f)\n", params->f);
    printf("\t-l                     Exploitation control parameter (default: %f)\n", params->l);
    printf("\t-R,   --radius         Huddle radius (default: %f)\n", params->R);
    printf("\t-M                     Movement parameter (default: %f)\n", params->M);
    printf("\t-s,   --scale          Scale factor (default: %f)\n", params->scale);
    printf("\t-h,   --help           Display this help message\n");
}

void read_cli(int argc, char *argv[], Parameters *params)
{
    set_default_parameters(params);

    for (int i = 1; i < argc - 1; i += 2)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            print_help(argv[0], params);
            exit(0);
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--penguins") == 0)
        {
            params->n_agents = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--variables") == 0)
        {
            params->n_variables = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0)
        {
            params->n_iterations = atoi(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-lb") == 0 || strcmp(argv[i], "--lower_bound") == 0)
        {
            params->lower_bound = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-ub") == 0 || strcmp(argv[i], "--upper_bound") == 0)
        {
            params->upper_bound = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-f") == 0)
        {
            params->f = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-l") == 0)
        {
            params->l = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-R") == 0 || strcmp(argv[i], "--radius") == 0)
        {
            params->R = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-M") == 0)
        {
            params->M = atof(argv[i + 1]);
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--scale") == 0)
        {
            params->scale = atof(argv[i + 1]);
        }
    }
}
