#ifndef LIBTEER_COMPRESS_H
#define LIBTEER_COMPRESS_H

void libTeer_bzip2_decompress();
void libTeer_bzip2_compress();

void libTeer_gzip_decompress();
void libTeer_gzip_compress();

void libTeer_xz_decompress();
void libTeer_xz_compress();

void libTeer_zstd_decompress();
void libTeer_zstd_compress();

#endif