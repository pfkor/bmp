#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

Image* create_image(unsigned int width, unsigned int height){
    if (width == 0 || height == 0){
        return NULL;
    }

    Image* image= malloc(sizeof(Image));
    if (!image){
        return NULL;
    }

    image->width = width;
    image->height = height;

    image->data = malloc(sizeof(Color*) * height);
    if (!image->data){
        free(image);
        return NULL;
    }
    for (int i = 0; i < height; i++){
        image->data[i] = malloc(sizeof(Color) * width);
        if (!image->data[i]){
            for (int j = 0; j < i; j++){
                free(image->data[j]); // free prev rows
            }
            free(image->data);
            free(image);
            return NULL;
        }
    }
    return image;
}

void destroy_image(Image* image){
    if (!image){
        return;
    }

    if (image->data){
        for (int i = 0; i < image->height; i++){
            free(image->data[i]);
        }
        free(image->data);
    }
    free(image);
    return;
}

Image* load_bmp(const char *filepath){
    if (!filepath){
        return NULL;
    }

    FILE *input = fopen(filepath, "rb");
    if (!input){
        fprintf(stderr, "File not found!\n");
        return NULL;
    }

    Header buffer;
    if (fread(&buffer, sizeof(Header), 1, input) != 1){
        fprintf(stderr, "Cannot read BMP header!\n");
        fclose(input);
        return NULL;
    }

    if (buffer.bmp.id == 0x424D){
        fprintf(stderr, "Not valid BMP!\n");
        fclose(input);
        return NULL;
    }

    int width = buffer.dib.width;
    int height = buffer.dib.height;

    if (height < 0){
        height = -height;
    }

    Image *image = create_image(width, height);
    if (!image){
        fprintf(stderr, "Failed to allocate image!\n");
        fclose(input);
        return NULL;
    }

    int padding = (4 - (width * 3) % 4) % 4;
    fseek(input, buffer.bmp.offset, SEEK_SET);

    for (int y = height - 1; y >= 0; y--){
        for (int x = 0; x < width; x++){
            Pixel_24 p;

            if (fread(&p, sizeof(Pixel_24), 1, input) != 1){
                fprintf(stderr, "Failed to read pixel [%d][%d] data!\n", y, x);
                destroy_image(image);
                fclose(input);
                return NULL;
            }
            
            set_color(image, x, y, pixel_to_color(p));
        }

        fseek(input, padding, SEEK_CUR);
    }

    fclose(input);
    return image;
}

void save_bmp (const char *filepath, Image *image){

    if (!filepath || !image){
        return;
    }

    int width = image->width;
    int height = image->height;
    
    int padding = (4 - (width * 3) % 4) % 4;
    int rawBM = (width * 3 + padding) * height;
        
    BMP_Header header_bmp = {0x424D, 54 + rawBM, 0, 0, 54};
    DIB_Header header_dib = {40, width, height, 1, 24, 0, rawBM, 2835, 2835, 0, 0};
    Header header = {header_bmp, header_dib};

    FILE *output = fopen(filepath, "wb");
    if (!output){
        fprintf(stderr, "Failed to open/create an output file!\n");
        return;
    }

    if (fwrite(&header, sizeof(header), 1, output) != 1){
        fprintf(stderr, "Failed to write the image!\n");
        fclose(output);
        return;
    }

    for (int y = height-1; y >= 0; y--){
        for (int x = 0; x < width; x++){
            Pixel_24 p = color_to_pixel(get_color(image, x, y));
            fwrite(&p, sizeof(Pixel_24), 1, output);
        }
        for (int z = 0; z < padding; z++){
            fputc(0, output);
        }
    }

    fclose(output);
}

Color get_color(Image *image, int x, int y){
    if (!image || !image->data || x < 0 || x >= image->width || y < 0 || y >= image->height){
        return (Color){0, 0, 0};
    }
    return image->data[y][x];
}

void set_color(Image *image, int x, int y, Color color){
    if (!image || !image->data || x < 0 || x >= image->width || y < 0 || y >= image->height){
        return;
    }
    image->data[y][x] = color;
    return;
}