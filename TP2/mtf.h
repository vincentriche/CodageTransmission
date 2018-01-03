#include "core.h"

#define TABLE_SIZE 223
unsigned char table[TABLE_SIZE];

void Move_To_Front(int i);
void Create_Table();
void Encode_MTF(const unsigned char *sour, unsigned int *dest, size_t source_size);
void Decode_MTF(const unsigned int *sour, unsigned char *dest, size_t source_size);
