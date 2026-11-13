#include "../include/libTeer/private.h"
#include "../include/libTeer.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#include <aclapi.h>
#else
#include <pwd.h>
#include <grp.h>
#endif

static const char ZERO = {0};
static const char ZERO_2[2] = {0};
static const char ZERO_8[8] = {0};
static const char ZERO_12[12] = {0};
static const char ZERO_100[100] = {0};
static const char ZERO_155[155] = {0};

void rand_str(char *dest, size_t length)
{
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0)
    {
        size_t index = (double)rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

char *read_single_record(FILE *tarball)
{
    char *buffer = malloc(LIBTEER_RECORD_LENGTH * sizeof(char));
    if (buffer == NULL)
    {
        fprintf(stderr, "Failed to allocate memory!\n");
        return NULL;
    }
    for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
    {
        buffer[i] = fgetc(tarball);
    }
    return buffer;
}

bool is_record_ustar_header(const char *record)
{
    if (record == NULL)
        return false;

    return memcmp(record + 257, "ustar", 5) == 0;
}

void print_ustar_header_info(LibTeer_UStarHeader *header)
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

char *libTeer_get_version()
{
    return LIBTEER_VERSION_STRING;
}

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

libTeer_Compression_Mode get_mode_from_filename(const char *filename)
{
    char *dupfnmape = strdup(filename);
    libTeer_Compression_Mode return_mode;
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
        fprintf(stderr, "Failed to determine compression mode from file extension '%s'!\n", ext);
        free(dupfnmape);
        return -1;
    }
    free(dupfnmape);
    return return_mode;
}

void print_record(const char *record)
{
    for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
    {
        printf("%c", record[i]);
    }
    printf("\n");
    for (int i = 0; i < LIBTEER_RECORD_LENGTH; i++)
    {
        printf("%d", (int)record[i]);
    }
    printf("\n");
}

struct ustar_filebuf *file_to_ustar_buffer(const char *filename)
{
    if (filename == NULL)
    {
        return NULL;
    }
    LibTeer_UStarHeader *header = malloc(sizeof(LibTeer_UStarHeader));
    if (header == NULL) {
        fprintf(stderr, "Failed to allocate memory!\n");
        return NULL;
    }
    struct stat st;
    if (stat(filename, &st))
    {
        fprintf(stderr, "Failed to stat file %s!\n", filename);
        free(header);
        return NULL;
    }
    struct ustar_filebuf *filebuf = malloc(sizeof(struct ustar_filebuf));
    if (filebuf == NULL) {
        fprintf(stderr, "Failed to allocate memory!\n");
        free(header);
        return NULL;
    }

    if (strlen(filename) < 100) {
        snprintf(header->file_path, 100, "%s", filename);
        memcpy(header->filename_prefix, ZERO_155, 155);
    } else if (strlen(filename) < 256) {
        char *dupfilename = strdup(filename);
        char prefix_buf[156];
        snprintf(prefix_buf, 156, "%s", filename);
        char *prefix_end = strdup(strrchr(prefix_buf, '/'));
        prefix_buf[strlen(prefix_buf) - strlen(prefix_end)] = '\0';
        size_t prefix_len = strlen(prefix_buf);
        memmove(dupfilename, dupfilename + prefix_len + 1, strlen(dupfilename) - prefix_len);
        if (strlen(dupfilename) > 99) {
            fprintf(stderr, "Filename %s cannot be split!\n", filename);
            free(dupfilename);
            free(prefix_end);
            free(filebuf);
            free(header);
            return NULL;
        }
        snprintf(header->file_path, strlen(dupfilename) + 1, "%s", dupfilename);
        snprintf(header->filename_prefix, strlen(prefix_buf) + 1, "%s", prefix_buf);
        free(dupfilename);
        free(prefix_end);
    } else {
        fprintf(stderr, "Filename %s is longer than 255 characters!\n", filename);
        free(header);
        free(filebuf);
        return NULL;
    }
    snprintf(header->file_mode, 8, "%07o", st.st_mode & 07777);
    #ifdef _WIN32
        snprintf(header->owner_id, 8, "%07o", LIBTEER_DEFAULT_UID);
        snprintf(header->group_id, 8, "%07o", LIBTEER_DEFAULT_GID);
    #else
        snprintf(header->owner_id, 8, "%07o", st.st_uid);
        snprintf(header->group_id, 8, "%07o", st.st_gid);
    #endif
    snprintf(header->file_size, 12, "%011o", (unsigned int)st.st_size);
    #ifdef _WIN32
        snprintf(header->modif_time, 12, "%011o", (unsigned int)st.st_mtime);
    #else
        snprintf(header->modif_time, 12, "%011o", (unsigned int)st.st_mtim.tv_sec);
    #endif
    header->link = ZERO;
    memcpy(header->name_linked_file, ZERO_100, 100);
    snprintf(header->indicator, 6, "ustar");
    memcpy(header->version, ZERO_2, 2);
    #ifdef _WIN32
        PSID owner_sid = NULL;
        PSID group_sid = NULL;
        PSECURITY_DESCRIPTOR sd = NULL;

        DWORD result = GetNamedSecurityInfoA(
        (LPSTR)filename,
        SE_FILE_OBJECT,
        OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION,
        &owner_sid,
        &group_sid,
        NULL,
        NULL,
        &sd
    );

    if (result != ERROR_SUCCESS) {
        fprintf(stderr, "Failed to aquire security information: %lu\n", result);
        free(header);
        free(filebuf);
        return NULL;
    }

    PSID sids[] = { owner_sid, group_sid };
    const char *labels[] = { "Owner", "Group" };

    for (int i = 0; i < 2; i++) {
        char name[32];
        char domain[32];

        DWORD name_size = sizeof(name);
        DWORD domain_size = sizeof(domain);

        SID_NAME_USE sid_type;

        if (LookupAccountSidA(
                NULL,
                sids[i],
                name,
                &name_size,
                domain,
                &domain_size,
                &sid_type))
        {
            snprintf(header->username, 32, "%s", name);
            snprintf(header->groupname, 32, "%s", name);
        }
        else {
            fprintf(stderr, "%s: Failed to lookup account information (%lu)\n",
                   labels[i], GetLastError());
        }
    }

    LocalFree(sd);


    #else
        struct passwd *user = getpwuid(st.st_uid);
        snprintf(header->username, 32, "%s", user->pw_name);
        struct group *g = getgrgid(st.st_gid);
        snprintf(header->groupname, 32, "%s", g->gr_name);
    #endif
    memcpy(header->device_major_number, ZERO_8, 8);
    memcpy(header->device_minor_number, ZERO_8, 8);
    memcpy(header->padding, ZERO_12, 12);

    unsigned int checksum = 0;
    memset(header->checksum, ' ', sizeof(header->checksum));
    const unsigned char *bytes = (const unsigned char *)header;
    for (int i = 0; i < sizeof(*header); i++)
    {
        checksum += bytes[i];
    }
    snprintf(header->checksum, sizeof(header->checksum),"%06o", checksum);
    header->checksum[6] = '\0';
    header->checksum[7] = ' ';

    if (S_ISREG(st.st_mode))
    {
        int record_count = 0;
        if (st.st_size % 512 == 0)
        {
            record_count = st.st_size / 512;
        }
        else
        {
            record_count = st.st_size / 512 + 1;
        }
        LibTeer_Record *records = calloc(record_count, sizeof(LibTeer_Record));
        if (records == NULL)
        {
            free(filebuf);
            free(header);
            return NULL;
        }

        FILE *file = fopen(filename, "r");
        if (file == NULL)
        {
            fprintf(stderr, "Failed to open file %s!\n", filename);
            free(filebuf);
            free(header);
            free(records);
            return NULL;
        }

        int read = 0;
        for (int i = 0; i < record_count; i++)
        {
            read = fread(records[i].data, sizeof(char), LIBTEER_RECORD_LENGTH, file);
            if (read != 512)
            {
                while (read < 512)
                {
                    records[i].data[read] = ZERO;
                    read++;
                }
            }
        }
        filebuf->records = records;
        filebuf->record_count = record_count;
    } else {
        filebuf->records = NULL;
        filebuf->record_count = 0;
    }
    filebuf->header = header;
    return filebuf;
}

char *ustar_header_to_data(LibTeer_UStarHeader *header)
{
    if (header == NULL)
        return NULL;
    char *data = malloc(sizeof(LibTeer_UStarHeader));
    if (data == NULL)
        return NULL;
    memcpy(data, header, sizeof(LibTeer_UStarHeader));
    return data;
}