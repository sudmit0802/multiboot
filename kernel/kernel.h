#ifndef _KERNEL_KERNEL_H_
#define _KERNEL_KERNEL_H_

//#include <sys/constants.h>

#include <kernel/multiboot.h>

#include <stdint.h>

int kernel_init(struct multiboot_info *mboot_info);
void higher_half_test();

void kernel_main(int magic_number, struct multiboot_info *mboot_info);

extern uint32_t kernel_stack_top_vaddr;

#endif
