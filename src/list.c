#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LibTeer_Filelist *libTeer_basic_list_files(const char *archive_name) {
    if (archive_name == NULL) return NULL;
    struct headerlist *list = get_headers_of_file(archive_name);
    if (list == NULL) {
        fprintf(stderr, "Failed to get list of headers!\n");
        return NULL;
    }

    LibTeer_Filelist *filellist = malloc(sizeof(LibTeer_Filelist));
    if (filellist == NULL) {
        free_headerlist(list);
        return NULL;
    }

    filellist->length = list->length;
    filellist->data = calloc(list->length, sizeof(char *));

    if (filellist->data == NULL && list->length > 0) {
        free(filellist);
        free_headerlist(list);
        return NULL;
    }

    for (int i = 0; i < list->length; i++) {
        char path[257];

        if (list->headers[i]->filename_prefix[0] != '\0') {
            snprintf(path, sizeof(path), "%s/%s",
                     list->headers[i]->filename_prefix,
                     list->headers[i]->file_path);
        } else {
            snprintf(path, sizeof(path), "%s",
                     list->headers[i]->file_path);
        }

        filellist->data[i] = strdup(path);

        if (filellist->data[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(filellist->data[j]);
            }
            free(filellist->data);
            free(filellist);
            free_headerlist(list);
            return NULL;
        }
    }

    free_headerlist(list);
    return filellist;
}

LibTeer_Filelist *libTeer_list_files(const char *archive_name, LibTeer_Compression_Mode compression_mode) {
    if (archive_name == NULL) return NULL;
    
    LibTeer_Compression_Mode used_mode = AUTO_COMPRESSION;
    if (compression_mode < 0 || compression_mode > COMPRESSION_MODE_SIZE) {
        fprintf(stderr, "Compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE -1);
        return NULL;
    }

    if (compression_mode == 0) {
        used_mode = get_mode_from_filename(archive_name);
        if (used_mode == -1) {
            return NULL;
        }
    } else {
        used_mode = compression_mode;
    }

    char rand_filename[1024];
    char str[] = {[41] = '\1'};
    rand_str(str, sizeof str - 1);
    int rc = snprintf(rand_filename, 1024, "/tmp/%s", str);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not generate filename\n");
        return NULL;
    }

    bool error = false;

    switch (used_mode)
    {
    case NOCOMPRESS:
        return libTeer_basic_list_files(archive_name);
    case GZIP:
        rc = libTeer_gzip_decompress(archive_name, rand_filename);
        if (rc != 0) return NULL;
        break;
    case XZ:
        rc = libTeer_xz_decompress(archive_name, rand_filename);
        if (rc != 0) return NULL;
        break;
    case BZIP2:
        rc = libTeer_bzip2_decompress(archive_name, rand_filename);
        if (rc != 0) return NULL;
        break;
    case ZSTD:
        rc = libTeer_zstd_decompress(archive_name, rand_filename);
        if (rc != 0) return NULL;
        break;
    case AUTO_COMPRESSION:
        error = true;
        fprintf(stderr, "An unexpected error occured!\n");
        break;
    case COMPRESSION_MODE_SIZE:
        error = true;
        fprintf(stderr, "An unexpected error occured!\n");
        break;
    }

    if (error) return NULL;

    if (used_mode != NOCOMPRESS) {
        LibTeer_Filelist *list =  libTeer_basic_list_files(rand_filename);
        remove(rand_filename);
        return list;
    }

    return NULL;
}