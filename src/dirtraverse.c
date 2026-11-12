#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../include/libTeer/private.h"

static int list_count = 0;

char *add_trailing_slash(const char *directory)
{
    if (directory == NULL)
        return NULL;

    size_t len = strlen(directory);

    char *result = malloc(len + 2);
    if (result == NULL)
        return NULL;

    strcpy(result, directory);

    if (len > 0 && directory[len - 1] != '/') {
        result[len] = '/';
        result[len + 1] = '\0';
    }

    return result;
}

char **get_directory_list_recursive(const char *directory, char **list)
{
    if (directory == NULL)
        return list;

    DIR *dp = opendir(directory);

    if (dp == NULL) {
        perror(directory);
        return list;
    }

    struct dirent *ep;

    while ((ep = readdir(dp)) != NULL) {

        if (strcmp(ep->d_name, ".") == 0 ||
            strcmp(ep->d_name, "..") == 0)
            continue;

        size_t len = strlen(directory) + strlen(ep->d_name) + 2;
        char *path = malloc(len);

        if (path == NULL)
            continue;

        snprintf(path, len, "%s%s", directory, ep->d_name);

        struct stat st;

        if (lstat(path, &st) == -1) {
            free(path);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            path = realloc(path, len + 1);
            snprintf(path, len + 1, "%s%s/", directory, ep->d_name);
        }

        char **tmp = realloc(
            list,
            (list_count + 1) * sizeof(char *)
        );

        if (tmp == NULL) {
            free(path);
            break;
        }

        list = tmp;
        list[list_count++] = strdup(path);

        if (list[list_count - 1] == NULL) {
            list_count--;
            free(path);
            break;
        }

        if (S_ISDIR(st.st_mode)) {
            list = get_directory_list_recursive(path, list);
        }

        free(path);
    }

    closedir(dp);

    return list;
}

struct filellist *get_directory_list(const char *directory)
{
    char *realdir = malloc(sizeof(char) * 256);
    if (directory[strlen(directory) -1] != '/') {
        snprintf(realdir, 256, "%s/", directory);
    } else {
        snprintf(realdir, 256, "%s", directory);
    }
    struct filellist *list = malloc(sizeof(struct filellist));
    if (list == NULL)
        return NULL;

    list_count = 1;

    char **list_data = malloc(sizeof(char *));
    if (list_data == NULL)
        return NULL;
    
    list_data[0] = strdup(realdir);

    list_data = get_directory_list_recursive(realdir, list_data);
    list->data = list_data;
    list->length = list_count;
    free(realdir);
    return list;
}

void free_directory_list(struct filellist *list)
{
    if (list == NULL)
        return;

    for (size_t i = 0; i < list->length; i++) {
        free(list->data[i]);
    }

    free(list->data);
    free(list);
}