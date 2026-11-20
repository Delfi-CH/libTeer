#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("extracting src/stub.c from src2.tar...\n");
    libTeer_filextract("src2.tar", "src/stub.c", "/tmp", AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}