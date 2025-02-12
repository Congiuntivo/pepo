#ifndef CLI_H
#define CLI_H

// Read command line arguments
void read_cli(int argc, char *argv[], int *n_agents, int *n_variables, int *n_iterations, double *lower_bound, double *upper_bound, double *f, double *l, double *R, double *M, double *scale);


#endif