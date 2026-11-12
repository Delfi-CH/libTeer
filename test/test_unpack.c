#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar...\n");
    libTeer_unpack("foo.tar", "/tmp", AUTO_COMPRESSION);
    printf("done\n");
    printf("unpacking foo.tar.gz...\n");
    libTeer_unpack("foo.tar.gz", "/tmp", AUTO_COMPRESSION);   
    printf("done\n"); 
    printf("unpacking foo.tar.xz...\n");
    libTeer_unpack("foo.tar.xz", "/tmp", AUTO_COMPRESSION);   
    printf("done\n"); 
    printf("unpacking foo.tar.bz2...\n");
    libTeer_unpack("foo.tar.bz2", "/tmp", AUTO_COMPRESSION); 
    printf("done\n"); 
    printf("unpacking foo.tar.zstd...\n");
    libTeer_unpack("foo.tar.zstd", "/tmp", AUTO_COMPRESSION);  
    printf("done\n"); 
    return 0;
}