#include "../include/libTeer/tar.h"
#include "../include/libTeer/compress.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar.xz...\n");
    libTeer_xz_unpack("foo.tar.xz", "/tmp");   
    printf("done\n"); 
    return 0;
}