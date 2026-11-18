#include "../include/libTeer.h"
#include <stdio.h>

int main() {
    printf("listing files of foo.tar\n");
    LibTeer_Filelist *list = libTeer_list_files("foo.tar", AUTO_COMPRESSION);
    if (list == NULL) {
        printf("failed to get filellist\n");
        return 1;
    }
    for (int i = 0; i < list->length; i++) {
        printf("%s\n", list->data[i]);
    }
    libTeer_free_directory_list(list);
    printf("listing files of foo.tar.gz\n");

    LibTeer_Filelist *list2 = libTeer_list_files("foo.tar.gz", AUTO_COMPRESSION);
    if (list2 == NULL) {
        printf("failed to get filellist\n");
        return 1;
    }
    for (int i = 0; i < list2->length; i++) {
        printf("%s\n", list2->data[i]);
    }
    libTeer_free_directory_list(list2);

    printf("listing files of foo.tar.bz2\n");
    LibTeer_Filelist *list3 = libTeer_list_files("foo.tar.bz2", AUTO_COMPRESSION);
    if (list3 == NULL) {
        printf("failed to get filellist\n");
        return 1;
    }
    for (int i = 0; i < list3->length; i++) {
        printf("%s\n", list3->data[i]);
    }
    libTeer_free_directory_list(list3);

    printf("listing files of foo.tar.xz\n");
    LibTeer_Filelist *list4 = libTeer_list_files("foo.tar.xz", AUTO_COMPRESSION);
    if (list4 == NULL) {
        printf("failed to get filellist\n");
        return 1;
    }
    for (int i = 0; i < list4->length; i++) {
        printf("%s\n", list4->data[i]);
    }
    libTeer_free_directory_list(list4);

    printf("listing files of foo.tar.zstd\n");
    LibTeer_Filelist *list5 = libTeer_list_files("foo.tar.zstd", AUTO_COMPRESSION);
    if (list5 == NULL) {
        printf("failed to get filellist\n");
        return 1;
    }
    for (int i = 0; i < list5->length; i++) {
        printf("%s\n", list5->data[i]);
    }
    libTeer_free_directory_list(list5);

    printf("done\n");
    return 0;
}