#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    int rc = 0;
    printf("merging foo.tar into src.tar...\n");
    rc = libTeer_basic_merge("foo.tar", "src.tar");
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("merging foo.tar.gz into src.tar...\n");
    rc = libTeer_merge("foo.tar.gz", "src.tar", AUTO_COMPRESSION, AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("merging src.tar into foo.tar.xz...\n");
    rc = libTeer_merge("src.tar", "foo.tar.xz", AUTO_COMPRESSION, AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("merging foo.tar.bz2 into src.tar...\n");
    rc = libTeer_merge("foo.tar.bz2", "src.tar", AUTO_COMPRESSION, AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("merging src.tar into foo.tar.zstd...\n");
    rc = libTeer_merge("src.tar", "foo.tar.zstd\n", AUTO_COMPRESSION, AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("done\n");
    return 0;
}