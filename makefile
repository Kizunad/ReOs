OS_NAME := Os

CC := i686-elf-gcc
AS := i686-elf-as

CFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LDFLAGS := -ffreestanding -O2 -nostdlib -lgcc

BUILD_DIR := build
OBJECT_DIR := $(BUILD_DIR)/obj
ISO_DIR := $(BUILD_DIR)/iso
BOOT_DIR := $(ISO_DIR)/boot
GRUB_DIR := $(BOOT_DIR)/grub
BIN_DIR := $(BUILD_DIR)/bin

SOURCE_FILES := $(shell find -name "*.[cS]")
INCLUDES_DIR := includes libc/includes
INCLUDES := $(patsubst %, -I%, $(INCLUDES_DIR))

OBJECT_FILES := $(patsubst ./%, $(OBJECT_DIR)/%.o, $(SOURCE_FILES))

directory_build:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJECT_DIR)
	@mkdir -p $(ISO_DIR)
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(GRUB_DIR)
	@mkdir -p $(BIN_DIR)

find_source:
	@echo $(SOURCE_FILES)
	
$(OBJECT_DIR)/%.c.o : %.c
	@mkdir -p $(dir $@)
	@echo "Compiling: $< -> $@"
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(OBJECT_DIR)/%.S.o : %.S
	@mkdir -p $(dir $@)
	@echo "Compiling: $< -> $@"
	$(AS) $(INCLUDES) -c $< -o $@

compile_source: $(OBJECT_FILES)

link:
	$(CC) -T linker.ld -o $(BIN_DIR)/$(OS_NAME).bin $(OBJECT_FILES) $(LDFLAGS)

grub:
	@cp $(BIN_DIR)/$(OS_NAME).bin $(BOOT_DIR)/$(OS_NAME).bin
	@cp grub.cfg $(GRUB_DIR)/grub.cfg
	@grub-mkrescue -o $(BUILD_DIR)/$(OS_NAME).iso $(ISO_DIR)

clean:
	@rm -rf $(BUILD_DIR)

run: clean directory_build compile_source link grub
	@echo "Finished Build"
	@qemu-system-i386 -cdrom $(BUILD_DIR)/$(OS_NAME).iso


.PHONY: directory_build find_source compile_source link grub clean run
