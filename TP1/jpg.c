#include "matrice.h"
#include "dct.h"
#include "jpg.h"
#include "image.h"

/*
 * Calcul de la DCT ou de l'inverse DCT sur un petit carré de l'image.
 * On fait la transformation de l'image ``sur place'' c.a.d.
 * que le paramètre "image" est utilisé pour l'entrée et la sortie.
 *
 * DCT de l'image :  DCT * IMAGE * DCT transposée
 * Inverse        :  DCT transposée * I' * DCT
 */
void dct_image(int inverse, int nbe, Matrice *image)
{
    Matrice *DCT = allocation_matrice_float(nbe, nbe);
    Matrice *DCTt = allocation_matrice_float(nbe, nbe);
    Matrice *Mtemp = allocation_matrice_float(nbe, nbe);

    coef_dct(DCT);
    transposition_matrice(DCT, DCTt);

    if (inverse == 0) // DCT * IMAGE * DCT transposée
    {
        produit_matrices_float(DCT, image, Mtemp);
        produit_matrices_float(Mtemp, DCTt, image);
    }
    else // DCT transposée * I' * DCT
    {
        produit_matrices_float(DCTt, image, Mtemp);
        produit_matrices_float(Mtemp, DCT, image);
    }

    liberation_matrice_float(DCT);
    liberation_matrice_float(DCTt);
    liberation_matrice_float(Mtemp);
}

/*
 * Quantification/Déquantification des coefficients de la DCT
 * Si inverse est vrai, on déquantifie.
 * Attention, on reste en calculs flottant (en sortie aussi).
 */
void quantification(int nbe, int qualite, Matrice *extrait, int inverse)
{
    for (int i = 0; i < nbe; i++)
    {
        for (int j = 0; j < nbe; j++)
        {
            float value = 1 + (i + j + 1) * qualite;
            if (inverse == 0) // Quantification
                extrait->t[i][j] = extrait->t[i][j] / value;
            else // Déquantification
                extrait->t[i][j] = extrait->t[i][j] * value;
        }
    }
}
/*
 * ZIGZAG.
 * On fournit à cette fonction les coordonnées d'un point
 * et elle nous donne le suivant (Toujours YX comme d'habitude)
 *  
 *      x
 * +---+---+---+---+     +---+---+---+
 * | 0 | 1 | 2 | 3 |     | 0 | 1 | 2 |
 * | ----/ | /---/ |     |  ---/ | | |
 * |   |/  |/  |/  |     |   |/  |/| |
y* +---/---/---/---+     +---/---/-|-+
 * | 1/| 2/| 3/| 4 |     | 1/| 2/| | |
 * | / | / | / | | |     | / | / | | |
 * | | |/  |/  |/| |     | | |/  |/3 |
 * +-|-/---/---/-|-+     +-|-/---/---+
 * | |/|  /|  /| | |     | |/|  /|   |
 * | / | / | / | | |     | / | ----- |
 * | 2 |/3 |/4 |/5 |     | 2 | 3 | 4 |
 * +---/---/---/---+     +---+---+---+
 * |  /|  /|  /|   |    
 * | /---/ | /---- |    
 * | 3 | 4 | 5 | 6 |    
 * +---+---+---+---+    
 */

void zigzag(int nbe, int *y, int *x)
{
    int l = *y, c = *x;
    if (l == nbe - 1 && c == nbe - 1)
        return;

    int s = (l + c) % 2;

    if (s == 0) // Monte diagonale
    {
        if (l == 0 && c < nbe - 1) // Bord haut
        {
            c = c + 1;
        }
        else if (c == nbe - 1) // Bord droit
        {
            l = l + 1;
        }
        else // Diagonale
        {
            l = l - 1;
            c = c + 1;
        }
    }
    else // Descend diagonale
    {
        if (c == 0 && l < nbe - 1) // Bord gauche
            l = l + 1;
        else if (l == nbe - 1) // Bord bas
            c = c + 1;
        else // Diagonale
        {
            l = l + 1;
            c = c - 1;
        }
    }
    *y = l;
    *x = c;
}
/*
 * Extraction d'une matrice de l'image (le résultat est déjà alloué).
 * La sous-image carrée à la position et de la taille indiquée
 * est stockée dans matrice "extrait"
 */

static void extrait_matrice(int y, int x, int nbe, const struct image *entree, Matrice *extrait)
{
    int i, j;

    for (j = 0; j < nbe; j++)
        for (i = 0; i < nbe; i++)
            if (j + y < entree->hauteur && i + x < entree->largeur)
                extrait->t[j][i] = entree->pixels[j + y][i + x];
            else
                extrait->t[j][i] = 0;
}

/*
 * Insertion d'une matrice de l'image.
 * C'est l'opération inverse de la précédente.
 */

static void insert_matrice(int y, int x, int nbe, Matrice *extrait, struct image *sortie)
{
    int i, j;

    for (j = 0; j < nbe; j++)
        for (i = 0; i < nbe; i++)
            if (j + y < sortie->hauteur && i + x < sortie->largeur)
            {
                if (extrait->t[j][i] < 0)
                    sortie->pixels[j + y][i + x] = 0;
                else
                {
                    if (extrait->t[j][i] > 255)
                        sortie->pixels[j + y][i + x] = 255;
                    else
                        sortie->pixels[j + y][i + x] = rint(extrait->t[j][i]);
                }
            }
}

/*
 * Compression d'une l'image :
 * Pour chaque petit carré on fait la dct et l'on stocke dans un fichier
 */
void compresse_image(int nbe, const struct image *entree, FILE *f)
{
    static Matrice *tmp = NULL;
    int i, j, k;

    if (tmp == NULL)
    {
        tmp = allocation_matrice_float(nbe, nbe);
    }

    for (j = 0; j < entree->hauteur; j += nbe)
        for (i = 0; i < entree->largeur; i += nbe)
        {
            extrait_matrice(j, i, nbe, entree, tmp);
            dct_image(0, nbe, tmp);
            for (k = 0; k < nbe; k++)
                assert(fwrite(tmp->t[k], sizeof(tmp->t[0][0]), nbe, f) == nbe);
        }
}

/*
 * Décompression image
 * On récupère la DCT de chaque fichier, on fait l'inverse et
 * on insère dans l'image qui est déjà allouée
 */
void decompresse_image(int nbe, struct image *entree, FILE *f)
{
    static Matrice *tmp = NULL;
    int i, j, k;

    if (tmp == NULL)
    {
        tmp = allocation_matrice_float(nbe, nbe);
    }

    for (j = 0; j < entree->hauteur; j += nbe)
        for (i = 0; i < entree->largeur; i += nbe)
        {
            for (k = 0; k < nbe; k++)
                assert(fread(tmp->t[k], sizeof(tmp->t[0][0]), nbe, f) == nbe);
            dct_image(1, nbe, tmp);
            insert_matrice(j, i, nbe, tmp, entree);
        }
}
