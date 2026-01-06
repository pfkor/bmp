#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "bmp.h"
#include "filters.h"
#include "pipeline.h"

#define min(x, y) (((x) < (y)) ? (x) : (y)) // плохое решение, только для быстрой реализации матрицы
#define max(x, y) (((x) > (y)) ? (x) : (y))



void console_img (img*);
void filter (img*, img*, int);
void print_help();

int main(int argN, char* args[]) {
    if (argN < 3) {
        return 0;
    }

    char *input_file = args[1];
    char *output_file = args[2];

    img *inputIm = parse(input_file);
    FilterPipeline *pipeline = create_pipeline();

    for(int i = 3; i < argN; i++){
        Filter new_filter;
        if(strcmp(args[i], "-red") == 0){
          new_filter = filter_init(RED, NULL, &red);
        }
        else if(strcmp(args[i], "-blue") == 0){
            new_filter = filter_init(BLUE, NULL, &blue);
        }
        else if(strcmp(args[i], "-green") == 0) {
            new_filter = filter_init(GREEN, NULL, &green);
        }
        else if(strcmp(args[i], "-yellow") == 0) {
            new_filter = filter_init(YELLOW, NULL, &yellow);
        }
        else if(strcmp(args[i], "-magenta") == 0) {
            new_filter = filter_init(MAGENTA, NULL, &magenta);
        }
        else if(strcmp(args[i], "-cyan") == 0) {
            new_filter = filter_init(CYAN, NULL, &cyan);
        }
        else if(strcmp(args[i], "-neg") == 0) {
            new_filter = filter_init(NEG, NULL, &neg);
        }
        else if(strcmp(args[i], "-gs") == 0) {
            new_filter = filter_init(GS, NULL, &grayscale);
        }

        add_filter(pipeline,new_filter);

    }

    apply_pipeline(inputIm, pipeline);
    print(inputIm,output_file);
    free_pipeline(pipeline);

}