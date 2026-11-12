#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("creating test.tar...\n");
    libTeer_create("test", "test.tar", AUTO_COMPRESSION);
    printf("creating test.tar.gz...\n");
    libTeer_create("test", "test.tar.gz", AUTO_COMPRESSION);
    printf("creating test.tar.xz...\n");
    libTeer_create("test", "test.tar.xz", AUTO_COMPRESSION);
    printf("creating test.tar.bz2...\n");
    libTeer_create("test", "test.tar.bz2", AUTO_COMPRESSION);
    printf("creating test.tar.zstd  ...\n");
    libTeer_create("test", "test.tar.zstd", AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}