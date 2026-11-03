#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar...\n");
    libTeer_unpack("foo.tar", "/tmp", AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}