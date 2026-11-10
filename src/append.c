#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static const char zero_record[LIBTEER_RECORD_LENGTH] = {0};

int libTeer_basic_append(const char *archive_name, const char *filename)
{
    if (archive_name == NULL)
    {
        fprintf(stderr, "Archive name is null! Aborting...\n");
        return 1;
    }
    if (filename == NULL)
    {
        fprintf(stderr, "Filename is null! Aborting...\n");
        return 1;
    }

    struct ustar_filebuf *fbuf = file_to_ustar_buffer(filename);
    if (fbuf == NULL)
    {
        fprintf(stderr, "Failed to convert %s into a buffer!\n", filename);
        return 1;
    }

    struct stat st;
    if (stat(archive_name, &st))
    {
        fprintf(stderr, "Failed to stat file %s!\n", archive_name);
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        return 1;
    }
    if (st.st_size % LIBTEER_RECORD_LENGTH)
    {
        fprintf(stderr, "Not a tar file %s, filelength not divisible by %d!\n", archive_name, LIBTEER_RECORD_LENGTH);
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        return 1;
    }

    FILE *archive = fopen(archive_name, "r+");
    if (archive == NULL)
    {
        fprintf(stderr, "Failed to open file %s!\n", archive_name);
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        return 1;
    }

    char (*target_records)[LIBTEER_RECORD_LENGTH] = malloc(st.st_size);
    if (target_records == NULL)
    {
        fprintf(stderr, "Could not allocate memory!\n");
        fclose(archive);
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        return 1;
    }
    int target_records_length = st.st_size / LIBTEER_RECORD_LENGTH;
    for (int i = 0; i < target_records_length; i++)
    {
        char *data = read_single_record(archive);
        if (data == NULL)
        {
            free(target_records);
            fclose(archive);
            if (fbuf->records != NULL)
            {
                free(fbuf->records);
            }
            free(fbuf->header);
            free(fbuf);
            fprintf(stderr, "Could not allocate memory\n");
            return 1;
        }
        memcpy(target_records[i], data, LIBTEER_RECORD_LENGTH);
        free(data);
    }

    int target_filled_headers = 0;

    if (memcmp(target_records[target_records_length - 1], zero_record, LIBTEER_RECORD_LENGTH) == 0 && memcmp(target_records[target_records_length - 2], zero_record, LIBTEER_RECORD_LENGTH) == 0)
    {
        for (int j = 0; j < target_records_length; j++)
        {
            if (memcmp(target_records[j], zero_record, LIBTEER_RECORD_LENGTH) != 0)
                target_filled_headers++;
        }
    }
    else
    {
        fprintf(stderr, "File %s does not have 2 empty records at the end!\n", archive_name);
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        free(target_records);
        fclose(archive);
        return 1;
    }

    char (*new_records)[LIBTEER_RECORD_LENGTH] = realloc(target_records, (target_filled_headers + fbuf->record_count + 3 /*1 header + two zero records*/) * LIBTEER_RECORD_LENGTH);
    if (new_records == NULL)
    {
        fprintf(stderr, "Could not allocate memory!\n");
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        free(target_records);
        fclose(archive);
        return 1;
    }
    target_records = new_records;
    target_records_length = (target_filled_headers + fbuf->record_count + 3 /*1 header + two zero records*/);

    char *header_data = ustar_header_to_data(fbuf->header);
    if (header_data == NULL)
    {
        fprintf(stderr, "Could not allocate memory!\n");
        if (fbuf->records != NULL)
        {
            free(fbuf->records);
        }
        free(fbuf->header);
        free(fbuf);
        free(target_records);
        fclose(archive);
        return 1;
    }
    memcpy(target_records[target_filled_headers], header_data, LIBTEER_RECORD_LENGTH);
    free(header_data);
    for (int j = 0; j < fbuf->record_count; j++)
    {
        memcpy(target_records[target_filled_headers + j + 1], &fbuf->records[j], LIBTEER_RECORD_LENGTH);
    }
    memcpy(target_records[target_records_length - 1], zero_record, LIBTEER_RECORD_LENGTH);
    memcpy(target_records[target_records_length - 2], zero_record, LIBTEER_RECORD_LENGTH);

    rewind(archive);
    for (int i = 0; i < target_records_length; i++)
    {
        for (int j = 0; j < LIBTEER_RECORD_LENGTH; j++)
        {
            if (fputc(target_records[i][j], archive) == EOF)
            {
                fprintf(stderr, "Could not write to %s!\n", archive_name);
                if (fbuf->records != NULL)
                {
                    free(fbuf->records);
                }
                free(fbuf->header);
                free(fbuf);
                free(target_records);
                fclose(archive);
                return 1;
            }
        }
    }

    free(target_records);
    if (fbuf->records != NULL)
    {
        free(fbuf->records);
    }
    free(fbuf->header);
    free(fbuf);
    if (fclose(archive) != 0) {
        fprintf(stderr, "Could not close file %s!\n", archive_name);
        return 1;
    }

    return 0;
}

int libTeer_append(const char *archive_name, const char *filename, libTeer_Compression_Mode compression_mode)
{
    int rc = 0;
    libTeer_Compression_Mode used_mode = AUTO_COMPRESSION;
    if (compression_mode < 0 || compression_mode > COMPRESSION_MODE_SIZE) {
        fprintf(stderr, "Compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE -1);
        rc = 1;
        return rc;
    }

    if (compression_mode == 0) {
        used_mode = get_mode_from_filename(archive_name);
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

    rc = 0;

    switch (used_mode)
    {
    case NOCOMPRESS:
        break;
    case GZIP:
        rc = libTeer_gzip_decompress(archive_name, rand_filename);
        break;
    case XZ:
        rc = libTeer_xz_decompress(archive_name, rand_filename);
        break;
    case BZIP2:
        rc = libTeer_bzip2_decompress(archive_name, rand_filename);
        break;
    case ZSTD:
        rc = libTeer_zstd_decompress(archive_name, rand_filename);
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
    if (rc != 0)
    {
        return rc;
    }

    if (used_mode == NOCOMPRESS) {
        rc = libTeer_basic_append(archive_name, filename);
    } else {
        rc = libTeer_basic_append(rand_filename, filename);
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
        rc = libTeer_gzip_compress(rand_filename, archive_name);
        break;
    case XZ:
        rc = libTeer_xz_compress(rand_filename, archive_name, LIBTEER_DEFAULT_XZ_COMPRESSION_LEVEL);
        break;
    case BZIP2:
        rc = libTeer_bzip2_compress(rand_filename, archive_name, LIBTEER_DEFAULT_BZIP2_COMPRESSION_LEVEL, LIBTEER_DEFAULT_BZIP2_WORK_FACTOR);
        break;
    case ZSTD:
        rc = libTeer_zstd_compress(rand_filename, archive_name, LIBTEER_DEFAULT_ZSTD_COMPRESSION_LEVEL);
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

    return rc;
}