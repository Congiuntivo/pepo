#include <stdlib.h>
#include <math.h>

#include "agent.h"
#include "utils.h" // Utility functions for random_double

// Initialize an agent by allocating memory and assigning random positions
void init_agent(Agent *agent, int n_variables, double lower_bound, double upper_bound)
{
    agent->n_variables = n_variables;
    agent->fitness = INFINITY;

    // Allocate memory for agent's position
    agent->position = (double *)malloc(n_variables * sizeof(double));

    // Initialize the position randomly within bounds
    for (int i = 0; i < n_variables; i++)
    {
        agent->position[i] = random_double(lower_bound, upper_bound);
    }
}

// Free memory allocated for an agent
void free_agent(Agent *agent)
{
    free(agent->position);
}