#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

#include "epo.h"
#include "utils.h"

// Calculate the double social force
double social_force(EPO *epo);

// Initialize the EPO structure with given parameters
void init_epo(EPO *epo, double R, double M, double f, double l, int max_itr, double scale)
{
    epo->f = f;
    epo->l = l;
    epo->M = M;
    epo->R = R;
    epo->itr = 0;
    epo->max_itr = max_itr;
    epo->scale = scale;
}

double temperature_profile(EPO *epo)
{
    // Calculate the temperature profile according to Eq. 7
    double T = (epo->R > 1) ? 0 : 1;
    double T_p = T - ((double)epo->max_itr / (epo->itr - epo->max_itr));
    return T_p;
}

// EPO algorithm update: Updates agents' positions in the search space
void epo_update(EPO *epo, Space *space)
{
    // Get number of threads
    int num_threads = omp_get_max_threads();

    // Temporary arrays for each thread
    double **P_grid = (double **)malloc(num_threads * sizeof(double *));
    double **A = (double **)malloc(num_threads * sizeof(double *));
    double **D_ep = (double **)malloc(num_threads * sizeof(double *));
    for (int i = 0; i < num_threads; i++)
    {
        P_grid[i] = (double *)malloc(space->n_variables * sizeof(double));
        A[i] = (double *)malloc(space->n_variables * sizeof(double));
        D_ep[i] = (double *)malloc(space->n_variables * sizeof(double));
    }

    // Temperature profile (Eq. 7)
    double T_p = temperature_profile(epo);

#define MAX_VARIABLES 500
    double best_agent_position[MAX_VARIABLES];
    memcpy(best_agent_position, space->best_agent.position, space->n_variables * sizeof(double));

#pragma omp parallel for schedule(dynamic) shared(epo, space, T_p, P_grid, A, D_ep)
    for (int current_peng = 0; current_peng < space->n_agents; current_peng++)
    {
        int thread_num = omp_get_thread_num();

        double *current_agent_position = space->agents[current_peng].position;

        // Polygon grid accuracy (Eq. 10)
        for (int j = 0; j < space->n_variables; j++)
        {
            P_grid[thread_num][j] = fabs(best_agent_position[j] - current_agent_position[j]);
        }

        // Avoidance coefficient (Eq. 9)
        double rand_num = random_double(0, 1);
        for (int j = 0; j < space->n_variables; j++)
        {
            A[thread_num][j] = epo->M * (T_p + P_grid[thread_num][j]) * rand_num - T_p;
        }

        double S = social_force(epo);

        // Distance between current agent and emperor penguin (Eq. 8)
        for (int j = 0; j < space->n_variables; j++)
        {
            double C = random_double(0, 1);
            D_ep[thread_num][j] = fabs((S * best_agent_position[j]) - (current_agent_position[j] * C));
        }

        // Update position (Eq. 13)
        for (int j = 0; j < space->n_variables; j++)
        {
            double update = A[thread_num][j] * D_ep[thread_num][j] * epo->scale;
            space->agents[current_peng].position[j] -= update;
            update *= random_double(-1, 1);
            current_agent_position[j] = best_agent_position[j] - update;

            // Clamp position to bounds
            if (current_agent_position[j] < space->lower_bound)
            {
                current_agent_position[j] = space->lower_bound;
            }
            else if (current_agent_position[j] > space->upper_bound)
            {
                current_agent_position[j] = space->upper_bound;
            }
        }
    }

    // Free temporary arrays
    for (int i = 0; i < num_threads; i++)
    {
        free(P_grid[i]);
        free(A[i]);
        free(D_ep[i]);
    }
    free(P_grid);
    free(A);
    free(D_ep);

    // Increment iterations
    epo->itr++;
}

double social_force(EPO *epo)
{
    // Calculate the social force according to Eq. 12
    double sf = 0.0;
    sf = epo->f * exp(-((double)epo->itr / epo->l)) - exp(-epo->itr);
    sf = pow(sf, 2);
    sf = sqrt(sf);
    return sf;
}