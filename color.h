#pragma once

#include <stdint.h>

typedef struct {
    float r;
    float g;
    float b;
} Color;

typedef struct {
    uint8_t b;
    uint8_t g;
    uint8_t r;
} Pixel_24;  

Color pixel_to_color(Pixel_24 pixel);
Pixel_24 color_to_pixel(Color color);


void limit_color(Color *color);

float get_distance(Color from, Color to);
float get_distance_weighted(Color from, Color to, float r_w, float g_w, float b_w);
int get_nearest(Color main, Color *arr, int len);
int get_nearest_weighted(Color main, Color *arr, int len, float r_w, float g_w, float  b_w);
