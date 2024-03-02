#ifndef SNAKE_H
#define SNAKE_H
#include <stdint.h>
#include <stddef.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

typedef struct  {
 int x;
 int y;
 int h;
 int w;
} GameObject;

void snake();

#endif