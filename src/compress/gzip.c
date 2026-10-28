#include "../../include/libTeer/tar.h"
#include "../../include/libTeer/compress.h"
#include "../../include/libTeer/private.h"
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>

int libTeer_gzip_decompress(const char *infile, const char *outfile)
{
    gzFile in = gzopen(infile, "rb");
    if (in == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", infile);
        return 1;
    }

    FILE *out = fopen(outfile, "wb");
    if (out == NULL)
    {
        fprintf(stderr, "Failed to open file %s\n", outfile);
        gzclose(in);
        return 1;
    }

    char buf[LIBTEER_COMPRESSION_BUFFER_SIZE] = {0};
    int read;
    int result = 0;

    while ((read = gzread(in, buf, sizeof(buf))) > 0)
    {
        size_t written = fwrite(buf, 1, (size_t)read, out);
        if (written != (size_t)read)
        {
            fprintf(stderr, "fwrite failed\n");
            break;
        }
    }

    if (read < 0)
    {
        int errnum;
        fprintf(stderr, "Decompression failed: %s\n",
                gzerror(in, &errnum));
        result = 1;
    }

    if (fclose(out) != 0)
    {
        fprintf(stderr, "Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (gzclose(in) != Z_OK)
    {
        fprintf(stderr, "Failed to close input file %s\n", infile);
        result = 1;
    }

    return result;
}

int libTeer_gzip_compress(const char *infile, const char *outfile)
{
    return 0;
}

int libTeer_gzip_unpack(const char *filename, const char *directory)
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
    rc = libTeer_gzip_decompress(filename, rand_filename);
    if (rc != 0)
    {
        fprintf(stderr, "error: could not decmopress %s\n", filename);
        return 1;
    }
    rc = libTeer_unpack(rand_filename, directory);
    if (rc < 0)
    {
        fprintf(stderr, "error: could not unpack %s\n", rand_filename);
        return 1;
    }

    return 0;
}