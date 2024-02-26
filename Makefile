CC      = gcc
CFLAGS  = -m32 -Wall -fno-builtin -nostdinc -nostdlib
LD      = ld

OBJFILES = \
	loader.o  \
	common/printf.o  \
	common/screen.o  \
	common/bios.o  \
	common/vbe.o  \
	common/qdivrem.o  \
	common/udivdi3.o  \
	common/umoddi3.o  \
	common/divdi3.o  \
	common/moddi3.o  \
	common/setjmp.o  \
	common/string.o  \
	common/s_floor.o  \
	x86emu/x86emu.o  \
	x86emu/x86emu_util.o  \
	fractal.o \
	kernel.o

image:
	@echo "Creating hdd.img..."
	dd if=/dev/zero of=./hdd.img bs=512 count=16065 1>/dev/null 2>&1

	@echo "Creating bootable first FAT32 partition..."
	sudo losetup /dev/loop8 ./hdd.img
	(echo c; echo u; echo n; echo p; echo 1; echo ; echo ; echo a; echo t; echo c; echo w;) | sudo fdisk /dev/loop8 1>/dev/null 2>&1 || true

	@echo "Mounting partition to /dev/loop9..."
	sudo losetup /dev/loop9 ./hdd.img --offset `echo \`sudo fdisk -lu /dev/loop8 | sed -n 9p | awk '{print $$3}'\`*512 | bc` --sizelimit `echo \`sudo fdisk -lu /dev/loop8 | sed -n 9p | awk '{print $$4}'\`*512 | bc`
	sudo losetup -d /dev/loop8

	@echo "Format partition..."
	sudo mkdosfs /dev/loop9

	@echo "Copy kernel and grub files on partition..."
	mkdir tempdir
	sudo mount /dev/loop9 tempdir
	sudo mkdir tempdir/boot
	sudo cp -r grub tempdir/boot/
	sudo cp kernel.bin tempdir/
	sleep 1
	sudo umount /dev/loop9
	rm -rf tempdir
	sudo losetup -d /dev/loop9

	@echo "Installing GRUB..."
	echo "device (hd0) hdd.img \n \
	       root (hd0,0)         \n \
	       setup (hd0)          \n \
	       quit\n" | grub_0.97/usr/sbin/grub --batch 1>/dev/null
	@echo "Done!"

all: kernel.bin image
rebuild: clean all
.s.o:
	as --32 -o $@ $<
.c.o:
	$(CC) -Ix86emu -Iinclude $(CFLAGS) -o $@ -c $<
kernel.bin: $(OBJFILES)
	$(LD) -T linker.ld -m elf_i386 -o  $@ $^ 
clean:
	-sudo losetup -d /dev/loop8
	-sudo umount /dev/loop9
	-sudo rm -rf tempdir
	-sudo losetup -d /dev/loop9
	sudo rm -f $(OBJFILES) hdd.img kernel.bin
run:
	qemu-system-i386 -no-reboot -no-shutdown -drive format=raw,file=hdd.img