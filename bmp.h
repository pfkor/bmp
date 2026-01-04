#include <stdint.h>
#pragma once

#pragma pack(push, 1) // отключение padding-а структур, так как BMP и DIB header-ы имеют разный размер

typedef struct {
    char id[2];
    uint32_t size_of_file;
    uint16_t unused_1;
    uint16_t unused_2;
    uint32_t offset;
} BMP_Header;

typedef struct {
    uint32_t dib_bytes;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bits;
    uint32_t bi_rgb;
    uint32_t raw_bitmap;
    int32_t  resolution_h;
    int32_t  resolution_v;
    uint32_t palette_colors;
    uint32_t important_colors;
} DIB_Header;

typedef struct {
    BMP_Header bmp;
    DIB_Header dib;
} File_Header;

#pragma pack(pop) // возвращаем как было (там стек)

typedef struct pxl {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} pixel;

typedef struct image{
    unsigned int width;
    unsigned int height;
    pixel **bitmap;
} img; // Вопрос: достаточно ли такой структуры? Лучше сразу исправить, если нужна какая-то еще инфа про картинку

img* img_init(unsigned int, unsigned int);
void img_destroy(img*);

img* parse (char *);
void print (img *, char *);