#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> 

#include "bmp.h"
#include "filters.h"
#include "pipeline.h"

void arg_error(char* filter_name);
void print_help();

int main(int argn, char *args[]){

    if (argn < 3){
        print_help();
        return 1;
    }
    char *input_path = args[1];
    char *output_path = args[2];

    Image *image = load_bmp(input_path);
    if (!image){
        fprintf(stdout, "Unable to load input file\n");
        destroy_image(image);
        return 1;
    }   

    FilterPipeline *pipeline = create_pipeline();

    for (int i = 3; i < argn; i++){
        Filter cur_filter;
        if (strcmp(args[i], "-neg") == 0){
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
                arg_error(args[i]);
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
                arg_error(args[i]);
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
                arg_error(args[i]);
                return 1;
            }
        }
        else if (strcmp(args[i], "-blur") == 0){
            cur_filter.Type = BLUR;
            cur_filter.params.blur.sigma = atof(args[++i]);
        }
        else if (strcmp(args[i], "-cluster") == 0){
            if(i + 1 < argn){
                cur_filter.Type = CLUSTER;
                cur_filter.params.cluster.centroids = atoi(args[i + 1]);
                i++;
            }
            else{
                arg_error(args[i]);
                return 1;
            }
        }
        else if (strcmp(args[i], "-mosaic") == 0){
            if(i + 2 < argn){
                cur_filter.Type = MOSAIC;
                cur_filter.params.mosaic.filepath = args[i+1];
                cur_filter.params.mosaic.tiles_number = atoi(args[i+2]);
                i+=2;
            }
            else{
                arg_error(args[i]);
                return 1;
            }
        }
        else if (strcmp(args[i], "-fish") == 0){
            if(i + 1 < argn){
                cur_filter.Type = FISH;
                cur_filter.params.fisheye.strength = atof(args[i + 1]);
                i++;
            }
            else{
                arg_error(args[i]);
                return 1;
            }
        }
        else{
            fprintf(stderr, "Unknown filter %s!\n", args[i]);
            continue;
        }
        add_filter(pipeline, cur_filter);
    }

    apply_pipeline(image, pipeline);
    save_bmp(output_path, image);

    fprintf(stdout, "Program finished! %d filters applied", pipeline->count);

    destroy_image(image);
    destroy_pipeline(pipeline);
    return 0;
}


void print_help(){
    fprintf(stdout, "Usage: {program} {input file} {output file} [-{filter} [param 1] [param 2] ...] ... \n");
    fprintf(stdout, "\nFilters usage:\n");
    fprintf(stdout, "\t-crop {width} {height}                    Cut image to new size\n");
    fprintf(stdout, "\t-gs                                       Make image monochrome\n");
    fprintf(stdout, "\t-neg                                      Invert colors\n");
    fprintf(stdout, "\t-sharp                                    Make edges clear\n");
    fprintf(stdout, "\t-edge {threshold}                         Find and highlight edges\n");
    fprintf(stdout, "\t-med {window}                             Reduce noise\n");
    fprintf(stdout, "\t-blur {sigma}                             Make image blurry\n");
    fprintf(stdout, "\t-cluster {centroids}                      Reduce number of colors\n");
    fprintf(stdout, "\t-fish {strength}                          Curving image from center\n");
    fprintf(stdout, "\t-mosaic {dataset file} {dataset size}     Create image made of small pictures\n");
}

void arg_error(char* filter_name){
    fprintf(stderr, "Wrong or no args for filter: %s\n", filter_name);
    print_help();
}
