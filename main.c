#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <mpi.h>

#include "space.h"
#include "epo.h"
#include "utils.h"
#include "csv.h"

// Example fitness function: Sphere function
double sphere_function(double *position, int n_variables);

// Read command line arguments
void read_cli(int argc, char *argv[], int *n_agents, int *n_variables, int *n_iterations, double *lower_bound, double *upper_bound, double *f, double *l, double *R, double *M, double *scale);

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
        update_best_agent(&space, sphere_function);

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
