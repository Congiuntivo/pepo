#ifndef AGENT_H
#define AGENT_H

// Define the Agent structure
typedef struct
{
    double *position; // Array to hold agent's position (solution)
    int n_variables;  // Number of dimensions (or variables)
    double fitness;   // Fitness value of the agent
} Agent;

// Function prototypes for agent-related operations

// Agent intializer
void init_agent(Agent *agent, int n_variables, double lower_bound, double upper_bound);

// Agent destructor
void free_agent(Agent *agent);

#endif