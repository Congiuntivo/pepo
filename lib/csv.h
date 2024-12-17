#ifndef CSV_H
#define CSV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Constants
#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 128

// Function Prototypes
FILE *csv_open(const char *filename, const char** field_names, int n_fields);
void csv_close(FILE *file);

bool csv_write_row(FILE *file, const char **fields, int n_fields);

#endif