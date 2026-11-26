#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <libgen.h>
#include "../include/libTeer.h"

char *get_file_extension(char *filename)
{
    char *basefile = basename(filename);

    char *token = strtok(basefile, ".");
    char *lasttoken;
    while (token != NULL)
    {
        lasttoken = token;
        token = strtok(NULL, ".");
        if (token == NULL)
        {
            return lasttoken;
        }
    }
    return NULL;
}


LibTeer_Compression_Mode get_mode_from_filename(const char *filename)
{
    char *dupfnmape = strdup(filename);
    LibTeer_Compression_Mode return_mode;
    char *ext = get_file_extension(dupfnmape);
    if (strncmp(ext, "tar", 3) == 0)
    {
        return_mode = NOCOMPRESS;
    }
    else if (strncmp(ext, "gz", 2) == 0)
    {
        return_mode = GZIP;
    }
    else if (strncmp(ext, "gzip", 4) == 0)
    {
        return_mode = GZIP;
    }
    else if (strncmp(ext, "tgz", 3) == 0)
    {
        return_mode = GZIP;
    }
    else if (strncmp(ext, "xz", 2) == 0)
    {
        return_mode = XZ;
    }
    else if (strncmp(ext, "lzma", 4) == 0)
    {
        return_mode = XZ;
    }
    else if (strncmp(ext, "txz", 3) == 0)
    {
        return_mode = XZ;
    }
    else if (strncmp(ext, "lz", 2) == 0)
    {
        return_mode = XZ;
    }
    else if (strncmp(ext, "bz2", 3) == 0)
    {
        return_mode = BZIP2;
    }
    else if (strncmp(ext, "zst", 3) == 0)
    {
        return_mode = ZSTD;
    }
    else if (strncmp(ext, "zstd", 4) == 0)
    {
        return_mode = ZSTD;
    }
    else if (strncmp(ext, "tstd", 3) == 0)
    {
        return_mode = ZSTD;
    }
    else
    {
        eprintf("Failed to determine compression mode from file extension '%s'!\n", ext);
        free(dupfnmape);
        return -1;
    }
    free(dupfnmape);
    return return_mode;
}