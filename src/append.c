#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include <stdio.h>

int libTeer_basic_append(const char *archive_name, const char *filename) {
    struct ustar_filebuf *fbuf = file_to_ustar_buffer(filename); 
    if (fbuf == NULL) {
        fprintf(stderr, "Failed to convert %s into a buffer!\n", filename);
        return 1;
    } 
    if (fbuf->records != NULL) {
        free(fbuf->records);
    }
    free(fbuf->header);
    free(fbuf);
    
    return 0;
}

int libTeer_append(const char *archive_name, const char *filename, libTeer_Compression_Mode compression_mode) {
    return 0;
} 