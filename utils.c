#include "utils.h"

// Generate random double between `lower` and `upper`
double random_double(double lower, double upper)
{
    return ((double)rand() / RAND_MAX) * (upper - lower) + lower;
}