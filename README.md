# libTeer

library for handeling tarballs

## Features:

**Tar formats:**

- [ ] unix v7 tar support
- [x] ustar support
- [ ] GNU tar support
- [ ] pax support

**Operations:**

- [ ] Creating archives
- [x] Unpacking archives
- [ ] Appending to archives
- [ ] Merging archives

**(De)Compression:**

- [x] gzip
- [ ] xz/lzma
- [ ] bz2
- [x] zstd


## Usage

### Basic examples

#### Unpacking
```c
#include <libTeer/tar.h>
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
    libTeer_unpack(tarfile, output_directory);
    printf("done\n");
    return 0;
}
```

## Building

### Quickstart

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

### Requirements

- (GNU) Make
- gcc/clang

#### For compression: 

- zlib-dev
- liblzma-dev
- libbz2-dev
- libzstd-dev

#### For building compression with static library:

- wget
- GNU autotools

### Build options

```bash
make FEATURE_COMPRESSION=false
# disables handling of gzip,xz,bz2 and zstd compressed archives, removes any dependencies outside of libc

make install
# installs the .so or .a into /usr/lib and the nessecary headers into /usr/include/libTeer, requieres root permissions
# on fedora linux use it like
# make install PREFIX=/usr LIBDIR=lib64

make test
# build and run the test programs

make clean
# removes all artifacts from the directories

make OUTPUT=static
# creates a .a archive instead of a shared object for static linking. if FEATURE_COMPRESSION=true, it will also build static versions of the compression libraries in thirdparty/

make CC=clang
# builds the library with clang

make CC=clang FEATURE_MSAN=true FEATURE_COMPRESSION=false
# builds the base library with the memory sanitizer 
```