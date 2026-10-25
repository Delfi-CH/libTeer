#ifndef LIBTAR_TAR_H
#define LIBTAR_TAR_H

// Data structures

typedef struct {
    char data[512];
} Record;

typedef struct __attribute__((packed)) {
    char file_path[100];
    char file_mode[8];
    char owner_id[8];
    char group_id[8];
    char file_size[12];
    char modif_type[12];
    char checksum[8];
    char link;
    char name_linked_file[100];
    char padding[255];
} libTar_StandardHeader;

typedef struct __attribute__((packed)) {
    char file_path[100];
    char file_mode[8];
    char owner_id[8];
    char group_id[8];
    char file_size[12];
    char modif_type[12];
    char checksum[8];
    char type_flag;
    char name_linked_file[100];
    char indicator[6];
    char version[2];
    char username[32];
    char groupname[32];
    char device_major_number[8];
    char device_minor_number[8];
    char filename_prefix[155];
    char padding[24];
} libTar_UStarHeader;

// Functions

void libtar_append();
void libtar_merge();
void libtar_create();
void libtar_unpack();

#endif