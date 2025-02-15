#include <stdlib.h>
#include <math.h>

#include "space.h"
#include "agent.h"

// Initialize the search space
void init_space(Space *space, int n_agents, int n_variables, double lower_bound, double upper_bound)
{
    space->n_agents = n_agents;
    space->n_variables = n_variables;
    space->lower_bound = lower_bound;
    space->upper_bound = upper_bound;

    // Allocate memory for agents
    space->agents = (Agent *)malloc(n_agents * sizeof(Agent));

    // Initialize all agents
    for (int i = 0; i < n_agents; i++)
    {
        init_agent(&space->agents[i], n_variables, lower_bound, upper_bound);
    }

    // Initialize the best agent
    init_agent(&space->best_agent, n_variables, lower_bound, upper_bound);
}

// Free memory allocated for the search space
void free_space(Space *space)
{
    for (int i = 0; i < space->n_agents; i++)
    {
        free_agent(&space->agents[i]);
    }
    free(space->agents);
    free_agent(&space->best_agent);
}

// Update the best agent based on fitness values
void update_best_agent(Space *space, double (*fitness_function)(double *, int))
{
    for (int i = 0; i < space->n_agents; i++)
    {
        // Calculate fitness for each agent
        space->agents[i].fitness = fitness_function(space->agents[i].position, space->n_variables);

        // Update the best agent if the current agent is better
        if (space->agents[i].fitness < space->best_agent.fitness)
        {
            space->best_agent.fitness = space->agents[i].fitness;
            for (int j = 0; j < space->n_variables; j++)
            {
                space->best_agent.position[j] = space->agents[i].position[j];
            }
        }
    }
}

Agent *get_best_agent(Space *space, double (*fitness_function)(double *, int))
{
    int best_agent_index = 0;
    double best_fitness = fitness_function(space->agents[0].position, space->n_variables);
    for (int i = 1; i < space->n_agents; i++)
    {
        space->agents[i].fitness = fitness_function(space->agents[i].position, space->n_variables);
        if (space->agents[i].fitness < best_fitness)
        {
            best_fitness = space->agents[i].fitness;
            best_agent_index = i;
        }
    }
    return &space->agents[best_agent_index];
}