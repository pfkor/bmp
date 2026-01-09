#pragma once 

#include <stdint.h>
#include "color.h"

#pragma pack(push, 1) // отключение padding-а структур, так как BMP и DIB header-ы имеют разный размер

typedef struct {
    uint16_t id;
    uint32_t size;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;
} BMP_Header;

typedef struct {
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bits;
    uint32_t compression;
    uint32_t raw_bitmap;
    int32_t  resolution_h;
    int32_t  resolution_v;
    uint32_t palette_colors;
    uint32_t important_colors;
} DIB_Header;

typedef struct {
    BMP_Header bmp;
    DIB_Header dib;
} Header;

#pragma pack(pop) // возвращаем как было (там стек)

typedef struct {
    unsigned int width;
    unsigned int height;
    Color **data;
} Image;

Image* create_image(unsigned int width, unsigned int height);
void destroy_image(Image *image);

Image* load_bmp(const char *filepath);
void save_bmp(const char *filepath, Image *image);

Color get_color(Image *image, int x, int y);
void set_color(Image *image, int x, int y, Color color);
