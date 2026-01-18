#include "pipeline.h"
#include "bmp.h"

FilterPipeline* create_pipeline(){
    FilterPipeline* result = malloc(sizeof(FilterPipeline));
    result->filters = NULL;
    result->count = result->capacity = 0;
    return  result;
}

void destroy_pipeline(FilterPipeline* pipeline){
    free(pipeline->filters);
    free(pipeline);
}

void add_filter(FilterPipeline* pipeline, Filter filter){

    if(pipeline->count >= pipeline->capacity){
        pipeline->filters = realloc(pipeline->filters, (pipeline->capacity + 1) * 2 * sizeof(Filter));
        pipeline->capacity = (pipeline->capacity + 1) * 2;
    }
    pipeline->filters[pipeline->count] = filter;
    pipeline->count++;
}

void apply_pipeline(Image* image, FilterPipeline* pipeline){
    
    pipeline->success = pipeline->count;
    int result = 0;

    for(int i = 0; i < pipeline->count; i++){
        Filter curFilter = pipeline->filters[i];

        switch (curFilter.Type) {

        case NEG: 
            result = negative(image);
            if(result == 0){
                printf("Filter \"negative\" successfully applied.\n");
            } else {
                printf("Filter \"negative\" failed.\n");
            }
            break;
        case GS: 
            result = monochrome(image);
            if(result == 0){
                printf("Filter \"grayscale\" successfully applied.\n");
            } else {
                printf("Filter \"grayscale\" failed.\n");
            }
            break;

        case CROP: 
            result = crop(image, 0, curFilter.params.crop.width, 0, curFilter.params.crop.height);
            if(result == 0){
                printf("Filter \"crop\" successfully applied.\n");
            } else {
                printf("Filter \"crop\" failed.\n");
            }
            break;

        case SHARP: 
            result = matrix_sharpening(image);
            if(result == 0){
                printf("Filter \"sharpening\" successfully applied.\n");
            } else {
                printf("Filter \"sharpening\" failed.\n");
            }
            break;
        case EDGE: 
            result = edge(image, curFilter.params.edge.threshold);
            if(result == 0){
                printf("Filter \"edges\" successfully applied.\n");
            } else {
                printf("Filter \"edges\" failed.\n");
            }
            break;

        case MED: 
            result = median_by_channel(image, curFilter.params.median.window);
            if(result == 0){
                printf("Filter \"median\" successfully applied.\n");
            } else {
                printf("Filter \"median\" failed.\n");
            }
            break;

        case BLUR: 
            result = gaussian_blur(image, curFilter.params.blur.sigma);
            if(result == 0){
                printf("Filter \"gaussian blur\" successfully applied.\n");
            } else {
                printf("Filter \"gaussian blur\" failed.\n");
            }
            break;


        case CLUSTER: 
            result = kmeans_cluster(image, curFilter.params.cluster.centroids);
            if(result == 0){
                printf("Filter \"posterisation\" successfully applied.\n");
            } else {
                printf("Filter \"posterisation\" failed.\n");
            }
            break;

        case MOSAIC: 
            result = create_tiles(curFilter.params.mosaic.filepath, &curFilter.params.mosaic.tiles_number) + replace_tiles(image, curFilter.params.mosaic.tiles_number);
            if(result == 0){
                printf("Filter \"mosaic\" successfully applied.\n");
            } else {
                if (result == 2) result--;
                printf("Filter \"mosaic\" failed.\n");
            }
            break;

        case FISH:
            result = fish_eye(image, curFilter.params.fisheye.strength);
            if(result == 0){
                printf("Filter \"fish eye\" successfully applied.\n");
            } else {
                printf("Filter \"fish eye\" failed.\n");
            }
            break;

        default: break;
        }

        pipeline->success -= result;
    }
}