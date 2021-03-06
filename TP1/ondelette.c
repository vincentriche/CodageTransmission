#include "bases.h"
#include "bitstream.h"
#include "sf.h"
#include "intstream.h"
#include "image.h"
#include "rle.h"
#include "exception.h"
#include "matrice.h"
#include "ondelette.h"

/*
 * Cette fonction effectue UNE SEULE itération d'une ondelette 1D
 * Voici quelques exemples de calculs
 *
 * Entree            Sortie
 * A                   A
 * A B               (A+B)/2 (A-B)/2
 * A B C             (A+B)/2    C    (A-B)/2
 * A B C D           (A+B)/2 (C+D)/2 (A-B)/2 (C-D)/2
 * A B C D E         (A+B)/2 (C+D)/2   E     (A-B)/2 (C-D)/2
 * A B C D E F       (A+B)/2 (C+D)/2 (E+F)/2 (A-B)/2 (C-D)/2 (E-F)/2
 */

void ondelette_1d(const float *entree, float *sortie, int nbe)
{
    int j = nbe / 2 + nbe % 2;
    for (int i = 0; i < nbe / 2; i++)
    {
        sortie[i] = (entree[2 * i] + entree[2 * i + 1]) / 2.0;
        sortie[i + j] = (entree[2 * i] - entree[2 * i + 1]) / 2.0;
    }
    if (nbe % 2 == 1)
        sortie[nbe / 2] = entree[nbe - 1];
}

/*
 * Comme pour la DCT, on applique dans un sens puis dans l'autre.
 *
 * La fonction reçoit "image" et la modifie directement.
 *
 * Vous pouvez allouer plusieurs images intermédiaires pour
 * simplifier la fonction.
 *
 * Par exemple pour une image  3x6
 *    3x6 ondelette horizontale
 *    On transpose, on a donc une image 6x3
 *    6x3 ondelette horizontale
 *    On transpose à nouveau, on a une image 3x6
 *    On ne travaille plus que sur les basses fréquences (moyennes)
 *    On ne travaille donc que sur le haut gauche de l'image de taille 2x3
 *
 * On recommence :
 *    2x3 horizontal   
 *    imT => 3x2
 *    3x2 horizontal
 *    imT => 2x3
 *    basse fréquences => 1x2
 *
 * On recommence :
 *    1x2 horizontal
 *    imT => 2x1
 *    2x1 horizontal (ne fait rien)
 *    imT => 1x2
 *    basse fréquences => 1x1
 *
 * L'image finale ne contient qu'un seul pixel de basse fréquence.
 * Les autres sont des blocs de plus ou moins haute fréquence.
 * Sur une image 8x8 :
 *
 * M   	F1H  F2H  F2H  F3H  F3H  F3H  F3H
 * F1V 	F1HV F2H  F2H  F3H  F3H  F3H  F3H
 * F2V 	F2V  F2HV F2HV F3H  F3H  F3H  F3H
 * F2V 	F2V  F2HV F2HV F3H  F3H  F3H  F3H
 * F3V 	F3V  F3V  F3V  F3HV F3HV F3HV F3HV
 * F3V 	F3V  F3V  F3V  F3HV F3HV F3HV F3HV
 * F3V 	F3V  F3V  F3V  F3HV F3HV F3HV F3HV
 * F3V 	F3V  F3V  F3V  F3HV F3HV F3HV F3HV
 *
 * La fréquence F2 est plus petite (moins haute) que la fréquence F3
 * F1H  Indique que c'est une fréquence horizontale
 * F1V  Indique que c'est une fréquence verticale
 * F1HV Indique que c'est une fréquence calculée dans les 2 directions
 * 
 */

void ondelette_2d(Matrice *image)
{
    int hau = image->height;
    int lar = image->width;

    while (hau != 1 || lar != 1)
    {
        // Ondelette Horizontal
        Matrice *imO = allocation_matrice_float(hau, lar);
        for (int i = 0; i < hau; i++)
            ondelette_1d(image->t[i], imO->t[i], lar);

        // Transposition
        Matrice *imT = allocation_matrice_float(lar, hau);
        transposition_matrice(imO, imT);

        // Ondelette Horizontal
        Matrice *imTO = allocation_matrice_float(lar, hau);
        for (int i = 0; i < lar; i++)
            ondelette_1d(imT->t[i], imTO->t[i], hau);

        // Transposition
        transposition_matrice(imTO, image);

        // Libération
        liberation_matrice_float(imO);
        liberation_matrice_float(imT);
        liberation_matrice_float(imTO);

        hau = hau / 2 + (hau % 2);
        lar = lar / 2 + (lar % 2);
    }
}

/*
 * Quantification de l'ondelette.
 * La facteur de qualité initial s'applique à la fréquence la plus haute.
 * Quand on divise la fréquence par 2 on divise qualité par 8
 * tout en restant supérieur à 1.
 * Une qualité de 1 indique que l'on a pas de pertes.
 */

void quantif_ondelette(Matrice *image, float qualite)
{
    int hau = image->height;
    int lar = image->width;

    while (qualite > 1.0f)
    {
        int h = hau / 2 + hau % 2;
        for (int i = h; i < hau; i++)
        {
            for (int j = 0; j < lar; j++)
            {
                image->t[i][j] /= qualite;
            }
        }
        
        int l = lar / 2 + lar % 2;
        for (int i = 0; i < hau; i++)
        {
            for (int j = l; j < lar; j++)
            {
                image->t[i][j] /= qualite;
            }
        }    
        hau = h; 
        lar = l;       
        qualite = qualite / 8.0f;
    }
}

/*  
 * Sortie des coefficients dans le bonne ordre afin
 * d'être bien compressé par la RLE.
 * Cette fonction n'est pas optimale, elle devrait faire
 * un parcours de Péano sur chacun des blocs.
 */

void codage_ondelette(Matrice *image, FILE *f)
{
    int j, i;
    float *t, *pt;
    struct intstream *entier, *entier_signe;
    struct bitstream *bs;
    struct shannon_fano *sf;
    int hau, lar;

    /*
   * Conversion de la matrice en une table linéaire
   * Pour pouvoir utiliser la fonction "compresse"
   */
    hau = image->height;
    lar = image->width;
    ALLOUER(t, hau * lar);
    pt = t;

    while (hau != 1 || lar != 1)
    {
        for (j = 0; j < hau; j++)
            for (i = 0; i < lar; i++)
                if (j >= (hau + 1) / 2 || i >= (lar + 1) / 2)
                    *pt++ = image->t[j][i];

        hau = (hau + 1) / 2;
        lar = (lar + 1) / 2;
    }
    *pt = image->t[0][0];
    /*
   * Compression RLE avec Shannon-Fano
   */
    bs = open_bitstream("-", "w");
    sf = open_shannon_fano();
    entier = open_intstream(bs, Shannon_fano, sf);
    entier_signe = open_intstream(bs, Shannon_fano, sf);

    compresse(entier, entier_signe, image->height * image->width, t);

    close_intstream(entier);
    close_intstream(entier_signe);
    close_bitstream(bs);
    free(t);
}

/*
*******************************************************************************
* Fonctions inverses
*******************************************************************************
*/

void ondelette_1d_inverse(const float *entree, float *sortie, int nbe)
{
    int j = nbe / 2 + nbe % 2;
    for (int i = 0; i < nbe / 2; ++i)
    {
        sortie[2 * i] = entree[i] + entree[i + j];
        sortie[2 * i + 1] = entree[i] - entree[i + j];
    }
    if (nbe % 2 == 1)
        sortie[nbe - 1] = entree[nbe / 2];
}

void ondelette_2d_inverse(Matrice *image)
{
    int hau = image->height;
    int lar = image->width;

    if (hau != 1 || lar != 1)
    {
        image->height = hau / 2 + hau % 2;
        image->width = lar / 2 + lar % 2;
        ondelette_2d_inverse(image);
    }
    image->height = hau;
    image->width = lar;

    // Transposition
    Matrice *imT = allocation_matrice_float(lar, hau);
    transposition_matrice(image, imT);

    // Ondelette Horizontal
    Matrice *imTO = allocation_matrice_float(lar, hau);
    for (int i = 0; i < lar; i++)
        ondelette_1d_inverse(imT->t[i], imTO->t[i], hau);

    // Transposition
    Matrice *imO = allocation_matrice_float(hau, lar);
    transposition_matrice(imTO, imO);

    // Ondelette Horizontal
    for (int i = 0; i < hau; i++)
        ondelette_1d_inverse(imO->t[i], image->t[i], lar);

    // Libération
    liberation_matrice_float(imT);
    liberation_matrice_float(imTO);
    liberation_matrice_float(imO);
}

void dequantif_ondelette(Matrice *image, float qualite)
{
    int hau = image->height;
    int lar = image->width;

    while (qualite > 1.0f)
    {
        int l = lar / 2 + lar % 2;
        for (int i = 0; i < hau; i++)
        {
            for (int j = l; j < lar; j++)
            {
                image->t[i][j] *= qualite;
            }
        }

        int h = hau / 2 + hau % 2;
        for (int i = h; i < hau; i++)
        {
            for (int j = 0; j < lar; j++)
            {
                image->t[i][j] *= qualite;
            }
        }
        hau = h; 
        lar = l;
        qualite = qualite / 8.0f;
    }
}

void decodage_ondelette(Matrice *image, FILE *f)
{
    int j, i;
    float *t, *pt;
    struct intstream *entier, *entier_signe;
    struct bitstream *bs;
    struct shannon_fano *sf;
    int lar = image->width, hau = image->height;

    /*
   * Decompression RLE avec Shannon-Fano
   */
    ALLOUER(t, hau * lar);
    bs = open_bitstream("-", "r");
    sf = open_shannon_fano();
    entier = open_intstream(bs, Shannon_fano, sf);
    entier_signe = open_intstream(bs, Shannon_fano, sf);

    decompresse(entier, entier_signe, hau * lar, t);

    close_intstream(entier);
    close_intstream(entier_signe);
    close_bitstream(bs);

    /*
   * Met dans la matrice
   */
    pt = t;
    while (hau != 1 || lar != 1)
    {
        for (j = 0; j < hau; j++)
            for (i = 0; i < lar; i++)
                if (j >= (hau + 1) / 2 || i >= (lar + 1) / 2)
                    image->t[j][i] = *pt++;

        hau = (hau + 1) / 2;
        lar = (lar + 1) / 2;
    }
    image->t[0][0] = *pt++;

    free(t);
}

/*
 * Programme de test.
 * La ligne suivante compile, compresse et décompresse l'image
 * et affiche la taille compressée.

export QUALITE=1  # Qualité de "quantification"
export SHANNON=1  # Si 1, utilise shannon-fano dynamique
ondelette <DONNEES/bat710.pgm 1 >xxx && ls -ls xxx && ondelette_inv <xxx | xv -

 */

void ondelette_encode_image(float qualite)
{
    struct image *image;
    Matrice *im;
    int i, j;

    image = lecture_image(stdin);
    assert(fwrite(&image->hauteur, 1, sizeof(image->hauteur), stdout) == sizeof(image->hauteur));
    assert(fwrite(&image->largeur, 1, sizeof(image->largeur), stdout) == sizeof(image->largeur));
    assert(fwrite(&qualite, 1, sizeof(qualite), stdout) == sizeof(qualite));

    im = allocation_matrice_float(image->hauteur, image->largeur);
    for (j = 0; j < image->hauteur; j++)
        for (i = 0; i < image->largeur; i++)
            im->t[j][i] = image->pixels[j][i];

    fprintf(stderr, "Compression ondelette, image %fx%f\n", image->largeur, image->hauteur);
    ondelette_2d(im);
    fprintf(stderr, "Quantification qualité = %g\n", qualite);
    quantif_ondelette(im, qualite);
    fprintf(stderr, "Codage\n");
    codage_ondelette(im, stdout);

    // affiche_matrice_float(im, image->hauteur, image->largeur);
}

void ondelette_decode_image()
{
    int hau, lar;
    float qualite;
    struct image *image;
    Matrice *im;

    assert(fread(&hau, 1, sizeof(hau), stdin) == sizeof(hau));
    assert(fread(&lar, 1, sizeof(lar), stdin) == sizeof(lar));
    assert(fread(&qualite, 1, sizeof(qualite), stdin) == sizeof(qualite));

    im = allocation_matrice_float(hau, lar);

    fprintf(stderr, "Décodage\n");
    decodage_ondelette(im, stdin);

    fprintf(stderr, "Déquantification qualité = %g\n", qualite);
    dequantif_ondelette(im, qualite);

    fprintf(stderr, "Décompression ondelette, image %fx%f\n", lar, hau);
    ondelette_2d_inverse(im);

    // affiche_matrice_float(im, hau, lar);
    image = creation_image_a_partir_de_matrice_float(im);
    ecriture_image(stdout, image);
}
