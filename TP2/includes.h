#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

typedef struct
{
    const unsigned char *buffer;
    size_t size;
} Source;

typedef struct
{
    unsigned char *buffer;
    unsigned char *i_buffer;
    int *index;
} BWT;

typedef struct
{
    unsigned char c;
    int f;
} Element;

typedef struct
{
    int *buffer;
    unsigned char *i_buffer;
    int index;
} MTF;
