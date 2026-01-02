#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

img* img_init(unsigned int w, unsigned int h){
    img *image = malloc(sizeof(img));
    if (!image) return NULL;

    image->width = w;
    image->height = h;

    image->bitmap = malloc(sizeof(pixel*) * h);
    if (!image->bitmap){
        free(image);
        return NULL;
    }

    for (int i = 0; i < h; i++){
        image->bitmap[i] = malloc(sizeof(pixel) * w);
        
        if (!image->bitmap[i]){
            for (int j = 0; j < i; j++){
                free(image->bitmap[j]);
            }
            free(image->bitmap);
            free(image);
            return NULL;
        }
    }
    return image;
}

void img_destroy(img* image){
    if (!image) return;

    if (image->bitmap){
        for (int i = 0; i < image->height; i++){
            free(image->bitmap[i]);
        }
        free(image->bitmap);
    }
    free(image);
}

img* parse(char *filepath){

    FILE *input = fopen(filepath, "rb");
    if (!input){
        fprintf(stderr, "File not found!\n");
        return NULL;
    }

    File_Header buffer;
    size_t bytes_read = fread(&buffer, sizeof(File_Header), 1, input);
    if (bytes_read != 1){
        fprintf(stderr, "Cannot read BMP header!\n");
        fclose(input);
        return NULL;
    }

    if (buffer.bmp.id[0] != 'B' || buffer.bmp.id[1] != 'M'){
        fprintf(stderr, "Not valid BMP!\n");
        fclose(input);
        return NULL;
    }

    int height = buffer.dib.height;
    if (height < 0){
        height = -height;
    }

    img *image = img_init(buffer.dib.width, height);
    if (!image){
        fprintf(stderr, "Failed to allocate image!\n");
        fclose(input);
        return NULL;
    }

    int padding = (4 - (buffer.dib.width * 3) % 4) % 4;
    fseek(input, buffer.bmp.offset, SEEK_SET);

    for (int y = image->height - 1; y >= 0; y--){
        for (int x = 0; x < image->width; x++){
            pixel p;
            if (fread(&p, sizeof(pixel), 1, input) != 1){
                fprintf(stderr, "Failed to read pixel [%d][%d] data!\n", y, x);
                img_destroy(image);
                fclose(input);
                return NULL;
            }

            unsigned char temp = p.b;
            p.b = p.r;
            p.r = temp;

            image->bitmap[y][x] = p;
        }

        fseek(input, padding, SEEK_CUR);
    }

    fclose(input);
    return image;
}

void print (img *image, char *filename){

    int padding = (4 - (image->width * 3) % 4) % 4;
    int rawBM = (image->width * 3 + padding) * image->height;
        
    BMP_Header header_bmp = {"BM", 54 + rawBM, 0, 0, 54};
    DIB_Header header_dib = {40, image->width, image->height, 1, 24, 0, rawBM, 2835, 2835, 0, 0};
    File_Header header = {header_bmp, header_dib};

    FILE *output = fopen(filename, "wb");
    if (!output){
        fprintf(stderr, "Failed to open/create an output file!\n");
        return;
    }

    if (fwrite(&header, sizeof(header), 1, output) != 1){
        fprintf(stderr, "Failed to write the image!\n");
        fclose(output);
        return;
    }

    for (int y = image->height-1; y >= 0; y--){
        for (int x = 0; x < image->width; x++){
            pixel p = image->bitmap[y][x];
            
            unsigned char temp = p.b;
            p.b = p.r;
            p.r = temp;

            fwrite(&p, sizeof(pixel), 1, output);
        }
        for (int z = 0; z < padding; z++){
            fputc(0, output);
        }
    }

    fclose(output);
}