#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "bmp.h"
#include "filters.h"
#include "pipeline.h"

void console_img (Image*);

int main(int argn, char *args[]){

    if (argn < 3){
        fprintf(stdout, "Usage: ./program <input> <output> <...>\n");
        return 1;
    }

    char *input_path = args[1];
    char *output_path = args[2];

    Image *image = load_bmp(input_path);
    if (!image){
        destroy_image(image);
        return 1;
    }   

    FilterPipeline *pipeline = create_pipeline();
    for (int i = 3; i < argn; i++){
        Filter cur_filter;
        if (strcmp(args[i], "-red") == 0){
            cur_filter = filter_init(RED, NULL, &red);
        } 
        else if (strcmp(args[i], "-green") == 0){
            cur_filter = filter_init(GREEN, NULL, &green);
        }
        else if (strcmp(args[i], "-blue") == 0){
            cur_filter = filter_init(BLUE, NULL, &blue);
        }
        else if (strcmp(args[i], "-neg") == 0){
            cur_filter = filter_init(NEG, NULL, &neg);
        }
        else if (strcmp(args[i], "-gs") == 0){
            cur_filter = filter_init(GS, NULL, &grayscale);
        }
        else{
            fprintf(stderr, "Unknown filter %s!\n", args[i]);
            continue;
        }
        add_filter(pipeline, cur_filter);
    }

    apply_pipeline(image, pipeline);
    save_bmp(output_path, image);

    destroy_image(image);
    free(pipeline);
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