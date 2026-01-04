#include <stdio.h>
#include <stdint.h>

#include "color.h"

Color pixel_to_color(Pixel_24 pixel){
    return (Color){(float) pixel.r, (float) pixel.g, (float) pixel.b};
}

Pixel_24 color_to_pixel(Color color){
    return (Pixel_24){(uint8_t) color.b, (uint8_t) color.g, (uint8_t) color.r};
}
