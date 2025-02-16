#ifndef CLI_H
#define CLI_H

#include "f_functions.h"

/* === Parameters Structure for CLI Options === */
typedef struct
{
    int n_agents;
    int n_variables;
    int n_iterations;
    double lower_bound;
    double upper_bound;
    double f;
    double l;
    double R;
    double M;
    double scale;
    char fitness_function[MAX_FUN_NAME_LEN];
} Parameters;

// Read command line arguments
void read_cli(int argc, char *argv[], Parameters *params);

#endif