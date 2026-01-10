#pragma once

#include "bmp.h"

typedef enum {
    COPY, CROP, FLIP_H, FLIP_V, FLIP_BOTH,
    RED, GREEN, BLUE, NEG, YELLOW, CYAN, MAGENTA, GS,
    SHARP,
} FilterType;

typedef union {
    void (*pixelFunc)(Color*);
    // тут ещё потом матрицы будут,но может и надо будет какое-то другое решение, а то вдруг нужна будет и матрица и функция
}FilterParams;

typedef struct {
    FilterType Type;
    char args[1024];
    FilterParams params;
}Filter;

Filter filter_init(FilterType Type, char args[1024], void (*pixel_func)(Color*));

void all_pixel_proccess(Image* image, void (*pixel_func)(Color*));

void crop(Image* image, unsigned int x_from, unsigned int x_to, unsigned int y_from, unsigned int y_to);
void multiply_channels(Image* image, float r_factor, float g_factor, float b_factor);
void matrix_sharpening(Image* image);

void red(Color* pxl);
void green(Color* pxl);
void blue(Color* pxl);
void neg(Color* pxl);
void yellow(Color* pxl);
void cyan(Color* pxl);
void magenta(Color* pxl);
void grayscale(Color* pxl);
void flip_h(Image* image);
void flip_v(Image* image);
void flip_both(Image* image);
