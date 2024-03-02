# Makefile for building TheGame kernel
TARGET_ELF1=kernel1.elf
TARGET_ELF2=kernel2.elf
TARGET_DIR=../bin/kernel/

INCLUDE_DIR=../

AS = i686-linux-gnu-as
CC = i686-linux-gnu-gcc-11

# if we're on Windows use WSL to run bash commands

BASH =


LDFLAGS=-T linker.ld -ffreestanding -nostdlib -lgcc 

CCFLAGS=-std=gnu99 -ffreestanding -Wall -Wextra -I $(INCLUDE_DIR)


OBJ1 =\
	boot.o						\
	kernel1.o 					\
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
	graphics/vgafnt.o			\
	graphics/render.o			\
	io/ports.o					\
	io/qemu_log.o 				\
	devices/keyboard.o			\
	devices/timer.o 			\
	mm/phys_memory.o 			\
	mm/virt_memory.o			\
	mm/kheap.o 					\
	fs/vfs.o					\
	snake.o


OBJ2 =\
	boot.o kernel2.o 			\
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
	graphics/vgafnt.o			\
	graphics/render.o			\
	io/ports.o					\
	io/qemu_log.o 				\
	devices/keyboard.o			\
	devices/timer.o 			\
	mm/phys_memory.o 			\
	mm/virt_memory.o			\
	mm/kheap.o 					\
	fs/vfs.o					\
	tetris.o					

all: $(TARGET_ELF1) $(TARGET_ELF2)

$(TARGET_ELF1): $(OBJ1)
	$(BASH) mkdir -p $(TARGET_DIR)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF1) $(OBJ1)

$(TARGET_ELF2): $(OBJ2)
	$(CC) $(LDFLAGS) -o $(TARGET_DIR)$(TARGET_ELF2) $(OBJ2)

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
	$(BASH) rm -rf $(TARGET_DIR)kernel1.elf
	$(BASH) rm -rf $(TARGET_DIR)kernel2.elf

