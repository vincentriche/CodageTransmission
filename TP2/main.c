#include "mtf.h"

void DislayLine(Source source, BWT bwt, MTF mtf)
{
    printf("\nCOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Source :   '%s' size of %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("-------------------------------------------------------\n");
    printf("\nDECOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Inv MTF :  '%s'\n", mtf.inv_buffer);
    printf("Inv BWT:   '%s'\n", bwt.inv_buffer);
    printf("-------------------------------------------------------\n");
}

void DislayCompressionLine(Source source, BWT bwt, MTF mtf)
{
    printf("\nCOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Source :   '%s' size of %zu\n", source.buffer, source.size);
    printf("BWT :      '%s'\n", bwt.buffer);
    printf("MTF :      ");
    for (size_t i = 0; i < source.size; i++)
        printf("%d ", mtf.buffer[i]);
    printf("\n-------------------------------------------------------\n");
}

void DislayDecompressionLine(BWT bwt, MTF mtf)
{

    printf("\nDECOMPRESSION\n");
    printf("-------------------------------------------------------\n");
    printf("Inv MTF :  '%s'\n", mtf.inv_buffer);
    printf("Inv BWT:   '%s'\n", bwt.inv_buffer);
    printf("-------------------------------------------------------\n");
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
    mtf.buffer = (unsigned int *)malloc(sizeof(unsigned int) * source.size + 1);
    Encode_MTF(bwt.buffer, mtf.buffer, source.size);

    // Inverse MTF Part
    mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    mtf.inv_buffer[source.size] = 0x0;
    Decode_MTF(mtf.buffer, mtf.inv_buffer, source.size);

    // Inverse BWT Part
    bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.inv_buffer[source.size] = 0x0;
    Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, source.size, bwt.index[0]);

    DislayLine(source, bwt, mtf);

    free(bwt.buffer);
    free(bwt.inv_buffer);
    free(mtf.buffer);
    free(mtf.inv_buffer);
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

void Compress_File(const char *f, Source source, BWT bwt, MTF mtf)
{
    char *file = strdup(f);
    char *new_filename = Generate_Filename(file, "C");
    free(file);

    FILE *fp = fopen(f, "r");
    FILE *new_fp = fopen(new_filename, "w");

    if (!fp)
        printf("File doesn't exists.\n");

    int indexes = 0;
    int max_line = 9999;
    char line[max_line];
    while (fgets(line, max_line, fp) != NULL)
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

        // Write File
        for (size_t i = 0; i < source.size; i++)
            fprintf(new_fp, "%d.", mtf.buffer[i]);
        fprintf(new_fp, "\n");

        DislayCompressionLine(source, bwt, mtf);

        free(bwt.buffer);
        free(mtf.buffer);
    }
    fclose(fp);
    fclose(new_fp);
    free(new_filename);
}

void Decompress_File(const char *f, Source source, BWT bwt, MTF mtf)
{
    // TO DO : Réussi à faire une fonction de création de nom de fichiers !
    char *file = strdup(f);
    char *filename = Generate_Filename(file, "C");
    free(file);

    file = strdup(f);
    char *new_filename = Generate_Filename(file, "D");
    free(file);

    FILE *fp = fopen(filename, "r");
    FILE *new_fp = fopen(new_filename, "w");

    if (!fp)
        printf("File doesn't exists.\n");

    int indexes = 0;
    int max_line = 9999;
    char line[max_line];
    while (fgets(line, max_line, fp) != NULL)
    {
        strtok(line, "\n");
        if (line[0] == '\n')
        {
            fprintf(new_fp, "\n");
            continue;
        }

        unsigned int *temp_values = (unsigned int *)malloc(sizeof(unsigned int) * 9999);

        int size = 0;
        char *tok = strtok(line, ".");
        temp_values[0] = atoi(tok);
        while (tok != NULL)
        {
            temp_values[size++] = atoi(tok);
            tok = strtok(NULL, ".");
        }

        unsigned int *source_buffer = (unsigned int *)malloc(sizeof(unsigned int) * size + 1);
        for (int i = 0; i < size; i++)
            source_buffer[i] = temp_values[i];

        // Source Part
        source.buffer = NULL;
        source.size = size;

        // Inverse MTF Part
        mtf.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        mtf.inv_buffer[source.size] = 0x0;
        Decode_MTF(source_buffer, mtf.inv_buffer, source.size);

        // Inverse BWT Part
        bwt.inv_buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
        bwt.inv_buffer[source.size] = 0x0;
        Decode_BWT(mtf.inv_buffer, bwt.inv_buffer, source.size, bwt.index[indexes++]);

        // Write File
        fprintf(new_fp, "%s\n", bwt.inv_buffer);

        DislayDecompressionLine(bwt, mtf);

        free(temp_values);
        free(source_buffer);
        free(bwt.inv_buffer);
        free(mtf.inv_buffer);
    }
    fclose(fp);
    fclose(new_fp);
    free(filename);
    free(new_filename);
}

int main()
{
    Source source;
    BWT bwt;
    MTF mtf;

    bwt.index = (int *)calloc(100, sizeof(int));

    //PipelineLine((unsigned char *)"Codage Transmission", source, bwt, mtf);

    char *filename = "data/file2";
    Compress_File(filename, source, bwt, mtf);
    Decompress_File(filename, source, bwt, mtf);

    free(bwt.index);
    return 0;
}