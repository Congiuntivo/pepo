#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include "space.h"

// Helper function to format the positions of the agents into a single string
void init_seeds(int seed);

// Generate random double between bounds
double random_double(double lower, double upper);

// Logs the population to a CSV file
void log_population(Space *space, FILE *file, double temperature_profile, int iteration);

#endif