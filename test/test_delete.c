#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("deleting src/dirtraverse.c from src2.tar...\n");
    libTeer_delete("src2.tar", "src/dirtraverse.c", AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}