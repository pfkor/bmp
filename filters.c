#include "filters.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include "bmp.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define swap(type) \
    void swap_ ## type(type* a, type* b) { \
        type temp = *a;\
        *a = *b;\
        *b = temp;\
    }

swap(float);

#ifdef _WIN32
    #define CLEAR_CONSOLE "cls"
#else
    #define CLEAR_CONSOLE "clear"
#endif

#define FREE_IF_NEEDED(ptr) \
  if(ptr) free(ptr);



void crop(Image* image, unsigned int x_from, unsigned int x_to, unsigned int y_from, unsigned int y_to){
    if (!image || !image->data) return;
    
    unsigned int w = image->width;
    unsigned int h = image->height;
    if (w == 0 || h == 0 || \
    x_from >= w || x_to > w || x_to <= x_from || \
    y_from >= h || y_to > h || y_to <= y_from){
        return;
    }

    unsigned int new_w = x_to - x_from;
    unsigned int new_h = y_to - y_from;

    Color **temp = malloc(sizeof(Color *) * new_h);
    if (!temp) return;

    for (unsigned int y = 0; y < new_h; y++){
        temp[y] = malloc(sizeof(Color) * new_w);
        if (!temp[y]){

            for (unsigned int j = 0; j < y; j++){
                free(temp[j]);
            }
            free(temp);
            return;
        }
    }

    for (unsigned int y = 0; y < new_h; y++){
        for (unsigned int x = 0; x < new_w; x++){
            temp[y][x] = get_color(image, x_from + x, y_from + y);
        }
    }

    for (int y = 0; y < h; y++){
        free(image->data[y]);
    }
    free(image->data);

    image->width = new_w;
    image->height = new_h;
    image->data = temp;
}

void multiply_channels(Image* image, float r_factor, float g_factor, float b_factor){
    if (!image || !image->data) return;
    if (r_factor < 0 || r_factor > 1 || g_factor < 0 || g_factor > 1 || b_factor < 0 || b_factor > 1){
        return;
    }
    
    for (unsigned int y = 0; y < image->height; y++){
        for (unsigned int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r *= r_factor;
            c.g *= g_factor;
            c.b *= b_factor;
            set_color(image, x, y, c);
        }
    }
}

void negative (Image *image){
    if (!image || !image->data) return;

    for (unsigned int y = 0; y < image->height; y++){
        for (unsigned int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r = 255 - c.r;
            c.g = 255 - c.g;
            c.b = 255 - c.b;
            set_color(image, x, y, c);
        }
    }
}

void monochrome (Image *image){
    if (!image || !image->data) return;

    for (unsigned int y = 0; y < image->height; y++){
        for (unsigned int x = 0; x < image->width; x++){
            Color c = get_color(image, x, y);
            c.r = 0.299 * c.r + 0.587 * c.g + 0.114 * c.b;
            c.g = c.r;
            c.b = c.r;
            set_color(image, x, y, c);
        }
    }   
}

void get_window(Image *window, unsigned int x, unsigned int y, Image *image){
    if (!window || !window->data || !image || !image->data) return;
    if (window->width != window->height || window->width % 2 == 0) return;

    unsigned int w = image->width;
    unsigned int h = image->height;
    int radius = (window->width - 1) / 2;

    for (int ky = -radius; ky <= radius; ky++){
        for (int kx = -radius; kx <= radius; kx++){

            int pixel_x = (int)x + kx;
            int pixel_y = (int)y + ky;

            if (pixel_x < 0) pixel_x = 0;
            if (pixel_x >= w) pixel_x = w - 1;
            if (pixel_y < 0) pixel_y = 0;
            if (pixel_y >= h) pixel_y = h - 1;
            
            set_color(window, kx + radius, ky + radius, get_color(image, pixel_x, pixel_y));
        }
    }
}
Color impose_matrix(Image *window, float *matrix,  int size){
    if (!window || !window->data) return (Color){0, 0, 0};
    Color res = {0, 0, 0};
    
    for (int y = 0; y < window->height; y++){
        for (int x = 0; x < window->width; x++){
            Color cur = get_color(window, x, y);
            res.r += cur.r * matrix[y * size + x];
            res.g += cur.g * matrix[y * size + x];
            res.b += cur.b * matrix[y * size + x];
        }
    }

    limit_color(&res);
    return res;
}

void matrix_sharpening(Image* image){

    if (!image || !image->data) return;

    unsigned int w = image->width;
    unsigned int h = image->height;


    float kernel[9] = {0, -1, 0, -1, 5, -1, 0, -1, 0};

    Image *temp = create_image(w, h);
    if (!temp){
        return;
    }

    Image *window = create_image(3, 3);
    if (!window){
        destroy_image(temp);
        return;
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){

            get_window(window, x, y, image);
            Color sum = impose_matrix(window, kernel, 3);
            limit_color(&sum);

            set_color(temp, x, y, sum);
        }
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){
            set_color(image, x, y, get_color(temp, x, y));
        }
    }

    destroy_image(window);
    destroy_image(temp);
}

void gaussian_blur(Image *image, float sigma){
    if (!image || !image->data) return;
    clock_t timer;
    timer = clock();

    unsigned int w = image->width;
    unsigned int h = image->height;

    int kernel_size = 2 * (int)ceil(sigma*3) + 1;
    // int kernel_size = 3;

    float *kernel = malloc(sizeof(float) * kernel_size * kernel_size);
    if (!kernel) return;

    int center = kernel_size/2;
    float sum = 0.0;
    for (int i = 0; i < kernel_size; i++){
        for (int j = 0; j < kernel_size; j++){
            int x = i - center;
            int y = j - center;

            // G(x, y) = (1 / (2πσ²)) × exp(-(x² + y²) / (2σ²))          
            float exponent = -(x*x + y*y) / (2*sigma*sigma);  
            float value = exp(exponent) / (2*M_PI*sigma*sigma);

            kernel[i*kernel_size + j] = value;
            sum += value;
        }
    }
    for (int i = 0; i < kernel_size * kernel_size; i++) kernel[i] /= sum;

    Image *temp = create_image(w, h);
    if (!temp){
        free(kernel);
        return;
    }
    Image *window = create_image(kernel_size, kernel_size);
    if (!window){
        free(kernel);
        destroy_image(temp);
        return;
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){

            get_window(window, x, y, image);
            Color sum = impose_matrix(window, kernel, kernel_size);
            limit_color(&sum);

            set_color(temp, x, y, sum);
        }
        // if (y % 256 == 0) printf("%d/%d\n", y, h);
    }
    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){
            set_color(image, x, y, get_color(temp, x, y));
        }
    }

    timer = clock() - timer;
    double time_taken = ((double)timer) / CLOCKS_PER_SEC;
    printf("It was %f seconds\n", time_taken);

    free(kernel);
    destroy_image(temp);
    destroy_image(window);

}

void edge(Image* image, float threshold){
    if (!image || !image->data) return;

    monochrome(image);
    unsigned int w = image->width;
    unsigned int h = image->height;


    float kernel[9] = {0, -1, 0, -1, 4, -1, 0, -1, 0};


    Image *temp = create_image(w, h);
    if (!temp){
        return;
    }

    Image *window = create_image(3, 3);
    if (!window){
        destroy_image(temp);
        return;
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){

            get_window(window, x, y, image);
            Color sum = impose_matrix(window, kernel, 3);
            if(sum.r/255 > threshold || sum.g/255 > threshold  || sum.b/255 > threshold){
                set_color(temp, x, y, (Color){255, 255, 255});
            }
            else{
                set_color(temp, x, y, (Color){0, 0, 0});
            }

        }
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){
            set_color(image, x, y, get_color(temp, x, y));
        }
    }

    destroy_image(window);
    destroy_image(temp);
}


static int partition(float arr[], int left, int right, int pivot_index) {
    int pivot_value = arr[pivot_index];

    swap_float(&arr[pivot_index], &arr[right]); // убираем пивот в конец, потом найдется нужное место

    int store_index = left;
    for (int i = left; i < right; i++) { // пробегаем не доходя до конца, где лежит пивот
        if (arr[i] <= pivot_value) {
            swap_float(&arr[i], &arr[store_index]);
            store_index++;
        }
    }
    swap_float(&arr[store_index], &arr[right]); // store_index будет именно там где граница между маленькими и большими
    return store_index;
}
static float quick_select(float arr[], int left, int right, int k){
    while (left < right) {
        int pivot_index = left + rand() % (right - left + 1);
        pivot_index = partition(arr, left, right, pivot_index);

        if (k == pivot_index) {
            return arr[k];
        } else if (k < pivot_index) {
            right = pivot_index - 1;
        } else {
            left = pivot_index + 1;
        }
    }
    return arr[left];

}


void median_by_channel(Image* image, int wind_size){
    clock_t timer;
    timer = clock();
    if (!image || !image->data) return;

    unsigned int w = image->width;
    unsigned int h = image->height;


    Image *temp = create_image(w, h);
    if (!temp){
        return;
    }

    Image* window = create_image(wind_size, wind_size);
    if (!window){
        destroy_image(temp);
        return;
    }
    unsigned int window_side = window->width;
    unsigned int pixel_count = window_side*window_side;

    float* r_vals = malloc(pixel_count * sizeof(float ));
    float* g_vals = malloc(pixel_count * sizeof(float ));
    float* b_vals = malloc(pixel_count * sizeof(float ));

    if(!r_vals || !g_vals || !b_vals){
        FREE_IF_NEEDED(r_vals);
        FREE_IF_NEEDED(g_vals);
        FREE_IF_NEEDED(b_vals);
        destroy_image(temp);
        destroy_image(window);
    }

    int checkpoint = h/20;

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){
            get_window(window, x, y, image);

            unsigned int count = 0;
            for(int y1 = 0; y1 < window_side; y1++){
                for(int x1 = 0; x1 < window_side; x1++){
                    Color cur_color = get_color(window, x1, y1);
                    r_vals[count] = cur_color.r;
                    g_vals[count] = cur_color.g;
                    b_vals[count] = cur_color.b;
                    count++;
                }
            }

            int k = count /2;

            float median_red = quick_select(r_vals, 0, count-1, k);
            float median_green = quick_select(g_vals, 0, count-1, k);
            float median_blue = quick_select(b_vals, 0, count-1, k);

            Color med = {median_red, median_green,median_blue};
            if (x == 0 && y % checkpoint == 0){
                system(CLEAR_CONSOLE);
                printf("Median processing: [");
                for (int i = 0; i < (int)(100*y/h); i++) printf("#");
                for (int i = (int)(100*y/h); i < 100; i++) printf(".");
                printf("] %d%%\n", (int)(100*y/h));                
            }
            limit_color(&med);
            set_color(temp, x, y, med);

        }
    }

    for (unsigned int y = 0; y < h; y++){
        for (unsigned int x = 0; x < w; x++){
            set_color(image, x, y, get_color(temp, x, y));
        }
    }
    timer = clock() - timer;

    system(CLEAR_CONSOLE);
    printf("Median processing: [");
    for (int i = 0; i < 100; i++) printf("#");
    printf("] 100%%\n");
    
    double time_taken = ((double)timer) / CLOCKS_PER_SEC;
    printf("It was %f seconds\n", time_taken);
    free(r_vals);
    free(g_vals);
    free(b_vals);
    destroy_image(window);
    destroy_image(temp);
}

static Color* select_centroid(Image* image, int k){
    srand(time(NULL));
    unsigned int h = image->height;
    unsigned int w = image->width;
    double total = 0;

    Color* centroids = malloc(k * sizeof(Color));
    double** cumulative =  malloc(h * sizeof(double *));
    char rows_alloced = 1;
    for(int i = 0; i < h; i++){
        cumulative[i] = malloc(w * sizeof(double));
    }
    float** dists = malloc(h * sizeof(double*));
    char rows_alloced_1 = 1;
    for(int i = 0; i < h; i++){
        dists[i] = malloc(w * sizeof(double));
        if(!dists[i]) rows_alloced = 0;
    }

    if(!centroids || !cumulative || !dists || !rows_alloced || !rows_alloced_1){
        FREE_IF_NEEDED(centroids);
        FREE_IF_NEEDED(cumulative);
        FREE_IF_NEEDED(dists);
        if(!rows_alloced){
            for(int i = 0; i < h; i++){
                free(cumulative[i]);
            }
        }
        if(!rows_alloced_1){
            for(int i = 0; i < h; i++){
                free(dists[i]);
            }
        }
        printf("Cluster filter: allocation failed\n");
        return NULL;

    }

    int random_x = rand() % w;
    int random_y = rand() % h;
    centroids[0] = get_color(image, random_x, random_y);

    for(int i = 1; i < k; i++){
        float cur_sum = 0;
        for(int y = 0; y < h; y++){
            for(int x = 0; x < w; x++){
                Color cur_color = get_color(image,x,y);
                double min_dist = min_distance(cur_color, centroids, i);
                dists[y][x] = min_dist;
                total += min_dist;
                cur_sum += min_dist;
                cumulative[y][x] = cur_sum;

            }
        }
        int random_num = rand() % (int)(total + 1);

        char found = 0;
        for(int y = 0; y < h && !found; y++){
            for(int x = 0; x < w && !found; x++){
                if(random_num < cumulative[y][x]){
                    centroids[i] = get_color(image, x,y);
                    found = 1;

                }

            }
        }
    }
    return centroids;


}

void kmeans_cluster(Image* image, int centr_c){
    srand(time(NULL));
    unsigned int w = image->width;
    unsigned int h = image->height;
    int tolerance = 1;
    int max_iters = 20;

    Color* centroids = malloc(centr_c * sizeof(Color));
    Color* new_centroids = malloc(centr_c * sizeof(Color));

    double* red_avg = malloc(sizeof(double) * centr_c);
    double* green_avg = malloc(sizeof(double) * centr_c);
    double* blue_avg =  malloc(sizeof(double) * centr_c);
    int* labeled_count =  malloc(sizeof(int) * centr_c);

    int** centroid_labels = malloc(h * sizeof(int*));
    char rows_alloced = 1;
    for(int i = 0; i < h; i++){
        centroid_labels[i] = malloc(w * sizeof(int));
        if(!centroid_labels[i]) rows_alloced = 0;
    }

    if(!centroids || !new_centroids || !red_avg || !green_avg || !blue_avg || !labeled_count || !centroid_labels || !rows_alloced){
        printf("Cluster filter: allocation failed\n");
            if(!rows_alloced){
                for(int i = 0; i < h; i++){
                    free(centroid_labels[i]);
                }
            }
        FREE_IF_NEEDED(centroids);
        FREE_IF_NEEDED(new_centroids);
        FREE_IF_NEEDED(red_avg);
        FREE_IF_NEEDED(green_avg);
        FREE_IF_NEEDED(blue_avg);
        FREE_IF_NEEDED(labeled_count);
        FREE_IF_NEEDED(centroid_labels);
        return;

    }

    centroids = select_centroid(image, centr_c);

    for(int n =  0; n < max_iters; n++){
        memset(red_avg, 0, centr_c * sizeof(double));
        memset(green_avg, 0, centr_c * sizeof(double));
        memset(blue_avg, 0, centr_c * sizeof(double));
        memset(labeled_count, 0, centr_c * sizeof(int));

        for(int y = 0; y < h; y++){
            for(int x = 0; x < w; x++){
                Color cur_color = get_color(image, x, y);
                int cur_label = get_nearest(cur_color, centroids, centr_c);
                centroid_labels[y][x] = cur_label;

                labeled_count[cur_label]++;
                red_avg[cur_label] += cur_color.r;
                green_avg[cur_label] += cur_color.g;
                blue_avg[cur_label] += cur_color.b;
            }
        }
        char centrs_changed = 0;
        for(int i = 0; i < centr_c; i++){
            int cur_labeled_count = labeled_count[i];
            if(cur_labeled_count != 0){
                new_centroids[i] = (Color){red_avg[i]/cur_labeled_count, green_avg[i]/cur_labeled_count, blue_avg[i]/cur_labeled_count};
                float dist = get_distance(centroids[i], new_centroids[i]);
                if(dist > tolerance){
                    centrs_changed = 1;
                }
            }
            else{
                new_centroids[i] = centroids[i];
            }
        }
        if(centrs_changed){
            memcpy(centroids, new_centroids, centr_c * sizeof(Color));
            memset(new_centroids, 0, centr_c * sizeof(Color));
        }
        else{
            break;
        }


    }
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            int cur_label = centroid_labels[y][x];
            Color centr_color = centroids[cur_label];
            set_color(image, x, y, centr_color);
        }
    }

    free(centroids);
    free(new_centroids);
    free(red_avg);
    free(green_avg);
    free(blue_avg);
    free(labeled_count);
    for(int i = 0; i < h; i++){
        free(centroid_labels[i]);
    }
    free(centroid_labels);

}



void create_tiles(char *filepath, int *tiles_number){
    FILE *input = fopen(filepath, "rb");
    if (!input){
        fprintf(stderr, "Failed to open dataset file!\n");
        return;
    }

    uint8_t skip, value;
    Image *current = create_image(32, 32);
    char *filename = malloc(sizeof(char) * 32);
    if (!current || !filename){
        fprintf(stderr, "Failed to allocate memory!\n");
        fclose(input);
        if (current) free(current);
        if (filename) free(filename);
        return;
    }

    for (int i = 0; i < *tiles_number; i++){
        if (fread(&skip, sizeof(uint8_t), 1, input) != 1) printf("Failed to skip a byte\n");

        for (int y = 0; y < 32; y++){
            for (int x = 0; x < 32; x++){
                if (fread(&value, sizeof(uint8_t), 1, input) != 1){
                    fprintf(stderr, "Failed to read red %d!\n", i);
                    fclose(input);
                    destroy_image(current);
                    free(filename);
                    *tiles_number = i-1;
                    return;
                }
                current->data[y][x].r = value;
            }
        }
        for (int y = 0; y < 32; y++){
            for (int x = 0; x < 32; x++){
                if (fread(&value, sizeof(uint8_t), 1, input) != 1){
                    fprintf(stderr, "Failed to read green %d!\n", i);
                    fclose(input);
                    destroy_image(current);
                    free(filename);
                    *tiles_number = i-1;
                    return;
                }
                current->data[y][x].g = value;
            }
        }
        for (int y = 0; y < 32; y++){
            for (int x = 0; x < 32; x++){
                if (fread(&value, sizeof(uint8_t), 1, input) != 1){
                    fprintf(stderr, "Failed to read blue %d!\n", i);
                    fclose(input);
                    destroy_image(current);
                    free(filename);
                    *tiles_number = i-1;
                    return;
                }
                current->data[y][x].b = value;
            }
        }
        sprintf(filename, "./tiles/%03d.bmp", i);
        save_bmp(filename, current);
    }

    free(current);
    free(filename);
    fclose(input);
}


Color get_average(Image *image, unsigned int x_from, unsigned int x_to, unsigned int y_from, unsigned int y_to){
    if (!image || !image->data) return (Color){0, 0, 0};
    unsigned int sum_r = 0, sum_g = 0, sum_b = 0;
    unsigned int c = 0;

    unsigned int y_lim = (y_to >= image->height) ? image->height : y_to;
    unsigned int x_lim = (x_to >= image->width) ? image->width : x_to;

    for (unsigned int y = y_from; y < y_lim; y++){
        for (unsigned int x = x_from; x < x_lim; x++){
            Color cur = get_color(image, x, y);
            sum_r += cur.r;
            sum_g += cur.g;
            sum_b += cur.b;
            c++;
        }
    }
    if (c == 0) return (Color){0, 0, 0};
    Color res = {sum_r/c, sum_g/c, sum_b/c};
    limit_color(&res);
    return res;
}


void replace_tiles(Image *image, int tiles_number){
    if (!image || !image->data) return;

    int tile_size = 32;
    Image *current = create_image(tile_size, tile_size);
    char *filepath = malloc(sizeof(char) * 100);
    if (!current){
        fprintf(stderr, "Failed to allocate memory!\n");
        if (current) destroy_image(current);
        if (filepath) free(filepath);
        return;
    }

    Color palette[tiles_number];

    for (int i = 0; i < tiles_number; i++){
        sprintf(filepath, "./tiles/%03d.bmp", i);
        current = load_bmp(filepath);
        if (!current){
            fprintf(stderr, "Failed to load tile! %d\n", i);
            free(filepath);
            return;
        }

        palette[i] = get_average(current, 0, tile_size, 0, tile_size);    
    }

    unsigned int w = image->width;
    unsigned int h = image->height;

    for (unsigned int y = 0; y <= h; y += tile_size){
        for (unsigned int x = 0; x <= w; x += tile_size){

            Color average = get_average(image, x, x+tile_size, y, y+tile_size);
            int nearest_idx = get_nearest(average, palette, tiles_number);
            
            sprintf(filepath, "./tiles/%03d.bmp", nearest_idx);
            current = load_bmp(filepath);
            if (!current){
                fprintf(stderr, "Failed to load tile! %d\n", nearest_idx);
                free(filepath);
                return;
            }

            for (int ky = 0; ky < tile_size; ky++){
                for (int kx = 0; kx < tile_size; kx++){
                    set_color(image, x+kx, y+ky, get_color(current, kx, ky));
                }
            }
        }
    }

    free(current);
    free(filepath);
}

void fish_eye(Image* image, float strength){
    unsigned int w = image->width;
    unsigned int h = image->height;
    Image* temp = create_image(w,h);

    float cx = w / 2.0f;
    float cy = h / 2.0f;

    float vec_r_x = w - 1 - cx;
    float vec_r_y = h - 1 - cy;

    float min_r = vec_r_y < vec_r_x ? vec_r_y : vec_r_x;

    for (int y_out = 0; y_out < h; y_out++){
        for (int x_out = 0; x_out < w; x_out++){
            float new_x = x_out - cx;
            float new_y = y_out - cy;

            float dist = sqrt(new_x * new_x + new_y * new_y);
            float dist_norm = dist / min_r;

            if(dist_norm > 1.0f){
                set_color(temp,x_out, y_out, (Color){0,0,0});
                continue;
            }

            float new_dist_norm = dist_norm * ((1 + strength * dist_norm * dist_norm)/ (1 + strength));
            float  new_dist = new_dist_norm * min_r;

            float x_src;
            float y_src;

            if(dist != 0){
                float factor = new_dist / dist;
                x_src = cx + new_x * factor;
                y_src = cy + new_y * factor;
            }
            else{
                x_src = cx;
                y_src = cy;
            }

            int x0 = (int) floor(x_src);
            int y0 = (int) floor(y_src);
            int x1 = x0 + 1;
            int y1 = y0 + 1;

            float dx = x_src - x0;
            float dy = y_src - y0;
            float dx1 = 1.0 - dx;
            float dy1 = 1.0 - dy;

            Color res;

            Color p00 = get_color(image,x0, y0);
            Color p01 = get_color(image,x0, y1);
            Color p10 = get_color(image,x1, y0);
            Color p11 = get_color(image,x1, y1);
            float w00 = dx1 * dy1;
            float w01 = dx1 * dy;
            float w10 = dx * dy1;
            float w11 = dx * dy;

            float red = w00 * p00.r + w01 * p01.r + w10 * p10.r + w11 * p11.r;
            float green = w00 * p00.g + w01 * p01.g + w10 * p10.g + w11 * p11.g;
            float blue = w00 * p00.b + w01 * p01.b + w10 * p10.b + w11 * p11.b;

            res = (Color){red,green,blue};

            set_color(temp, x_out, y_out, res);
        }
    }
    for(int y = 0; y < h; y++){
        for(int x = 0; x < w; x++){
            Color cur_color = get_color(temp, x, y);
            set_color(image, x, y, cur_color);
        }
    }
    destroy_image(temp);
}