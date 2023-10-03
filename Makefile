# Directories
SRC_DIR := source
BUILD_DIR := build

# Tools
ASSEMBLER := vasmz80_oldstyle
ASSEMBLER_FLAGS := -Fbin -L $(BUILD_DIR)/listing.txt -o

# Find all assembly files in the source directory
ASM_FILES := $(SRC_DIR)/main.asm

# Generate corresponding object filenames in the build directory
OBJ_FILES := $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.bin,$(ASM_FILES))

# Targets
all: $(OBJ_FILES)

# Rule to assemble .asm files into .bin files
$(BUILD_DIR)/%.bin: $(SRC_DIR)/%.asm | $(BUILD_DIR)
	$(ASSEMBLER) $(ASSEMBLER_FLAGS) $@ $<

# Rule to create the build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean rule to remove build directory and all its contents
clean:
	rm -rf $(BUILD_DIR)

bootBin: all
	dd if=$(BUILD_DIR)/main.bin of=$(BUILD_DIR)/boot.bin bs=16k conv=sync

.PHONY: all clean
