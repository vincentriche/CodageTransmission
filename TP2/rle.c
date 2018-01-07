#include "rle.h"

/*
    Pour chaque caractère de la source:
        si c'est un 0, on compte le nombre de 0 qui suivent. On écrit alors 
            le caractère 0 ainsi que le nombre de 0.
        si ce n'est pas un 0, on écrit le caractère.
    On renvoie la nouvelle taille de la chaine 
*/
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

/*
    Pour chaque caractère de la source:
        si c'est un 0, on prend le caractère suivant qui donne le nombre de 0 à 
            écrire. 
        si ce n'est pas un 0, on écrit le caractère.
    On renvoie la nouvelle taille de la chaine.
*/
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