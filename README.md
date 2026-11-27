# libTeer

library for working with ustar archives

current version: v0.8.4

**Note:**

This library is a hobby project and not intended for use in serious projects.

## Installation

**Arch Linux:**
```bash
wget https://raw.githubusercontent.com/Delfi-CH/libTeer/refs/heads/main/pkg/PKGBUILD
makepkg -si
```

**Other platforms:**

See [https://github.com/Delfi-CH/libTeer#building](https://github.com/Delfi-CH/libTeer#building)

## Features:

**Operations:**

- [x] Creating archives
- [x] Unpacking archives
- [x] Appending to archives
- [x] Merging archives
- [x] List the contents of an archive
- [x] Extract specific files from an archive
- [x] Delete specific files from an archive
- [ ] Operations on files and on data in memory

**(De)Compression:**

- [x] gzip
- [x] xz/lzma
- [x] bz2
- [x] zstd

## Known issues / Limitations

- Paths can only be 255 characters long. This is a limitation of the ustar format.
- No (current) explicit support for GNU TAR (old and new), original UNIX V7 and PAX archive formats.

## Usage

### Basic examples

#### Unpacking
```c
#include <libTeer.h>
#include <stdio.h>

int main(int argc, char **argv) {

    char *program_name = argv[0];
    char *tarfile = argv[1];
    char *output_directory = argv[2];

    if (argc < 3) {
        printf("usage: %s <input.tar> <output>\n", program_name);
        return 1;
    }

    printf("unpacking %s to %s\n", tarfile, output_directory);
    libTeer_unpack(tarfile, output_directory, AUTO_COMPRESSION);
    printf("done\n");
    return 0;
}
```

## Building

This project uses [nob.h (github.com/tsoding/nob.h)](https://github.com/tsoding/nob.h) as its build system.

### Linux

#### Quickstart

1. Install the required packages

**Ubuntu 26.04:**

```bash
sudo apt update && sudo apt install build-essential git zlib1g-dev liblzma-dev libbz2-dev libzstd-dev 
```

**Debian 13:**

```bash
sudo apt update && sudo apt install build-essential git zlib1g-dev liblzma-dev libbz2-dev libzstd-dev 
```

**Fedora 45:**

```bash
sudo dnf install gcc make git zlib-devel xz-devel bzip2-devel libzstd-devel
```

**Arch Linux:**

```bash
sudo pacman -S base-devel zlib xz bzip2 zstd
```

2. Build the project

```bash
git clone https://github.com/Delfi-CH/libTeer.git
cd libTeer
cc nob.c -o nob && ./nob
```

#### Requirements

- gcc/clang

**For compression:** 

- zlib-dev
- liblzma-dev
- libbz2-dev
- libzstd-dev

**For building a static library with compression:**

- wget
- GNU autotools
- GNU make
- cmake

#### Build options

```
Usage: ./nob [OPTIONS] [--] [ARGS]
OPTIONS:
    --help
        Print this message and exit.
    --enable-compression
        Build the library with compression.
        Default: true
    --disable-compression
        Build the library without compression.
    --enable-shared
        Build a shared library.
        Default: true
    --disable-shared
        Dont build a shared library.
    --enable-static
        Build a static library.
        Default: true
    --disable-static
        Dont build a static library.
    --cc <str>
        What C Compiler to use.
        Default: cc
    --ar <str>
        What archiver to use.
        Default: ar
    --cflags <str> ... --cflags <str> ...
        Additional Flags passed to the C Compiler

```

### Windows (mingw)

#### Requirements

- mingw

**For compression:** 

- zlib-dev
- liblzma-dev
- libbz2-dev
- libzstd-dev

**For building a static library with compression:**

- wget
- GNU autotools
- GNU make
- cmake

#### Building the project

```bash
git clone https://github.com/Delfi-CH/libTeer.git
cd libTeer
cc nob.c -o nob -DTARGET_MINGW && ./nob --cc=x86_64-w64-mingw32-gcc
```