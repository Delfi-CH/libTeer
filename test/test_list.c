#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("listing files of foo.tar\n");
    LibTeer_Filelist *list = libTeer_basic_list_files("foo.tar");
    if (list == NULL) {
        printf("failed to get filellist\n");
        return 1;
    }
    for (int i = 0; i < list->length; i++) {
        printf("%s\n", list->data[i]);
    }
    libTeer_free_directory_list(list);
    printf("done\n");
    return 0;
}