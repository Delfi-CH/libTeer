#ifndef LIBTEER_PRIVATE_H
#define LIBTEER_PRIVATE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libTeer.h"

void rand_str(char *dest, size_t length);
bool is_record_ustar_header(const char *record);
char *read_single_record(FILE *tarball);
void print_ustar_header_info(LibTeer_UStarHeader *header);
char *get_file_extension(const char *filename);
libTeer_Compression_Mode get_mode_from_filename(const char *filename);
void print_record(const char *record);

#endif