#include "filters.h"

#include <stdlib.h>
#include <string.h>

#include "bmp.h"

Filter filter_init(filterType Type, char args[1024], void (*pixel_func)(pixel*)) {
    Filter new_filter;
    new_filter.Type = Type;
    if (args != NULL) {
        strcpy(new_filter.args, args);
    }
    new_filter.params.pixelFunc = pixel_func;

    return new_filter;
}

void all_pixel_proccess(img* image, void (*pixel_func)(pixel*)){
    unsigned int h = image->height;
    unsigned int w = image->width;

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            pixel_func(&image->bitmap[y][x]);
        }
    }

}
// Я понимаю что это не очень читабельно, но мне душа не позволяет эти функции в одну строчку разворачивать, надеюсь не помешает
void red(pixel* pxl){*pxl = (pixel){pxl->r, 0, 0};}

void green(pixel* pxl){*pxl = (pixel){0, pxl->g, 0};}

void blue(pixel* pxl){*pxl = (pixel){0, 0, pxl->b};}

void yellow(pixel* pxl){*pxl = (pixel){pxl->r, pxl->g, 0};}

void neg(pixel* pxl){*pxl = (pixel){255-pxl->r, 255-pxl->g, 255-pxl->b};}

void cyan(pixel* pxl){*pxl = (pixel){0, pxl->g, pxl->b};}

void magenta(pixel* pxl){*pxl = (pixel){pxl->r, 0, pxl->b};}

void grayscale(pixel* pxl){
    int average = (0.299 * pxl->r + 0.587 * pxl->g + 0.114 * pxl->b);
    *pxl = (pixel){average, average, average};
}



