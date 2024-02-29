# Makefile for building TheGame kernel
TARGET_ELF=kernel.elf
TARGET_DIR=../bin/kernel/

INCLUDE_DIR=../

AS = i686-linux-gnu-as
CC = i686-linux-gnu-gcc-11

# if we're on Windows use WSL to run bash commands

BASH =


LDFLAGS=-T linker.ld -ffreestanding -nostdlib -lgcc 

CCFLAGS=-std=gnu99 -ffreestanding -Wall -Wextra -I $(INCLUDE_DIR)


OBJ =\
	boot.o kernel.o 			\
	libk/string.o 				\
	libk/math.o 				\
	libk/assert.o				\
	gdt.o 						\
	gdt_asm.o 					\
	idt.o 						\
	idt_asm.o 					\
	tss.o 						\
	tss_asm.o 					\
	tty.o						\
	cpu_detect.o 				\
	interrupts.o 				\
	interrupts_asm.o 			\
	paging.o 					\
	graphics/vesafb.o 			\
	io/ports.o					\
	io/qemu_log.o 				\
	devices/keyboard.o			\
	devices/timer.o 			\
	mm/phys_memory.o 			\
	mm/virt_memory.o			\
	mm/kheap.o 					\
	fs/vfs.o					\
	graphics/vgafnt.o			\
	snake.o

all: $(TARGET_ELF)

$(TARGET_ELF): $(OBJ)
	$(BASH) mkdir -p $(TARGET_DIR)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF) $(OBJ) -lSDL2

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $< 

%.o: %.s
	$(AS) -o $@ $<

clean:
	$(BASH) rm -rf *.o
	$(BASH) rm -rf io/*.o
	$(BASH) rm -rf devices/*.o
	$(BASH) rm -rf mm/*.o
	$(BASH) rm -rf graphics/*.o
	$(BASH) rm -rf fs/*.o
	$(BASH) rm -rf pm/*.o
	$(BASH) rm -rf libk/*.o
	$(BASH) rm -rf gui/*.o
	$(BASH) rm -rf $(TARGET_DIR)kernel.elf

