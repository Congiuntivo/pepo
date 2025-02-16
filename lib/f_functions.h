#ifndef F_FUNCTIONS_H
#define F_FUNCTIONS_H

typedef double (*fitness_function_t)(double *, int);

fitness_function_t get_fitness_function(const char *name);

/* === Fitness Function Prototypes === */
double matyas_function(double *position, int n_variables);
double sphere_function(double *position, int n_variables);
double bukin_function(double *position, int n_variables);
double mccormick_function(double *position, int n_variables);
double michealewicz_function(double *position, int n_variables);


#define SPHERE "sphere"
#define MATYAS "matyas"
#define BUKIN "bukin"
#define MCCORMICK "mccormick"
#define MICHEALEWICZ "michealewicz"

#define MAX_FUN_NAME_LEN 20

#endif