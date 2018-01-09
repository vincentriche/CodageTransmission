#include "rle.h"

#define MAX_LINE 9999
#define RLE_BUFFER 9999

void DislayLine(Source source, BWT bwt, MTF mtf, RLE rle)
{
    printf("\nCOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Source :   '%s'\n", source.buffer);
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
    printf("-------------------------------------------------------\n\n");
}

void DislayLine2RLE(Source source, BWT bwt, MTF mtf, RLE rle)
{
    printf("\nCOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Source :   '%s'\n", source.buffer);
    printf("RLE  :      ");
    for (size_t i = 0; i < rle.size; i++)
        printf("%d ", rle.buffer[i]);
    printf("\nBWT  :      '%s'\n", bwt.buffer);
    printf("MTF  :      ");
    for (size_t i = 0; i < rle.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n");
    printf("RLE0  :      ");
    for (size_t i = 0; i < rle.size0; i++)
        printf("%d ", rle.buffer0[i]);
    printf("\n-------------------------------------------------------\n");
    printf("\nDECOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Inv RLE0 :  ");
    for (size_t i = 0; i < rle.size; i++)
        printf("%d ", rle.inv_buffer0[i]);
    printf("\nInv MTF  :  '%s'\n", mtf.inv_buffer);
    printf("Inv BWT :   ");
    for (size_t i = 0; i < rle.size; i++)
        printf("%d ", bwt.inv_buffer[i]);
    printf("\nInv RLE0 :  ");
    for (size_t i = 0; i < rle.size0; i++)
        printf("%c", rle.inv_buffer[i]);
    printf("\n-------------------------------------------------------\n\n");
}

void Compare_File(const char *f1, const char *f2)
{
    char *command = (char *)malloc(sizeof(char) * 1000);
    strcpy(command, "diff ");
    strcat(command, f1);
    strcat(command, " ");
    strcat(command, f2);
    strcat(command, " > data/output.txt");

    printf("\nVérification de l'intégrité du fichier décompressé: \n");
    printf("Commande : %s\n", command);
    system(command);

    FILE *fp = fopen("data/output.txt", "r");
    if (!fp)
    {
        printf("Le fichier n'existe pas.\n");
        return;
    }

    char line[MAX_LINE];
    int i = 0;
    while (fgets(line, MAX_LINE, fp) != NULL)
        i++;

    if (i == 0)
        printf("    --> Les fichiers sont identiques.\n");
    else
        printf("    --> Les fichiers sont différents. %d lignes différentes.\n", i);

    fclose(fp);
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

    printf("\nVérification de la taille du fichier compressé: \n");
    printf("Commande : %s\n", command);
    system(command);

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
    printf("    --> Taille du fichier original :  %d octets.\n", sizeO);
    printf("    --> Taille du fichier compressé : %d octets.\n", sizeC);
    printf("    --> Ratio de compression de %f.\n", sizeO / (float)sizeC);

    fclose(fp);

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

void Pipeline_Line2RLE(unsigned char *s, Source source, BWT bwt, MTF mtf, RLE rle)
{
    // Compression
    // Source
    source.buffer = s;
    source.size = strlen((const char *)source.buffer);

    // RLE
    rle.buffer = (unsigned char *)calloc(source.size * RLE_BUFFER + 1, sizeof(unsigned char));
    rle.size = Encode_RLE(source.buffer, rle.buffer, source.size);
    rle.buffer = (unsigned char *)realloc(rle.buffer, sizeof(unsigned char) * rle.size + 1);

    // BWT
    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    bwt.buffer[rle.size] = 0x0;
    bwt.index = Encode_BWT(rle.buffer, bwt.buffer, rle.size);

    // MTF
    mtf.buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, rle.size);

    // RLE 0
    rle.buffer0 = (unsigned char *)calloc(rle.size * RLE_BUFFER + 1, sizeof(unsigned char));
    rle.size0 = Encode_RLE0(mtf.buffer, rle.buffer0, rle.size);
    rle.buffer0 = (unsigned char *)realloc(rle.buffer0, sizeof(unsigned char) * rle.size0 + 1);

    // Decompression
    // Inverse RLE 0
    rle.inv_buffer0 = (unsigned char *)malloc(sizeof(unsigned char) * rle.size0 * RLE_BUFFER + 1);
    rle.size = Decode_RLE0(rle.buffer0, rle.inv_buffer0, rle.size0);
    rle.inv_buffer0 = (unsigned char *)realloc(rle.inv_buffer0, sizeof(unsigned char) * rle.size + 1);

    // Inverse MTF
    mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    mtf.inv_buffer[rle.size] = 0x0;
    Decode_MTF(rle.inv_buffer0, mtf.inv_buffer, rle.size);

    // Inverse BWT
    bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    bwt.inv_buffer[rle.size] = 0x0;
    Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, rle.size, bwt.index);

    // Inverse RLE
    rle.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size * RLE_BUFFER + 1);
    rle.size0 = Decode_RLE(bwt.inv_buffer, rle.inv_buffer, rle.size);
    rle.inv_buffer = (unsigned char *)realloc(rle.inv_buffer, sizeof(unsigned char) * rle.size0 + 1);

    // Affichage
    DislayLine2RLE(source, bwt, mtf, rle);

    printf("    --> Taille de la source :  %zu octets.\n", sizeof(source.buffer));
    printf("    --> Taille de la source compressée : %zu octets.\n", sizeof(rle.buffer));
    printf("    --> Ratio de compression de %f.\n", sizeof(source.buffer) / (float)sizeof(rle.buffer));

    // Free
    free(rle.inv_buffer);
    free(mtf.inv_buffer);
    free(bwt.inv_buffer);
    free(rle.inv_buffer0);
    free(mtf.buffer);
    free(rle.buffer);
    free(rle.buffer0);
    free(bwt.buffer);
}

void Pipeline_Line(unsigned char *s, Source source, BWT bwt, MTF mtf, RLE rle)
{
    // Compression
    // Source
    source.buffer = s;
    source.size = strlen((const char *)source.buffer);

    // BWT
    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.buffer[source.size] = 0x0;
    bwt.index = Encode_BWT(source.buffer, bwt.buffer, source.size);

    // MTF
    mtf.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, source.size);

    // RLE
    rle.buffer = (unsigned char *)calloc(source.size * RLE_BUFFER + 1, sizeof(unsigned char));
    rle.size = Encode_RLE0(mtf.buffer, rle.buffer, source.size);
    rle.buffer = (unsigned char *)realloc(rle.buffer, sizeof(unsigned char) * rle.size + 1);

    // Decompression
    // Inverse RLE
    rle.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size * RLE_BUFFER + 1);
    rle.size = Decode_RLE0(rle.buffer, rle.inv_buffer, rle.size);
    rle.inv_buffer = (unsigned char *)realloc(rle.inv_buffer, sizeof(unsigned char) * rle.size + 1);

    // Inverse MTF
    mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    mtf.inv_buffer[source.size] = 0x0;
    Decode_MTF(rle.inv_buffer, mtf.inv_buffer, source.size);

    // Inverse BWT
    bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.inv_buffer[source.size] = 0x0;
    Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, source.size, bwt.index);

    // Affichage
    DislayLine(source, bwt, mtf, rle);

    printf("    --> Taille de la source :  %zu octets.\n", sizeof(source.buffer));
    printf("    --> Taille de la source compressée : %zu octets.\n", sizeof(rle.buffer));
    printf("    --> Ratio de compression de %f.\n", sizeof(source.buffer) / (float)sizeof(rle.buffer));

    // Free
    free(rle.buffer);
    free(rle.inv_buffer);
    free(mtf.inv_buffer);
    free(mtf.buffer);
    free(bwt.inv_buffer);
    free(bwt.buffer);
}
void Compress_File2RLE(const char *f, Source source, BWT bwt, MTF mtf, RLE rle)
{
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

    unsigned char *line = (unsigned char *)calloc(MAX_LINE + 1, sizeof(unsigned char));

    size_t size = fread(line, 1, MAX_LINE, fp);
    line = (unsigned char *)realloc(line, sizeof(unsigned char) * size + 1);

    // Source
    source.buffer = (unsigned char *)line;
    source.size = size;

    // RLE
    rle.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size * RLE_BUFFER + 1);
    rle.size = Encode_RLE(source.buffer, rle.buffer, source.size);
    rle.buffer = (unsigned char *)realloc(rle.buffer, sizeof(unsigned char) * rle.size + 1);

    // BWT Part
    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    bwt.buffer[rle.size] = 0x0;
    bwt.index = Encode_BWT(rle.buffer, bwt.buffer, rle.size);

    // MTF Part
    mtf.buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, rle.size);

    // RLE 0
    rle.buffer0 = (unsigned char *)malloc(sizeof(unsigned char) * rle.size * RLE_BUFFER + 1);
    rle.size0 = Encode_RLE0(mtf.buffer, rle.buffer0, rle.size);
    rle.buffer0 = (unsigned char *)realloc(rle.buffer0, sizeof(unsigned char) * rle.size0 + 1);

    // Conversion de l'index en bytes pour écriture dans le fichier
    unsigned char bytes[4];
    bytes[0] = (bwt.index >> 24) & 0xFF;
    bytes[1] = (bwt.index >> 16) & 0xFF;
    bytes[2] = (bwt.index >> 8) & 0xFF;
    bytes[3] = bwt.index & 0xFF;

    size_t output_size = rle.size0 + 4;
    unsigned char *output = (unsigned char *)malloc(sizeof(unsigned char) * output_size + 1);

    memcpy(output, bytes, 4);
    for (size_t i = 4; i < output_size; i++)
        output[i] = rle.buffer0[i - 4];

    // Ecriture fichier
    fwrite(output, 1, output_size, new_fp);

    free(output);
    free(bwt.buffer);
    free(mtf.buffer);
    free(rle.buffer);
    free(rle.buffer0);
    free(line);

    free(new_filename);
    fclose(fp);
    fclose(new_fp);

    printf("Compression terminée.\n");
}

void Compress_File(const char *f, Source source, BWT bwt, MTF mtf, RLE rle)
{
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

    unsigned char *line = (unsigned char *)calloc(MAX_LINE + 1, sizeof(unsigned char));

    size_t size = fread(line, 1, MAX_LINE, fp);
    line = (unsigned char *)realloc(line, sizeof(unsigned char) * size + 1);

    // Source
    source.buffer = (unsigned char *)line;
    source.size = size;

    // BWT Part
    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.buffer[source.size] = 0x0;
    bwt.index = Encode_BWT(source.buffer, bwt.buffer, source.size);

    // MTF Part
    mtf.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, source.size);

    // RLE
    rle.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size * RLE_BUFFER + 1);
    rle.size = Encode_RLE0(mtf.buffer, rle.buffer, source.size);
    rle.buffer = (unsigned char *)realloc(rle.buffer, sizeof(unsigned char) * rle.size + 1);

    // Conversion de l'index en bytes pour écriture dans le fichier
    unsigned char bytes[4];
    bytes[0] = (bwt.index >> 24) & 0xFF;
    bytes[1] = (bwt.index >> 16) & 0xFF;
    bytes[2] = (bwt.index >> 8) & 0xFF;
    bytes[3] = bwt.index & 0xFF;

    size_t output_size = rle.size + 4;
    unsigned char *output = (unsigned char *)malloc(sizeof(unsigned char) * output_size + 1);

    memcpy(output, bytes, 4);
    for (size_t i = 4; i < output_size; i++)
        output[i] = rle.buffer[i - 4];

    // Ecriture fichier
    fwrite(output, 1, output_size, new_fp);

    free(output);
    free(bwt.buffer);
    free(mtf.buffer);
    free(rle.buffer);
    free(line);

    free(new_filename);
    fclose(fp);
    fclose(new_fp);

    printf("Compression terminée.\n");
}

void Decompress_File2RLE(const char *f, Source source, BWT bwt, MTF mtf, RLE rle)
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

    unsigned char *line = (unsigned char *)calloc(MAX_LINE + 1, sizeof(unsigned char));

    size_t size = fread(line, 1, MAX_LINE, fp);
    line = (unsigned char *)realloc(line, sizeof(unsigned char) * size + 1);

    // On récupère l'index pour BWT
    size_t index = (unsigned char)(line[0]) << 24 |
                   (unsigned char)(line[1]) << 16 |
                   (unsigned char)(line[2]) << 8 |
                   (unsigned char)(line[3]);

    // Source
    source.size = size - 4;
    source.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    for (size_t i = 4; i < size; i++)
        source.buffer[i - 4] = line[i];

    // Inverse RLE 0
    rle.inv_buffer0 = (unsigned char *)malloc(sizeof(unsigned char) * source.size * RLE_BUFFER + 1);
    rle.size = Decode_RLE0(source.buffer, rle.inv_buffer0, source.size);
    rle.inv_buffer0 = (unsigned char *)realloc(rle.inv_buffer0, sizeof(unsigned char) * rle.size + 1);

    // Inverse MTF
    mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    mtf.inv_buffer[rle.size] = 0x0;
    Decode_MTF(rle.inv_buffer0, mtf.inv_buffer, rle.size);

    // Inverse BWT
    bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    bwt.inv_buffer[rle.size] = 0x0;
    bwt.index = index;
    Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, rle.size, bwt.index);

    // Inverse RLE 0
    rle.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size * RLE_BUFFER + 1);
    rle.size0 = Decode_RLE(bwt.inv_buffer, rle.inv_buffer, rle.size);
    rle.inv_buffer = (unsigned char *)realloc(rle.inv_buffer, sizeof(unsigned char) * rle.size0 + 1);

    // Write File
    fwrite(rle.inv_buffer, 1, rle.size0, new_fp);

    fclose(fp);
    fclose(new_fp);

    printf("Décompression terminée.\n");

    // Comparaison de l'intégrité du fichier original avec le fichier décompressé
    Compare_File(f, new_filename);

    // Comparaison de la taille du fichier original avec le fichier compressé
    Compare_Size(f, filename);

    free(rle.inv_buffer0);
    free(mtf.inv_buffer);
    free(bwt.inv_buffer);
    free(rle.inv_buffer);
    free(source.buffer);
    free(line);
    free(filename);
    free(new_filename);
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

    unsigned char *line = (unsigned char *)calloc(MAX_LINE + 1, sizeof(unsigned char));

    size_t size = fread(line, 1, MAX_LINE, fp);
    line = (unsigned char *)realloc(line, sizeof(unsigned char) * size + 1);

    // On récupère l'index pour BWT
    size_t index = (unsigned char)(line[0]) << 24 |
                   (unsigned char)(line[1]) << 16 |
                   (unsigned char)(line[2]) << 8 |
                   (unsigned char)(line[3]);

    // Source
    source.size = size - 4;
    source.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    for (size_t i = 4; i < size; i++)
        source.buffer[i - 4] = line[i];

    // Inverse RLE
    rle.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size * RLE_BUFFER + 1);
    rle.size = Decode_RLE0(source.buffer, rle.inv_buffer, source.size);
    rle.inv_buffer = (unsigned char *)realloc(rle.inv_buffer, sizeof(unsigned char) * rle.size + 1);

    // Inverse MTF
    mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    mtf.inv_buffer[rle.size] = 0x0;
    Decode_MTF(rle.inv_buffer, mtf.inv_buffer, rle.size);

    // Inverse BWT
    bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * rle.size + 1);
    bwt.inv_buffer[rle.size] = 0x0;
    bwt.index = index;
    Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, rle.size, bwt.index);

    // Write File
    fwrite(bwt.inv_buffer, 1, rle.size, new_fp);

    fclose(fp);
    fclose(new_fp);

    printf("Décompression terminée.\n");

    // Comparaison de l'intégrité du fichier original avec le fichier décompressé
    Compare_File(f, new_filename);

    // Comparaison de la taille du fichier original avec le fichier compressé
    Compare_Size(f, filename);

    free(bwt.inv_buffer);
    free(mtf.inv_buffer);
    free(rle.inv_buffer);
    free(source.buffer);
    free(line);
    free(filename);
    free(new_filename);
}

int main(int argc, char *argv[])
{
    Source source;
    BWT bwt;
    MTF mtf;
    RLE rle;

    if (argc > 2)
    {
        char *filename = argv[2];
        if (strcmp(argv[1], "-rle2") == 0)
        {
            Compress_File2RLE(filename, source, bwt, mtf, rle);
            Decompress_File2RLE(filename, source, bwt, mtf, rle);
        }
        else if (strcmp(argv[1], "-rle") == 0)
        {
            Compress_File(filename, source, bwt, mtf, rle);
            Decompress_File(filename, source, bwt, mtf, rle);
        }
        else
            printf("Mauvaise ou pas d'options. Options possibles : -rle ou -rle2 (-data/filename).\n");
    }
    else
    {
        if (strcmp(argv[1], "-rle2") == 0)
            Pipeline_Line2RLE((unsigned char *)"Coodage Traaaaaaansmissiooooooon", source, bwt, mtf, rle);
        else if (strcmp(argv[1], "-rle") == 0)
            Pipeline_Line((unsigned char *)"Coodage Traaaaaaansmissiooooooon", source, bwt, mtf, rle);
        else
            printf("Mauvaise ou pas d'options. Options possibles : -rle ou -rle2 (-data/filename).\n");
    }

    return 0;
}