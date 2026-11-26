#include "../../include/libTeer.h"
#include "../../include/libTeer/compress.h"
#include "../../include/libTeer/private.h"
#include <bzlib.h>
#include <stdio.h>
#include <stdlib.h>

const size_t BUFFER_SIZE = 512;

int libTeer_bzip2_decompress_file(const char *infile, const char *outfile)
{
    FILE *in = fopen(infile, "rb");
    if (in == NULL)
    {
        eprintf( "Failed to open file %s\n", infile);
        return 1;
    }

    FILE *out = fopen(outfile, "wb");
    if (out == NULL)
    {
        eprintf( "Failed to open file %s\n", outfile);
        fclose(in);
        return 1;
    }

    int bzerr = 0;
    int result = 0;

    BZFILE *bz = BZ2_bzReadOpen(&bzerr, in, 0, 0, NULL, 0);

    if (bzerr != BZ_OK)
    {
        eprintf( "Error opening bzip file: %d\n", bzerr);
        fclose(in);
        fclose(out);
        return 1;
    }

    char buf[LIBTEER_COMPRESSION_BUFFER_SIZE];
    int read;

    while ((read = BZ2_bzRead(&bzerr, bz, buf, sizeof(buf))) > 0)
    {
        size_t written = fwrite(buf, 1, (size_t)read, out);
        if (written != (size_t)read)
        {
            eprintf( "fwrite failed\n");
            result = 1;
        }
        if (bzerr != BZ_STREAM_END)
        {
            break;
        }
        if (bzerr != BZ_OK)
        {
            eprintf( "Error reading from bzip file: %d\n", bzerr);
            result = 1;
        }

        if (result) {
            break;
        }
    }

    BZ2_bzReadClose(&bzerr, bz);
    if (bzerr != BZ_OK)
    {
        eprintf( "Error closing bzip file: %d\n", bzerr);
        result = 1;
    }

    if (fclose(in) != 0)
    {
        eprintf( "Failed to close input file %s\n", infile);
        result = 1;
    }

    if (fclose(out) != 0)
    {
        eprintf( "Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (result)
    {
        remove(outfile);
    }

    return result;
}

int libTeer_bzip2_compress_file(const char *infile, const char *outfile, int compression_level, int work_factor)
{
    if (compression_level < 1 || compression_level > 9) {
        eprintf( "Compression level out of range! Use a value from %d to %d\n", 1, 9);
        return -1;
    }
    if (work_factor < 1 || work_factor > 250) {
        eprintf( "Work factor out of range! Use a value from %d to %d\n", 1, 250);
        return -1;
    }
    FILE *in = fopen(infile, "rb");
    if (in == NULL)
    {
        eprintf( "Failed to open file %s\n", infile);
        return 1;
    }

    FILE *out = fopen(outfile, "wb");
    if (out == NULL)
    {
        eprintf( "Failed to open file %s\n", outfile);
        fclose(in);
        return 1;
    }

    int bzerr = 0;
    int result = 0;

    BZFILE *bz = BZ2_bzWriteOpen(&bzerr, out, compression_level, 0, work_factor);

    if (bzerr != BZ_OK)
    {
        eprintf( "Error opening bzip file: %d\n", bzerr);
        fclose(in);
        fclose(out);
        return 1;
    }

    char buf[LIBTEER_COMPRESSION_BUFFER_SIZE];
    size_t read;

    while ((read = fread(buf, 1, sizeof(buf), in)) > 0)
    {
        BZ2_bzWrite(&bzerr, bz, buf, (int)read);
        if (bzerr != BZ_OK)
        {
            eprintf( "Error reading from bzip file: %d\n", bzerr);
            result = 1;
        }

        if (result) {
            break;
        }
    }
    if (result) {
        BZ2_bzWriteClose(&bzerr, bz, 1, NULL, NULL);
    } else {
        BZ2_bzWriteClose(&bzerr, bz, 0, NULL, NULL);
    }

    if (bzerr != BZ_OK)
    {
        eprintf( "Error closing bzip file: %d\n", bzerr);
        result = 1;
    }

    if (fclose(in) != 0)
    {
        eprintf( "Failed to close input file %s\n", infile);
        result = 1;
    }

    if (fclose(out) != 0)
    {
        eprintf( "Failed to close output file %s\n", outfile);
        result = 1;
    }

    if (result)
    {
        remove(outfile);
    }

    return result;
}