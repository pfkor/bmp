#pragma once

typedef struct image img;
typedef struct pxl pixel;

typedef enum {
    COPY, FLIP_H, FLIP_V, FLIP_BOTH,
    RED, GREEN, BLUE, NEG, YELLOW, CYAN, MAGENTA, GS,
    SHARP,
} filterType;

typedef struct {
    filterType Type;
    char args[1024];
    union {
        void (*pixelFunc)(pixel*);
        // тут ещё потом матрицы будут,но может и надо будет какое-то другое решение, а то вдруг нужна будет и матрица и функция
    }params;

}Filter;

Filter filter_init(filterType Type, char args[1024] );

void all_pixel_proccess(img* image, void (*pixel_func)(pixel*));

void red(pixel* pxl);
void green(pixel* pxl);
void blue(pixel* pxl);
void neg(pixel* pxl);
void yellow(pixel* pxl);
void cyan(pixel* pxl);
void magenta(pixel* pxl);
void grayscale(pixel* pxl);
