#include "../../include/libTeer.h"
#include "../../include/libTeer/compress.h"
#include "../../include/libTeer/private.h"
#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>

int libTeer_gzip_decompress_file(const char *infile, const char *outfile)
{
    gzFile in = gzopen(infile, "rb");
    if (in == NULL)
    {
        eprintf("Failed to open file %s\n", infile);
        return 1;
    }

    FILE *out = fopen(outfile, "wb");
    if (out == NULL)
    {
        eprintf("Failed to open file %s\n", outfile);
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
            eprintf("fwrite failed\n");
            break;
        }
    }

    if (read < 0)
    {
        int errnum;
        eprintf("Decompression failed: %s\n",
                gzerror(in, &errnum));
        result = 1;
    }

    if (fclose(out) != 0)
    {
        eprintf("Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (gzclose(in) != Z_OK)
    {
        eprintf("Failed to close input file %s\n", infile);
        result = 1;
    }

    if (result) {
        remove(outfile);
    }

    return result;
}

int libTeer_gzip_compress_file(const char *infile, const char *outfile)
{
    FILE *in = fopen(infile, "rb");
    if (in == NULL)
    {
        eprintf("Failed to open file %s\n", infile);
        return 1;
    }

    gzFile out = gzopen(outfile, "wb");
    if (out == NULL)
    {
        eprintf("Failed to open file %s\n", outfile);
        fclose(in);
        return 1;
    }

    char buf[LIBTEER_COMPRESSION_BUFFER_SIZE] = {0};
    size_t read;
    int result = 0;

    while ((read = fread(buf, 1, sizeof(buf), in)) > 0)
    {
        int written = gzwrite(out, buf, read);
        if ((size_t)written != read)
        {
            eprintf("gzwrite failed\n");
            break;
        }
    }

    if (read < 0)
    {
        int errnum;
        eprintf("Compression failed: %s\n",
                gzerror(out, &errnum));
        result = 1;
    }

    if (fclose(in) != 0)
    {
        eprintf("Failed to close input file %s\n", infile);
        result = 1;
    }

    if (gzclose(out) != Z_OK)
    {
        eprintf("Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (result) {
        remove(outfile);
    }

    return result;
}