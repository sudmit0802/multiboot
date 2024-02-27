ISODIR := iso
MULTIBOOT := $(ISODIR)/boot/my_game.elf
MAIN := my_game.img

CC := gcc
CFLAGS := -m32 -Wall -fno-builtin -nostdinc -nostdlib -std=gnu99
LDFLAGS := -ffreestanding -m32 -nostdlib
AS := as

SRC_DIR := common x86emu
OBJ_DIR := obj

SRC_C := $(wildcard $(addsuffix /*.c,$(SRC_DIR)))
SRC_S := $(wildcard $(addsuffix /*.s,$(SRC_DIR)))

SRC_OBJ := $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC_C)))
SRC_OBJ += $(patsubst %.s,$(OBJ_DIR)/%.o,$(notdir $(SRC_S)))

$(OBJ_DIR)/%.o: common/%.c
	$(CC) -Ix86emu -Iinclude $(CFLAGS) -o $@ -c $<

$(OBJ_DIR)/%.o: common/%.s
	$(AS) -32 $< -o $@

$(OBJ_DIR)/%.o: x86emu/%.c
	$(CC) -Ix86emu -Iinclude $(CFLAGS) -o $@ -c $<

$(MULTIBOOT): $(SRC_OBJ)
	$(CC) $(LDFLAGS) -o '$@' -T linker.ld $^ -lgcc
	grub-mkrescue -o '$(MAIN)' '$(ISODIR)'

all: $(MULTIBOOT)

clean:
	rm -rf $(OBJ_DIR)/*.o
	rm -rf $(MAIN)

rebuild: clean all

run: rebuild
	qemu-system-i386 \
    -monitor stdio \
    -vga std \
    -machine accel=tcg \
    -m 2G \
    -no-reboot \
    -no-shutdown \
    -drive 'file=$(MAIN),format=raw' \
    -boot once=c,menu=on \
    -net none

.PHONY: all clean rebuild run