#include "bmp.h"
#include <stdio.h>

bmp_t* bmp_init(char *image){
    int fd,
        success,
        mask = 0b1101;

    bmp_t *bmp = malloc(sizeof(bmp_t));

    if( (fd = open(image, O_RDONLY)) == -1 ){ return NULL; }
    if( (success = pread(fd, bmp, BMP_HEADER_SIZE, 0)) <= 0 ){ return NULL; }

    reverse_endian(sizeof(bmp->magic), &bmp->magic);
    if( bmp->magic != BMP_MAGIC ){ return NULL; }

    //Check if bitsPerPx is 1,4, or 8 and that the dataOffset is greater than the header size
    //bigger chance of a color Table actually existing
    if( (bmp->bitsPerPx & mask) >= 1 && bmp->dataOffset > BMP_HEADER_SIZE ){
        //set color table size
        bmp->colorTableSize = pow(2, bmp->bitsPerPx)*4; //nPixels (4 bytes per px)
        bmp->colorTable = calloc(bmp->colorTableSize, sizeof(uint8_t));
        success = pread(fd, &bmp->colorTable, bmp->colorTableSize, BMP_HEADER_SIZE);
    }

    //if( (success = lseek(fd, bmp->dataOffset, SEEK_SET)) == -1 ){ return NULL; }

    bmp->colorTableSize = 0;
    bmp->colorTable = NULL;
    bmp->data = calloc(bmp->dataSize, sizeof(uint8_t));
    if( (success = pread(fd, &bmp->data, bmp->dataSize, bmp->dataOffset)) == -1 ){ return NULL; }

    close(fd);
    return bmp;
}

void reverse_endian(uint64_t size, void* value){
    char *result = (char*)calloc(size, sizeof(char));
    for( int i = 0; i < size; ++i ){ result[i] = ((char*)value)[size-i-1]; }
    for( int i = 0; i < size; ++i ){ ((char*)value)[i] = result[i]; }
}

void bmp_print(bmp_t* bmp){
    if( !bmp ){ return; }

    fprintf(stderr, "MAGIC:                %" PRIx16 "\n"
                    "SIZE:                 %" PRIu32 "\n"
                    "RESERVED:             %" PRIu32 "\n"
                    "START OF PIXEL(S):    %" PRIu32 "\n"
                    "HEADER SIZE:          %" PRIu32 "\n"
                    "WIDTH:                %" PRIu32 "\n"
                    "HEIGHT:               %" PRIu32 "\n"
                    "PLANE(S):             %" PRIu16 "\n"
                    "BITS PER PIXEL:       %" PRIu16 "\n"
                    "COMPRESSION:          %" PRIu32 "\n"
                    "PIXEL TOTAL SIZE:     %" PRIu32 "\n"
                    "X RESOLUTION:         %" PRId32 "\n"
                    "Y RESOLUTION:         %" PRId32 "\n"
                    "N COLOR(S):           %" PRIu32 "\n"
                    "N COLOR(S) IMPORTANT: %" PRIu32 "\n",
                    bmp->magic, bmp->imageSize, bmp->reserved, bmp->dataOffset, bmp->hdrSize, bmp->width,
                    bmp->height, bmp->planes, bmp->bitsPerPx, bmp->compression, bmp->dataSize,
                    bmp->xResolution, bmp->yResolution, bmp->nColors, bmp->nColorsImportant);
}

int bmp_write_data(bmp_t *bmp, uint8_t *data, uint64_t size, uint8_t mode){
    if( !bmp ){ return 0; }

    for( int i = 0; i < size; ++i ){
        switch(mode){
            case 0:
                        bmp->data[i] = data[i]; break;
            default:
                        abort();
        }
    }
}















