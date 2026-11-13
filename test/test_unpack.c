#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    int rc = 0;
    printf("unpacking foo.tar...\n");
    rc = libTeer_unpack("foo.tar", "/tmp", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("unpacking foo.tar.gz...\n");
    rc = libTeer_unpack("foo.tar.gz", "/tmp", AUTO_COMPRESSION); 
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }  
    printf("unpacking foo.tar.xz...\n");
    rc = libTeer_unpack("foo.tar.xz", "/tmp", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    } 
    printf("unpacking foo.tar.bz2...\n");
    rc = libTeer_unpack("foo.tar.bz2", "/tmp", AUTO_COMPRESSION);
    if (rc == -2) {
        printf("compression mode not supported\n");
        return 0;
    } else if (rc != 0) {
        printf("something went wrong\n");
        return 1;
    }
    printf("unpacking foo.tar.zstd...\n");
    rc = libTeer_unpack("foo.tar.zstd\n", "/tmp", AUTO_COMPRESSION);
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