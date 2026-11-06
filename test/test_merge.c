#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("merging foo.tar into src.tar...\n");
    libTeer_basic_merge("foo.tar", "src.tar");
    printf("done\n");
    printf("merging foo.tar.gz into src.tar...\n");
    libTeer_merge("foo.tar.gz", "src.tar", AUTO_COMPRESSION, AUTO_COMPRESSION);
    printf("done\n");
    printf("merging src.tar into foo.tar.xz...\n");
    libTeer_merge("src.tar", "foo.tar.xz", AUTO_COMPRESSION, AUTO_COMPRESSION);
    printf("done\n");
    printf("merging foo.tar.bz2 into src.tar...\n");
    libTeer_merge("foo.tar.bz2", "src.tar", AUTO_COMPRESSION, AUTO_COMPRESSION);
    printf("done\n");
    printf("merging src.tar into foo.tar.zstd...\n");
    libTeer_merge("src.tar", "foo.tar.zstd", AUTO_COMPRESSION, AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}