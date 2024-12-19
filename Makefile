# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm
INC = -I lib

# Targets
TARGET = epo.out

# Source files
MAIN = main.c
LIB_SRC = agent.c utils.c space.c epo.c csv.c
LIB_SRC := $(addprefix lib/, $(LIB_SRC))

SOURCES = $(MAIN) $(LIB_SRC)

# Object file directory
BUILD_DIR = build

# Object files
MAIN_OBJ = $(BUILD_DIR)/$(MAIN:.c=.o)
LIB_OBJ = $(LIB_SRC:lib/%.c=$(BUILD_DIR)/%.o)
OBJECTS = $(MAIN_OBJ) $(LIB_OBJ)

# Default target
all: $(TARGET)

# Build the target
$(TARGET): $(BUILD_DIR) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(CFLAGS) $(INC)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	@echo "Creating directory: $(BUILD_DIR)"
	mkdir -p $(BUILD_DIR)

# Compile library files into object files
$(BUILD_DIR)/%.o: lib/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Compile main file into object file
$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm -rf $(BUILD_DIR)

# Phony targets
.PHONY: all clean