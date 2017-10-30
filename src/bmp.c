#include "bmp.h"
#include <stdio.h>
#include <string.h>

bmp_t* bmp_init(char *image){
    int fd,
        success,
        mask = 0b1101;

    bmp_t *bmp = malloc(sizeof(bmp_t));

    if( (fd = open(image, O_RDONLY)) == -1 ){ return NULL; }
    if( (success = pread(fd, bmp, BMP_HEADER_SIZE, 0)) <= 0 ){ return NULL; }

    reverse_endian(sizeof(bmp->magic), &bmp->magic);
    if( bmp->magic != BMP_MAGIC ){ return NULL; }

    bmp->colorTableSize = 0;
    bmp->colorTable = NULL;

    //Check if bitsPerPx is 1,4, or 8 and that the dataOffset is greater than the header size
    //bigger chance of a color Table actually existing
    if( (bmp->bitsPerPx & mask) >= 1 && bmp->dataOffset > BMP_HEADER_SIZE ){
        //set color table size
        bmp->colorTableSize = pow(2, bmp->bitsPerPx)*4; //nPixels (4 bytes per px)
        bmp->colorTable = calloc(bmp->colorTableSize, sizeof(uint8_t));

        //printf("COLOR TABLE! %d ---- %d ----- %d --- %p (%d)\n\n", (bmp->bitsPerPx&mask), bmp->dataOffset, BMP_HEADER_SIZE, bmp->colorTable, bmp->colorTableSize);
        success = pread(fd, &bmp->colorTable, bmp->colorTableSize, BMP_HEADER_SIZE);
    }

    bmp->data = calloc(bmp->dataSize, sizeof(uint8_t));
    if( (success = pread(fd, bmp->data, bmp->dataSize, bmp->dataOffset)) == -1 ){ return NULL; }

    //bmp->reserved = 0xA0000000; //0001 1101 1001 0000 0010 1010 0111 0111

    close(fd);

    return bmp;
}

void reverse_endian(uint64_t size, void* value){
    char *result = (char*)calloc(size, sizeof(char));
    for( int i = 0; i < size; ++i ){ result[i] = ((char*)value)[size-i-1]; }
    for( int i = 0; i < size; ++i ){ ((char*)value)[i] = result[i]; }
}

void bmp_print(bmp_t* bmp){
    uint32_t    reserved    = 0xFF000000,
                startFree   = (bmp->reserved)&(~reserved);

    uint8_t     fileCount   = ((bmp->reserved)&(reserved))>>24;

    fprintf(stderr, "MAGIC:                   %" PRIx16 "\n"
                    "SIZE:                    %" PRIu32 "\n"
                    "RESERVED:                %" PRIu32 "\n"
                    "FILE COUNT (RESERVED):   %" PRIu8  "\n"
                    "START OF FREE (RESERVED) %" PRIu32 "\n"
                    "START OF PIXEL(S):       %" PRIu32 "\n"
                    "HEADER SIZE:             %" PRIu32 "\n"
                    "WIDTH:                   %" PRIu32 "\n"
                    "HEIGHT:                  %" PRIu32 "\n"
                    "PLANE(S):                %" PRIu16 "\n"
                    "BITS PER PIXEL:          %" PRIu16 "\n"
                    "COMPRESSION:             %" PRIu32 "\n"
                    "PIXEL TOTAL SIZE:        %" PRIu32 "\n"
                    "X RESOLUTION:            %" PRId32 "\n"
                    "Y RESOLUTION:            %" PRId32 "\n"
                    "N COLOR(S):              %" PRIu32 "\n"
                    "N COLOR(S) IMPORTANT:    %" PRIu32 "\n",
                    bmp->magic, bmp->imageSize, bmp->reserved, fileCount, startFree, bmp->dataOffset,
                    bmp->hdrSize, bmp->width, bmp->height, bmp->planes, bmp->bitsPerPx, bmp->compression,
                    bmp->dataSize, bmp->xResolution, bmp->yResolution, bmp->nColors, bmp->nColorsImportant);
}

int bmp_write_data(bmp_t *bmp, uint8_t *data, uint64_t size, uint8_t mode){
    uint32_t    mask        = 0xFC,
                reserved    = 0xFF000000,
                startFree   = (bmp->reserved)&(~reserved),
                i, j, k;

    uint8_t     fileCount   = ((bmp->reserved)&(reserved))>>24;


    i = startFree;
    j = k = 0;
    switch(mode){
        case 0:
                    //1:1
                    if( size+startFree > bmp->dataSize){ return -1; }
                    for( i = startFree; i < size; ++i ){
                        bmp->data[i] = data[i];
                    }
                    break;
        case 1:
                    //lsb2
                    if( (size*4)+startFree > bmp->dataSize ){ return -1; }
                    while( i < size ){
                        for( k = 0; k < 8; k+=2 ){
                            bmp->data[i] &= mask;
                            bmp->data[i] |= ((data[j]>>k)&(~mask));
                            ++i;
                        }
                        ++j;
                    }
                    break;
        default:
                    return 0;
    }

    ++fileCount;            //per function call should be a per file.
    startFree += i;          //set new position of free byte.

    memset(&reserved, 0, sizeof(uint32_t));
    reserved = (reserved|fileCount)<<24;
    reserved = (reserved|startFree);

    bmp->reserved = reserved;
    return 1;
}

void bmp_free(bmp_t* bmp){
    if( bmp->colorTable != NULL ){ free(bmp->colorTable); }
    free(bmp->data);
    free(bmp);
}

int bmp_out(bmp_t *bmp, char *name){
    int fd,
        success;

    //reverse-reverse magic
    reverse_endian(sizeof(bmp->magic), &bmp->magic);

    if( (fd = open(name, O_WRONLY|O_CREAT|O_TRUNC, 0700)) == -1 ){ return 0; }
    if( (success = write(fd, bmp, BMP_HEADER_SIZE)) <= 0 ){ return 0; }
    if( bmp->colorTable != NULL && (success = write(fd, bmp->colorTable, bmp->colorTableSize)) <= 0 ){ return 0; }
    if( (success = write(fd, bmp->data, bmp->dataSize)) <= 0 ){ return 0; }

    close(fd);
    return 1;
}












