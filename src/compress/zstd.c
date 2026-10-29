#include "../../include/libTeer/tar.h"
#include "../../include/libTeer/compress.h"
#include "../../include/libTeer/private.h"
#include <zstd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>

int libTeer_zstd_decompress(const char *infile, const char *outfile) {
    FILE *in = fopen(infile, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", infile);
        return 1;
    }

    FILE *out = fopen(outfile, "wb");
    if (out == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", outfile);
        fclose(in);
        return 1;
    }

    ZSTD_DStream *stream = ZSTD_createDStream();
    if (stream == NULL) {
        fprintf(stderr, "Failed to create zdstream \n");
        fclose(in);
        fclose(out);
        return 1;
    }

    size_t ret = ZSTD_initDStream(stream);
    if (ZSTD_isError(ret)) {
        fprintf(stderr, "ZSTD_initDStream: %s\n", ZSTD_getErrorName(ret));
        ZSTD_freeDStream(stream);
        fclose(in);
        fclose(out);
        return 1;
    } 

    char in_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];
    char out_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];

    size_t remaining = 1;
    int result = 0;
    size_t read;

    while ((read = fread(in_buf, 1, sizeof(in_buf), in)) > 0) {
        ZSTD_inBuffer input = {in_buf, read, 0};
        while (input.pos < input.size) {
            ZSTD_outBuffer output = {out_buf, read, 0};

            remaining = ZSTD_decompressStream(stream, &output, &input);

            if (ZSTD_isError(remaining)) {
                fprintf(stderr, "Decompression error: %s\n", ZSTD_getErrorName(remaining));
                result = 1;
                break;
            }

            if (fwrite(out_buf, 1, output.pos, out) != output.pos) {
                fprintf(stderr, "Writing failed\n");
                result = 1;
                break;
            }

            if (output.pos == 0 &&
                input.pos == 0 &&
                input.size > 0) {
                fprintf(stderr, "Decompression stalled\n");
                result = 1;
                break;
            }
        }

        if (result) {
            break;
        }
    }

    if (!result && remaining != 0) {
        fprintf(stderr, "Incomplete or truncated zstd frame\n");
        result = 1;
    }

    ZSTD_freeDStream(stream);

    if (fclose(in) != 0)
    {
        fprintf(stderr, "Failed to close input file %s\n", infile);
        result = 1;
    }

    if (fclose(out) != 0)
    {
        fprintf(stderr, "Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (result) {
        remove(outfile);
    }

    return result;
}

int libTeer_zstd_compress(const char *infile, const char *outfile, int compression_level) {
    if (compression_level < ZSTD_minCLevel() || compression_level > ZSTD_maxCLevel()) {
        fprintf(stderr, "Compression level to large! Use a value from %d to %d\n", ZSTD_minCLevel(), ZSTD_maxCLevel());
        return -1;
    }

    FILE *in = fopen(infile, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", infile);
        return 1;
    }

    FILE *out = fopen(outfile, "wb");
    if (out == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", outfile);
        fclose(in);
        return 1;
    }

    ZSTD_CStream *stream = ZSTD_createCStream();
    if (stream == NULL) {
        fprintf(stderr, "Failed to create zdstream \n");
        fclose(in);
        fclose(out);
        return 1;
    }

    size_t ret = ZSTD_initCStream(stream, compression_level);
    if (ZSTD_isError(ret)) {
        fprintf(stderr, "ZSTD_initDStream: %s\n", ZSTD_getErrorName(ret));
        ZSTD_freeCStream(stream);
        fclose(in);
        fclose(out);
        return 1;
    } 

    char in_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];
    char out_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];

    size_t remaining = 1;
    int result = 0;
    size_t read;

    while ((read = fread(in_buf, 1, sizeof(in_buf), in)) > 0) {
        ZSTD_inBuffer input = {in_buf, read, 0};
        while (input.pos < input.size) {
            ZSTD_outBuffer output = {out_buf, read, 0};

            remaining = ZSTD_compressStream2(stream, &output, &input, ZSTD_e_continue);

            if (ZSTD_isError(remaining)) {
                fprintf(stderr, "Compression error: %s\n", ZSTD_getErrorName(remaining));
                result = 1;
                break;
            }

            if (fwrite(out_buf, 1, output.pos, out) != output.pos) {
                fprintf(stderr, "Writing failed\n");
                result = 1;
                break;
            }

            if (output.pos == 0 &&
                input.pos == 0 &&
                input.size > 0) {
                fprintf(stderr, "Compression stalled\n");
                result = 1;
                break;
            }
        }

        if (result) {
            break;
        }
    }

    if (!result && remaining != 0) {
        fprintf(stderr, "Incomplete or truncated zstd frame\n");
        result = 1;
    }

    if (!result) {
        size_t remaining;

        do {
            ZSTD_inBuffer input = {NULL, 0, 0};
            ZSTD_outBuffer output = {out_buf, sizeof(out_buf), 0};

            remaining = ZSTD_compressStream2(
                stream, &output, &input, ZSTD_e_end);

            if (ZSTD_isError(remaining)) {
                fprintf(stderr, "Compression finalization error: %s\n",
                        ZSTD_getErrorName(remaining));
                result = 1;
                break;
            }

            if (fwrite(out_buf, 1, output.pos, out) != output.pos) {
                fprintf(stderr, "Writing failed during finalization\n");
                result = 1;
                break;
            }
        } while (remaining != 0);
    }

    ZSTD_freeCStream(stream);

    if (fclose(in) != 0)
    {
        fprintf(stderr, "Failed to close input file %s\n", infile);
        result = 1;
    }

    if (fclose(out) != 0)
    {
        fprintf(stderr, "Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (result) {
        remove(outfile);
    }

    return result;
}

int libTeer_zstd_unpack(const char *filename, const char *directory)
{
    char rand_filename[1024];
    char str[] = {[41] = '\1'};
    rand_str(str, sizeof str - 1);
    int rc = 0;
    rc = snprintf(rand_filename, 1024, "/tmp/%s", str);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not generate filename\n");
        return 1;
    }
    rc = libTeer_zstd_decompress(filename, rand_filename);
    if (rc != 0)
    {
        fprintf(stderr, "error: could not decompress %s\n", filename);
        return 1;
    }
    rc = libTeer_unpack(rand_filename, directory);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not unpack %s\n", rand_filename);
        return 1;
    }

    remove(rand_filename);

    return 0;
}