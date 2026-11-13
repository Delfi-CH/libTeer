#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    int rc = 0;
    printf("creating test.tar...\n");
    rc = libTeer_create("test", "test.tar", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("creating test.tar.gz...\n");
    rc = libTeer_create("test", "test.tar.gz", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("creating test.tar.xz...\n");
    rc = libTeer_create("test", "test.tar.xz", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("creating test.tar.bz2...\n");
    rc = libTeer_create("test", "test.tar.bz2", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("creating test.tar.zstd...\n");
    rc = libTeer_create("test", "test.tar.zstd\n", AUTO_COMPRESSION);
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