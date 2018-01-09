#include "mtf.h"

void Write_Bytes(size_t n, unsigned char *dest, size_t index);
int Read_Bytes(const unsigned char *dest, size_t index);

int Encode_RLE(const unsigned char *sour, unsigned char *dest, size_t source_size);
int Decode_RLE(const unsigned char *sour, unsigned char *dest, size_t rle_size);