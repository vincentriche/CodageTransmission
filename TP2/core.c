#include "core.h"

int bwtCompare(const void *a, const void *b)
{
    int i = *(int *)a;
    int j = *(int *)b;

    for (size_t k = 0; k < data.sourceSize; k++)
    {
        int value = data.source[i] - data.source[j];
        if (value != 0)
            return value;

        i = (i + 1) % data.sourceSize;
        j = (j + 1) % data.sourceSize;
    }
    return 0;
}

int ibwtCompare(const void *a, const void *b)
{
    int i = *(unsigned char *)a;
    int j = *(unsigned char *)b;
    return i - j;
}

void BWT()
{
    int *indexes = (int *)malloc(sizeof(int) * data.sourceSize);

    for (size_t i = 0; i < data.sourceSize; i++)
        indexes[i] = i;

    qsort((void *)indexes, data.sourceSize, sizeof(int), bwtCompare);

    for (size_t i = 0; i < data.sourceSize; i++)
    {
        if (indexes[i] != 0)
            data.bwtDestination[i] = data.source[indexes[i] - 1];
        else
        {
            data.bwtDestination[i] = data.source[data.sourceSize - 1];
            data.originIndex = i;
        }
    }
    free(indexes);
}

void IBWT()
{
    Element *p;
    p = (Element *)calloc(data.sourceSize, sizeof(Element));

    int *indexes = (int *)calloc(255, sizeof(int));
    int *cptD = (int *)calloc(255, sizeof(int));
    int *cptF = (int *)calloc(255, sizeof(int));

    memcpy(data.ibwtDestination, data.bwtDestination, data.sourceSize);
    qsort((void *)data.ibwtDestination, data.sourceSize, sizeof(unsigned char), ibwtCompare);

    unsigned char c;
    for (size_t i = 0; i < data.sourceSize; i++)
    {
        c = data.ibwtDestination[i];

        if (cptD[c] == 0)
            indexes[c] = i;

        cptD[c]++;
        p[i].c = data.bwtDestination[i];
        p[i].f = cptF[data.bwtDestination[i]]++;
    }

    // calcule de l'inverse
    int i = (data.sourceSize - 1);
    int tmp_pos = data.originIndex;
    while (i >= 0)
    {
        c = p[tmp_pos].c;
        int ind = p[tmp_pos].f;
        tmp_pos = indexes[c] + ind;
        data.ibwtDestination[i--] = c;
    };

    free(indexes);
    free(cptD);
    free(cptF);
}