#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("merging foo.tar into src.tar...\n");
    libTeer_basic_merge("foo.tar", "src.tar");
    printf("done\n");
    return 0;
}