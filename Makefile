CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = myrpal
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -g $^ -o $@

.PHONY: clean run debug

run: $(TARGET)
	./$(TARGET)

debug: $(TARGET)
	gdb ./$(TARGET)

clean:
	rm -f $(BUILD_DIR)/*.o $(TARGET)
