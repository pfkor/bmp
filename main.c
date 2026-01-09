#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "bmp.h"
#include "filters.h"
#include "pipeline.h"

void console_img (Image*);

int main(){

    char *path = "./img/lenna.bmp";
    
    Image *source = load_bmp(path);
    if (!source){
        fprintf(stderr, "Failed to parse %s file!\n", path);
        return 1;
    }

    // console_img(source);
    save_bmp("./img/saved.bmp", source);
    destroy_image(source);

    source = load_bmp("./img/saved.bmp");
    if (!source){
        fprintf(stderr, "Failed to parse %s file!\n", path);
        return 1;
    }
    destroy_image(source);
    return 0;
}

void console_img (Image *image){ // требовалось для проверки парсера, пока не было вывода. Не нужно(?) но пусть пока останется
    char gradient[16] = " .,_-=+*nm%#&BW@";   
    
    for (int y = 0; y < image->height; y += 10){
        for (int x = 0; x < image->width; x += 10){

            Color p = get_color(image, x, y);
            float brightness = ((0.299 * p.r + 0.587 * p.g + 0.114 * p.b) / 16);
            printf("%c", gradient[(int)brightness % 16]);
        }
        printf("\n");
    }
}