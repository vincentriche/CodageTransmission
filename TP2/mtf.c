#include "mtf.h"

/*
    Initialise la table de symboles.
*/
void Create_Table()
{
    for (int i = 0; i < TABLE_SIZE; i++)
        table[i] = i;
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
    On crée la table de symboles.
    Pour chaque caractère de la source BWT, on stocke l'index du caractère 
    correspondant dans la table de symboles. 
    La destination contient l'ensemle des index.
*/
void Encode_MTF(const unsigned char *sour, unsigned char *dest, size_t source_size)
{
    Create_Table();

    int k = 0;
    for (size_t i = 0; i < source_size; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {            
            if (sour[i] == table[j])
            {
                dest[k++] = j;
                Move_To_Front(j);
            }
        }
    }
}

/*
    On crée la table de symboles [' ', 'a-z', 'A-Z', '0-9].
    On récupère l'index de la table de symbole dans la chaine MTF, pour trouver
    le symbole de la source BWT. 
    La destination contient la chaine originale.
*/
void Decode_MTF(const unsigned char *sour, unsigned char *dest, size_t source_size)
{
    Create_Table();
    for (size_t i = 0; i < source_size; i++)
    {
        int index = sour[i];
        dest[i] = (unsigned char)table[index];
        Move_To_Front(index);
    }
}