#include "mtf.h"

int main()
{
    Source source;
    BWT bwt;
    MTF mtf;

    // BWT Part
    source.buffer = (unsigned char *)"CODAGE TRANSMISSION";
    source.size = strlen((const char *)source.buffer);

    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.buffer[source.size] = 0x0;
    bwt.index = Encode_BWT(source.buffer, bwt.buffer, source.size);

    // MTF Part
    mtf.buffer = (int *)malloc(sizeof(int) * source.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, source.size);

    // Inverse MTF Part
    mtf.i_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    mtf.i_buffer[source.size] = 0x0;
    Decode_MTF(mtf.buffer, mtf.i_buffer, source.size);

    // Inverse BWT Part
    bwt.i_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.i_buffer[source.size] = 0x0;
    Decode_BWT(mtf.i_buffer, bwt.i_buffer, source.size, bwt.index);

    // Affichage
    printf("originale: '%s' taille de %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n");
    printf("Inv MTF :  '%s'\n", mtf.i_buffer);
    printf("Inv BWT:   '%s'\n", bwt.i_buffer);
    return 0;
}