#include "../include/libTeer/tar.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar.gz...\n");
    libTeer_gzip_unpack("foo.tar.gz", "/tmp");   
    printf("done\n"); 
    return 0;
}