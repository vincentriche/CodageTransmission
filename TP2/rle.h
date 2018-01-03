#include "mtf.h"

unsigned int *Encode_RLE(const unsigned int *sour, size_t source_size, size_t rle_size);
void Decode_RLE(const unsigned int *sour, unsigned int *dest, size_t source_size);