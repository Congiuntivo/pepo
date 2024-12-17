#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "space.h"
#include "epo.h"
#include "utils.h"
#include "csv.h"

// Example fitness function: Sphere function
double sphere_function(double *position, int n_variables);

// Helper functions for CSV logging
void log_population(Space *space, FILE *file, double temperature_profile, int iteration);
void format_position_string(char *position_str, size_t position_str_size, const double *position, int n_variables);

// Read command line arguments
void read_cli(int argc, char *argv[], int *n_agents, int *n_variables, int *n_iterations, double *lower_bound, double *upper_bound, double *f, double *l, double *R, double *M);

int main(int argc, char *argv[])
{
    int n_agents, n_variables, n_iterations;
    double lower_bound, upper_bound, f, l, R, M;

    // Read command line arguments
    read_cli(argc, argv, &n_agents, &n_variables, &n_iterations, &lower_bound, &upper_bound, &f, &l, &R, &M);

    srand(time(NULL)); // Set random seed

    // Create CSV file
    const char *field_names[] = {"Iteration", "Fitness", "Position", "TempP"};
    FILE *file = csv_open("output.csv", field_names, 4);

    if (!file)
    {
        fprintf(stderr, "Failed to create output.csv\n");
        return EXIT_FAILURE;
    }

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

        // Write to CSV file: iteration and best fitness
        log_population(&space, file, temperature_profile(&epo), iteration);

        // Perform EPO update
        epo_update(&epo, &space);

        // Print progress
        printf("Iteration %d: Best Fitness = %.6f\n", iteration, space.best_agent.fitness);
        for (int i = 0; i < n_agents; i++)
        {
            printf("\tAgent %d: Fitness = %.6f\n", i, space.agents[i].fitness);
            for (int j = 0; j < n_variables; j++)
            {
                printf("\t\tPosition %d: %.6f\n", j, space.agents[i].position[j]);
            }
        }
    }

    // Free allocated memory
    free_space(&space);

    return 0;
}

void log_population(Space *space, FILE *file, double temperature_profile, int iteration)
{
    // Constants for buffer sizes
    const size_t ITERATION_STR_SIZE = 12;
    const size_t FITNESS_STR_SIZE = 50;
    const size_t POSITION_STR_SIZE = 200;
    const size_t TEMP_PROFILE_STR_SIZE = 50;

    // Create and format iteration and temperature profile strings
    char iteration_str[ITERATION_STR_SIZE];
    char tempP_str[TEMP_PROFILE_STR_SIZE];
    snprintf(iteration_str, sizeof(iteration_str), "%d", iteration);
    snprintf(tempP_str, sizeof(tempP_str), "%.6f", temperature_profile);

    // Loop over all agents in the space
    for (int i = 0; i < space->n_agents; i++)
    {
        // Create buffers for fitness and position
        char fitness_str[FITNESS_STR_SIZE];
        snprintf(fitness_str, sizeof(fitness_str), "%.6f", space->agents[i].fitness);

        char position_str[POSITION_STR_SIZE];
        format_position_string(position_str, sizeof(position_str), space->agents[i].position, space->n_variables);

        // Prepare the row values
        const char *row_values[4] = {iteration_str, fitness_str, position_str, tempP_str};

        // Write to CSV file
        csv_write_row(file, row_values, 4);
    }
}

// Helper function to format the positions of the agents into a single string
void format_position_string(char *position_str, size_t position_str_size, const double *position, int n_variables)
{
    position_str[0] = '\0'; // Initialize the position string to be empty
    char temp[50];          // Temporary buffer for formatting each variable

    for (int j = 0; j < n_variables; j++)
    {
        // Append the current value to the position string
        snprintf(temp, sizeof(temp), "%.6f", position[j]);
        strncat(position_str, temp, position_str_size - strlen(position_str) - 1);

        // Append an underscore if it's not the last variable
        if (j < n_variables - 1)
        {
            strncat(position_str, "_", position_str_size - strlen(position_str) - 1);
        }
    }
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
    *n_iterations = 100;
    *lower_bound = -100.0;
    *upper_bound = 100.0;
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
        i += 2;
    }
}