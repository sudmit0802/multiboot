CC      = gcc
CFLAGS  = -m32 -Wall -fno-builtin -nostdinc -nostdlib
LD      = ld

OBJFILES = \
	loader.o  \
	common/printf.o  \
	common/screen.o  \
	kernel.o

image:
	@echo "Creating hdd.img..."
	@dd if=/dev/zero of=./hdd.img bs=512 count=16065 1>/dev/null 2>&1

	@echo "Creating bootable first FAT32 partition..."
	@losetup /dev/loop7 ./hdd.img
	@(echo c; echo u; echo n; echo p; echo 1; echo ; echo ; echo a; echo t; echo c; echo w;) | fdisk /dev/loop7 1>/dev/null 2>&1 || true

	@echo "Mounting partition to /dev/loop9..."
	@losetup /dev/loop9 ./hdd.img --offset `echo \`fdisk -lu /dev/loop7 | sed -n 9p | awk '{print $$3}'\`*512 | bc` --sizelimit `echo \`fdisk -lu /dev/loop7 | sed -n 9p | awk '{print $$4}'\`*512 | bc`
	@losetup -d /dev/loop7

	@echo "Format partition..."
	@mkdosfs /dev/loop9

	@echo "Copy kernel and grub files on partition..."
	@mkdir -p tempdir
	@mount /dev/loop9 tempdir
	@mkdir tempdir/boot
	@cp -r grub tempdir/boot/
	@cp kernel.bin tempdir/
	@sleep 1
	@umount /dev/loop9
	@rm -r tempdir
	@losetup -d /dev/loop9

	@echo "Installing GRUB..."
	@echo "device (hd0) hdd.img \n \
	       root (hd0,0)         \n \
	       setup (hd0)          \n \
	       quit\n" | grub_0.97/usr/sbin/grub --batch 1>/dev/null
	@echo "Done!"

all: kernel.bin image
rebuild: clean all
.s.o:
	as --32 -o $@ $<
.c.o:
	$(CC) -Iinclude $(CFLAGS) -o $@ -c $<
kernel.bin: $(OBJFILES)
	$(LD) -T linker.ld -m elf_i386 -o  $@ $^ 
clean:
	rm -f $(OBJFILES) hdd.img kernel.bin
run:
	qemu-system-i386 -hda hdd.img