# libTeer

library for working with ustar archives

current version: v0.8.1

**Note:**

This library is a hobby project and not intended for use in serious projects.

## Installation

**Arch Linux:**

```bash
git clone https://github.com/Delfi-CH/libTeer.git
cd libTeer
make archlinux-install
```

or

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

**(De)Compression:**

- [x] gzip
- [x] xz/lzma
- [x] bz2
- [x] zstd

## Known issues / Limitations

- Windows builds cannot use compression.
- Paths can only be 255 characters long. This is a limitation of the ustar format.
- No explicit support for GNU TAR (old and new), original UNIX V7 and PAX archive formats.

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
make
```

#### Requirements

- GNU Make
- GNU coreutils
- gcc/clang

**For compression:** 

- zlib-dev
- liblzma-dev
- libbz2-dev
- libzstd-dev

**For building a static library with compression:**

- wget
- GNU autotools

#### Build options

```bash
make FEATURE_COMPRESSION=false
# disables handling of gzip,xz,bz2 and zstd compressed archives, removes any dependencies outside of libc

make install
# installs the .so or .a into /usr/lib and the nessecary headers into /usr/include/libTeer, requieres root permissions
# on fedora linux the libdir is /usr/lib64, see example below
# make install PREFIX=/usr LIBDIR=lib64

make test
# build and run the test programs

make clean
# removes all artifacts from the directories

make VERSION=0.1.0
# use a specific version string instead of the latest commit

make OUTPUT=static
# creates a .a archive instead of a shared object for static linking. if FEATURE_COMPRESSION=true, it will also download and build static versions of the compression libraries and include them in the .a archive

make OPTFLAGS=-O2
# build the library with optimisations

make CC=clang
# builds the library with clang

make CC=clang FEATURE_MSAN=true FEATURE_COMPRESSION=false
# builds the base library with the memory sanitizer 

make archlinux-install
# build and install an Arch Linux PKGBUILD 

make PKGTYPE=git archlinux-install 
# build and install an Arch Linux PKGBUILD, made from the latest git commit 
```

### Windows (mingw)

#### Requirements

- GNU Make
- GNU coreutils
- mingw

#### Building the project

```bash
git clone https://github.com/Delfi-CH/libTeer.git
cd libTeer
make PLATFORM=windows
```

#### Build options

```bash
make PLATFORM=windows test
# build and run the test programs

make clean
# removes all artifacts from the directories

make VERSION=0.1.0
# use a specific version string instead of the latest commit

make OUTPUT=static
# creates a .a archive instead of a shared object for static linking.
```