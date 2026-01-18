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
  for(int i = 0; i < pipeline->count; i++){
    Filter curFilter = pipeline->filters[i];

    switch (curFilter.Type) {

    case NEG:     negative(image); break;
    case GS:      monochrome(image); break;

    case CROP:    crop(image, 0, curFilter.params.crop.width, 0, curFilter.params.crop.height); break;

    case SHARP:   matrix_sharpening(image); break;
    case EDGE:    edge(image, curFilter.params.edge.threshold); break;

    case MED:     median_by_channel(image, curFilter.params.median.window); break;

    case BLUR:    gaussian_blur(image, curFilter.params.blur.sigma); break;


    case CLUSTER: kmeans_cluster(image, curFilter.params.cluster.centroids); break;

    case MOSAIC: 
        create_tiles(curFilter.params.mosaic.filepath, &curFilter.params.mosaic.tiles_number);
        replace_tiles(image, curFilter.params.mosaic.tiles_number);
        break;

    case FISH:   fish_eye(image, curFilter.params.fisheye.strength);  break;


    default: break;
    }
  }
}