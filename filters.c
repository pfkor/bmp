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

// void all_pixel_proccess(Image* image, void (*pixel_func)(Color*)){
//     if (!image || !image->data) return;
//     unsigned int h = image->height;
//     unsigned int w = image->width;
//     for(int y = 0; y < h; y++){
//         for(int x = 0; x < w; x++){
//             pixel_func(&image->data[y][x]);
//         }
//     }
// }

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
            temp[y][x] = get_color(image, x_from + x, y_from + y);
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
    if (!image || !image->data) return;
    if (r_factor < 0 || r_factor > 1 || g_factor < 0 || g_factor > 1 || b_factor < 0 || b_factor > 1){
        return;
    }
    
    for (unsigned int y = 0; y < image->height; y++){
        for (unsigned int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r *= r_factor;
            c.g *= g_factor;
            c.b *= b_factor;
            set_color(image, x, y, c);
        }
    }
}

void negative (Image *image){
    if (!image || !image->data) return;

    for (unsigned int y = 0; y < image->height; y++){
        for (unsigned int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r = 255 - c.r;
            c.g = 255 - c.g;
            c.b = 255 - c.b;
            set_color(image, x, y, c);
        }
    }
}

void monochrome (Image *image){
    if (!image || !image->data) return;

    for (unsigned int y = 0; y < image->height; y++){
        for (unsigned int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            c.g = c.r;
            c.b = c.r;
            set_color(image, x, y, c);
        }
    }   
}

void get_window(Image *window, unsigned int x, unsigned int y, Image *image){
    if (!window || !window->data || !image || !image->data) return;
    if (window->width != window->height || window->width % 2 == 0) return;

    unsigned int w = image->width;
    unsigned int h = image->height;
    int radius = (window->width - 1) / 2;

    for (int ky = -radius; ky <= radius; ky++){
        for (int kx = -radius; kx <= radius; kx++){

            int pixel_x = (int)x + kx;
            int pixel_y = (int)y + ky;

            if (pixel_x < 0) pixel_x = 0;
            if (pixel_x >= w) pixel_x = w - 1;
            if (pixel_y < 0) pixel_y = 0;
            if (pixel_y >= h) pixel_y = h - 1;
            
            set_color(window, kx + radius, ky + radius, get_color(image, pixel_x, pixel_y));
        }
    }
}
Color impose_matrix(Image *window, int **matrix){
    if (!window || !window->data) return (Color){0, 0, 0};
    Color res = {0, 0, 0};
    
    for (int y = 0; y < window->height; y++){
        for (int x = 0; x < window->width; x++){
            Color cur = get_color(window, x, y);
            res.r += cur.r * matrix[y][x];
            res.g += cur.g * matrix[y][x];
            res.b += cur.b * matrix[y][x];
        }
    }

    limit_color(&res);
    return res;
}
void matrix_sharpening(Image* image){

    if (!image || !image->data) return;

    unsigned int w = image->width;
    unsigned int h = image->height;
    
    int **kernel = malloc(sizeof(int *) * 3);
    if (!kernel) return;
    
    for (int i = 0; i < 3; i++){
        kernel[i] = (int*)malloc(3 * sizeof(int));
        if (!kernel[i]){
            for (int j = 0; j < i; j++) free(kernel[j]);
            free(kernel);
            return;
        }
    }
    
    int kernel_data[3][3] = {
        { 0, -1,  0},
        {-1,  5, -1},
        { 0, -1,  0}
    };
    
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < 3; j++){
            kernel[i][j] = kernel_data[i][j];
        }
    }

    Image *temp = create_image(w, h);
    if (!temp){
        for (int i = 0; i < 3; i++) free(kernel[i]);
        free(kernel);
        return;
    }

    Image *window = create_image(3, 3);
    if (!window){
        destroy_image(temp);
        for (int i = 0; i < 3; i++) free(kernel[i]);
        free(kernel);
        return;
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){

            get_window(window, x, y, image);
            Color sum = impose_matrix(window, kernel);
            limit_color(&sum);

            set_color(temp, x, y, sum);
        }
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){
            set_color(image, x, y, get_color(temp, x, y));
        }
    }

    destroy_image(window);
    destroy_image(temp);
    for (int i = 0; i < 3; i++) free(kernel[i]);
    free(kernel);
}



// Я понимаю что это не очень читабельно, но мне душа не позволяет эти функции в одну строчку разворачивать, надеюсь не помешает
void red(Color* pxl){*pxl = (Color){pxl->r, 0, 0};}

void green(Color* pxl){*pxl = (Color){0, pxl->g, 0};}

void blue(Color* pxl){*pxl = (Color){0, 0, pxl->b};}

void yellow(Color* pxl){*pxl = (Color){pxl->r, pxl->g, 0};}

void neg(Color* pxl){*pxl = (Color){255-pxl->r, 255-pxl->g, 255-pxl->b};}

void cyan(Color* pxl){*pxl = (Color){0, pxl->g, pxl->b};}

void magenta(Color* pxl){*pxl = (Color){pxl->r, 0, pxl->b};}


