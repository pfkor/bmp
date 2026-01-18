#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "color.h"

Color pixel_to_color(Pixel_24 pixel){
    return (Color){(float) pixel.r, (float) pixel.g, (float) pixel.b};
}

Pixel_24 color_to_pixel(Color color){
    if (color.r < 0 || color.r >= 256 || color.g < 0 \
     || color.g >= 256 || color.b < 0 || color.b >= 256){
        return (Pixel_24){0, 0, 0};
    }
    return (Pixel_24){(uint8_t) color.b, (uint8_t) color.g, (uint8_t) color.r};
}

void limit_color (Color *color){
    if (color->r < 0) color->r = 0;
    if (color->g < 0) color->g = 0;
    if (color->b < 0) color->b = 0;
    if (color->r > 255) color->r = 255;
    if (color->g > 255) color->g = 255;
    if (color->b > 255) color->b = 255;
}

float get_distance(Color from, Color to){
    float r = from.r - to.r;
    float g = from.g - to.g;
    float b = from.b - to.b;
    return r*r + g*g + b*b; // корень больно дорогой, я пока убрал, мы пока дистанцию вроде только для сравнений используем, если что добавим параметр.
}

float min_distance(Color main, Color *arr, int len){
    float dist, min_dist = HUGE_VALF;
    int idx = 0;
    for (int i = 0; i < len; i++){
        dist = get_distance(main, arr[i]);
        if (dist < min_dist){
            min_dist = dist;
            idx = i;
        }
    }
    return min_dist;
}

int get_nearest(Color main, Color *arr, int len){
    float dist, min_dist = HUGE_VALF;
    int idx = 0;
    for (int i = 0; i < len; i++){
        dist = get_distance(main, arr[i]);
        if (dist < min_dist){
            min_dist = dist;
            idx = i;
        }
    }
    return idx;
}


