#include "core.h"

const unsigned char *source;
size_t size;

int Compare_BWT(const void *a, const void *b)
{
    int i = *(int *)a;
    int j = *(int *)b;

    for (size_t k = 0; k < size; k++)
    {
        int value = source[i] - source[j];
        if (value != 0)
            return value;
        i = (i + 1) % size;
        j = (j + 1) % size;
    }
    return 0;
}

int Compare_IBWT(const void *a, const void *b)
{
    int i = *(unsigned char *)a;
    int j = *(unsigned char *)b;
    return i - j;
}

int Encode_BWT(const unsigned char *sour, unsigned char *dest, const size_t source_size)
{
    source = sour;
    size = source_size;

    int *indexes = (int *)malloc(sizeof(int) * source_size);

    for (size_t i = 0; i < source_size; i++)
        indexes[i] = i;

    qsort((void *)indexes, source_size, sizeof(int), Compare_BWT);

    int index = 0;
    for (size_t i = 0; i < source_size; i++)
    {
        if (indexes[i] != 0)
            dest[i] = sour[indexes[i] - 1];
        else
        {
            dest[i] = sour[source_size - 1];
            index = i;
        }
    }
    free(indexes);

    return index;
}

typedef struct
{
    unsigned char c;
    int f;
} Element;

void Decode_BWT(const unsigned char *sour, unsigned char *dest, size_t source_size, int index)
{
    Element *e;
    e = (Element *)calloc(source_size, sizeof(Element));

    int *indexes = (int *)calloc(255, sizeof(int));
    int *cpt_D = (int *)calloc(255, sizeof(int));
    int *cpt_F = (int *)calloc(255, sizeof(int));

    memcpy(dest, sour, source_size);
    qsort((void *)dest, source_size,
          sizeof(unsigned char), Compare_IBWT);

    for (size_t i = 0; i < source_size; i++)
    {
        unsigned char c = dest[i];

        if (cpt_D[c] == 0)
            indexes[c] = i;

        cpt_D[c]++;
        e[i].c = sour[i];
        e[i].f = cpt_F[sour[i]]++;
    }

    int i = (source_size - 1);
    int tmp_pos = index;
    while (i >= 0)
    {
        unsigned char c = e[tmp_pos].c;
        int ind = e[tmp_pos].f;
        tmp_pos = indexes[c] + ind;
        dest[i--] = c;
    };

    free(indexes);
    free(cpt_D);
    free(cpt_F);
    free(e);
}