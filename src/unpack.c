#include "../include/libTeer/tar.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

const char libtar_ustar_indicator[6] = "ustar";

char *read_single_record(FILE *tarball)
{
    char *buffer = malloc(RECORD_LENGTH * sizeof(char));
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory!\n");
        return NULL;
    }
    for (int i = 0; i < RECORD_LENGTH; i++)
    {
        buffer[i] = fgetc(tarball);
    }
    return buffer;
}

bool is_record_ustar_header(char *record)
{
    LibTeer_UStarHeader header;
    memcpy(&header, record, sizeof(header));
    if (strncmp(header.indicator, libtar_ustar_indicator, 5))
        return false;
    return true;
}

void print_header_info(LibTeer_UStarHeader *header)
{
    printf("file_path: %s\n", header->file_path);
    printf("file_mode: %ld\n", strtol(header->file_mode, NULL, 10));
    printf("owner_id: %ld\n", strtol(header->owner_id, NULL, 8));
    printf("group_id: %ld\n", strtol(header->group_id, NULL, 8));
    printf("file_size: %ld\n", strtol(header->file_size, NULL, 8));
    printf("modif_time: %ld\n", strtol(header->modif_time, NULL, 8));
    printf("checksum: %s\n", header->checksum);
    printf("link: %c\n", header->link);
    printf("name_linked_file: %s\n", header->name_linked_file);
    printf("indicator: %s\n", header->indicator);
    printf("version: %ld\n", strtol(header->version, NULL, 10));
    printf("username: %s\n", header->username);
    printf("groupname: %s\n", header->groupname);
    printf("device_major_number: %ld\n", strtol(header->device_major_number, NULL, 10));
    printf("device_minor_number: %ld\n", strtol(header->device_minor_number, NULL, 10));
    printf("filename_prefix: %s\n", header->filename_prefix);
}

int libtar_unpack(const char *filename)
{
    struct stat st;
    if (stat(filename, &st))
    {
        fprintf(stderr, "Failed to stat file %s!\n", filename);
        return 1;
    }

    if (st.st_size % RECORD_LENGTH)
    {
        fprintf(stderr, "Not a tar file %s, filelength not divisible by %d!\n", filename, RECORD_LENGTH);
        return 1;
    }

    FILE *tarball = fopen(filename, "r");

    char *buf = malloc(RECORD_LENGTH * sizeof(char));
    FILE *current_file = NULL;
    char current_filename[255] = {0};
    LibTeer_UStarHeader *header = malloc(sizeof(LibTeer_UStarHeader));
    for (int i = 0; i < st.st_size / RECORD_LENGTH; i++)
    {
        buf = read_single_record(tarball);
        if (is_record_ustar_header(buf))
        {
            memcpy(header, buf, sizeof(*header));
            print_header_info(header);

            // is it a directory?
            if (header->file_path[strlen(header->file_path) - 1] == '/')
            {
                stat(header->file_path, &st);
                if (S_ISDIR(st.st_mode)) {
                    continue;
                } else {
                    mkdir(header->file_path, strtol(header->file_mode, NULL, 8));
                }
            } else {

            }
        }
        else
        {
            for (int i = 0; i < RECORD_LENGTH; i++)
            {
                printf("%c", buf[i]);
            }
            printf("\n");
        }
    }

    return 0;
}