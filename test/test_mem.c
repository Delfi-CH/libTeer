#include "../include/libTeer.h"
#include "../include/libTeer/compress.h"
#include "../include/libTeer/private.h"
#include <stdio.h>

int main() {
    printf("Reading foo.tar.bz2 into memory...\n");
    struct decompressed_buffer *buf = libTeer_bzip2_decompress_buffer("foo.tar.bz2");
    if (buf == NULL) {
        eprintf("Failed to read from buffer!\n");
        return 1;
    }
    for (int i = 0; i < buf->lentgh; i++) {
        printf("%c", buf->data[i]);
    }
    free_decompressed_buffer(buf);
    printf("Reading foo.tar into memory...\n");
    struct decompressed_buffer *buf2 = load_normal_file_to_buffer("foo.tar");
    if (buf2 == NULL) {
        eprintf("Failed to read from buffer!\n");
        return 1;
    }
    for (int i = 0; i < buf2->lentgh; i++) {
        printf("%c", buf2->data[i]);
    }
    free_decompressed_buffer(buf2);
    printf("\ndone\n");
    return 0;
}