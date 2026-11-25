#define LIBTEER_RECORD_LENGTH 512
#define LIBTEER_DEFAULT_UID 1000
#define LIBTEER_DEFAULT_GID 1000

#ifndef LIBTEER_VERSION_STRING
#define LIBTEER_VERSION_STRING "unknown"
#endif

#ifndef LIBTEER_H
#define LIBTEER_H

// Includes

#include <stddef.h>

// Enums

typedef enum mode
{
    AUTO_COMPRESSION = 0,
    NOCOMPRESS = 1,
    GZIP = 2,
    ZLIB = 2,
    XZ = 3,
    LZMA = 3,
    BZIP2 = 4,
    ZSTD = 5,
    COMPRESSION_MODE_SIZE
} LibTeer_Compression_Mode;

typedef enum format
{
    AUTO_FORMAT = 0,
    POSIX = 1, // Not yet implemented
    USTAR = 2,
    PAX = 3, // Not yet implemented
    GNU = 4, // Not yet implemented
    OLDGNU = 5, // Not yet implemented
} LibTeer_File_Format;

// Data structures

typedef struct
{
    char data[LIBTEER_RECORD_LENGTH];
} LibTeer_Record;

/*
    Data structures taken from
    https://github.com/tsoding/tarballs/
    which is MIT-licensed
*/
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

typedef union header
{
    LibTeer_StandardHeader *posix;
    LibTeer_UStarHeader *ustar;
} LibTeer_Header_Union;


typedef struct generic_header {
    LibTeer_File_Format format;
    LibTeer_Header_Union as;
} LibTeer_Header;

typedef struct filellist
{
    char **data;
    int length;
} LibTeer_Filelist;

typedef struct file {
    LibTeer_Header *header;
    LibTeer_Record *data_records;
    size_t data_record_count;
} LibTeer_FileData;

typedef struct archive {
    LibTeer_File_Format format;
    LibTeer_Compression_Mode compression_mode;
    LibTeer_FileData *files;
    size_t file_count;
} LibTeer_Archive;

// Functions

/*
    Get the current version string, returns a semver-like format for releases eg. (0.1.1) or the git commit hash.
*/
char *libTeer_get_version();

/*
    Get a list of files in an archive.
    The resulting LibTeer_Filelist has to be freed with 'free_directory_list(LibTeer_Filelist *list)'
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the filename.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
LibTeer_Filelist *libTeer_list_files(const char *archive_name, LibTeer_Compression_Mode compression_mode);

/*
    Get a list of files in an archive.
    The resulting LibTeer_Filelist has to be freed with 'free_directory_list(LibTeer_Filelist *list)'
    Returns 0 on success and anything else on failiure.
*/
LibTeer_Filelist *libTeer_basic_list_files(const char *archive_name);

/*
    Append file to archive.
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the filename.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
int libTeer_append(const char *archive_name, const char *filename, LibTeer_Compression_Mode compression_mode);

/*
    Append file to archive.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_basic_append(const char *archive_name, const char *filename);

/*
    Merge the contents of source into target.
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the filename.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
int libTeer_merge(const char *source, const char *target, LibTeer_Compression_Mode source_compression_mode, LibTeer_Compression_Mode target_compression_mode);

/*
    Merge the contents of source into target.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_basic_merge(const char *source, const char *target);

/*
    Create an archive from a directory.
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the output name.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
int libTeer_create(const char *directory, const char *output, LibTeer_Compression_Mode compression_mode);

/*
    Create an archive from a directory.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_basic_create(const char *directory, const char *output);

/*
    Unpack a tar archive into a directory.
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the filename.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
int libTeer_unpack(const char *filename, const char *directory, LibTeer_Compression_Mode compression_mode);

/*
    Unpack an uncompressed tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_basic_unpack(const char *filename, const char *directory);

/*
    Extract a specific file from an archive to outdir.
    If outdir is null, the current working directory is used instead.
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the filename.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
int libTeer_filextract(const char *archive_name, const char *filename, const char *outdir, LibTeer_Compression_Mode compression_mode);

/*
    Extract a specific file from an archive to outdir.
    If outdir is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_basic_filextract(const char *archive_name, const char *filename, const char *outdir);

/*
    Delete a specific file from an archive.
    If compression_mode is set to 0/AUTO_COMPRESSION, the mode will be determined by the extension of the filename.
    Returns 0 on success, -2 on unsupported compression mode / file type and anything else on failiure.
*/
int libTeer_delete(const char *archive_name, const char *filename, LibTeer_Compression_Mode compression_mode);


/*
    Delete a specific file from an archive.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_basic_delete(const char *archive_name, const char *filename);

/*
    Free a filelist datastructure
*/
void libTeer_free_directory_list(LibTeer_Filelist *list);

#endif