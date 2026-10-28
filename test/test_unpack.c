#include "../include/libTeer/tar.h"
#include <stdio.h>

int main() {
    printf("unpacking foo.tar...\n");
    libTeer_unpack("foo.tar", "/tmp");
    printf("done\n");
    return 0;
}