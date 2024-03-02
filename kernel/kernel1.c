
#include <kernel/kernel.h>

// Then include kernel subsystem's headers
#include <kernel/cpu_detect.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/interrupts.h>
#include <kernel/tss.h>
#include <kernel/tty.h>

// Devices
#include <kernel/devices/keyboard.h>
#include <kernel/devices/timer.h>

// Memory manager
#include <kernel/mm/kheap.h>
#include <kernel/mm/phys_memory.h>
#include <kernel/mm/virt_memory.h>

// File system
#include <kernel/fs/vfs.h>

// Graphics
#include <kernel/graphics/vesafb.h>

// Math
#include <kernel/libk/math.h>

#include <kernel/snake.h>

uint32_t kernel_stack_top_vaddr;

int kernel_init(struct multiboot_info *mboot_info) {

  tty_init();

  svga_mode_info_t *svga_mode = (svga_mode_info_t *)mboot_info->vbe_mode_info;
  framebuffer_addr = (uint8_t *)svga_mode->physbase;
  framebuffer_pitch = svga_mode->pitch;
  framebuffer_bpp = svga_mode->bpp;
  framebuffer_width = svga_mode->screen_width;
  framebuffer_height = svga_mode->screen_height;
  framebuffer_size = framebuffer_height * framebuffer_pitch;
  back_framebuffer_addr = framebuffer_addr;

  gdt_install();
  idt_install();
  pmm_init(mboot_info);
  vmm_init();
  kheap_init();
  init_vbe(mboot_info);
  
  interrupt_disable_all();
  keyboard_install();
  interrupt_enable_all();


  interrupt_disable_all();
  keyboard_install();
  interrupt_enable_all();

  return 1;
}


void kernel_main(int magic_number, struct multiboot_info *mboot_info) // Arguments are passed by _start in boot.s
{

  asm("movl %%esp,%0" : "=r"(kernel_stack_top_vaddr));
  (void)magic_number;
  multiboot_info_t mboot_info_copy = *mboot_info;
  kernel_init(&mboot_info_copy);

  uint32_t esp;
  asm volatile("mov %%esp, %0" : "=r"(esp));

  tss_set_stack(0x10, esp);

  draw_fill(0, 0, VESA_WIDTH, VESA_HEIGHT, VESA_BLACK);
  
  snake();

}
