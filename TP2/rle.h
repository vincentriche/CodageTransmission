#include "mtf.h"

int Encode_RLE(const unsigned char *sour, unsigned char *dest, size_t source_size);
int Decode_RLE(const unsigned char *sour, unsigned char *dest, size_t rle_size);