#define LIBTEER_RECORD_LENGTH 512

#ifndef LIBTEER_VERSION_STRING
#define LIBTEER_VERSION_STRING "unknown"
#endif

#ifndef LIBTEER_H
#define LIBTEER_H

// Data structures

/*
    Data structures taken from
    https://github.com/tsoding/tarballs/
    which is MIT-licensed
*/

typedef struct
{
    char data[LIBTEER_RECORD_LENGTH];
} LibTeer_Record;

typedef struct __attribute__((packed))
{
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

typedef struct __attribute__((packed))
{
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
    char padding[12];
} LibTeer_UStarHeader;

// Enums

typedef enum mode
{
    NOCOMPRESS = 0,
    GZIP = 1,
    ZLIB = 1,
    XZ = 2,
    LZMA = 2,
    BZIP2 = 3,
    ZSTD = 4,
} libTeer_Compression_Mode;

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

/*
    Get the current version string, returns a semver-like format for releases eg. (0.1.1) or the git commit hash.
*/
char *libTeer_get_version();

/*
    Decompress and unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_gzip_unpack(const char *filename, const char *directory);

/*
Decompress and unpack a tar archive into a directory.
If the directory is null, the current working directory is used instead.
Returns 0 on success and anything else on failiure.
If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_xz_unpack(const char *filename, const char *directory);

/*
    Decompress and unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_zstd_unpack(const char *filename, const char *directory);

/*
    Decompress and unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_bzip2_unpack(const char *filename, const char *directory);

#endif