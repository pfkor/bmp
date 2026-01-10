#include "filters.h"

#include <stdlib.h>
#include <string.h>

#include "bmp.h"

Filter filter_init(FilterType Type, char args[1024], void (*pixel_func)(Color*)) {
    Filter new_filter;
    new_filter.Type = Type;
    if (args != NULL) {
        strcpy(new_filter.args, args);
    }
    new_filter.params.pixelFunc = pixel_func;

    return new_filter;
}

void all_pixel_proccess(Image* image, void (*pixel_func)(Color*)){
    unsigned int h = image->height;
    unsigned int w = image->width;

    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            pixel_func(&image->data[y][x]);
        }
    }

}

void crop(Image* image, unsigned int x_from, unsigned int x_to, unsigned int y_from, unsigned int y_to){
    if (!image || !image->data) return;
    
    unsigned int w = image->width;
    unsigned int h = image->height;
    if (w == 0 || h == 0 || \
    x_from >= w || x_to >= w || x_to <= x_from || \
    y_from >= h || y_to >= h || y_to <= y_from){
        return;
    }

    unsigned int new_w = x_to - x_from;
    unsigned int new_h = y_to - y_from;

    Color **temp = malloc(sizeof(Color *) * new_h);
    if (!temp) return;

    for (unsigned int y = 0; y < new_h; y++){
        temp[y] = malloc(sizeof(Color) * new_w);
        if (!temp[y]){

            for (unsigned int j = 0; j < y; j++){
                free(temp[j]);
            }
            free(temp);
            return;
        }
    }

    for (unsigned int y = 0; y < new_h; y++){
        for (unsigned int x = 0; x < new_w; x++){
            set_color(temp, x, y, get_color(image, y_from + y, x_from + x));
        }
    }

    for (int y = 0; y < h; y++){
        free(image->data[y]);
    }
    free(image->data);

    image->width = new_w;
    image->height = new_h;
    image->data = temp;
}

void multiply_channels(Image* image, float r_factor, float g_factor, float b_factor){
    if (r_factor < 0 || r_factor > 1 || g_factor < 0 || g_factor > 1 || b_factor < 0 || b_factor > 1){
        return;
    }
    
    for (int y = 0; y < image->height; y++){
        for (int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r *= r_factor;
            c.g *= g_factor;
            c.b *= b_factor;
            set_color(image, x, y, c);
        }
    }
}

// Я понимаю что это не очень читабельно, но мне душа не позволяет эти функции в одну строчку разворачивать, надеюсь не помешает
void red(Color* pxl){*pxl = (Color){pxl->r, 0, 0};}

void green(Color* pxl){*pxl = (Color){0, pxl->g, 0};}

void blue(Color* pxl){*pxl = (Color){0, 0, pxl->b};}

void yellow(Color* pxl){*pxl = (Color){pxl->r, pxl->g, 0};}

void neg(Color* pxl){*pxl = (Color){255-pxl->r, 255-pxl->g, 255-pxl->b};}

void cyan(Color* pxl){*pxl = (Color){0, pxl->g, pxl->b};}

void magenta(Color* pxl){*pxl = (Color){pxl->r, 0, pxl->b};}

void grayscale(Color* pxl){
    int average = (0.299 * pxl->r + 0.587 * pxl->g + 0.114 * pxl->b);
    *pxl = (Color){average, average, average};
}



