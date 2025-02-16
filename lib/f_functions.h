#ifndef F_FUNCTIONS_H
#define F_FUNCTIONS_H

/* === Fitness Function Prototypes === */
double matyas_function(double *position, int n_variables);
double sphere_function(double *position, int n_variables);
double bukin_function(double *position, int n_variables);
double mccormick_function(double *position, int n_variables);
double michealewicz_function(double *position, int n_variables);

#endif