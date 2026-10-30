#include "../include/libTeer/tar.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar.bz2...\n");
    libTeer_bzip2_unpack("foo.tar.bz2", "/tmp");   
    printf("done\n"); 
    return 0;
}