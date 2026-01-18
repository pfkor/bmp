#pragma once

#include "bmp.h"

typedef enum {
    COPY, CROP, FLIP_H, FLIP_V, FLIP_BOTH,
    RED, GREEN, BLUE, NEG, YELLOW, CYAN, MAGENTA, GS,
    SHARP, EDGE, MED, CLUSTER, BLUR,
    MOSAIC
} FilterType;

typedef union {
    struct {
        int width;
        int height;
    } crop;
    struct {
        float threshold;
    } edge;
    struct {
        int window;
    } median;
    struct {
        float sigma;
    } blur;
    struct {
        int centroids;
    } cluster;
    struct {
        int tile_size;
        char *filepath;
        int tiles_number;
    } mosaic;
    struct {
        char dummy;  // Заглушка для фильтров без параметров
    } none;
}FilterParams;

typedef struct {
    FilterType Type;
    FilterParams params;
}Filter;

// Filter filter_init(FilterType Type, char args[1024], void (*pixel_func)(Color*));
// void all_pixel_proccess(Image* image, void (*pixel_func)(Color*));

void crop(Image* image, unsigned int x_from, unsigned int x_to, unsigned int y_from, unsigned int y_to);
void multiply_channels(Image* image, float r_factor, float g_factor, float b_factor);

void matrix_sharpening(Image* image);
void gaussian_blur(Image* image, float sigma);

void edge(Image* image, float threshold);

// void median(Image* image, int wind_size);
void median_by_channel(Image* image, int wind_size);
void kmeans_cluster(Image* image, int centr_c);

void negative (Image *image);
void monochrome (Image *image);

void create_tiles(char *filepath, int tiles_number);
void average_tiles(Image *image, int tile_width);
void replace_tiles(Image *image, int tiles_number);