#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "bmp.h"

void console_img (img*);

int main(){

    char *path = "./img/lenna.bmp";
    
    img *source = parse(path);
    if (!source){
        fprintf(stderr, "Failed to parse %s file!\n", path);
        return 1;
    }

    img_destroy(source);
    return 0;
}

void console_img (img *image){ // требовалось для проверки парсера, пока не было вывода. Не нужно(?) но пусть пока останется
    char gradient[16] = " .,_-=+*nm%#&BW@";   
    
    for (int y = 0; y < image->height; y += 4){
        for (int x = 0; x < image->width; x += 2){

            pixel p = image->bitmap[y][x];
            int brightness = ((int)(0.299 * p.r + 0.587 * p.g + 0.114 * p.b) / 16) % 16;
            printf("%c", gradient[brightness]);
        }
        printf("\n");
    }
}
