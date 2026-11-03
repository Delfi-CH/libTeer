#include "../../include/libTeer.h"
#include "../../include/libTeer/compress.h"
#include "../../include/libTeer/private.h"
#include <lzma.h>
#include <stddef.h>
#include <stdbool.h>

int libTeer_xz_decompress(const char *infile, const char *outfile)
{
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

    lzma_stream stream = LZMA_STREAM_INIT;

    lzma_ret ret = lzma_stream_decoder(
        &stream,
        UINT64_MAX,
        LZMA_CONCATENATED);

    if (ret != LZMA_OK)
    {
        fprintf(stderr, "Failed to initialize LZMA decoder\n");
        fclose(in);
        fclose(out);
        remove(outfile);
        return 1;
    }

    uint8_t in_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];
    uint8_t out_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];

    int result = 0;
    bool eof = false;

    stream.next_in = NULL;
    stream.avail_in = 0;

    while (true)
    {
        if (stream.avail_in == 0 && !eof)
        {
            size_t read = fread(in_buf, 1, sizeof(in_buf), in);
            stream.next_in = in_buf;
            stream.avail_in = read;

            if (read < sizeof(in_buf))
            {
                if (ferror(in))
                {
                    fprintf(stderr, "Failed to read input file\n");
                    result = 1;
                    break;
                }

                eof = true;
            }
        }

        lzma_action action = !eof ? LZMA_RUN : LZMA_FINISH;

        stream.next_out = out_buf;
        stream.avail_out = sizeof(out_buf);

        ret = lzma_code(&stream, action);

        size_t written = sizeof(out_buf) - stream.avail_out;

        if (written > 0)
        {
            if (fwrite(out_buf, 1, written, out) != written)
            {
                fprintf(stderr, "Writing failed\n");
                result = 1;
                break;
            }
        }

        if (ret == LZMA_STREAM_END)
        {
            result = 0;
            break;
        }

        if (ret != LZMA_OK)
        {
            fprintf(stderr, "LZMA decompression failed: %d\n", ret);
            result = 1;
            break;
        }

        if (result)
        {
            break;
        }
    }

    lzma_end(&stream);

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

    if (result)
    {
        remove(outfile);
    }

    return result;
}

int libTeer_xz_compress(const char *infile, const char *outfile, int compression_level)
{
    if (compression_level < 1 || compression_level > 9)
    {
        fprintf(stderr, "Compression level out of range! Use a value from %d to %d\n", 1, 9);
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

    lzma_stream stream = LZMA_STREAM_INIT;

    lzma_ret ret = lzma_easy_encoder(
        &stream,
        compression_level,
        LZMA_CHECK_CRC64);

    if (ret != LZMA_OK)
    {
        fprintf(stderr, "Failed to initialize LZMA encoder\n");
        fclose(in);
        fclose(out);
        remove(outfile);
        return 1;
    }

    uint8_t in_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];
    uint8_t out_buf[LIBTEER_COMPRESSION_BUFFER_SIZE];

    int result = 0;
    bool eof = false;

    stream.next_in = NULL;
    stream.avail_in = 0;

    lzma_action action = LZMA_RUN;

    while (true)
    {
        action = !eof ? LZMA_RUN : LZMA_FINISH;

        if (stream.avail_in == 0 && !eof) {
            size_t read = fread(in_buf, 1, sizeof(in_buf), in);
            stream.next_in = in_buf;
            stream.avail_in = read;

            if (read < sizeof(in_buf))
            {
                if (ferror(in))
                {
                    fprintf(stderr, "Failed to read input file\n");
                    result = 1;
                    break;
                }

                eof = true;
            }
        }

        stream.next_out = out_buf;
        stream.avail_out = sizeof(out_buf);

        ret = lzma_code(&stream, action);

        size_t written = sizeof(out_buf) - stream.avail_out;

        if (written > 0)
        {
            if (fwrite(out_buf, 1, written, out) != written)
            {
                fprintf(stderr, "Writing failed\n");
                result = 1;
                break;
            }
        }

        if (ret == LZMA_STREAM_END) {
            break;
        }

        if (ret != LZMA_OK)
        {
            fprintf(stderr, "LZMA compression failed: %d\n", ret);
            result = 1;
            break;
        }

        if (result)
        {
            break;
        }
    }

    lzma_end(&stream);

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

    if (result)
    {
        remove(outfile);
    }

    return result;
}

int libTeer_xz_unpack(const char *filename, const char *directory)
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
    rc = libTeer_xz_decompress(filename, rand_filename);
    if (rc != 0)
    {
        fprintf(stderr, "error: could not decompress %s\n", filename);
        return 1;
    }
    rc = libTeer_basic_unpack(rand_filename, directory);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not unpack %s\n", rand_filename);
        return 1;
    }

    remove(rand_filename);

    return 0;
}