#include "csv.h"

// Opens a CSV file for writing, writes the header (field names), and returns the file pointer
FILE *csv_open(const char *filename, const char **field_names, int n_fields) {
    // Validate input
    if (!filename || !field_names || n_fields <= 0) {
        fprintf(stderr, "Invalid arguments to csv_open.\n");
        return NULL;
    }

    // Open file for writing
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Failed to open CSV file");
        return NULL;
    }

    // Write the header row (field names)
    for (int i = 0; i < n_fields; i++) {
        // If the field name contains a comma, wrap it in quotes
        if (strchr(field_names[i], ',')) {
            fprintf(file, "\"%s\"", field_names[i]);
        } else {
            fprintf(file, "%s", field_names[i]);
        }

        // Write a comma unless it's the last field
        if (i < n_fields - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n"); // End the header row

    return file;
}

// Closes the CSV file
void csv_close(FILE *file) {
    if (file) {
        fclose(file);
    }
}

// Writes a single row to the CSV file
bool csv_write_row(FILE *file, const char **fields, int n_fields) {
    // Validate input
    if (!file || !fields || n_fields <= 0) {
        fprintf(stderr, "Invalid arguments to csv_write_row.\n");
        return false;
    }

    // Write the fields
    for (int i = 0; i < n_fields; i++) {
        // If the field contains a comma, wrap it in quotes
        if (strchr(fields[i], ',')) {
            fprintf(file, "\"%s\"", fields[i]);
        } else {
            fprintf(file, "%s", fields[i]);
        }

        // Write a comma unless it's the last field
        if (i < n_fields - 1) {
            fprintf(file, ",");
        }
    }
    fprintf(file, "\n"); // End the row

    return true;
}