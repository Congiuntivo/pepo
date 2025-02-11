#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "epo.h"
#include "utils.h"

// Calculate the double social force
double social_force(EPO *epo);

// Initialize the EPO structure with given parameters
void init_epo(EPO *epo, double R, double M, double f, double l, int max_itr)
{
    epo->f = f;
    epo->l = l;
    epo->M = M;
    epo->R = R;
    epo->itr = 0;
    epo->max_itr = max_itr;
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
    // Temperature profile (Eq. 7)
    double T_p = temperature_profile(epo);
    for (int current_peng = 0; current_peng < space->n_agents; current_peng++)
    {
        // Polygon grid accuracy (Eq. 10)
        double *P_grid = (double *)malloc(space->n_variables * sizeof(double));
        for (int j = 0; j < space->n_variables; j++)
        {
            P_grid[j] = fabs(space->best_agent.position[j] - space->agents[current_peng].position[j]);
        }

        // DEBUG: Print the P_grid
        printf("P_grid:\n\t");
        for (int j = 0; j < space->n_variables; j++)
        {
            printf("%f ", P_grid[j]);
        }
        printf("\n");
        // END DEBUG

        // Avoidance coefficient (Eq. 9)
        double *A = (double *)malloc(space->n_variables * sizeof(double));
        double rand_num = random_double(0, 1);
        for (int j = 0; j < space->n_variables; j++)
        {
            A[j] = epo->M * (T_p + P_grid[j]) * rand_num - T_p;
        }

        // DEBUG: Print A vector
        printf("A:\n\t");
        for (int j = 0; j < space->n_variables; j++)
        {
            printf("%f ", A[j]);
        }
        printf("\n");
        // END DEBUG

        // Social forces (Eq. 12)
        // TODO: Check if f & l need to be randomized
        double S = social_force(epo);

        // DEBUG: Print the social force
        printf("S: %f\n", S);

        // Distance between current agent and emperor penguin (Eq. 8)
        double *D_ep = (double *)malloc(space->n_variables * sizeof(double));
        for (int j = 0; j < space->n_variables; j++)
        {
            double C = random_double(0, 1);
            D_ep[j] = fabs((S * space->best_agent.position[j]) - (space->agents[current_peng].position[j] * C));
            printf("%f ", D_ep[j]);
        }

        // DEBUG: Print D_ep vector
        printf("D_ep:\n\t");
        for (int j = 0; j < space->n_variables; j++)
        {
            printf("%f ", D_ep[j]);
        }
        printf("\n");
        // END DEBUG

        // DEBUG: Print the current agent's position
        printf("Old position:\n\t");
        for (int j = 0; j < space->n_variables; j++)
        {
            printf("%f ", space->agents[current_peng].position[j]);
        }
        printf("\nUpdates:\n\t");
        // END DEBUG

        // Update position (Eq. 13)
        for (int j = 0; j < space->n_variables; j++)
        {
            double update = A[j] * D_ep[j];
            space->agents[current_peng].position[j] = space->best_agent.position[j] - update;
            // space->agents[current_peng].position[j] = space->best_agent.position[j] - update;
            // DEBUG: Print the update
            printf("%f ", update);
            // END DEBUG

            // Clamp position to bounds
            if (space->agents[current_peng].position[j] < space->lower_bound)
            {
                space->agents[current_peng].position[j] = space->lower_bound;
            }
            else if (space->agents[current_peng].position[j] > space->upper_bound)
            {
                space->agents[current_peng].position[j] = space->upper_bound;
            }
        }

        // DEBUG: Print the current agent's position
        printf("\nNew position:\n\t");
        for (int j = 0; j < space->n_variables; j++)
        {
            printf("%f ", space->agents[current_peng].position[j]);
        }
        printf("\n\n");
        // END DEBUG

        // TODO: Free once and malloc once
        // Free temporary arrays
        free(P_grid);
        free(A);
        free(D_ep); 
    }
    epo->itr++;
}

double social_force(EPO *epo)
{
    // Calculate the social force according to Eq. 12
    double sf = 0.0;
    sf = epo->f * exp(-((double)epo->itr / epo->l)) - exp(-epo->itr);
    sf = pow(sf, 2);
    sf = sqrt(sf);
    // TODO: Check if needs only to be squared or both even if it makes no sens
    return sf;
}