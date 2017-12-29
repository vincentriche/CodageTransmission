#include "includes.h"

int BWTCompare(const void *a, const void *b);
int IBWTCompare(const void *a, const void *b);
int Encode_BWT(const unsigned char *sour, unsigned char *dest, const size_t source_size);
void Decode_BWT(const unsigned char *sour, unsigned char *dest, const size_t source_size, int index);