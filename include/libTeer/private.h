#ifndef LIBTEER_PRIVATE_H
#define LIBTEER_PRIVATE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include "../libTeer.h"

// Structs

struct ustar_filebuf {
    LibTeer_UStarHeader *header;
    LibTeer_Record *records;
    int record_count;
};

struct filellist
{
    char **data;
    int length;
};


// Functions

void rand_str(char *dest, size_t length);
bool is_record_ustar_header(const char *record);
char *read_single_record(FILE *tarball);
void print_ustar_header_info(LibTeer_UStarHeader *header);
char *get_file_extension(char *filename);
libTeer_Compression_Mode get_mode_from_filename(const char *filename);
void print_record(const char *record);
struct ustar_filebuf *file_to_ustar_buffer(const char *filename);
char *ustar_header_to_data(LibTeer_UStarHeader *header);
struct filellist *get_directory_list(const char *directory);
void free_directory_list(struct filellist *list);

#endif