#include <stdbool.h>
#include <stdio.h>

#define NOB_IMPLEMENTATION
#include "include/thirdparty/nob.h"
#define FLAG_IMPLEMENTATION
#include "include/thirdparty/flag.h"

#define SRCDIR "src/"
#define BUILDDIR "build/"

#ifdef _WIN32
#define DYNAMIC_OUTPUT_EXTENSION ".dll"
#elifdef TARGET_MINGW
#define DYNAMIC_OUTPUT_EXTENSION ".dll"
#else
#define DYNAMIC_OUTPUT_EXTENSION ".so"
#endif

#ifndef DYNAMIC_OUTPUT_EXTENSION
#define DYNAMIC_OUTPUT_EXTENSION ".so"
#endif

Procs procs = {0};
Flag_List output_objects = {0};

void usage(FILE *stream)
{
    fprintf(stream, "Usage: ./nob [OPTIONS] [--] [ARGS]\n");
    fprintf(stream, "OPTIONS:\n");
    flag_print_options(stream);
}

void cmd_append_cflags(Cmd *cmd, Flag_List *cflags)
{
    for (size_t i = 0; i < cflags->count; ++i)
    {
        cmd_append(cmd, cflags->items[i]);
    }
}

void prepare_build_cmd(Cmd *cmd, char *cc_command, Flag_List *cflags, char *infile, char *outfile)
{
    cmd_append(cmd, cc_command);
    cmd_append_cflags(cmd, cflags);
    cmd_append(cmd, infile);
    cmd_append(cmd, "-o");
    cmd_append(cmd, outfile);
}

int build_file(char *cc_command, Flag_List *cflags, char *infile, char *outfile)
{
    Cmd cmd = {0};
    prepare_build_cmd(&cmd, cc_command, cflags, infile, outfile);
    if (!cmd_run(&cmd, .async = &procs))
        return 1;
}

int build_core_files(char *cc_command, Flag_List *cflags)
{
    char core_files[12][64] = {
        "append",
        "create",
        "delete",
        "dirtraverse",
        "extract",
        "file",
        "list",
        "merge",
        "misc",
        "record",
        "unpack",
        "ustar",
    };
    for (int i = 0; i < ARRAY_LEN(core_files); ++i)
    {
        if (build_file(cc_command, cflags, temp_sprintf(SRCDIR"%s.c", core_files[i]), temp_sprintf(BUILDDIR"%s.o", core_files[i])) != 0)
            return 1;
    }
    return 0;
}

int build_compression_files(char *cc_command, Flag_List *cflags) {
    char compression_files[4][64] = {
        "bzip2",
        "gzip",
        "xz",
        "zstd"
    };
    for (int i = 0; i < ARRAY_LEN(compression_files); ++i)
    {
        if (build_file(cc_command, cflags, temp_sprintf(SRCDIR"compress/%s.c", compression_files[i]), temp_sprintf(BUILDDIR"%s.o", compression_files[i])) != 0)
            return 1;
    }
    return 0;
}

int link_static(char *ar_command) {
    Cmd cmd = {0};
    cmd_append(&cmd, ar_command, "rcs", "libTeer.a");
    return 0;
}

int link_dynamic(char *cc_command, Flag_List *ldflags) {
    Cmd cmd = {0};
    cmd_append(&cmd, cc_command, "-shared");
    for (size_t i = 0; i < ldflags->count; ++i)
    {
        cmd_append(&cmd, ldflags->items[i]);
    }

    for (size_t j = 0; j < output_objects.count; ++j) {
        cmd_append(&cmd, output_objects.items[j]);
    }

    cmd_append(&cmd, "-o", "libTeer"DYNAMIC_OUTPUT_EXTENSION);
    if (!cmd_run(&cmd))
        return 1;
    #ifdef TARGET_MINGW
        cmd_append(&cmd, "gendef", "libTeer"DYNAMIC_OUTPUT_EXTENSION);
        if (!cmd_run(&cmd))
            return 1;
        cmd_append(&cmd, "x86_64-w64-mingw32-dlltool", "-d" ,"libTeer.def", "-D", "libTeer"DYNAMIC_OUTPUT_EXTENSION, "-l", "libTeer"DYNAMIC_OUTPUT_EXTENSION".a");
        if (!cmd_run(&cmd))
            return 1;
    #endif
    return 0;
}

bool count_files(Nob_Walk_Entry entry) {
    size_t *count = (size_t*)entry.data;
    if (entry.type == FILE_REGULAR) {
        flag_list_append(const char *, &output_objects, temp_strdup(entry.path));
        *count += 1;
    }
    return true;
}

int main(int argc, char **argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);

    bool *help = flag_bool("-help", false, "Print this message and exit.");
    bool *enable_compression = flag_bool("-enable-compression", true, "Build the library with compression.");
    bool *disable_compression = flag_bool("-disable-compression", false, "Build the library without compression.");
    bool *enable_shared = flag_bool("-enable-shared", true, "Build a shared library.");
    bool *disable_shared = flag_bool("-disable-shared", false, "Dont build a shared library.");
    bool *enable_static = flag_bool("-enable-static", true, "Build a static library.");
    bool *disable_static = flag_bool("-disable-static", false, "Dont build a static library.");

    char **cc_command = flag_str("-cc", "cc", "What C Compiler to use.");
    char **ar_command = flag_str("-ar", "ar", "What archiver to use.");
    Flag_List cflags = {0};
    flag_list_append(const char *, &cflags, "-Wall");
    flag_list_append(const char *, &cflags, "-Werror");
    flag_list_append(const char *, &cflags, "-pedantic");
    flag_list_append(const char *, &cflags, "-c");
    flag_list_var(&cflags, "-cflags", "Additional Flags passed to the C Compiler");

    if (!flag_parse(argc, argv))
    {
        usage(stderr);
        flag_print_error(stderr);
        exit(1);
    }

    if (*help)
    {
        usage(stdout);
        exit(0);
    }

    argc = flag_rest_argc();
    argv = flag_rest_argv();

    bool do_compression = false;
    if (*disable_compression)
    {
        do_compression = false;
    }
    else if (*enable_compression)
    {
        do_compression = true;
    }
    bool do_shared = false;
    if (*disable_shared)
    {
        do_shared = false;
    }
    else if (*enable_shared)
    {
        do_shared = true;
    }
    bool do_static = false;
    if (*disable_static)
    {
        do_static = false;
    }
    else if (*enable_static)
    {
        do_static = true;
    }

    Flag_List ldflags = {0};
    if (do_shared && do_compression)
    {
        flag_list_append(const char *, &ldflags, "-lz");
        flag_list_append(const char *, &ldflags, "-lzstd");
        flag_list_append(const char *, &ldflags, "-lbz2");
        flag_list_append(const char *, &ldflags, "-llzma");
    }
#ifdef _WIN32
    if (do_shared)
    {
        flag_list_append(const char *, &ldflags, "-ladvapi32");
    }
#elifdef TARGET_MINGW
    if (do_shared)
    {
        flag_list_append(const char *, &ldflags, "-ladvapi32");
    }
#endif

    if (do_shared)
    {
        flag_list_append(const char *, &cflags, "-fPIC");
    }

    Cmd cmd = {0};
    #ifdef _WIN32
    cmd_append(&cmd, "rmdir", "/s", "/q", BUILDDIR);
    #else
    cmd_append(&cmd, "rm", "-rf", BUILDDIR);
    #endif
    cmd_run(&cmd);

    if (!nob_mkdir_if_not_exists(BUILDDIR))
        return 1;
    if (build_core_files(*cc_command, &cflags) != 0)
        return 1;

    if (!do_compression)
    {
        if (build_file(*cc_command, &cflags, SRCDIR"stub.c", BUILDDIR"stub.o") != 0)
            return 1;
    }
    else
    {
        if (build_compression_files(*cc_command, &cflags) != 0)
        return 1;
    }
    if (!procs_flush(&procs)) return 1;

    size_t count = 0;
    if (!nob_walk_dir("build", count_files, .data = &count)) return 1;

    if (do_shared) {
        if (link_dynamic(*cc_command, &ldflags) != 0) return 1;
    } else {
        if (link_static(*ar_command) != 0) return 1;
    }

    return 0;
}