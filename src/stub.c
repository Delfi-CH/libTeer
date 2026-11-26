#include "../include/libTeer.h"
#include "../include/libTeer/compress.h"

int libTeer_gzip_decompress_file(const char *infile, const char *outfile) {
    return -2;
}
int libTeer_gzip_compress_file(const char *infile, const char *outfile) {
    return -2;
}

int libTeer_xz_decompress_file(const char *infile, const char *outfile) {
    return -2;
}
int libTeer_xz_compress_file(const char *infile, const char *outfile, int compression_level) {
    return -2;
}

int libTeer_zstd_decompress_file(const char *infile, const char *outfile) {
    return -2;
}
int libTeer_zstd_compress_file(const char *infile, const char *outfile, int compression_level) {
    return -2;
}

int libTeer_bzip2_decompress_file(const char *infile, const char *outfile) {
    return -2;
}
int libTeer_bzip2_compress_file(const char *infile, const char *outfile, int compression_level, int work_factor) {
    return -2;
}
struct decompressed_buffer *libTeer_bzip2_decompress_buffer(const char *filename) {
    return NULL;
}