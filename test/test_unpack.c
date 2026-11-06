#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar...\n");
    libTeer_unpack("foo.tar", "/tmp", AUTO_COMPRESSION);
    printf("done\n");
    printf("unpacking foo.tar.gz...\n");
    libTeer_gzip_unpack("foo.tar.gz", "/tmp");   
    printf("done\n"); 
    printf("unpacking foo.tar.xz...\n");
    libTeer_xz_unpack("foo.tar.xz", "/tmp");   
    printf("done\n"); 
    printf("unpacking foo.tar.bz2...\n");
    libTeer_bzip2_unpack("foo.tar.bz2", "/tmp");   
    printf("done\n"); 
    printf("unpacking foo.tar.zstd...\n");
    libTeer_zstd_unpack("foo.tar.zstd", "/tmp");   
    printf("done\n"); 
    return 0;
}