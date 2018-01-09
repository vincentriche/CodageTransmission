#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    unsigned char *buffer;
    size_t size;
} Source;

typedef struct
{
    unsigned char *buffer;
    unsigned char *inv_buffer;
    size_t index;
} BWT;

typedef struct
{
    unsigned char *buffer;
    unsigned char *inv_buffer;
    size_t index;
} MTF;

typedef struct
{
    unsigned char *buffer;
    unsigned char *inv_buffer;
    size_t size;
} RLE;
