#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "filters.h"

typedef struct {
    Filter* filters;
    int count;
    int success;
    int capacity;
} FilterPipeline;

FilterPipeline* create_pipeline();
void destroy_pipeline(FilterPipeline* pipeline);

void add_filter(FilterPipeline* pipeline, Filter filter);
void apply_pipeline(Image* image, FilterPipeline* pipeline);

