# Makefile for building TheGame
KERNEL_FILE1=bin/kernel/kernel1.elf
KERNEL_FILE2=bin/kernel/kernel2.elf
ISODIR=isodir/
TARGET_ISO=TheGame.iso
LOGFILE=TheGame_serial.log




check-multiboot: $(KERNEL_FILE1) $(KERNEL_FILE2)
	 grub-file --is-x86-multiboot $(KERNEL_FILE1)
	 grub-file --is-x86-multiboot $(KERNEL_FILE2)

compile-kernel:
	make -C kernel -f Makefile DEBUG=$(DEBUG)

build-iso: compile-kernel check-multiboot
	 rm -rf $(ISODIR)
	 mkdir -p $(ISODIR)boot
	 mkdir -p $(ISODIR)boot/grub
	 cp $(KERNEL_FILE1) $(ISODIR)boot/kernel1.elf
	 cp $(KERNEL_FILE2) $(ISODIR)boot/kernel2.elf
	 cp grub/grub.cfg $(ISODIR)boot/grub/grub.cfg
	 grub-mkrescue -o $(TARGET_ISO) $(ISODIR)

clean-logs:
	 rm -rf $(LOGFILE)

rerun: clean-logs
	qemu-system-i386 -m 2048 -cdrom $(TARGET_ISO) -monitor stdio -serial file:$(LOGFILE)


clean:
	rm -rf $(ISODIR)
	rm -rf $(TARGET_ISO)
	rm -rf $(LOGFILE)
	make -C kernel -f Makefile clean

all: build-iso

run: clean clean-logs all rerun





