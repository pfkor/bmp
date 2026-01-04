#include <stdio.h>
#include <stdint.h>

#include "color.h"

Color pixel_to_color(Pixel_24 pixel){
    if (pixel.r < 0 || pixel.r >= 256 || pixel.g < 0 \
     || pixel.g >= 256 || pixel.b < 0 || pixel.b >= 256){
        return (Color){0, 0, 0};
    }
    return (Color){(float) pixel.r, (float) pixel.g, (float) pixel.b};
}

Pixel_24 color_to_pixel(Color color){
    if (color.r < 0 || color.r >= 256 || color.g < 0 \
     || color.g >= 256 || color.b < 0 || color.b >= 256){
        return (Pixel_24){0, 0, 0};
    }
    return (Pixel_24){(uint8_t) color.b, (uint8_t) color.g, (uint8_t) color.r};
}
