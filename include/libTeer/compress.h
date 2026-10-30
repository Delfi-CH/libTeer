#define LIBTEER_COMPRESSION_BUFFER_SIZE 16384

#ifndef LIBTEER_COMPRESS_H
#define LIBTEER_COMPRESS_H

// Functions

int libTeer_bzip2_decompress(const char *infile, const char *outfile);
int libTeer_bzip2_compress(const char *infile, const char *outfile, int compression_level, int work_factor);

/*
    Decompress and unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_bzip2_unpack(const char *filename, const char *directory);

/*
decompresses infile to outfile.
*/
int libTeer_gzip_decompress(const char *infile, const char *outfile);

/*
compresses infile to outfile.
*/
int libTeer_gzip_compress(const char *infile, const char *outfile);

/*
    Decompress and unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_gzip_unpack(const char *filename, const char *directory);

void libTeer_xz_decompress();
void libTeer_xz_compress();

/*
decompresses infile to outfile.
*/
int libTeer_zstd_decompress(const char *infile, const char *outfile);

/*
compresses infile to outfile.
*/
int libTeer_zstd_compress(const char *infile, const char *outfile, int compression_level);

/*
    Decompress and unpack a tar archive into a directory.
    If the directory is null, the current working directory is used instead.
    Returns 0 on success and anything else on failiure.
*/
int libTeer_zstd_unpack(const char *filename, const char *directory);

#endif