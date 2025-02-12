#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "space.h"
#include "epo.h"
#include "utils.h"
#include "csv.h"
#include "cli.h"

// Example fitness function: Sphere function
double sphere_function(double *position, int n_variables);
double matyas_function(double *position, int n_variables);
double bukin_function(double *position, int n_variables);
double mccormick_function(double *position, int n_variables);
double michealewicz_function(double *position, int n_variables);

// Helper functions for CSV logging
void log_population(Space *space, FILE *file, double temperature_profile, int iteration);
void format_position_string(char *position_str, size_t position_str_size, const double *position, int n_variables);


typedef struct
{
    double fitness;
    int rank;
} FitnessRank;

int main(int argc, char *argv[])
{
    int rank, comm_size, n_agents, n_variables, n_iterations;
    double lower_bound, upper_bound, f, l, R, M, scale;

    // Initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    // Root reads command line arguments and broadcasts
    if (rank == 0)
    {
        read_cli(argc, argv, &n_agents, &n_variables, &n_iterations,
                 &lower_bound, &upper_bound, &f, &l, &R, &M, &scale);
    }

    // Broadcast parameters to all processes
    MPI_Bcast(&n_agents, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n_variables, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&n_iterations, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&lower_bound, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&upper_bound, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&f, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&l, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&R, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Bcast(&scale, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Calculate local number of agents
    int local_n_agents = n_agents / comm_size;
    if (rank < n_agents % comm_size)
        local_n_agents++;

    // Set different random seeds for each process
    srand(time(NULL) + rank);

    // Root process handles file I/O
    FILE *file = NULL;
    if (rank == 0)
    {
        const char *field_names[] = {"Iteration", "Fitness", "Position", "TempP"};
        file = csv_open("output.csv", field_names, 4);
        if (!file)
        {
            fprintf(stderr, "Failed to create output.csv\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    // Initialize local search space and EPO
    Space space;
    init_space(&space, local_n_agents, n_variables, lower_bound, upper_bound);
    EPO epo;
    init_epo(&epo, R, M, f, l, n_iterations, scale);

    // Optimization loop
    for (int iteration = 1; iteration <= n_iterations; iteration++)
    {
        // Update fitness and find the best agent
        update_best_agent(&space, michealewicz_function);

        // Prepare for reduction
        FitnessRank local, global;
        local.fitness = space.best_agent.fitness;
        local.rank = rank;

        // Perform MPI_Allreduce with MPI_MINLOC
        MPI_Allreduce(&local, &global, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        // Broadcast best agent position to all processes
        MPI_Bcast(space.best_agent.position, n_variables, MPI_DOUBLE, global.rank, MPI_COMM_WORLD);
        // Evaluate best agent fitness
        space.best_agent.fitness = sphere_function(space.best_agent.position, n_variables);

        if (rank == 0)
        {
            printf("Itr %d: Best fitness: %.6f\n", iteration, space.best_agent.fitness);
            // Write to CSV file: iteration and best fitness
            log_population(&space, file, temperature_profile(&epo), iteration);
        }

        // Perform EPO update
        epo_update(&epo, &space);
    }

    if (rank == 0)
    {
        printf("Best agent found:\n");
        printf("Fitness: %.6f\n", space.best_agent.fitness);

        // Close CSV file
        csv_close(file);
    }

    // Free allocated memory
    free_space(&space);

    MPI_Finalize();

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

// Example fitness function: Matyas function
double matyas_function(double *position, int n_variables)
{
    double x_square = position[0] * position[0];
    double y_square = position[1] * position[1];
    double x_y = position[0] * position[1];
    double fitness = 0.26 * (x_square + y_square) - (0.48 * x_y);
    return fitness;
}

double bukin_function(double *position, int n_variables)
{
    double x = position[0];
    double y = position[1];
    double fitness = 100 * sqrt(fabs(y - 0.01 * x * x)) + 0.01 * fabs(x + 10);
    return fitness;
}

double mccormick_function(double *position, int n_variables)
{
    double x = position[0];
    double y = position[1];
    double fitness = sin(x + y) + (x - y) * (x - y) - 1.5 * x + 2.5 * y + 1;
    return fitness;
}

double michealewicz_function(double *position, int n_variables)
{
    double fitness = 0.0;
    for (int i = 0; i < n_variables; i++)
    {
        fitness += -sin(position[i]) * pow(sin((i + 1) * position[i] * position[i] / M_PI), 20);
    }
    return fitness;
}
