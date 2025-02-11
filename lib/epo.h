#ifndef EPO_H
#define EPO_H

#include "space.h"

// Define the EPO structure to encapsulate parameters
typedef struct
{
    double R; // Huddle radius
    double M; // M is the movement parameter that maintains a gap between search agents for collision avoidance. The value of parameter M is set to 2
    double f; // Exploration control parameter in [2, 3]
    double l; // Exploitation control parameter in [1.5, 2]
    int itr; // Current iteration
    int max_itr; // Maximum number of iterations
    double scale; // Scale factor
} EPO;

// Function to initialize the EPO structure
void init_epo(EPO *epo, double R, double M, double f, double l, int max_itr, double scale);

double temperature_profile(EPO *epo);

// Function to update the space using the EPO algorithm
void epo_update(EPO *epo, Space *space);

#endif