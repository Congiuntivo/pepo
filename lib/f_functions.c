#include "f_functions.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/* === Fitness Function Implementations === */

// Funcion to get a function pointer based on the name of the function
fitness_function_t get_fitness_function(const char *name)
{
    if (strcmp(name, "sphere") == 0)
        return sphere_function;
    if (strcmp(name, "matyas") == 0)
        return matyas_function;
    if (strcmp(name, "bukin") == 0)
        return bukin_function;
    if (strcmp(name, "mccormick") == 0)
        return mccormick_function;
    if (strcmp(name, "michealewicz") == 0)
        return michealewicz_function;
    return NULL;
}

double sphere_function(double *position, int n_variables)
{
    double fitness = 0.0;
    for (int i = 0; i < n_variables; i++)
    {
        fitness += position[i] * position[i];
    }
    return fitness;
}

double matyas_function(double *position, int n_variables)
{
    if (n_variables < 2)
        return 0.0; // Requires at least 2 dimensions
    double x = position[0], y = position[1];
    return 0.26 * (x * x + y * y) - 0.48 * (x * y);
}

double bukin_function(double *position, int n_variables)
{
    if (n_variables < 2)
        return 0.0; // Requires at least 2 dimensions
    double x = position[0], y = position[1];
    return 100.0 * sqrt(fabs(y - 0.01 * x * x)) + 0.01 * fabs(x + 10.0);
}

double mccormick_function(double *position, int n_variables)
{
    if (n_variables < 2)
        return 0.0; // Requires at least 2 dimensions
    double x = position[0], y = position[1];
    return sin(x + y) + (x - y) * (x - y) - 1.5 * x + 2.5 * y + 1.0;
}

double michealewicz_function(double *position, int n_variables)
{
    double fitness = 0.0;
    for (int i = 0; i < n_variables; i++)
    {
        fitness += -sin(position[i]) * pow(sin(((i + 1) * position[i] * position[i]) / M_PI), 20);
    }
    return fitness;
}
