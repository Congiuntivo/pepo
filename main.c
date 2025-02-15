#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <stddef.h> // for offsetof
#include <mpi.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "space.h"
#include "epo.h"
#include "utils.h"
#include "csv.h"
#include "cli.h"

/* === Fitness Function Prototypes === */
static double sphere_function(double *position, int n_variables);
static double matyas_function(double *position, int n_variables);
static double bukin_function(double *position, int n_variables);
static double mccormick_function(double *position, int n_variables);
static double michealewicz_function(double *position, int n_variables);

/* === Helper Structure for MPI Reduction (fitness, rank) === */
typedef struct
{
    double fitness;
    int rank;
} FitnessRank;

/* === Create Custom MPI Datatype for Parameters Struct === */
static void create_parameters_type(MPI_Datatype *param_type)
{
    int blocklengths[10] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    MPI_Aint offsets[10];
    offsets[0] = offsetof(Parameters, n_agents);
    offsets[1] = offsetof(Parameters, n_variables);
    offsets[2] = offsetof(Parameters, n_iterations);
    offsets[3] = offsetof(Parameters, lower_bound);
    offsets[4] = offsetof(Parameters, upper_bound);
    offsets[5] = offsetof(Parameters, f);
    offsets[6] = offsetof(Parameters, l);
    offsets[7] = offsetof(Parameters, R);
    offsets[8] = offsetof(Parameters, M);
    offsets[9] = offsetof(Parameters, scale);

    MPI_Datatype types[10] = {MPI_INT, MPI_INT, MPI_INT,
                              MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE,
                              MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};

    MPI_Type_create_struct(10, blocklengths, offsets, types, param_type);
    MPI_Type_commit(param_type);
}

/* === Optimization Loop ===
 * This function performs the main iterative optimization process.
 * It updates the best agent locally, synchronizes the best solution
 * among all processes via MPI_Allreduce/MPI_Bcast, logs progress, and
 * performs the EPO update.
 */
static void optimize(int n_iterations, int n_variables, Space *space, EPO *epo,
                     double (*fitness_function)(double *, int), FILE *csv_file, int rank)
{
    // Synchronize the best agent's position among all processes

    /* Prepare local best info for global reduction */
    FitnessRank local_best;
    local_best.fitness = space->best_agent.fitness;
    local_best.rank = rank;

    /* Global reduction to find the overall best agent (lowest fitness) */
    FitnessRank global_best;
    MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

    MPI_Bcast(space->best_agent.position, n_variables, MPI_DOUBLE, global_best.rank, MPI_COMM_WORLD);
    space->best_agent.fitness = fitness_function(space->best_agent.position, n_variables);

    for (int iteration = 1; iteration <= n_iterations; iteration++)
    {
        /* Update the local best agent based on the chosen fitness function */
        Agent *local_best_agent = get_best_agent(space, fitness_function);

        /* Prepare local best info for global reduction */
        FitnessRank local_best;
        local_best.fitness = local_best_agent->fitness;
        local_best.rank = rank;

        /* Global reduction to find the overall best agent (lowest fitness) */
        FitnessRank global_best;
        MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

        if (global_best.fitness < space->best_agent.fitness)
        {
            if (rank == global_best.rank)
            {
                /* Update the best agent in the space */
                memcpy(space->best_agent.position, local_best_agent->position, n_variables * sizeof(double));
            }

            /* Broadcast the best agent’s position from the process that had it */
            MPI_Bcast(space->best_agent.position, n_variables, MPI_DOUBLE, global_best.rank, MPI_COMM_WORLD);
            /* Update best agent's fitness based on the broadcast position */
            space->best_agent.fitness = fitness_function(space->best_agent.position, n_variables);
        }

        /* Root process prints and logs the current best fitness */
        if (rank == 0)
        {
            printf("Iteration %d: Best fitness = %.6f\n", iteration, space->best_agent.fitness);
            log_population(space, csv_file, temperature_profile(epo), iteration);
        }

        /* Update the EPO algorithm state */
        epo_update(epo, space);
    }

    // Synchronize the best agent's position among all processes
    MPI_Allreduce(&local_best, &global_best, 1, MPI_DOUBLE_INT, MPI_MINLOC, MPI_COMM_WORLD);

    if (global_best.fitness < space->best_agent.fitness)
    {
        /* Broadcast the best agent’s position from the process that had it */
        MPI_Bcast(space->best_agent.position, n_variables, MPI_DOUBLE, global_best.rank, MPI_COMM_WORLD);
        /* Update best agent's fitness based on the broadcast position */
        space->best_agent.fitness = fitness_function(space->best_agent.position, n_variables);
    }
}

int main(int argc, char *argv[])
{
    int rank, comm_size;
    clock_t start, end;

    start = clock();

    /* Initialize MPI */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    init_seeds(rank);

    /* === Read and Broadcast Parameters === */
    Parameters params;
    if (rank == 0)
    {
        /* Only the root reads CLI arguments */
        read_cli(argc, argv, &params);
    }
    MPI_Datatype MPI_PARAMS;
    create_parameters_type(&MPI_PARAMS);
    MPI_Bcast(&params, 1, MPI_PARAMS, 0, MPI_COMM_WORLD);
    MPI_Type_free(&MPI_PARAMS);

    /* Calculate the number of agents assigned to this process */
    int local_n_agents = params.n_agents / comm_size;
    if (rank < (params.n_agents % comm_size))
        local_n_agents++;

    /* === Setup CSV Logging on the Root Process === */
    FILE *csv_file = NULL;
    if (rank == 0)
    {
        const char *field_names[] = {"Iteration", "Fitness", "Position", "TempP"};
        csv_file = csv_open("output.csv", field_names, 4);
        if (!csv_file)
        {
            fprintf(stderr, "Failed to open output.csv\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
    }

    clock_t init_start, init_end;
    init_start = clock();

    /* === Initialize Search Space and EPO Algorithm Structures === */
    Space space;
    init_space(&space, local_n_agents, params.n_variables, params.lower_bound, params.upper_bound);

    EPO epo;
    init_epo(&epo, params.R, params.M, params.f, params.l, params.n_iterations, params.scale);

    init_end = clock();

    /* Select the fitness function to use (change as desired) */
    double (*fitness_function)(double *, int) = michealewicz_function;

    clock_t opt_start, opt_end;
    opt_start = clock();

    /* === Run the Optimization Loop === */
    optimize(params.n_iterations, params.n_variables, &space, &epo, fitness_function, csv_file, rank);

    opt_end = clock();

    free_space(&space);
    MPI_Finalize();

    /* === Final Output and Cleanup === */
    if (rank == 0)
    {
        printf("Best agent found:\n");
        printf("Fitness: %.6f\n", space.best_agent.fitness);
        csv_close(csv_file);

        end = clock();

        double init_time = (double)(init_end - init_start) / CLOCKS_PER_SEC;
        double opt_time = (double)(opt_end - opt_start) / CLOCKS_PER_SEC;
        double total_time = (double)(end - start) / CLOCKS_PER_SEC;

        printf("Initialization time: %.6f seconds\n", init_time);
        printf("Optimization time: %.6f seconds\n", opt_time);
        printf("Total time: %.6f seconds\n", total_time);
    }
    return 0;
}

/* === Fitness Function Implementations === */

static double sphere_function(double *position, int n_variables)
{
    double fitness = 0.0;
    for (int i = 0; i < n_variables; i++)
    {
        fitness += position[i] * position[i];
    }
    return fitness;
}

static double matyas_function(double *position, int n_variables)
{
    if (n_variables < 2)
        return 0.0; // Requires at least 2 dimensions
    double x = position[0], y = position[1];
    return 0.26 * (x * x + y * y) - 0.48 * (x * y);
}

static double bukin_function(double *position, int n_variables)
{
    if (n_variables < 2)
        return 0.0; // Requires at least 2 dimensions
    double x = position[0], y = position[1];
    return 100.0 * sqrt(fabs(y - 0.01 * x * x)) + 0.01 * fabs(x + 10.0);
}

static double mccormick_function(double *position, int n_variables)
{
    if (n_variables < 2)
        return 0.0; // Requires at least 2 dimensions
    double x = position[0], y = position[1];
    return sin(x + y) + (x - y) * (x - y) - 1.5 * x + 2.5 * y + 1.0;
}

static double michealewicz_function(double *position, int n_variables)
{
    double fitness = 0.0;
    for (int i = 0; i < n_variables; i++)
    {
        fitness += -sin(position[i]) * pow(sin(((i + 1) * position[i] * position[i]) / M_PI), 20);
    }
    return fitness;
}
