# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
INC = -I lib

# Targets
TARGET = epo.out

# Source files
MAIN = main.c
LIB_SRC = lib/agent.c lib/utils.c lib/space.c lib/epo.c
SOURCES = $(MAIN) $(LIB_SRC)

# Object files
MAIN_OBJ = $(MAIN:.c=.o)
LIB_OBJ = $(LIB_SRC:.c=.o)
OBJECTS = $(MAIN_OBJ) $(LIB_OBJ)

# Build the target
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS) $(INC)

# Build object files
%.o: %.c
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: clean