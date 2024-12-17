# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -lm

# Targets and sources
TARGET = epo
SOURCES = main.c epo.c space.c agent.c utils.c
HEADERS = epo.h space.h agent.h utils.h

# Build target
$(TARGET): $(SOURCES) $(HEADERS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS)

# Clean up object files and executables
clean:
	rm -f $(TARGET)