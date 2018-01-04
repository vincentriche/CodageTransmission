#include "mtf.h"

int Encode_RLE(const unsigned int *sour, unsigned int *dest, size_t source_size);
int Decode_RLE(const unsigned int *sour, unsigned int *dest, size_t rle_size);