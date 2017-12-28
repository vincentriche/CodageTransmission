#include "core.h"

int main()
{
    data.source = (unsigned char *)"CODAGE TRANSMISSION";
    data.sourceSize = strlen((const char *)data.source);

    data.bwtDestination = (unsigned char *)malloc(sizeof(unsigned char) * data.sourceSize + 1);
    data.bwtDestination[data.sourceSize] = 0x0;
    data.ibwtDestination = (unsigned char *)malloc(sizeof(unsigned char) * data.sourceSize + 1);
    data.ibwtDestination[data.sourceSize] = 0x0;

    BWT();
    IBWT();

    printf("originale: '%s' taille de %zu\n", data.source, data.sourceSize);
    printf("BWT :      '%s'\n", data.bwtDestination);
    printf("IBWT:      '%s'\n", data.ibwtDestination);
    return 0;
}