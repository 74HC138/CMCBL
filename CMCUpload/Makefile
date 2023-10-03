# Directories
SRC_DIR := source
BUILD_DIR := build
BIN_DIR := .
INCLUDE_DIR := include
SRC_FILES := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_FILES))
TARGET := $(BIN_DIR)/CMCUpload

# Compiler and flags
CC := gcc
CFLAGS := -Wall -Wextra -g -I$(INCLUDE_DIR) -lm

# Targets
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR) $(TARGET)

install: $(TARGET)
	cp CMCUpload /bin/.

.PHONY: all clean
