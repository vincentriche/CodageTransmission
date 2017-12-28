#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    unsigned char *source;
    unsigned char *bwtDestination;
    unsigned char *ibwtDestination;
    size_t sourceSize;
    int originIndex;
} Data;
Data data;

typedef struct
{
    unsigned char c;
    int f;
} Element;

int bwtCompare(const void *a, const void *b);
int ibwtCompare(const void *a, const void *b);
void BWT();
void IBWT();