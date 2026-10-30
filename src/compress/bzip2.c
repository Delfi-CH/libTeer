#include "../../include/libTeer/tar.h"
#include "../../include/libTeer/compress.h"
#include "../../include/libTeer/private.h"
#include <bzlib.h>
#include <stdio.h>
#include <stdlib.h>

int libTeer_bzip2_decompress(const char *infile, const char *outfile)
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

    int bzerr = 0;
    int result = 0;

    BZFILE *bz = BZ2_bzReadOpen(&bzerr, in, 0, 0, NULL, 0);

    if (bzerr != BZ_OK)
    {
        fprintf(stderr, "Error opening bzip file: %d\n", bzerr);
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
            fprintf(stderr, "fwrite failed\n");
            result = 1;
        }
        if (bzerr != BZ_STREAM_END)
        {
            break;
        }
        if (bzerr != BZ_OK)
        {
            fprintf(stderr, "Error reading from bzip file: %d\n", bzerr);
            result = 1;
        }

        if (result) {
            break;
        }
    }

    BZ2_bzReadClose(&bzerr, bz);
    if (bzerr != BZ_OK)
    {
        fprintf(stderr, "Error closing bzip file: %d\n", bzerr);
        result = 1;
    }

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

int libTeer_bzip2_compress(const char *infile, const char *outfile, int compression_level, int work_factor)
{
    if (compression_level < 1 || compression_level > 9) {
        fprintf(stderr, "Compression level out of range! Use a value from %d to %d\n", 1, 9);
        return -1;
    }
    if (work_factor < 1 || work_factor > 250) {
        fprintf(stderr, "Work factor out of range! Use a value from %d to %d\n", 1, 250);
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

    int bzerr = 0;
    int result = 0;

    BZFILE *bz = BZ2_bzWriteOpen(&bzerr, out, compression_level, 0, work_factor);

    if (bzerr != BZ_OK)
    {
        fprintf(stderr, "Error opening bzip file: %d\n", bzerr);
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
            fprintf(stderr, "Error reading from bzip file: %d\n", bzerr);
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
        fprintf(stderr, "Error closing bzip file: %d\n", bzerr);
        result = 1;
    }

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

int libTeer_bzip2_unpack(const char *filename, const char *directory)
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
    rc = libTeer_bzip2_decompress(filename, rand_filename);
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