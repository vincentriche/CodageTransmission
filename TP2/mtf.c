#include "mtf.h"

/*
    Initilise la table de symboles [' ', 'a-z', 'A-Z', '0-9].
*/
void Create_Table()
{
    for (int i = 0; i < 26; i++)
    {
        table[i] = i + 'A';
        table[26 + i] = i + 'a';
    }

    for (int i = 0; i < 10; i++)
        table[52 + i] = i + '0';

    table[62] = ' ';
}

/*
    Déplace le symbole au début de la table de symboles.
*/
void Move_To_Front(int k)
{
    unsigned char c = table[k];
    for (int i = k - 1; i >= 0; i--)
        table[i + 1] = table[i];
    table[0] = c;
}

/*
    On crée la table de symboles [' ', 'a-z', 'A-Z', '0-9].
    Pour chaque caractère de la source BWT, on stocke l'index du caractère 
    correspondant dans la table de symboles. 
    La destination contient l'ensemle des index.
*/
void Encode_MTF(const unsigned char *sour, int *dest, size_t source_size)
{
    Create_Table();
    int *output = (int *)calloc(source_size, sizeof(int));

    int k = 0;
    for (size_t i = 0; i < source_size; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            if (sour[i] == table[j])
            {
                output[k] = j;
                Move_To_Front(j);
                k++;
            }
        }
    }

    for (size_t i = 0; i < source_size; i++)
        dest[i] = output[i];
    free(output);
}

/*
    On crée la table de symboles [' ', 'a-z', 'A-Z', '0-9].
    On récupère l'index de la table de symbole dans la chaine MTF, pour trouver
    le symbole de la source BWT. 
    La destination contient la chaine originale.
*/
void Decode_MTF(const int *sour, unsigned char *dest, size_t source_size)
{
    Create_Table();
    for (size_t i = 0; i < source_size; i++)
    {
        int index = sour[i];
        dest[i] = table[index];
        Move_To_Front(index);
    }
}