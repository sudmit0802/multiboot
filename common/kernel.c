#include "printf.h"
#include "screen.h"
#include "types.h"

void DrawFractal(void);

void kmain() {
  clear_screen();
  DrawFractal();
}