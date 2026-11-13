#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    int rc = 0;
    printf("appending LICENSE to src2.tar\n");
    rc = libTeer_append("src2.tar", "LICENSE", AUTO_COMPRESSION);
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