#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("appending LICENSE to src.tar\n");
    libTeer_append("src.tar", "src", AUTO_COMPRESSION);
    return 0;
}