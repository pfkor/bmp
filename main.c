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
          new_filter.Type = RED; // вот это все присваивание надо будет занести в отдельную функцию типа createFilter()
          new_filter.params.pixelFunc = &red;

        }
        if(strcmp(args[i], "-blue") == 0){
            new_filter.Type = BLUE;
            new_filter.params.pixelFunc = &blue;

        }
        if(strcmp(args[i], "-green") == 0){
            new_filter.Type = GREEN;
            new_filter.params.pixelFunc = &green;

        }
        if(strcmp(args[i], "-yellow") == 0){
            new_filter.Type = YELLOW;
            new_filter.params.pixelFunc = &yellow;

        }

        add_filter(pipeline,new_filter);




    }

    apply_pipeline(inputIm, pipeline);
    print(inputIm,output_file);

}