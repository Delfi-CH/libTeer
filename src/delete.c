#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>

int libTeer_basic_delete(const char *archive_name, const char *filename)
{
    if (filename == NULL)
    {
        fprintf(stderr, "Filename is null! Aborting...\n");
        return 1;
    }
    if (archive_name == NULL)
    {
        fprintf(stderr, "Archive name is null! Aborting...\n");
        return 1;
    }

    struct stat st;
    if (stat(archive_name, &st))
    {
        fprintf(stderr, "Failed to stat file %s!\n", archive_name);
        return 1;
    }
    if (st.st_size % LIBTEER_RECORD_LENGTH)
    {
        fprintf(stderr, "Not a tar file %s, filelength not divisible by %d!\n", archive_name, LIBTEER_RECORD_LENGTH);
        return 1;
    }

    FILE *archive = fopen(archive_name, "rb");
    if (archive == NULL)
    {
        fprintf(stderr, "Failed to open file %s!\n", archive_name);
        return 1;
    }

    size_t initial_record_count = st.st_size / LIBTEER_RECORD_LENGTH;
    LibTeer_Record *initial_records = calloc(initial_record_count, sizeof(LibTeer_Record));
    if (initial_records == NULL)
    {
        fprintf(stderr, "Failed to allocate Memory!\n");
        fclose(archive);
        return 1;
    }

    size_t final_record_count = 0;
    size_t deleted_record_begins = 0;
    size_t deleted_record_count = 0;
    bool is_target_record = false;
    for (int i = 0; i < initial_record_count; i++)
    {
        char *buf = read_single_record(archive);
        if (buf == NULL)
        {
            fprintf(stderr, "Failed to read buffer from %s!\n", archive_name);
            free(initial_records);
            fclose(archive);
            return 1;
        }

        memcpy(initial_records[i].data, buf, LIBTEER_RECORD_LENGTH);

        if (is_target_record)
        {
            if(is_record_ustar_header(buf)) {
                is_target_record = false;
                free(buf);
                continue;
            }
            deleted_record_count++;
        }
        else
        {
            if (is_record_ustar_header(buf))
            {
                LibTeer_UStarHeader *header = calloc(1, sizeof(LibTeer_UStarHeader));
                memcpy(header, buf, LIBTEER_RECORD_LENGTH);
                char path[257];
                if (header->filename_prefix[0] != '\0')
                {
                    snprintf(path, sizeof(path), "%s/%s", header->filename_prefix, header->file_path);
                }
                else
                {
                    snprintf(path, sizeof(path), "%s", header->file_path);
                }

                if (strncmp(filename, path, strlen(filename)) == 0)
                {
                    is_target_record = true;
                    deleted_record_begins = i;
                    free(buf);
                    free(header);
                    continue;
                }
                free(header);
            }
            final_record_count++;
        }

        free(buf);
    }
    size_t deleted_record_end = deleted_record_begins + deleted_record_count + 1;

    LibTeer_Record *final_records = calloc(final_record_count, sizeof(LibTeer_Record));
    if (final_records == NULL)
    {
        fprintf(stderr, "Failed to allocate Memory!\n");
        free(initial_records);
        fclose(archive);
        return 1;
    }

    size_t copied = 0;
    for (int i = 0; i < initial_record_count; i++) {
        if (i >= deleted_record_begins && i < deleted_record_end) {
            deleted_record_count++;
            continue;
        }
        if (copied >= final_record_count) {
            break;
        }
        memcpy(final_records[copied].data, initial_records[i].data, LIBTEER_RECORD_LENGTH);
        copied++;
    }
    free(initial_records);

    if (copied != final_record_count) {
        #ifdef _WIN32
            fprintf(stderr, "Did not copy right amount of data, expected %lld, got %lld\n", final_record_count, copied);
        #else
            fprintf(stderr, "Did not copy right amount of data, expected %ld, got %ld\n", final_record_count, copied);
        #endif
        free(final_records);
        return 1;
    }
    fclose(archive);
    archive = fopen(archive_name, "wb");
    if (archive == NULL)
    {
        fprintf(stderr, "Failed to reopen file %s!\n", archive_name);
        free(final_records);
        return 1;
    }

    for (int i = 0; i < final_record_count; i++) {
        size_t written = fwrite(final_records[i].data, LIBTEER_RECORD_LENGTH, 1, archive);
        if (written != LIBTEER_RECORD_LENGTH && written != 1) {
            #ifdef _WIN32
                fprintf(stderr, "Did not write correct amount of data to file %s, expected %d, got %lld\n", archive_name, LIBTEER_RECORD_LENGTH, written);
            #else
                fprintf(stderr, "Did not write correct amount of data to file %s, expected %d, got %ld\n", archive_name, LIBTEER_RECORD_LENGTH, written);
            #endif
            free(final_records);
            fclose(archive);
            return 1;
        }
    }

    free(final_records);
    fclose(archive);
    return 0;
}

int libTeer_delete(const char *archive_name, const char *filename, LibTeer_Compression_Mode compression_mode)
{
    int rc = 0;
    LibTeer_Compression_Mode used_mode = AUTO_COMPRESSION;
    if (compression_mode < 0 || compression_mode > COMPRESSION_MODE_SIZE)
    {
        fprintf(stderr, "Compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE - 1);
        rc = 1;
        return rc;
    }

    if (compression_mode == 0)
    {
        used_mode = get_mode_from_filename(archive_name);
        if (used_mode == -1)
        {
            rc = -2;
            return rc;
        }
    }
    else
    {
        used_mode = compression_mode;
    }

    char rand_filename[1024];
    char str[] = {[41] = '\1'};
    rand_str(str, sizeof str - 1);
    rc = snprintf(rand_filename, 1024, "%s%s", get_tmpdir_path(), str);
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
        rc = libTeer_gzip_decompress_file(archive_name, rand_filename);
        break;
    case XZ:
        rc = libTeer_xz_decompress_file(archive_name, rand_filename);
        break;
    case BZIP2:
        rc = libTeer_bzip2_decompress_file(archive_name, rand_filename);
        break;
    case ZSTD:
        rc = libTeer_zstd_decompress_file(archive_name, rand_filename);
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

    if (used_mode == NOCOMPRESS)
    {
        rc = libTeer_basic_delete(archive_name, filename);
    }
    else
    {
        rc = libTeer_basic_delete(rand_filename, filename);
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
        rc = libTeer_gzip_compress_file(rand_filename, archive_name);
        break;
    case XZ:
        rc = libTeer_xz_compress_file(rand_filename, archive_name, LIBTEER_DEFAULT_XZ_COMPRESSION_LEVEL);
        break;
    case BZIP2:
        rc = libTeer_bzip2_compress_file(rand_filename, archive_name, LIBTEER_DEFAULT_BZIP2_COMPRESSION_LEVEL, LIBTEER_DEFAULT_BZIP2_WORK_FACTOR);
        break;
    case ZSTD:
        rc = libTeer_zstd_compress_file(rand_filename, archive_name, LIBTEER_DEFAULT_ZSTD_COMPRESSION_LEVEL);
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

    if (rc != 0) return rc;

    if (used_mode != NOCOMPRESS) {
        rc = remove(rand_filename);
    }

    return rc;
}