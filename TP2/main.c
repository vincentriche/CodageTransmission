#include "mtf.h"

void DislayLine(Source source, BWT bwt, MTF mtf)
{
    printf("Source :   '%s' size of %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n");
    printf("Inv MTF :  '%s'\n", mtf.i_buffer);
    printf("Inv BWT:   '%s'\n", bwt.i_buffer);
}

void DislayCompressionLine(Source source, BWT bwt, MTF mtf)
{
    printf("Source :   '%s' size of %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n");
}

void DislayDecompressionLine(BWT bwt, MTF mtf)
{
    printf("Inv MTF :  '%s'\n", mtf.i_buffer);
    printf("Inv BWT:   '%s'\n", bwt.i_buffer);
}

void PipelineLine(unsigned char *s, Source source, BWT bwt, MTF mtf)
{
    // BWT Part
    source.buffer = s;
    source.size = strlen((const char *)source.buffer);

    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.buffer[source.size] = 0x0;
    bwt.index[0] = Encode_BWT(source.buffer, bwt.buffer, source.size);

    // MTF Part
    mtf.buffer = (int *)malloc(sizeof(int) * source.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, source.size);

    // Inverse MTF Part
    mtf.i_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    mtf.i_buffer[source.size] = 0x0;
    Decode_MTF(mtf.buffer, mtf.i_buffer, source.size);

    // Inverse BWT Part
    bwt.i_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.i_buffer[source.size] = 0x0;
    Decode_BWT(mtf.i_buffer, bwt.i_buffer, source.size, bwt.index[0]);

    //DislayLine(source, bwt, mtf);

    free(bwt.buffer);
    free(bwt.i_buffer);
    free(mtf.buffer);
    free(mtf.i_buffer);
}

void Compress_File(const char *f, Source source, BWT bwt, MTF mtf)
{

    // TO DO : Réussi à faire une fonction de création de nom de fichiers !
    char *file = strdup(f);
    char *tok = strtok(file, ".");
    char new_filename[100];
    strcpy(new_filename, tok);
    strcat(new_filename, "C");

    tok = strtok(NULL, ".");
    if (tok != NULL)
    {
        strcat(new_filename, ".");
        strcat(new_filename, tok);
    }

    FILE *fp = fopen(f, "r");
    FILE *new_fp = fopen(new_filename, "w");

    if (!fp)
        printf("File doesn't exists.\n");

    int k = 0;
    int max_line = 9999;
    char line[max_line];
    while (fgets(line, max_line, fp) != NULL)
    {
        strtok(line, "\n");

        // Source Part
        source.buffer = (unsigned char *)line;
        source.size = strlen((const char *)source.buffer);

        // BWT Part
        bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        bwt.buffer[source.size] = 0x0;
        bwt.index[k] = Encode_BWT(source.buffer, bwt.buffer, source.size);

        // MTF Part
        mtf.buffer = (int *)malloc(sizeof(int) * source.size + 1);
        Encode_MTF(bwt.buffer, mtf.buffer, source.size);

        k++;

        // Write File
        for (size_t i = 0; i < source.size; i++)
            fprintf(new_fp, "%d.", mtf.buffer[i]);
        fprintf(new_fp, "\n");

        printf("COMPRESSION\n");
        printf("-------------------------------------------------------\n");
        DislayCompressionLine(source, bwt, mtf);
        printf("-------------------------------------------------------\n");

        free(bwt.buffer);
        free(mtf.buffer);
    }
    fclose(fp);
    fclose(new_fp);

    free(file);
}

void Decompress_File(const char *f, Source source, BWT bwt, MTF mtf)
{
    // TO DO : Réussi à faire une fonction de création de nom de fichiers !
    char *file = strdup(f);
    char *tok = strtok(file, ".");

    char filename[100];
    char new_filename[100];

    strcpy(filename, tok);
    strcpy(new_filename, tok);

    strcat(filename, "C");
    strcat(new_filename, "D");

    tok = strtok(NULL, ".");
    if (tok != NULL)
    {
        strcat(filename, ".");
        strcat(filename, tok);
        strcat(new_filename, ".");
        strcat(new_filename, tok);
    }

    printf("%s %s\n", filename, new_filename);

    FILE *fp = fopen(filename, "r");
    FILE *new_fp = fopen(new_filename, "w");

    if (!fp)
        printf("File doesn't exists.\n");

    int k = 0;
    int max_line = 9999;
    char line[max_line];
    while (fgets(line, max_line, fp) != NULL)
    {
        strtok(line, "\n");

        int *temp_values = (int *)malloc(sizeof(int) * 9999);

        int l = 0;
        char *tok = strtok(line, ".");
        temp_values[0] = atoi(tok);
        while (tok != NULL)
        {
            temp_values[l] = atoi(tok);
            tok = strtok(NULL, ".");
            l++;
        }

        int *source_buffer = (int *)malloc(sizeof(int) * l + 1);
        for (int i = 0; i < l; i++)
            source_buffer[i] = temp_values[i];

        // Source Part
        source.buffer = NULL;
        source.size = l;

        // Inverse MTF Part
        mtf.i_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        mtf.i_buffer[source.size] = 0x0;
        Decode_MTF(source_buffer, mtf.i_buffer, source.size);

        // Inverse BWT Part
        bwt.i_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        bwt.i_buffer[source.size] = 0x0;
        Decode_BWT(mtf.i_buffer, bwt.i_buffer, source.size, bwt.index[k]);

        k++;

        // Write File
        fprintf(new_fp, "%s\n", bwt.i_buffer);

        printf("DECOMPRESSION\n");
        printf("-------------------------------------------------------\n");
        DislayDecompressionLine(bwt, mtf);
        printf("-------------------------------------------------------\n");

        free(temp_values);
        free(source_buffer);
        free(bwt.i_buffer);
        free(mtf.i_buffer);
    }
    fclose(fp);
    fclose(new_fp);

    free(file);
}

int main()
{
    Source source;
    BWT bwt;
    bwt.index = (int *)calloc(100, sizeof(int));
    MTF mtf;

    //PipelineLine((unsigned char *)"Codage Transmission", source, bwt, mtf);

    char *filename = "data/file1.txt";
    Compress_File(filename, source, bwt, mtf);
    Decompress_File(filename, source, bwt, mtf);

    free(bwt.index);
    return 0;
}