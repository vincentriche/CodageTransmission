#include "mtf.h"

int indexes[1000];

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

void PipelineLine(unsigned char *s)
{
    Source source;
    BWT bwt;
    MTF mtf;

    // BWT Part
    source.buffer = s;
    source.size = strlen((const char *)source.buffer);

    bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
    bwt.buffer[source.size] = 0x0;
    bwt.index = Encode_BWT(source.buffer, bwt.buffer, source.size);

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
    Decode_BWT(mtf.i_buffer, bwt.i_buffer, source.size, bwt.index);

    DislayLine(source, bwt, mtf);
}

void Compress_File(const char *filename)
{
    Source source;
    BWT bwt;
    MTF mtf;
    FILE *file = fopen(filename, "r");
    FILE *new_file = fopen("fileC.txt", "w");
    if (file)
    {
        int k = 0;
        int max_line = 9999;
        char line[max_line];
        while (fgets(line, max_line, file) != NULL)
        {
            strtok(line, "\n");

            // Source Part
            source.buffer = (unsigned char *)line;
            source.size = strlen((const char *)source.buffer);

            // BWT Part
            bwt.buffer = (unsigned char *)malloc(sizeof(unsigned char) * source.size + 1);
            bwt.buffer[source.size] = 0x0;
            indexes[k] = Encode_BWT(source.buffer, bwt.buffer, source.size);

            // MTF Part
            mtf.buffer = (int *)malloc(sizeof(int) * source.size + 1);
            Encode_MTF(bwt.buffer, mtf.buffer, source.size);

            k++;

            // Write File
            for (size_t i = 0; i < source.size; i++)
                fprintf(new_file, "%d.", mtf.buffer[i]);
            fprintf(new_file, "\n");

            printf("COMPRESSION\n");
            printf("-------------------------------------------------------\n");
            DislayCompressionLine(source, bwt, mtf);
            printf("-------------------------------------------------------\n");
        }
    }
    else
        printf("File doesn't exists.\n");
    fclose(file);
    fclose(new_file);

    free(mtf.buffer);
    free(bwt.buffer);
}

void Decompress_File(const char *filename)
{
    Source source;
    BWT bwt;
    MTF mtf;
    FILE *file = fopen(filename, "r");
    FILE *new_file = fopen("fileD.txt", "w");
    if (file)
    {
        int k = 0;
        int max_line = 9999;
        char line[max_line];
        while (fgets(line, max_line, file) != NULL)
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
            bwt.index = indexes[k];
            Decode_BWT(mtf.i_buffer, bwt.i_buffer, source.size, bwt.index);

            k++;

            // Write File
            fprintf(new_file, "%s\n", bwt.i_buffer);

            printf("DECOMPRESSION\n");
            printf("-------------------------------------------------------\n");
            DislayDecompressionLine(bwt, mtf);
            printf("-------------------------------------------------------\n");

            free(temp_values);
        }
    }
    else
        printf("File doesn't exists.\n");
    fclose(file);
    fclose(new_file);

    free(bwt.i_buffer);
    free(mtf.i_buffer);
}

int main()
{
    //PipelineLine((unsigned char *)"Codage Transmission");
    Compress_File("file.txt");
    Decompress_File("fileC.txt");
    return 0;
}