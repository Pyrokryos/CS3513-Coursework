# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Target executable
TARGET = myrpal

# Source directory
SRC_DIR = src

# Include directory
INCLUDE_DIR = include

# Build directory
BUILD_DIR = build

# Test directory
TEST_DIR = tests

# Generate object file names
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Rule to compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to link object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -g $^ -o $@ -lm

# Phony targets
.PHONY: clean debug run test

# Target to clean the build artifacts and test artifacts
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)
	rm -rf $(TEST_DIR)/generated/*

# Target to run the executable in debug mode using gdb
debug: $(TARGET)
	gdb ./$(TARGET)

# Target to run the executable
run: $(TARGET)
	./$(TARGET)

# Test target
test: $(TARGET)
	./eval_tests.sh --diff-only
