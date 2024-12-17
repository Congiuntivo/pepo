#ifndef SPACE_H
#define SPACE_H

#include "agent.h"

// Define the Space structure
typedef struct
{
    Agent *agents;      // Array of agents in the search space
    Agent best_agent;   // The best agent (global optimum found so far)
    int n_agents;       // Number of agents (population size)
    int n_variables;    // Number of dimensions per agent
    double lower_bound; // Lower bound for variables
    double upper_bound; // Upper bound for variables
} Space;

// Function prototypes for space-related operations

// Initialize the search space and agents
void init_space(Space *space, int n_agents, int n_variables, double lower_bound, double upper_bound);

// Free memory used by the search space
void free_space(Space *space);

// Update the best agent in the search space
void update_best_agent(Space *space, double (*fitness_function)(double *, int));

#endif