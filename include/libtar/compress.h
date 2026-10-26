#ifndef LIBTAR_COMPRESS_H
#define LIBTAR_COMPRESS_H

void libtar_bzip2_decompress();
void libtar_bzip2_compress();

void libtar_gzip_decompress();
void libtar_gzip_compress();

void libtar_xz_decompress();
void libtar_xz_compress();

void libtar_zstd_decompress();
void libtar_zstd_compress();

#endif