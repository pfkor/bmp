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
        fprintf(stdout, "Input file issue\n");
        destroy_image(image);
        return 1;
    }   

    FilterPipeline *pipeline = create_pipeline();

    for (int i = 3; i < argn; i++){
        Filter cur_filter;
        if (strcmp(args[i], "-red") == 0){
            cur_filter.Type = RED;
            cur_filter.params.none.dummy = 0;
        } 
        else if (strcmp(args[i], "-cyan") == 0){
            cur_filter.Type = CYAN;
            cur_filter.params.none.dummy = 0;
        }
        else if (strcmp(args[i], "-neg") == 0){
            cur_filter.Type = NEG;
            cur_filter.params.none.dummy = 0;
        }
        else if (strcmp(args[i], "-gs") == 0){
            cur_filter.Type = GS;
            cur_filter.params.none.dummy = 0;
        }
        else if (strcmp(args[i], "-crop") == 0){
            if(i + 2 < argn){
                cur_filter.Type = CROP;
                cur_filter.params.crop.width = atoi(args[i+1]);
                cur_filter.params.crop.height = atoi(args[i+2]);
                i+=2;
            }
            else{
                fprintf(stdout, "Wrong or no args for filter");
                return 1;
            }
        }
        else if (strcmp(args[i], "-sharp") == 0){
            cur_filter.Type = SHARP;
            cur_filter.params.none.dummy = 0;
        }
        else if (strcmp(args[i], "-edge") == 0){
            if(i + 1 < argn){
                cur_filter.Type = EDGE;
                cur_filter.params.edge.threshold = atof(args[i+1]);
                i++;
            }
            else{
                fprintf(stdout, "Wrong or no args for filter");
                return 1;
            }
        }
        else if (strcmp(args[i], "-med") == 0){
            if(i + 1 < argn){
                cur_filter.Type = MED;
                cur_filter.params.median.window = atoi(args[i+1]);
                i++;
            }
            else{
                fprintf(stdout, "Wrong or no args for filter");
                return 1;
            }
        }
        else if (strcmp(args[i], "-blur") == 0){
            cur_filter.Type = BLUR;
            cur_filter.params.blur.sigma = atof(args[++i]);
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
    destroy_pipeline(pipeline);
    fprintf(stdout, "Successfully runed\n");
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