# Kernel startup asm code

# Declare constants for the multiboot header
.set ALIGN,		1<<0						# align loaded modules on page boundaries
.set MEMINFO,	1<<1						# please provide us a memory map
.set VBE_MODE,  1<<2       					# VBE mode flag. GRUB will set it for us and provide info about it.
.set FLAGS,		ALIGN | MEMINFO	| VBE_MODE	# multiboot "flag" field
.set MAGIC,		0x1BADB002					# magic number to let the booloader find the header
.set CHECKSUM,	-(MAGIC + FLAGS)			# Checksum of the above


.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM
.long 0, 0, 0, 0, 0
.long 0 				# 0 = set graphics mode
.long 800, 600, 32 	    # Width, height, depth


# Reserve a stack for the initial thread.
.section .bootstrap_stack, "aw", @nobits
stack_bottom:
.skip 65536 # 64 KiB for the stack             old: 16384  # 16KiB
stack_top:


.set KERNEL_VIRTUAL_BASE, 0xC0000000                  # 3GB
.set KERNEL_PAGE_NUMBER, (KERNEL_VIRTUAL_BASE >> 22)  # Page directory index of kernel's 4MB PTE.

# Declares the boot Paging directory to load a virtual higher half kernel
.section .data
.align 0x1000
.global _boot_page_directory
_boot_page_directory:
.set i, 0x00000083
.rept 0x300 # 0x300 = 0xbfc00000/0x00400000 + 1
    .long i
    .set i, i+0x00400000
.endr

# map virt 0xc0000000-0xc0400000 to phys 0x00000000-0x00400000
.long 0x00000083

# identity map virt 0xc0400000-4G
.set i, 0xc0400083
.rept 0xFF # 0xFF = (0xffc00000 - 0xc0400000)/0x00400000 + 1
    .long i
    .set i, i+0x00400000
.endr


# Text section
.section .text
.global _loader
_loader:
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %edx
  or $0x00000003, %ecx
  mov %ecx, 0xFFC(%edx) # bpd + 4092 i.e we write address of page_dir|0000003 to last pde
  mov $(_boot_page_directory - KERNEL_VIRTUAL_BASE), %ecx
  mov %ecx, %cr3
  mov %cr4, %ecx
  or $0x00000010, %ecx
  mov %ecx, %cr4
  mov %cr0, %ecx
  or $0x80000000, %ecx
  mov %ecx, %cr0
  movl $_start, %edx
  jmp *%edx


.global _start
.type _start, @function
_start:
  	mov %cr0, %eax
  	or $1, %al
  	mov %eax, %cr0
	mov $stack_top, %esp
	push %ebx # Multiboot structure
	push %eax # Multiboot magic number
	call kernel_main
  cli   
   
1:
  movb $0xFE, %al    
  outb %al, $0x64 
	jmp 1b
  
.size _start, . - _start
