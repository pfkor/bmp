#pragma once

#include "bmp.h"

typedef enum {
    CROP, NEG, GS, SHARP,
    EDGE, MED, CLUSTER,
    BLUR, FISH, MOSAIC
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
        char *filepath;
        int tiles_number;
    } mosaic;
    struct {
        float strength;
    } fisheye;
    struct {
        char dummy;  // Заглушка для фильтров без параметров
    } none;
}FilterParams;

typedef struct {
    FilterType Type;
    FilterParams params;
}Filter;


int crop(Image* image, unsigned int x_from, unsigned int x_to, unsigned int y_from, unsigned int y_to);
int multiply_channels(Image* image, float r_factor, float g_factor, float b_factor);

int matrix_sharpening(Image* image);
int gaussian_blur(Image* image, float sigma);
int fast_gaussian_blur(Image* image, float sigma);

int edge(Image* image, float threshold);

int median_by_channel(Image* image, int wind_size);
int kmeans_cluster(Image* image, int centr_c);
int fish_eye(Image* image, float strength);

int negative (Image *image);
int monochrome (Image *image);

int create_tiles(char *filepath, int* tiles_number);
int replace_tiles(Image *image, int tiles_number);