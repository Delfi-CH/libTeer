#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

LibTeer_Filelist *libTeer_basic_list_files(const char *archive_name) {
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

LibTeer_Filelist *libTeer_list_files(const char *archive_name, libTeer_Compression_Mode compression_mode) {
    return NULL;
}