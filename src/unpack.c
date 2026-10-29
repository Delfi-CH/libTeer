#include "../include/libTeer/tar.h"
#include "../include/libTeer/private.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

int libTeer_unpack(const char *filename, const char *directory)
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
            char path[sizeof(header->file_path) + 1];
            memcpy(path, header->file_path, sizeof(header->file_path));
            path[sizeof(header->file_path)] = '\0';
            //print_header_info(header);

            // is it a directory?
            if (path[strlen(header->file_path) - 1] == '/')
            {
                struct stat path_st;
                stat(path, &path_st);
                if (S_ISDIR(path_st.st_mode)) {
                    continue;
                } else {
                    if(mkdir(path, strtol(header->file_mode, NULL, 8))) {
                        fprintf(stderr, "Failed to make directory %s!\n", path);
                        return 1;
                    };
                }
            } else {
                if (current_file != NULL) {
                    if(fclose(current_file)) {
                        fprintf(stderr, "Failed to close file %s!\n", current_filename);
                        return 1;
                    }
                }
                if (snprintf(current_filename, sizeof(char) * 255, "%s", path) < 0) {
                    fprintf(stderr, "Failed to write filename %s!\n", path);
                        return 1;
                }  
                current_file = fopen(current_filename, "w");
                if (current_file == NULL) {
                    fprintf(stderr, "Failed to open file %s", current_filename);
                    return 1;
                }
            }
        }
        else
        {
            if (current_file == NULL) {
                fprintf(stderr, "Attempted to write to a null pointer! Aborting...\n");
                return 1;
            }
            for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
            {
                if (buf[i] != '\0') {
                    fputc(buf[i], current_file);
                }
            }
        }
    }

    chdir(initial_working_directory);
    free(initial_working_directory);
    free(buf);
    free(header);
    return 0;
}