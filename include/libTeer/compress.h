#define LIBTEER_COMPRESSION_BUFFER_SIZE 16384
#define LIBTEER_DEFAULT_BZIP2_COMPRESSION_LEVEL 9
#define LIBTEER_DEFAULT_BZIP2_WORK_FACTOR 30
#define LIBTEER_DEFAULT_XZ_COMPRESSION_LEVEL 6
#define LIBTEER_DEFAULT_ZSTD_COMPRESSION_LEVEL 3

#ifndef LIBTEER_COMPRESS_H
#define LIBTEER_COMPRESS_H

// Functions

/*
decompresses infile to outfile.
*/
int libTeer_bzip2_decompress(const char *infile, const char *outfile);

/*
compresses infile to outfile.
*/
int libTeer_bzip2_compress(const char *infile, const char *outfile, int compression_level, int work_factor);

/*
decompresses infile to outfile.
*/
int libTeer_gzip_decompress(const char *infile, const char *outfile);

/*
compresses infile to outfile.
*/
int libTeer_gzip_compress(const char *infile, const char *outfile);

int libTeer_xz_decompress(const char *infile, const char *outfile);
int libTeer_xz_compress(const char *infile, const char *outfile, int compression_level);

/*
decompresses infile to outfile.
*/
int libTeer_zstd_decompress(const char *infile, const char *outfile);

/*
compresses infile to outfile.
*/
int libTeer_zstd_compress(const char *infile, const char *outfile, int compression_level);

/*
    Decompress and unpack a gzip compressed tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_gzip_unpack(const char *filename, const char *directory);

/*
    Decompress and unpack an xz/lzma compressed tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_xz_unpack(const char *filename, const char *directory);

/*
    Decompress and unpack a zstd compressed tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_zstd_unpack(const char *filename, const char *directory);

/*
    Decompress and unpack a bzip2 compressed tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
    If the library wasn't compiled with compression, it will always return -2 and do nothing.
*/
int libTeer_bzip2_unpack(const char *filename, const char *directory);

#endif