#pragma once

#include "bmp.h"

// typedef struct image img;
// typedef struct pxl pixel;

typedef enum {
    COPY, FLIP_H, FLIP_V, FLIP_BOTH,
    RED, GREEN, BLUE, NEG, YELLOW, CYAN, MAGENTA, GS,
    SHARP,
} FilterType;

typedef union {
    void (*pixelFunc)(Color*);
    // тут ещё потом матрицы будут,но может и надо будет какое-то другое решение, а то вдруг нужна будет и матрица и функция
}FilParams;

typedef struct {
    FilterType Type;
    char args[1024];
    FilParams params;
}Filter;

Filter filter_init(FilterType Type, char args[1024], void (*pixel_func)(Color*));

void all_pixel_proccess(Image* image, void (*pixel_func)(Color*));

void red(Pixel_24* pxl);
void green(Pixel_24* pxl);
void blue(Pixel_24* pxl);
void neg(Pixel_24* pxl);
void yellow(Pixel_24* pxl);
void cyan(Pixel_24* pxl);
void magenta(Pixel_24* pxl);
void grayscale(Pixel_24* pxl);
void flip_h(Image* image);
void flip_v(Image* image);
void flip_both(Image* image);
