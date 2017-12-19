#include "bases.h"
#include "matrice.h"
#include "dct.h"

/*
 * La fonction calculant les coefficients de la DCT (et donc de l'inverse)
 * car la matrice de l'inverse DCT est la transposée de la matrice DCT
 *
 * Cette fonction prend beaucoup de temps.
 * il faut que VOUS l'utilisiez le moins possible (UNE SEULE FOIS)
 *
 * FAITES LES CALCULS EN "double"
 *
 * La valeur de Pi est : M_PI
 *
 * Pour ne pas avoir de problèmes dans la suite du TP, indice vos tableau
 * avec [j][i] et non [i][j].
 */

void coef_dct(Matrice *table)
{
    int n = table->height;
    int m = table->width;
    double sqrtN = sqrt(n);

    for (int i = 0; i < n; i++)
        table->t[0][i] = 1.f / sqrtN;

    for (int i = 0; i < n; i++)
    {
        for (int j = 1; j < m; j++)
        {
            table->t[j][i] = (sqrt(2) / sqrt(n)) *
                             cos(j * M_PI * (2 * i + 1) / (2 * n));
        }
    }
}

/*
 * La fonction calculant la DCT ou son inverse.
 *
 * Cette fonction va être appelée très souvent pour faire
 * la DCT du son ou de l'image (nombreux paquets).
 */

void dct(int inverse,         /* ==0: DCT, !=0 DCT inverse */
         int nbe,             /* Nombre d'échantillons  */
         const float *entree, /* Le son avant transformation (DCT/INVDCT) */
         float *sortie        /* Le son après transformation */
)
{
    Matrice *DCT;
    DCT = allocation_matrice_float(nbe, nbe);
    coef_dct(DCT);
    if (inverse == 0)
    {
        produit_matrice_vecteur(DCT, entree, sortie);
    }
    else
    {
        Matrice *DCTt = allocation_matrice_float(nbe, nbe);
        transposition_matrice(DCT, DCTt);
        produit_matrice_vecteur(DCTt, entree, sortie);
        liberation_matrice_float(DCTt);
    }

    liberation_matrice_float(DCT);
}
