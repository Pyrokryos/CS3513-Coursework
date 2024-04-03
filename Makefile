CC = gcc                  # Compiler
CFLAGS = -Wall -Wextra -g # Compiler flags
TARGET = myrpal           # Target executable
INCLUDE_DIR = include     # Include directory
SRC_DIR = src             # Source directory
BUILD_DIR = build         # Build directory

SRCS = $(wildcard $(SRC_DIR)/*.c)              # Get all source files.
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o) # Generate object file names.

# Rule to compile each source file into object files.
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to build the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -g $^ -o $@

.PHONY: clean debug run # Phony targets

# Target to clean the build artifacts.
clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)

# Target to run the executable in debug mode using gdb.
debug: $(TARGET)
	gdb ./$(TARGET)

# Target to run the executable.
run: $(TARGET)
	./$(TARGET)
