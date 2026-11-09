#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include <stdio.h>

int libTeer_append(const char *archive_name, const char *filename, libTeer_Compression_Mode compression_mode) {
    struct ustar_filebuf *fbuf = file_to_ustar_buffer(filename);    
    print_ustar_header_info(&fbuf->header);
    for (int i = 0; i < fbuf->record_count; i++) {
        print_record((const char *) &fbuf->records[i].data);
    }
    if (fbuf == NULL) {
        fprintf(stderr, "Failed to convert %s into a buffer!\n", filename);
    }
    return 0;
}