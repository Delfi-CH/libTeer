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

struct ustar_headerlist 
{
    LibTeer_UStarHeader **headers;
    int length;
};


// Functions

void rand_str(char *dest, size_t length);
bool is_record_ustar_header(const char *record);
char *read_single_record(FILE *tarball);
void print_ustar_header_info(LibTeer_UStarHeader *header);
char *get_file_extension(char *filename);
LibTeer_Compression_Mode get_mode_from_filename(const char *filename);
void print_record(const char *record);
struct ustar_filebuf *file_to_ustar_buffer(const char *filename);
char *ustar_header_to_data(LibTeer_UStarHeader *header);
LibTeer_Filelist *get_directory_list(const char *directory);
struct ustar_headerlist *get_ustar_headers_of_file(const char *filename);
void free_ustar_headerlist(struct ustar_headerlist *list);
void free_directory_list(LibTeer_Filelist *list);
char *get_tmpdir_path();

#endif