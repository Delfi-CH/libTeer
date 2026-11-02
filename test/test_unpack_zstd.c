#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar.zstd...\n");
    libTeer_zstd_unpack("foo.tar.zstd", "/tmp");   
    printf("done\n"); 
    return 0;
}