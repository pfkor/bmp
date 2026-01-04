#include "pipeline.h"
#include "bmp.h"

FilterPipeline* create_pipeline(){

    FilterPipeline* result = malloc(sizeof(FilterPipeline));
    result->filters = NULL;
    result->count = result->capacity = 0;
    return  result;
}

void free_pipeline(FilterPipeline* pipeline){

    free(pipeline->filters);
    // тут ещё по идее надо будет проверять нет у фильтра с собой матрицы в кармане
}

void add_filter(FilterPipeline* pipeline, Filter filter){

    if(pipeline->count >= pipeline->capacity){
        pipeline->filters = realloc(pipeline->filters, (pipeline->capacity + 1) * 2);
        pipeline->capacity = (pipeline->capacity + 1) * 2;
    }
    printf("i was alive");
    pipeline->filters[pipeline->count] = filter;
    pipeline->count++;


}

img* apply_pipeline(img* image, FilterPipeline* pipeline){
  for(int i = 0; i < pipeline->count; i++){
      Filter currFilter = pipeline->filters[i];

      switch (currFilter.Type) {

          // фильтры с проходом по всем пикселям без аргументов
          case RED: case BLUE: case GREEN: case NEG: case YELLOW: case CYAN: case MAGENTA: case GS:
              all_pixel_proccess( image,currFilter.params.pixelFunc);
              break;



      }
  }
}