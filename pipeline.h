#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "filters.h"

typedef struct image img;

typedef struct {
    Filter* filters;
    int count;
    int capacity;
} FilterPipeline;

FilterPipeline* create_pipeline();
void free_pipeline(FilterPipeline* pipeline);
void add_filter(FilterPipeline* pipeline, Filter filter);
img* apply_pipeline(img* image, FilterPipeline* pipeline);

