#include "utils.h"
#include "csv.h"

// Helper function to format the positions of the agents into a single string
void format_position_string(char *position_str, size_t position_str_size, const double *position, int n_variables);


// Generate random double between `lower` and `upper`
double random_double(double lower, double upper)
{
    return ((double)rand() / RAND_MAX) * (upper - lower) + lower;
}

// Logs the population to a CSV file
void log_population(Space *space, FILE *file, double temperature_profile, int iteration)
{
    // Constants for buffer sizes
    const size_t ITERATION_STR_SIZE = 12;
    const size_t FITNESS_STR_SIZE = 50;
    const size_t POSITION_STR_SIZE = 200;
    const size_t TEMP_PROFILE_STR_SIZE = 50;

    // Create and format iteration and temperature profile strings
    char iteration_str[ITERATION_STR_SIZE];
    char tempP_str[TEMP_PROFILE_STR_SIZE];
    snprintf(iteration_str, sizeof(iteration_str), "%d", iteration);
    snprintf(tempP_str, sizeof(tempP_str), "%.6f", temperature_profile);

    // Loop over all agents in the space
    for (int i = 0; i < space->n_agents; i++)
    {
        // Create buffers for fitness and position
        char fitness_str[FITNESS_STR_SIZE];
        snprintf(fitness_str, sizeof(fitness_str), "%.6f", space->agents[i].fitness);

        char position_str[POSITION_STR_SIZE];
        format_position_string(position_str, sizeof(position_str), space->agents[i].position, space->n_variables);

        // Prepare the row values
        const char *row_values[4] = {iteration_str, fitness_str, position_str, tempP_str};

        // Write to CSV file
        csv_write_row(file, row_values, 4);
    }

    // Write the best agent to the CSV file
    char fitness_str[FITNESS_STR_SIZE];
    snprintf(fitness_str, sizeof(fitness_str), "%.6f", space->best_agent.fitness);

    char position_str[POSITION_STR_SIZE];
    format_position_string(position_str, sizeof(position_str), space->best_agent.position, space->n_variables);

    const char *row_values[4] = {iteration_str, fitness_str, position_str, tempP_str};
    csv_write_row(file, row_values, 4);
}

// Helper function to format the positions of the agents into a single string
void format_position_string(char *position_str, size_t position_str_size, const double *position, int n_variables)
{
    position_str[0] = '\0'; // Initialize the position string to be empty
    char temp[50];          // Temporary buffer for formatting each variable

    for (int j = 0; j < n_variables; j++)
    {
        // Append the current value to the position string
        snprintf(temp, sizeof(temp), "%.6f", position[j]);
        strncat(position_str, temp, position_str_size - strlen(position_str) - 1);

        // Append an underscore if it's not the last variable
        if (j < n_variables - 1)
        {
            strncat(position_str, "_", position_str_size - strlen(position_str) - 1);
        }
    }
}