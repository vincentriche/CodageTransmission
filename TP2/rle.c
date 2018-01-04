#include "rle.h"

int Encode_RLE(const unsigned int *sour, unsigned int *dest, size_t source_size)
{
    size_t size = 0;
    size_t cpt_zeros = 0;
    for (size_t i = 0; i < source_size; i++)
    {
        if (sour[i] == 0)
        {
            size_t h = i + 1;
            cpt_zeros = 0;
            while (h < source_size && sour[h] == 0)
            {
                cpt_zeros++;
                h++;
            }
            dest[size++] = sour[i];
            dest[size++] = cpt_zeros;
            i += cpt_zeros;
        }
        else
            dest[size++] = sour[i];
    }
    return size;
}

int Decode_RLE(const unsigned int *sour, unsigned int *dest, size_t rle_size)
{
    size_t size = 0;

    for (size_t i = 0; i < rle_size; i++)
    {
        if (sour[i] == 0)
        {
            dest[size++] = sour[i];
            size_t cpt_zeros = sour[i + 1];
            i++;
            
            size_t h = 0;
            while (h < cpt_zeros)
            {
                dest[size] = 0;
                h++;
                size++;
            }
        }
        else
            dest[size++] = sour[i];
    }
    return size;
}