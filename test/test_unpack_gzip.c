#include "../include/libTeer/tar.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>

int main() {
    printf("unpacking /tmp/minowebd-0.8.0-amd64.tar.gz...\n");
    libTeer_gzip_unpack("/tmp/minowebd-0.8.0-amd64.tar.gz", "/tmp");   
    printf("done\n"); 
    return 0;
}