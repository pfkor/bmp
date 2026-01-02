#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "bmp.h"

#define min(x, y) (((x) < (y)) ? (x) : (y))
#define max(x, y) (((x) > (y)) ? (x) : (y))

enum {
    COPY, FLIP_H, FLIP_V, FLIP_BOTH,
    RED, GREEN, BLUE, NEG, YELLOW, CYAN, MAGENTA, GS,
    SHARP,
};

void console_img (img*);
void filter (img*, img*, int);

int main(){

    char *path = "./img/lenna.bmp";
    
    img *source = parse(path);
    if (!source){
        fprintf(stderr, "Failed to parse %s file!\n", path);
        return 1;
    }
    // console_img(source);

    img *target = img_init(source->width, source->height);
    
    filter(source, target, FLIP_BOTH);
    print(target, "./img/01-flipped.bmp");

    filter(source, target, GREEN);
    print(target, "./img/02-green.bmp");

    filter(source, target, YELLOW);
    print(target, "./img/03-yellow.bmp");

    filter(source, target, GS);
    print(target, "./img/04-gs.bmp");

    filter(source, target, SHARP);
    print(target, "./img/05-sharp.bmp");

    img_destroy(target);
    img_destroy(source);
    return 0;
}

void console_img (img *image){
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

void filter(img* source, img* target, int mode){
    int w = source->width;
    int h = source->height;

    for (int y = 0; y < h; y++){
        for (int x = 0; x < w; x++){

            pixel p = source->bitmap[y][x];
            int r = p.r, g = p.g, b = p.b;

            pixel matrix[3][3] = {\
                source->bitmap[max(y-1, 0)][max(x-1, 0)],  source->bitmap[max(y-1, 0)][x],  source->bitmap[max(y-1, 0)][min(w-1, x+1)],\
                source->bitmap[y][max(x-1, 0)],            source->bitmap[y][x],            source->bitmap[y][min(w-1, x+1)], \
                source->bitmap[min(h-1, y+1)][max(x-1, 0)],source->bitmap[min(h-1, y+1)][x],source->bitmap[min(h-1, y+1)][min(w-1, x+1)], };

            switch (mode){

            case COPY: target->bitmap[y][x] = p; break;
            case FLIP_H: target->bitmap[y][x] = source->bitmap[y][w-x-1]; break;
            case FLIP_V: target->bitmap[y][x] = source->bitmap[h-y-1][x]; break;
            case FLIP_BOTH: target->bitmap[y][x] = source->bitmap[h-y-1][w-x-1]; break;

            case RED:     target->bitmap[y][x] = (pixel){r, 0, 0}; break;
            case GREEN:   target->bitmap[y][x] = (pixel){0, g, 0}; break;
            case BLUE:    target->bitmap[y][x] = (pixel){0, 0, b}; break;
            case NEG:     target->bitmap[y][x] = (pixel){255-r, 255-g, 255-b}; break;
            case YELLOW:  target->bitmap[y][x] = (pixel){r, g, 0}; break;
            case CYAN:    target->bitmap[y][x] = (pixel){0, g, b}; break;
            case MAGENTA: target->bitmap[y][x] = (pixel){r, 0, b}; break;
            case GS:
                int average = (0.299 * r + 0.587 * g + 0.114 * b);
                target->bitmap[y][x] = (pixel){average, average, average};
                break;

            case SHARP:
                target->bitmap[y][x] = (pixel){\
                max(min(5 * r - (matrix[0][1].r + matrix[1][0].r + matrix[1][2].r + matrix[2][1].r), 255), 0), \
                max(min(5 * g - (matrix[0][1].g + matrix[1][0].g + matrix[1][2].g + matrix[2][1].g), 255), 0), \
                max(min(5 * b - (matrix[0][1].b + matrix[1][0].b + matrix[1][2].b + matrix[2][1].b), 255), 0), \
                };
                break;

            default: target->bitmap[y][x] = (pixel){0, 0, 0}; break;
            }
            
        }
    }
}