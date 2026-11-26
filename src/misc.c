#define _XOPEN_SOURCE 500

#include "../include/libTeer/private.h"
#include "../include/libTeer/compress.h"
#include "../include/libTeer.h"
#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifdef _WIN32
#include <windows.h>
#endif

char *get_tmpdir_path() {
    #ifdef _WIN32
        static char tmp_path[MAX_PATH + 1];
        DWORD result = GetTempPathA(sizeof(tmp_path), tmp_path);
        if (result == 0) return NULL;
        if (result > sizeof(tmp_path)) return NULL;
        return tmp_path;
    #else
        return "/tmp/";
    #endif
}

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

char *libTeer_get_version()
{
    return LIBTEER_VERSION_STRING;
}

void free_decompressed_buffer(struct decompressed_buffer *buffer) {
    if (buffer == NULL) return;
    free(buffer->data);
    free(buffer);
    return;
}