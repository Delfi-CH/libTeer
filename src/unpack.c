#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int libTeer_basic_unpack(const char *filename, const char *directory)
{
    if (filename == NULL) {
        fprintf(stderr, "Filename is null! Aborting...\n");
        return 1;
    }

    char *initial_working_directory = malloc(4096 * sizeof(char));
    getcwd(initial_working_directory, 4096);
    if (directory == NULL) {
        directory = initial_working_directory;
    }
    struct stat st;
    if (stat(filename, &st))
    {
        fprintf(stderr, "Failed to stat file %s!\n", filename);
        return 1;
    }

    if (st.st_size % LIBTEER_RECORD_LENGTH)
    {
        fprintf(stderr, "Not a tar file %s, filelength not divisible by %d!\n", filename, LIBTEER_RECORD_LENGTH);
        return 1;
    }

    FILE *tarball = fopen(filename, "r");
    if (tarball == NULL) {
        fprintf(stderr, "Failed to open file %s!\n", filename);
        return 1;
    }

    if(chdir(directory)) {
        fprintf(stderr, "Failed to change directory to %s!\n", directory);
        return 1;
    }

    char *buf = NULL;
    FILE *current_file = NULL;
    char current_filename[255] = {0};
    LibTeer_UStarHeader *header = malloc(sizeof(LibTeer_UStarHeader));
    if (header == NULL) {
        fprintf(stderr, "Failed to allocate memory!\n");
        return 2;
    }

    for (int i = 0; i < st.st_size / LIBTEER_RECORD_LENGTH; i++)
    {
        buf = read_single_record(tarball);
        if (i == 0 && !is_record_ustar_header(buf)) {
            fprintf(stderr, "File %s is not a ustar tarball!\n", filename);
            return 1;
        }
        if (buf == NULL) {
            fprintf(stderr, "Failed to read buffer!\n");
            return 2;
        }

        if (is_record_ustar_header(buf))
        {   
            memcpy(header, buf, sizeof(*header));
            char path[257];
            if (header->filename_prefix[0] != '\0') {
                snprintf(path, sizeof(path), "%s/%s",header->filename_prefix, header->file_path);
            } else {
                snprintf(path, sizeof(path), "%s", header->file_path);
            }

            // is it a directory?
            if (path[strlen(header->file_path) - 1] == '/')
            {
                struct stat path_st;
                stat(path, &path_st);
                if (S_ISDIR(path_st.st_mode)) {
                    free(buf);
                    continue;
                } else {
                    #ifdef _WIN32
                    if(mkdir(path)) {
                        fprintf(stderr, "Failed to make directory %s!\n", path);
                        free(initial_working_directory);
                        free(buf);
                        free(header);
                        return 1;
                    };
                    #else
                    if(mkdir(path, strtol(header->file_mode, NULL, 8))) {
                        fprintf(stderr, "Failed to make directory %s!\n", path);
                        free(initial_working_directory);
                        free(buf);
                        free(header);
                        return 1;
                    };
                    #endif
                }
            } else {
                if (current_file != NULL) {
                    if(fclose(current_file)) {
                        fprintf(stderr, "Failed to close file %s!\n", current_filename);
                        free(initial_working_directory);
                        free(buf);
                        free(header);
                        return 1;
                    }
                }
                if (snprintf(current_filename, sizeof(char) * 255, "%s", path) < 0) {
                    fprintf(stderr, "Failed to write filename %s!\n", path);
                    free(initial_working_directory);
                    free(buf);
                    free(header);
                    return 1;
                }  
                current_file = fopen(current_filename, "w");
                if (current_file == NULL) {
                    fprintf(stderr, "Failed to open file %s", current_filename);
                    free(initial_working_directory);
                    free(buf);
                    free(header);
                    return 1;
                }
            }
        }
        else
        {
            if (current_file == NULL) {
                fprintf(stderr, "Attempted to write to a null pointer! Aborting...\n");
                free(initial_working_directory);
                free(buf);
                free(header);
                return 1;
            }
            for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
            {
                if (buf[i] != '\0') {
                    fputc(buf[i], current_file);
                }
            }
        }
        free(buf);
    }

    chdir(initial_working_directory);
    free(initial_working_directory);
    free(header);
    return 0;
}

int libTeer_unpack(const char *filename, const char *directory, libTeer_Compression_Mode compression_mode) {
    int rc = 0;
    libTeer_Compression_Mode used_mode = AUTO_COMPRESSION;
    if (compression_mode < 0 || compression_mode > COMPRESSION_MODE_SIZE) {
        fprintf(stderr, "Compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE -1);
        rc = 1;
        return rc;
    }

    if (compression_mode == 0) {
        used_mode = get_mode_from_filename(filename);
        if (used_mode == -1) {
            rc = -2;
            return rc;
        }
    } else {
        used_mode = compression_mode;
    }


    switch (used_mode)
    {
    case NOCOMPRESS:
        rc = libTeer_basic_unpack(filename, directory);
        break;
    case GZIP:
        rc = libTeer_gzip_unpack(filename, directory);
        break;
    case XZ:
        rc = libTeer_xz_unpack(filename, directory);
        break;
    case BZIP2:
        rc = libTeer_bzip2_unpack(filename, directory);
        break;
    case ZSTD:
        rc = libTeer_zstd_unpack(filename, directory);
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