#include "hifs.h"
static int vFlag = 0;
int main(int argc, char **argv){
    srand(time(NULL));
    
    test_bmp();
    return 0;
}

int test_bmp(){
    vFlag = 1;
    char    *image      = "../output/result.bmp",
            *newImage   = "../output/result.bmp";
    bmp_t   *myImage;

    int     size        = 50000,
            mode        = 0;
    uint8_t *data       = calloc(size, sizeof(uint8_t));

    say("TEST_BMP: Loading image, %s.\n", image);
    if( (myImage = bmp_init(image)) == NULL){
        say("ERROR: Unable to load image, %s.\n", image);
        return 0;
    }

    say("TEST_BMP: Generating random data; %d byte(s).\n", size);
    for( int i = 0; i < size; ++i ){
        say("TEST_BMP: %d/%d byte(s)\r", i+1, size);
        data[i] = (uint8_t)(rand()%256);
    }


    say("\nTEST_BMP: Printing image information.\n");
    bmp_print(myImage);

    say("TEST_DMP: (BEFORE WRITE) R:[%d] G:[%d] B:[%d]; d:[%d]\n", myImage->data[2], myImage->data[1], myImage->data[0], data[0] );
    say("BMP_TEST: Writing data. Mode = %d\n", mode);
    if( bmp_write_data(myImage, data, size, mode) <= 0 ){
        say("ERROR: Unable to write data.\n");
    }
    say("TEST_DMP: (AFTER WRITE) R:[%d] G:[%d] B:[%d]; d:[%d]\n", myImage->data[2], myImage->data[1], myImage->data[0], data[0] );

    say("BMP_TEST: Output new image, %s.\n", newImage);
    if( !bmp_out(myImage, newImage) ){
        say("ERROR: Unable to open, %s, for writing.\n", newImage);
        return 0;
    }
    //say("TEST_BMP: ")
    say("BMP_TEST: Free-ing image data.\n");
    bmp_free(myImage);
    return 1;
}


void say(char *format, ...){
    va_list vargs;

    if( vFlag ){
        va_start(vargs, format);
        vfprintf(stderr, format, vargs);
    }
}

void error(char *format, ...){
    va_list vargs;

    va_start(vargs, format);
    vfprintf(stderr, format, vargs);
    exit(errno);
}