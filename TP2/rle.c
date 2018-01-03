#include "rle.h"

unsigned int *Encode_RLE(const unsigned int *sour, size_t source_size, size_t rle_size)
{
    unsigned int *output = (unsigned int *)calloc(source_size * 100, sizeof(unsigned int));

    size_t k = 0;
    for (size_t i = 0; i < source_size; i++)
    {
        if (sour[i] != 0)
        {
            output[i] = sour[i];
            k++;
        }
    }

    rle_size = k;
    unsigned int *dest = (unsigned int *)malloc(sizeof(unsigned int) * rle_size + 1);
    for (size_t i = 0; i < rle_size; i++)
        dest[i] = output[i];
    free(output);

    return dest;
}

/*
continue;
        if (sour[i] == 0)
            cpt++;
        else
        {
            if (cpt > 1)
            {
                output[k - cpt] = 0;
                output[k] = cpt;
                k++;
                cpt = 0;
            }
            k++;
        }
        */
void Decode_RLE(const unsigned int *sour, unsigned int *dest, size_t source_size)
{
    int k = 0;
    for (size_t i = 0; i < source_size; i++)
    {
        if (sour[i] == 0)
        {
            dest[k] = 0;
            int cpt = sour[i + 1];
            for (int j = 0; j < cpt; j++, k++)
                dest[k] = 0;
            i++;
        }
        else
            dest[k] = sour[i];
        k++;
    }
}