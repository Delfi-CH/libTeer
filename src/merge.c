#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static const char zero_record[LIBTEER_RECORD_LENGTH] = {0}; 

int libTeer_basic_merge(const char *source, const char *target) {
    if (source == NULL) {
        fprintf(stderr, "Source filename is null! Aborting...\n");
        return 1;
    }
    if (target == NULL) {
        fprintf(stderr, "Target filename is null! Aborting...\n");
        return 1;
    }

    struct stat st1;
    if (stat(source, &st1))
    {
        fprintf(stderr, "Failed to stat file %s!\n", source);
        return 1;
    }

    struct stat st2;
    if (stat(target, &st2))
    {
        fprintf(stderr, "Failed to stat file %s!\n", target);
        return 1;
    }

    if (st1.st_size % LIBTEER_RECORD_LENGTH)
    {
        fprintf(stderr, "Not a tar file %s, filelength not divisible by %d!\n", source, LIBTEER_RECORD_LENGTH);
        return 1;
    }
    if (st2.st_size % LIBTEER_RECORD_LENGTH)
    {
        fprintf(stderr, "Not a tar file %s, filelength not divisible by %d!\n", target, LIBTEER_RECORD_LENGTH);
        return 1;
    }

    FILE *sourcefile = fopen(source, "r");
    if (sourcefile == NULL) {
        fprintf(stderr, "Failed to open file %s!\n", source);
        return 1;
    }

    FILE *targetfile = fopen(target, "r+");
    if (targetfile == NULL) {
        fprintf(stderr, "Failed to open file %s!\n", target);
        fclose(sourcefile);
        return 1;
    }

    char (*source_records)[LIBTEER_RECORD_LENGTH] = malloc(st1.st_size);
    if (source_records == NULL) {
        fprintf(stderr, "Could not allocate memory!\n");
        fclose(sourcefile);
        fclose(targetfile);
        return 1;
    }
    int source_records_length = st1.st_size / LIBTEER_RECORD_LENGTH;
    for (int i = 0; i < source_records_length; i++) {
        char *data = read_single_record(sourcefile);
        if (data == NULL) {
            free(source_records);
            fclose(targetfile);
            fprintf(stderr, "Could not allocate memory\n");
            return 1;
        }
        memcpy(source_records[i], data, LIBTEER_RECORD_LENGTH);
        free(data); 
    }

    if (fclose(sourcefile) != 0) {
        free(source_records);
        fclose(targetfile);
        fprintf(stderr, "Could not close file %s!\n", source);
        return 1;
    }

    char (*target_records)[LIBTEER_RECORD_LENGTH] = malloc(st2.st_size);
    if (target_records == NULL) {
        fprintf(stderr, "Could not allocate memory!\n");
        free(source_records);
        fclose(targetfile);
        return 1;
    }
    int target_records_length = st2.st_size / LIBTEER_RECORD_LENGTH;
    for (int i = 0; i < target_records_length; i++) {
        char *data = read_single_record(targetfile);
        if (data == NULL) {
            free(source_records);
            free(target_records);
            fclose(targetfile);
            fprintf(stderr, "Could not allocate memory\n");
            return 1;
        }
        memcpy(target_records[i], data, LIBTEER_RECORD_LENGTH);
        free(data); 
    }

    int target_filled_headers = 0;
    
    if (memcmp(target_records[target_records_length -1], zero_record, LIBTEER_RECORD_LENGTH) == 0 && memcmp(target_records[target_records_length -2], zero_record, LIBTEER_RECORD_LENGTH) == 0) {
        for (int j = 0; j < target_records_length; j++) {
            if (memcmp(target_records[j], zero_record, LIBTEER_RECORD_LENGTH) != 0) target_filled_headers++;
        }
    } else {
        fprintf(stderr, "File %s does not have 2 empty records at the end!\n", target);
        free(source_records);
        free(target_records);
        fclose(targetfile);
        return 1;
    }

    char (*new_records)[LIBTEER_RECORD_LENGTH] = realloc(target_records, (target_filled_headers + source_records_length) * LIBTEER_RECORD_LENGTH);
    if (new_records == NULL) {
        fprintf(stderr, "Could not allocate memory!\n");
        free(source_records);
        free(target_records);
        fclose(targetfile);
        return 1;
    }
    target_records = new_records;
    target_records_length = (target_filled_headers + source_records_length);
    for (int j = 0; j < source_records_length; j++) {
        memcpy(target_records[target_filled_headers + j], source_records[j], LIBTEER_RECORD_LENGTH);
    }

    free(source_records);

    rewind(targetfile);
    for (int i = 0; i < target_records_length; i++) {
        for (int j = 0; j < LIBTEER_RECORD_LENGTH; j++) {
            if (fputc(target_records[i][j], targetfile) == EOF) {
                fprintf(stderr, "Could not write to %s!\n", target);
                fclose(targetfile);
                free(new_records);
                return 1;
            }
        }
    }

    free(new_records);
    if (fclose(targetfile) != 0) {
        fprintf(stderr, "Could not close file %s!\n", target);
        return 1;
    }

    return 0;
}

int libTeer_merge(const char *source, const char *target, libTeer_Compression_Mode source_compression_mode, libTeer_Compression_Mode target_compression_mode) {
    int rc = 0;
    libTeer_Compression_Mode source_used_mode = AUTO_COMPRESSION;
    if (source_compression_mode < 0 || source_compression_mode > COMPRESSION_MODE_SIZE) {
        fprintf(stderr, "Source compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE - 1);
        rc = 1;
        return rc;
    }

    if (source_compression_mode == 0) {
        source_used_mode = get_mode_from_filename(source);
        if (source_used_mode == -1) {
            rc = -2;
            return rc;
        }
    } else {
        source_used_mode = source_compression_mode;
    }

    libTeer_Compression_Mode target_used_mode = AUTO_COMPRESSION;
    if (target_compression_mode < 0 || target_compression_mode > COMPRESSION_MODE_SIZE) {
        fprintf(stderr, "Target compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE - 1);
        rc = 1;
        return rc;
    }

    if (target_used_mode == 0) {
        target_used_mode = get_mode_from_filename(target);
        if (target_used_mode == -1) {
            rc = -2;
            return rc;
        }
    } else {
        target_used_mode = target_compression_mode;
    }

    char rand_filename_source[1024];
    char str_source[] = {[41] = '\1'};
    rand_str(str_source, sizeof str_source - 1);
    rc = snprintf(rand_filename_source, 1024, "/tmp/%s", str_source);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not generate filename\n");
        return 1;
    }

    char rand_filename_target[1024];
    char str_target[] = {[41] = '\1'};
    rand_str(str_target, sizeof str_target - 1);
    rc = snprintf(rand_filename_target, 1024, "/tmp/%s", str_target);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not generate filename\n");
        return 1;
    }

    switch (source_used_mode)
    {
    case NOCOMPRESS:
        break;
    case GZIP:
        rc = libTeer_gzip_decompress(source, rand_filename_source);
        break;
    case XZ:
        rc = libTeer_xz_decompress(source, rand_filename_source);
        break;
    case BZIP2:
        rc = libTeer_bzip2_decompress(source, rand_filename_source);
        break;
    case ZSTD:
        rc = libTeer_zstd_decompress(source, rand_filename_source);
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

    switch (target_used_mode)
    {
    case NOCOMPRESS:
        break;
    case GZIP:
        rc = libTeer_gzip_decompress(target, rand_filename_target);
        break;
    case XZ:
        rc = libTeer_xz_decompress(target, rand_filename_target);
        break;
    case BZIP2:
        rc = libTeer_bzip2_decompress(target, rand_filename_target);
        break;
    case ZSTD:
        rc = libTeer_zstd_decompress(target, rand_filename_target);
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

    if (rc != 0) {
        return rc;
    }

    if (source_used_mode == NOCOMPRESS && target_used_mode == NOCOMPRESS) {
        rc = libTeer_basic_merge(source, target);
    } else if (source_used_mode != NOCOMPRESS && target_used_mode == NOCOMPRESS) {
        rc = libTeer_basic_merge(rand_filename_source, target);   
    } else if (source_used_mode == NOCOMPRESS && target_used_mode != NOCOMPRESS) {
        rc = libTeer_basic_merge(source, rand_filename_target);
    } else if (source_used_mode != NOCOMPRESS && target_used_mode != NOCOMPRESS) {
        rc = libTeer_basic_merge(rand_filename_source, rand_filename_target);  
    }

    if (rc != 0) {
        return rc;
    }

    switch (target_used_mode)
    {
    case NOCOMPRESS:
        break;
    case GZIP:
        rc = libTeer_gzip_compress(rand_filename_target, target);
        break;
    case XZ:
        rc = libTeer_xz_compress(rand_filename_target, target, LIBTEER_DEFAULT_XZ_COMPRESSION_LEVEL);
        break;
    case BZIP2:
        rc = libTeer_bzip2_compress(rand_filename_target, target, LIBTEER_DEFAULT_BZIP2_COMPRESSION_LEVEL, LIBTEER_DEFAULT_BZIP2_WORK_FACTOR);
        break;
    case ZSTD:
        rc = libTeer_zstd_compress(rand_filename_target, target, LIBTEER_DEFAULT_ZSTD_COMPRESSION_LEVEL);
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