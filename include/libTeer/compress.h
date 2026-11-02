#define LIBTEER_COMPRESSION_BUFFER_SIZE 16384

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

#endif