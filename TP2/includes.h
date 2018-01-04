#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    const unsigned char *buffer;
    unsigned int *inv_buffer;
    size_t size;
} Source;

typedef struct
{
    unsigned char *buffer;
    unsigned char *inv_buffer;
    size_t *index;
} BWT;

typedef struct
{
    unsigned int *buffer;
    unsigned char *inv_buffer;
    size_t index;
} MTF;

typedef struct
{
    unsigned int *buffer;
    unsigned int *inv_buffer;
    size_t size;
} RLE;
