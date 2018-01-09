#include "rle.h"

void Write_Bytes(size_t n, unsigned char *dest, size_t index)
{
    dest[index + 0] = (unsigned char)((n >> 24) & 0xFF);
    dest[index + 1] = (unsigned char)((n >> 16) & 0xFF);
    dest[index + 2] = (unsigned char)((n >> 8) & 0xFF);
    dest[index + 3] = (unsigned char)(n & 0xFF);
}
/*
    Pour chaque caractère de la source:
        si c'est un 0, on compte le nombre de 0 qui suivent. On écrit alors 
            le caractère 0 ainsi que le nombre de 0.
        si ce n'est pas un 0, on écrit le caractère.
    On renvoie la nouvelle taille de la chaine 
*/
int Encode_RLE(const unsigned char *sour, unsigned char *dest, size_t source_size)
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
            dest[size++] = 0;
            //Write_Bytes(0, dest, size);
            Write_Bytes(cpt_zeros, dest, size);
            size += 4;
            i += cpt_zeros;
        }
        else
        {
            dest[size++] = (unsigned char)sour[i];//Write_Bytes((size_t)sour[i], dest, size);
        }
    }
    return size;
}

int Read_Bytes(const unsigned char *dest, size_t index)
{
    size_t n = (unsigned char)(dest[index + 0]) << 24 |
               (unsigned char)(dest[index + 1]) << 16 |
               (unsigned char)(dest[index + 2]) << 8 |
               (unsigned char)(dest[index + 3]);
    return n;
}

/*
    Pour chaque caractère de la source:
        si c'est un 0, on prend le caractère suivant qui donne le nombre de 0 à 
            écrire. 
        si ce n'est pas un 0, on écrit le caractère.
    On renvoie la nouvelle taille de la chaine.
*/
int Decode_RLE(const unsigned char *sour, unsigned char *dest, size_t source_size)
{
    size_t size = 0;

    for (size_t i = 0; i < source_size; i++)
    {
        size_t s = sour[i];//Read_Bytes(sour, i);
        if (s == 0)
        {
            dest[size++] = (unsigned char)0;
            size_t cpt_zeros = Read_Bytes(sour, i + 1);
            i += 4;

            size_t h = 0;
            while (h < cpt_zeros)
            {
                dest[size] = (unsigned char)0;
                h++;
                size++;
            }
        }
        else
            dest[size++] = (unsigned char)s;
    }
    return size;
}