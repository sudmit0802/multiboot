
#include <kernel/kernel.h>

// Then include kernel subsystem's headers
#include <kernel/vga.h>
#include <kernel/tty.h>
#include <kernel/ksh.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/tss.h>
#include <kernel/interrupts.h>
#include <kernel/cpu_detect.h>

// Devices
#include <kernel/devices/timer.h>
#include <kernel/devices/keyboard.h>
#include <kernel/devices/mouse.h>

// Memory manager
#include <kernel/mm/phys_memory.h>
#include <kernel/mm/virt_memory.h>
#include <kernel/mm/kheap.h>

// File system
#include <kernel/fs/vfs.h>
#include <kernel/fs/initrd.h>

// Syscalls
#include <kernel/syscall.h>

// Graphics
#include <kernel/graphics/vesafb.h>

uint32_t kernel_stack_top_vaddr;

int kernel_init(struct multiboot_info *mboot_info) {

    tty_init();

    svga_mode_info_t *svga_mode = (svga_mode_info_t*) mboot_info->vbe_mode_info;
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

    tty_printf("framebuffer_addr = %x\n", framebuffer_addr);
    tty_printf("framebuffer_size = %x\n", framebuffer_size);
    tty_printf("Resolution is %dx%d\n", VESA_WIDTH, VESA_HEIGHT);
   
    interrupt_disable_all();
    keyboard_install();
    syscall_init();
    interrupt_enable_all();

    return 1;
}

int HSVtoRGB(int _h, int _s, int _v) {
  double h = (double)_h / 255.0, s = (double)_s / 255.0, v = (double)_v / 255.0;
  double r = 0;
  double g = 0;
  double b = 0;

  if (s == 0) {
    r = v;
    g = v;
    b = v;
  } else {
    double varH = h * 6;
    double varI = floor(varH);
    double var1 = v * (1 - s);
    double var2 = v * (1 - (s * (varH - varI)));
    double var3 = v * (1 - (s * (1 - (varH - varI))));

    if (varI == 0) {
      r = v;
      g = var3;
      b = var1;
    } else if (varI == 1) {
      r = var2;
      g = v;
      b = var1;
    } else if (varI == 2) {
      r = var1;
      g = v;
      b = var3;
    } else if (varI == 3) {
      r = var1;
      g = var2;
      b = v;
    } else if (varI == 4) {
      r = var3;
      g = var1;
      b = v;
    } else {
      r = v;
      g = var1;
      b = var2;
    }
  }
  return ((int)(r * 255) << 16) | ((int)(g * 255) << 8) | (int)(b * 255);
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

    // init the kernel debug shell (ksh)
    //ksh_init();

    // run ksh main function
    //ksh_main();
    int x,y;
    int h = VESA_HEIGHT;
    int w = VESA_WIDTH;
    double zoom = 1, moveX = 0, moveY = 0;
    int maxIterations = 300;
    double cRe = -0.7;
    double cIm = 0.27015;
    int color;
    double newRe, newIm, oldRe, oldIm;

    for (x = 0; x < w; x++)
    for (y = 0; y < h; y++) {
      newRe = 1.5 * (x - w / 2) / (0.5 * zoom * w) + moveX;
      newIm = (y - h / 2) / (0.5 * zoom * h) + moveY;

      int i;
      for (i = 0; i < maxIterations; i++) {
        oldRe = newRe;
        oldIm = newIm;
        newRe = oldRe * oldRe - oldIm * oldIm + cRe;
        newIm = 2 * oldRe * oldIm + cIm;
        if ((newRe * newRe + newIm * newIm) > 4)
          break;
      }
      color = HSVtoRGB(i % 256, 255, 255 * (i < maxIterations));
      set_pixel(x,y,color&0xFFFFFF);
    }
    tty_setcolor(VESA_BLACK);
    tty_update_cursor(0,0);
    char cmd[1];
    while (1) {
        keyboard_gets(cmd, 1);
    }
    
    for (; ; ) {
        asm("hlt");
    }
}
