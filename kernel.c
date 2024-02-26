#include "printf.h"
#include "screen.h"
#include "types.h"

void DrawFractal(void);

int main()
{   
    clear_screen();
    printf("\nHello World!");
    clear_screen();
    DrawFractal();
    return 0;
}