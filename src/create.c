#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include "../include/libTeer.h"
#include <stdio.h>
#include <stdlib.h>

static const char zero_record[LIBTEER_RECORD_LENGTH] = {0};

int libTeer_basic_create(const char *directory, const char *output)
{
    if (directory == NULL)
    {
        fprintf(stderr, "Directory name is null! Aborting...\n");
        return 1;
    }
    if (output == NULL)
    {
        fprintf(stderr, "Output name is null! Aborting...\n");
        return 1;
    }

    FILE *outfile = fopen(output, "w");
    if (outfile == NULL)
    {
        fprintf(stderr, "Failed to open file '%s'!\n", output);
        return 1;
    }
    struct filellist *list = get_directory_list(directory);
    if (list == NULL)
    {
        fprintf(stderr, "Failed to parse directory: '%s'!\n", directory);
        fclose(outfile);
        return 1;
    }

    for (int i = 0; i < list->length; i++)
    {
        struct ustar_filebuf *fbuf = file_to_ustar_buffer(list->data[i]);
        if (fbuf == NULL)
        {
            fprintf(stderr, "Failed to convert %s into a buffer!\n", list->data[i]);
            free_directory_list(list);
            return 1;
        }

        size_t written = 0;
        char *header_data = ustar_header_to_data(fbuf->header);
        if (header_data == NULL) {
            fprintf(stderr, "Failed to allocate memory!\n");
            if (fbuf->records != NULL)
            {
                free(fbuf->records);
            }
            free(fbuf->header);
            free(fbuf);
            free_directory_list(list);
            fclose(outfile);
            return 1;
        }
        written = fwrite(header_data, 1, LIBTEER_RECORD_LENGTH, outfile);
        free(header_data);
        if (written != LIBTEER_RECORD_LENGTH)
        {
            fprintf(stderr, "Failed to write header to file! Expected %d, got %d bytes!\n", LIBTEER_RECORD_LENGTH, (int) written);
            if (fbuf->records != NULL)
            {
                free(fbuf->records);
            }
            free(fbuf->header);
            free(fbuf);
            free_directory_list(list);
            fclose(outfile);
            return 1;
        }
        for (int j = 0; j < fbuf->record_count; j++)
        {
            written = fwrite(&fbuf->records[j], 1, LIBTEER_RECORD_LENGTH, outfile);
            if (written != LIBTEER_RECORD_LENGTH)
            {
                fprintf(stderr, "Failed to write buffer to file! Expected %d, got %d bytes!\n", LIBTEER_RECORD_LENGTH, (int) written);
                if (fbuf->records != NULL)
                {
                    free(fbuf->records);
                }
                free(fbuf->header);
                free(fbuf);
                free_directory_list(list);
                fclose(outfile);
                return 1;
            }
        }

        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
    }

    size_t written = fwrite(zero_record, 1, LIBTEER_RECORD_LENGTH, outfile);
    if (written != LIBTEER_RECORD_LENGTH)
    {
        fprintf(stderr, "Failed to zero buffer to file! Expected %d, got %d bytes!\n", LIBTEER_RECORD_LENGTH, (int) written);
        free_directory_list(list);
        fclose(outfile);
        return 1;
    }

    free_directory_list(list);
    fclose(outfile);

    return 0;
}

int libTeer_create(const char *directory, const char *output, libTeer_Compression_Mode compression_mode)
{
    int rc = 0;
    libTeer_Compression_Mode used_mode = AUTO_COMPRESSION;
    if (compression_mode < 0 || compression_mode > COMPRESSION_MODE_SIZE) {
        fprintf(stderr, "Compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE -1);
        rc = 1;
        return rc;
    }

    if (compression_mode == 0) {
        used_mode = get_mode_from_filename(output);
        if (used_mode == -1) {
            rc = -2;
            return rc;
        }
    } else {
        used_mode = compression_mode;
    }

    char rand_filename[1024];
    char str[] = {[41] = '\1'};
    rand_str(str, sizeof str - 1);
    rc = snprintf(rand_filename, 1024, "/tmp/%s", str);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not generate filename\n");
        return 1;
    }

    if (used_mode == NOCOMPRESS) {
        rc = libTeer_basic_create(directory, output);
    } else if (used_mode == AUTO_COMPRESSION || used_mode == COMPRESSION_MODE_SIZE) {
        rc = -2;
        fprintf(stderr, "An unexpected error occured!\n");
    } else {
        rc = libTeer_basic_create(directory, rand_filename);
    }
    if (rc != 0)
    {
        return rc;
    }

    rc = 0;

    switch (used_mode)
    {
    case NOCOMPRESS:
        break;
    case GZIP:
        rc = libTeer_gzip_compress(rand_filename, output);
        break;
    case XZ:
        rc = libTeer_xz_compress(rand_filename, output, LIBTEER_DEFAULT_XZ_COMPRESSION_LEVEL);
        break;
    case BZIP2:
        rc = libTeer_bzip2_compress(rand_filename, output, LIBTEER_DEFAULT_BZIP2_COMPRESSION_LEVEL, LIBTEER_DEFAULT_BZIP2_WORK_FACTOR);
        break;
    case ZSTD:
        rc = libTeer_zstd_compress(rand_filename, output, LIBTEER_DEFAULT_ZSTD_COMPRESSION_LEVEL);
        break;
    case AUTO_COMPRESSION:
        rc = -2;
        fprintf(stderr, "An unexpected error occured!\n");
        break;
    case COMPRESSION_MODE_SIZE:
        rc = -2;
        fprintf(stderr, "An unexpected error occured!\n");
        break;
    }

    return 0;
}