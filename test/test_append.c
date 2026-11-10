#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("appending LICENSE to src2.tar\n");
    libTeer_append("src2.tar", "LICENSE", AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}