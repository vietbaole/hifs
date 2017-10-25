#ifndef __BMP_H__
#define __BMP_H__

#include <inttypes.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <math.h>

#define BMP_MAGIC       0x424D   // Windows regular format
#define BMP_HEADER_SIZE 54       // Bytes

typedef struct bmp_s{
    uint16_t    magic;

    uint32_t    imageSize,
                reserved,
                dataOffset,
                hdrSize,
                width,
                height;

    uint16_t    planes,
                bitsPerPx;

    uint32_t    compression,
                dataSize;

    int32_t     xResolution,
                yResolution;

    uint32_t    nColors,
                nColorsImportant,
                colorTableSize;

    uint8_t     *colorTable,
                *data;

} __attribute__((packed)) bmp_t;

void reverse_endian(uint64_t, void*);
bmp_t* bmp_init(char*);
void bmp_print(bmp_t*);

int bmp_write_data(bmp_t*, uint8_t*, uint64_t, uint8_t);

#endif