#include "../include/libTeer/tar.h"
#include <stdio.h>

int main() {
    libTeer_unpack("foo.tar", "/tmp");
    return 0;
}