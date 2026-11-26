#include "../include/libTeer.h"
#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

int libTeer_basic_filextract(const char *archive_name, const char *filename, const char *outdir) {
    if (filename == NULL) {
        eprintf("Filename is null! Aborting...\n");
        return 1;
    }
    if (archive_name == NULL) {
        eprintf("Archive name is null! Aborting...\n");
        return 1;
    }

    char *outfname = NULL;
    char *tmpdup = strdup(filename);
    if (tmpdup == NULL) {
            eprintf("Failed to allocate memory!\n");
            return 1;
        }
    char *basefname = basename(tmpdup);

    if (outdir == NULL) {
        outfname = strdup(filename);
        if (outfname == NULL) {
            eprintf("Failed to allocate memory!\n");
            free(tmpdup);
            return 1;
        }
    } else {
        outfname = calloc((strlen(filename) + strlen(outdir) + 2), sizeof(char));
        if (outfname == NULL) {
            eprintf("Failed to allocate memory!\n");
            free(tmpdup);
            return 1;
        }
        snprintf(outfname, (strlen(filename) + strlen(outdir) + 2),  "%s/%s", outdir, basefname);
    }

    struct stat st;
    if (stat(archive_name, &st))
    {
        eprintf("Failed to stat file %s!\n", archive_name);
        free(tmpdup);
        free(outfname);
        return 1;
    }

    if (st.st_size % LIBTEER_RECORD_LENGTH)
    {
        eprintf("Not a tar file %s, filelength not divisible by %d!\n", archive_name, LIBTEER_RECORD_LENGTH);
        free(tmpdup);
        free(outfname);
        return 1;
    }

    FILE *archive = fopen(archive_name, "rb");
    if (archive == NULL) {
        eprintf("Failed to open file %s!\n", archive_name);
        free(tmpdup);
        free(outfname);
        return 1;   
    }

    FILE *outfile = NULL;

    bool is_target_record = false;
    for (int i = 0; i < st.st_size / LIBTEER_RECORD_LENGTH; i++) {
        char *buf = read_single_record(archive);
        if (buf == NULL) {
            eprintf("Failed to read buffer from %s!\n", archive_name);
            fclose(archive);
            free(tmpdup);
            free(outfname);
            return 1;
        }

        if (is_target_record) {
            if (is_record_ustar_header(buf)) {
                is_target_record = false;
                free(buf);
                continue;
            }
            for (int j = 0; j < LIBTEER_RECORD_LENGTH; j++)
            {
                if (buf[j] != '\0' && outfile != NULL) {
                    fputc(buf[j], outfile);
                }
            }
            free(buf);
            continue;
        } else {
            if (is_record_ustar_header(buf)) {
            LibTeer_UStarHeader *header = calloc(1, sizeof(LibTeer_UStarHeader));
            memcpy(header, buf, LIBTEER_RECORD_LENGTH);
            char path[257];
            if (header->filename_prefix[0] != '\0') {
                snprintf(path, sizeof(path), "%s/%s",header->filename_prefix, header->file_path);
            } else {
                snprintf(path, sizeof(path), "%s", header->file_path);
            }
            if (strncmp(filename, path, strlen(filename)) == 0) {
                is_target_record = true;
                outfile = fopen(outfname, "wb");
                if (outfile == NULL) {
                    eprintf("Failed to create file %s!\n", outfname);
                    free(buf);
                    free(header);
                    free(tmpdup);
                    free(outfname);
                    return 1;
                }
            }            
            free(header);
        }
        free(buf);
        }
    }

    free(outfname);
    free(tmpdup);
    fclose(outfile);

    return 0;
}

int libTeer_filextract(const char *archive_name, const char *filename, const char *outdir, LibTeer_Compression_Mode compression_mode) {
    int rc = 0;
    LibTeer_Compression_Mode used_mode = AUTO_COMPRESSION;
    if (compression_mode < 0 || compression_mode > COMPRESSION_MODE_SIZE)
    {
        eprintf("Compression code not in range! Use a value from %d to %d!\n", 0, COMPRESSION_MODE_SIZE - 1);
        rc = 1;
        return rc;
    }

    if (compression_mode == 0)
    {
        used_mode = get_mode_from_filename(archive_name);
        if (used_mode == -1)
        {
            rc = -2;
            return rc;
        }
    }
    else
    {
        used_mode = compression_mode;
    }

    char rand_filename[1024];
    char str[] = {[41] = '\1'};
    rand_str(str, sizeof str - 1);
    rc = snprintf(rand_filename, 1024, "%s%s", get_tmpdir_path(), str);
    if (rc < 0)
    {
        eprintf("error: could not generate filename\n");
        return 1;
    }

    rc = 0;

    switch (used_mode)
    {
    case NOCOMPRESS:
        break;
    case GZIP:
        rc = libTeer_gzip_decompress_file(archive_name, rand_filename);
        break;
    case XZ:
        rc = libTeer_xz_decompress_file(archive_name, rand_filename);
        break;
    case BZIP2:
        rc = libTeer_bzip2_decompress_file(archive_name, rand_filename);
        break;
    case ZSTD:
        rc = libTeer_zstd_decompress_file(archive_name, rand_filename);
        break;
    case AUTO_COMPRESSION:
        rc = -2;
        eprintf("An unexpected error occured!\n");
        break;
    case COMPRESSION_MODE_SIZE:
        rc = -2;
        eprintf("An unexpected error occured!\n");
        break;
    }
    if (rc != 0)
    {
        return rc;
    }

    if (used_mode == NOCOMPRESS) {
        rc = libTeer_basic_filextract(archive_name, filename, outdir);
    } else {
        rc = libTeer_basic_filextract(rand_filename, filename, outdir);
    }
    if (rc != 0)
    {
        return rc;
    }

    if (used_mode != NOCOMPRESS) {
        rc = remove(rand_filename);
    }

    return rc;
}