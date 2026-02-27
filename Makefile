# Variables
CC := clang
CFLAGS := -Wall -Wextra -I/Users/jcrm/Documents/OpenGL/glfw-3.4/include -I/Users/jcrm/Documents/OpenGL/glad/include -I.
LDFLAGS := -mmacosx-version-min=15.4
LDLIBS := -L/Users/jcrm/Documents/OpenGL/glfw-3.4/build/src -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit

GLAD_SRC := /Users/jcrm/Documents/OpenGL/glad/src/glad.c

SRC := src/main.c $(GLAD_SRC)
OBJ := $(SRC:.c=.o)
BIN := main

.PHONY: all clean default

# Default target
default: all

# Build target
all: $(BIN)

# Link the executable
$(BIN): $(OBJ)
	$(CC) $(OBJ) $(LDFLAGS) $(LDLIBS) -o $@

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(BIN)
