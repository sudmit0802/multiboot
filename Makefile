# Makefile for building TheGame
KERNEL_FILE=bin/kernel/kernel.elf
ISODIR=isodir/
TARGET_ISO=TheGame.iso
LOGFILE=TheGame_serial.log




check-multiboot: $(KERNEL_FILE)
	 grub-file --is-x86-multiboot $(KERNEL_FILE)

compile-kernel:
	make -C kernel -f Makefile DEBUG=$(DEBUG)

build-iso: compile-kernel check-multiboot
	 rm -rf $(ISODIR)
	 mkdir -p $(ISODIR)boot
	 mkdir -p $(ISODIR)boot/grub
	 cp $(KERNEL_FILE) $(ISODIR)boot/kernel.elf
	 cp grub/grub.cfg $(ISODIR)boot/grub/grub.cfg
	 grub-mkrescue -o $(TARGET_ISO) $(ISODIR)

clean-logs:
	 rm -rf $(LOGFILE)

rerun: clean-logs
ifeq ($(DEBUG),1)
	qemu-system-i386 -s -S -m 512 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE) & gdb $(KERNEL_FILE) -ex "target remote localhost:1234" -tui
else
	qemu-system-i386 -m 2024 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE)
endif

clean:
	 rm -rf $(ISODIR)
	 rm -rf $(TARGET_ISO)
	 rm -rf $(LOGFILE)
	make -C kernel -f Makefile clean

all: build-iso

run: clean clean-logs all rerun





