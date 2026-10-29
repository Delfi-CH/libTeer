#include <libTeer/tar.h>
#include <stdio.h>

int main(int argc, char **argv) {

    char *program_name = argv[0];
    char *tarfile = argv[1];
    char *output_directory = argv[2];

    if (argc < 3) {
        printf("usage: %s <input.tar> <output>\n", program_name);
        return 1;
    }

    printf("unpacking %s to %s\n", tarfile, output_directory);
    libTeer_unpack(tarfile, output_directory);
    printf("done\n");
    return 0;
}