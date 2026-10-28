#define RECORD_LENGTH 512

#ifndef LIBTEER_TAR_H
#define LIBTEER_TAR_H

// Data structures

typedef struct {
    char data[RECORD_LENGTH];
} LibTeer_Record;

typedef struct __attribute__((packed)) {
    char file_path[100];
    char file_mode[8];
    char owner_id[8];
    char group_id[8];
    char file_size[12];
    char modif_time[12];
    char checksum[8];
    char link;
    char name_linked_file[100];
    char padding[255];
} LibTeer_StandardHeader;

typedef struct __attribute__((packed)) {
    char file_path[100];
    char file_mode[8];
    char owner_id[8];
    char group_id[8];
    char file_size[12];
    char modif_time[12];
    char checksum[8];
    char link;
    char name_linked_file[100];
    char indicator[6];
    char version[2];
    char username[32];
    char groupname[32];
    char device_major_number[8];
    char device_minor_number[8];
    char filename_prefix[155];
    char padding[24];
} LibTeer_UStarHeader;

// Functions

void libTeer_append();
void libTeer_merge();
void libTeer_create();

/*
    Unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_unpack(const char *filename, const char *directory);

#endif