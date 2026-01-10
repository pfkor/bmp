#include <stdio.h>
#include <stdint.h>

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

