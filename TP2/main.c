#include "rle.h"

#define DEBUG 0
#define MAX_LINE 9999
#define RLE_BUFFER 10

void DislayLine(Source source, BWT bwt, MTF mtf, RLE rle)
{
    printf("\nCOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Source :   '%s' size of %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n");
    printf("RLE :      ");
    for (size_t i = 0; i < rle.size; i++)
        printf("%d ", rle.buffer[i]);
    printf("\n-------------------------------------------------------\n");
    printf("\nDECOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Inv RLE :  ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", rle.inv_buffer[i]);
    printf("\n");
    printf("Inv MTF :  '%s'\n", mtf.inv_buffer);
    printf("Inv BWT:   '%s'\n", bwt.inv_buffer);
    printf("-------------------------------------------------------\n");
}

void DislayCompressionLine(Source source, BWT bwt, MTF mtf, RLE rle)
{
    printf("\nCOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Source :   '%s' size of %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n");
    printf("RLE :      ");
    for (size_t i = 0; i < rle.size; i++)
        printf("%d ", rle.buffer[i]);
    printf("\n-------------------------------------------------------\n");
}

void DislayDecompressionLine(Source source, BWT bwt, MTF mtf, RLE rle)
{

    printf("\nDECOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Inv RLE :  ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", rle.inv_buffer[i]);
    printf("\n");
    printf("Inv MTF :  '%s'\n", mtf.inv_buffer);
    printf("Inv BWT:   '%s'\n", bwt.inv_buffer);
    printf("-------------------------------------------------------\n");
}

void Compare_File(const char *f1, const char *f2)
{
    char *command = (char *)malloc(sizeof(char) * 1000);
    strcpy(command, "diff ");
    strcat(command, f1);
    strcat(command, " ");
    strcat(command, f2);
    strcat(command, " > data/output.txt");

    if (!DEBUG)
    {
        printf("\nVérification de l'intégrité du fichier décompressé: \n");
        printf("Commande : %s\n", command);
    }
    system(command);

    if (!DEBUG)
    {
        FILE *fp = fopen("data/output.txt", "r");
        if (!fp)
        {
            printf("Le fichier n'existe pas.\n");
            return;
        }

        char line[MAX_LINE];
        int i = 0;
        while (fgets(line, MAX_LINE, fp) != NULL)
        {
            printf("%s", line);
            i++;
        }

        if (i == 0)
            printf("Les fichiers sont identiques.\n");

        fclose(fp);
    }
    free(command);
}

void Compare_Size(const char *f1, const char *f2)
{
    char *command = (char *)malloc(sizeof(char) * 1000);
    strcpy(command, "(stat -c '%s' ");
    strcat(command, f1);
    strcat(command, " && stat -c '%s' ");
    strcat(command, f2);
    strcat(command, ")");
    strcat(command, " > data/output.txt");

    if (!DEBUG)
    {
        printf("\nVérification de la taille du fichier compressé: \n");
        printf("Commande : %s\n", command);
    }
    system(command);

    if (!DEBUG)
    {
        FILE *fp = fopen("data/output.txt", "r");
        if (!fp)
        {
            printf("Le fichier n'existe pas.\n");
            return;
        }

        char line[MAX_LINE];
        int i = 0, sizeO = 0, sizeC = 0;
        while (fgets(line, MAX_LINE, fp) != NULL)
        {
            if (i == 0)
                sizeO = atoi(line);
            if (i == 1)
                sizeC = atoi(line);
            i++;
        }
        printf("Taille du fichier original : %d octets, taille du fichier compressé : %d octets.\n", sizeO, sizeC);

        fclose(fp);
    }
    free(command);
}

char *Generate_Filename(char *filename, char *step)
{
    char *tok = strtok(filename, ".");
    char *new_filename = (char *)malloc(sizeof(char) * 100);
    strcpy(new_filename, tok);
    strcat(new_filename, step);

    tok = strtok(NULL, ".");
    if (tok != NULL)
    {
        strcat(new_filename, ".");
        strcat(new_filename, tok);
    }
    return new_filename;
}

void Pipeline_Line(unsigned char *s, Source source, BWT bwt, MTF mtf, RLE rle)
{
    // Compression
    // Source
    source.buffer = s;
    source.size = strlen((const char *)source.buffer);

    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.buffer[source.size] = 0x0;
    bwt.index[0] = Encode_BWT(source.buffer, bwt.buffer, source.size);

    // MTF
    mtf.buffer = (unsigned int *)malloc(sizeof(unsigned int) * source.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, source.size);

    // RLE
    rle.buffer = (unsigned int *)calloc(source.size * 4 + 1, sizeof(unsigned int));
    rle.size = Encode_RLE(mtf.buffer, rle.buffer, source.size);
    rle.buffer = (unsigned int *)realloc(rle.buffer, sizeof(unsigned int) * rle.size + 1);

    // Decompression
    // Inverse RLE
    rle.inv_buffer = (unsigned int *)malloc(sizeof(unsigned int) * source.size + 1);
    Decode_RLE(rle.buffer, rle.inv_buffer, rle.size);

    // Inverse MTF
    mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    mtf.inv_buffer[source.size] = 0x0;
    Decode_MTF(rle.inv_buffer, mtf.inv_buffer, source.size);

    // Inverse BWT
    bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.inv_buffer[source.size] = 0x0;
    Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, source.size, bwt.index[0]);

    // Affichage
    if (!DEBUG)
        DislayLine(source, bwt, mtf, rle);

    // Free
    free(rle.buffer);
    free(rle.inv_buffer);
    free(mtf.inv_buffer);
    free(mtf.buffer);
    free(bwt.inv_buffer);
    free(bwt.buffer);
}

void Compress_File(const char *f, Source source, BWT bwt, MTF mtf, RLE rle)
{
    if (!DEBUG)
        printf("Fichier d'entrée: %s\n", f);

    char *file = strdup(f);
    char *new_filename = Generate_Filename(file, "C");
    free(file);

    FILE *fp = fopen(f, "r");
    FILE *new_fp = fopen(new_filename, "w");

    if (!fp)
    {
        printf("Le fichier n'existe pas.\n");
        return;
    }

    int indexes = 0;
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, fp) != NULL)
    {
        strtok(line, "\n");
        if (line[0] == '\n')
        {
            fprintf(new_fp, "\n");
            continue;
        }

        // Source Part
        source.buffer = (unsigned char *)line;
        source.size = strlen((const char *)source.buffer);

        // BWT Part
        bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        bwt.buffer[source.size] = 0x0;
        bwt.index[indexes++] = Encode_BWT(source.buffer, bwt.buffer, source.size);

        // MTF Part
        mtf.buffer = (unsigned int *)malloc(sizeof(unsigned int) * source.size + 1);
        Encode_MTF(bwt.buffer, mtf.buffer, source.size);

        // RLE
        rle.buffer = (unsigned int *)malloc(sizeof(unsigned int) * source.size * RLE_BUFFER + 1);
        rle.size = Encode_RLE(mtf.buffer, rle.buffer, source.size);
        rle.buffer = (unsigned int *)realloc(rle.buffer, sizeof(unsigned int) * rle.size + 1);

        // Write File
        for (size_t i = 0; i < rle.size; i++)
            fprintf(new_fp, "%d.", rle.buffer[i]);
        fprintf(new_fp, "\n");

        if (DEBUG)
            DislayCompressionLine(source, bwt, mtf, rle);

        free(rle.buffer);
        free(mtf.buffer);
        free(bwt.buffer);
    }
    fclose(fp);
    fclose(new_fp);
    free(new_filename);

    if (!DEBUG)
        printf("Compression terminée.\n");
}

void Decompress_File(const char *f, Source source, BWT bwt, MTF mtf, RLE rle)
{
    char *file = strdup(f);
    char *filename = Generate_Filename(file, "C");
    free(file);

    file = strdup(f);
    char *new_filename = Generate_Filename(file, "D");
    free(file);

    FILE *fp = fopen(filename, "r");
    FILE *new_fp = fopen(new_filename, "w");

    if (!fp)
    {
        printf("Le fichier n'existe pas.\n");
        return;
    }

    int indexes = 0;
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, fp) != NULL)
    {
        strtok(line, "\n");
        if (line[0] == '\n')
        {
            fprintf(new_fp, "\n");
            continue;
        }

        // Source
        source.inv_buffer = (unsigned int *)malloc(sizeof(unsigned int) * MAX_LINE);

        int size = 0;
        char *tok = strtok(line, ".");
        source.inv_buffer[0] = atoi(tok);
        while (tok != NULL)
        {
            source.inv_buffer[size++] = atoi(tok);
            tok = strtok(NULL, ".");
        }

        source.inv_buffer = (unsigned int *)realloc(source.inv_buffer, sizeof(unsigned int) * size + 1);

        // Inverse RLE
        rle.size = size;
        rle.inv_buffer = (unsigned int *)malloc(sizeof(unsigned int) * rle.size * RLE_BUFFER + 1);
        source.size = Decode_RLE(source.inv_buffer, rle.inv_buffer, rle.size);
        rle.inv_buffer = (unsigned int *)realloc(rle.inv_buffer, sizeof(unsigned int) * source.size + 1);

        // Inverse MTF
        mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        mtf.inv_buffer[source.size] = 0x0;
        Decode_MTF(rle.inv_buffer, mtf.inv_buffer, source.size);

        // Inverse BWT
        bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        bwt.inv_buffer[source.size] = 0x0;
        Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, source.size, bwt.index[indexes++]);

        // Write File
        fprintf(new_fp, "%s\n", bwt.inv_buffer);

        if (DEBUG)
            DislayDecompressionLine(source, bwt, mtf, rle);

        free(bwt.inv_buffer);
        free(mtf.inv_buffer);
        free(rle.inv_buffer);
        free(source.inv_buffer);
    }
    // Suppression du dernier caractère '\n'
    fseek(new_fp, 0L, SEEK_END);
    int file_length = ftell(new_fp);
    truncate(new_filename, file_length - 1);

    if (!DEBUG)
        printf("Décompression terminée.\n");

    fclose(fp);
    fclose(new_fp);

    // Comparaison de l'intégrité du fichier original avec le fichier décompressé
    Compare_File(f, new_filename);

    // Comparaison de la taille du fichier original avec le fichier compressé
    Compare_Size(f, filename);

    free(filename);
    free(new_filename);
}

int main(int argc, char *argv[])
{
    Source source;
    BWT bwt;
    MTF mtf;
    RLE rle;
    bwt.index = (size_t *)calloc(100, sizeof(size_t));

    if (argc > 1)
    {
        char *filename = argv[1];
        Compress_File(filename, source, bwt, mtf, rle);
        Decompress_File(filename, source, bwt, mtf, rle);
    }
    else
        Pipeline_Line((unsigned char *)"Coodage Traaaaaaansmissiooooooon", source, bwt, mtf, rle);

    free(bwt.index);
    return 0;
}