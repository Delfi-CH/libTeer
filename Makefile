# Feature flags

FEATURE_COMPRESSION = true
FEATUR_MSAN = false

# To use compression with shared libraries, you need libbz2, zlib, liblzma and libzstd

# Compression library makeflags

MAKEFLAGS_BUNDLED_LIBRARIES = -j4

# Compiler options

CC = gcc
AR = gcc-ar
CFLAGS_EXTRA_1 = 
ifeq ($(CC), clang)
ifeq ($(FEATUR_MSAN), true)
ifeq ($(FEATURE_COMPRESSION), false)
	CFLAGS_EXTRA_1 = -fsanitize=memory
endif
endif
endif

OUTPUT = shared
ifeq ($(OUTPUT), shared)
	CFLAGS_EXTRA_2 = -fPIC
endif

CFLAGS = -Wall -Werror $(CFLAGS_EXTRA_1) $(CFLAGS_EXTRA_2)
LDFLAGS =
ifeq ($(OUTPUT), shared)
ifeq ($(FEATURE_COMPRESSION), true)
	LDFLAGS = -lz -lzstd -llzma -lbz2
endif
endif

# Directories & Files

SRCDIR=src
TESTDIR=test
INCLUDEDIR=include
BUILDDIR=build
THIRDPARTY_DIR=thirdparty
NAME=libTeer
PRODUCT=

FOODIR=foo
DEMOTAR=$(FOODIR).tar

ifeq ($(OUTPUT), shared)
	PRODUCT=$(NAME).so
else
	PRODUCT=$(NAME).a
endif

ifeq ($(FEATURE_COMPRESSION), true)
	HEADERS = $(INCLUDEDIR)/$(NAME)/tar.h $(INCLUDEDIR)/$(NAME)/compress.h $(INCLUDEDIR)/$(NAME)/private.h 
else
	HEADERS = $(INCLUDEDIR)/$(NAME)/tar.h $(INCLUDEDIR)/$(NAME)/private.h 
endif

CORE_SRC = $(SRCDIR)/append.c $(SRCDIR)/merge.c $(SRCDIR)/create.c $(SRCDIR)/unpack.c $(SRCDIR)/shared.c
CORE_OBJ = $(BUILDDIR)/append.o $(BUILDDIR)/merge.o $(BUILDDIR)/create.o $(BUILDDIR)/unpack.o $(BUILDDIR)/shared.o

COMPRESS_SRC = $(SRCDIR)/compress/bzip2.c $(SRCDIR)/compress/gzip.c $(SRCDIR)/compress/xz.c $(SRCDIR)/compress/zstd.c
COMPRESS_OBJ = $(BUILDDIR)/bzip2.o $(BUILDDIR)/gzip.o $(BUILDDIR)/xz.o $(BUILDDIR)/zstd.o 
COMPRESS_EXTERNAL_OBJ = $(THIRDPARTY_DIR)/libbz2.a $(THIRDPARTY_DIR)/libz.a $(THIRDPARTY_DIR)/liblzma.a $(THIRDPARTY_DIR)/libzstd.a

ifeq ($(FEATURE_COMPRESSION), true)
	OBJ = $(CORE_OBJ) $(COMPRESS_OBJ)
else
	OBJ = $(CORE_OBJ)
endif 

TEST_UNPACK_SRC=$(TESTDIR)/test_unpack.c 
TEST_UNPACK_OBJ=test_unpack

TEST_UNPACK_GZIP_SRC=$(TESTDIR)/test_unpack_gzip.c 
TEST_UNPACK_GZIP_OBJ=test_unpack_gzip

BZIP2_SRC = bzip2-1.0.8
BZIP2_OBJ = $(BZIP2_SRC)/libbz2.a
BZIP2_ARCHIVE = bzip2-1.0.8.tar.gz
BZIP2_URL = https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz

ZLIB_SRC = zlib-1.3.2
ZLIB_OBJ = $(ZLIB_SRC)/libz.a
ZLIB_ARCHIVE = zlib-1.3.2.tar.gz
ZLIB_URL = https://github.com/madler/zlib/releases/download/v1.3.2/zlib-1.3.2.tar.gz

LZMA_SRC = xz-5.8.4
LZMA_OBJ = $(LZMA_SRC)/src/liblzma/.libs/liblzma.a
LZMA_CONFIGURE_FLAGS = --disable-xz --disable-xzdec	--disable-scripts --disable-doc --disable-shared
LZMA_ARCHIVE = xz-5.8.4.tar.gz
LZMA_URL = https://github.com/tukaani-project/xz/releases/download/v5.8.4/xz-5.8.4.tar.gz

ZSTD_SRC = zstd-1.5.7
ZSTD_OBJ = $(ZSTD_SRC)/lib/libzstd.a
ZSTD_ARCHIVE = zstd-1.5.7.tar.gz
ZSTD_URL = https://github.com/facebook/zstd/releases/download/v1.5.7/zstd-1.5.7.tar.gz

# Targets

.PHONY: all
.PHONY: objects

all: $(BUILDDIR) $(PRODUCT)

$(BUILDDIR):
	mkdir --parents $(BUILDDIR)

$(THIRDPARTY_DIR):
	mkdir --parents $(THIRDPARTY_DIR)

ifeq ($(OUTPUT), shared)
$(PRODUCT): $(OBJ)
	$(CC) -shared $(OBJ) $(LDFLAGS) -o $(PRODUCT)
else 
ifeq ($(FEATURE_COMPRESSION), true)
$(PRODUCT): $(THIRDPARTY_DIR) $(OBJ) $(COMPRESS_EXTERNAL_OBJ)
	$(AR) rcs $(PRODUCT) $(OBJ)
else
$(PRODUCT): $(OBJ)
	$(AR) rcs $(PRODUCT) $(OBJ)
endif
endif

ifeq ($(FEATURE_COMPRESSION), true)
objects: $(CORE_OBJ) $(COMPRESS_OBJ)
else
objects: $(CORE_OBJ)
endif 

$(CORE_OBJ): $(BUILDDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

$(COMPRESS_OBJ): $(BUILDDIR)/%.o: $(SRCDIR)/compress/%.c $(HEADERS)
	$(CC) -c $(CFLAGS) $< -o $@

$(COMPRESS_EXTERNAL_OBJ): build-libraries
	cp $(BZIP2_OBJ) $(THIRDPARTY_DIR)/
	cp $(ZLIB_OBJ) $(THIRDPARTY_DIR)/
	cp $(LZMA_OBJ) $(THIRDPARTY_DIR)/
	cp $(ZSTD_OBJ) $(THIRDPARTY_DIR)/

.PHONY: build-libraries

build-libraries: $(BZIP2_OBJ) $(ZLIB_OBJ) $(LZMA_OBJ) $(ZSTD_OBJ)

$(BZIP2_OBJ): $(BZIP2_SRC)
	cd $(BZIP2_SRC) && make $(MAKEFLAGS_BUNDLED_LIBRARIES)

$(BZIP2_SRC): $(BZIP2_ARCHIVE)
	tar xzf $(BZIP2_ARCHIVE)

$(BZIP2_ARCHIVE):
	wget $(BZIP2_URL)

$(ZLIB_OBJ): $(ZLIB_SRC)
	cd $(ZLIB_SRC) && ./configure
	cd $(ZLIB_SRC) && make $(MAKEFLAGS_BUNDLED_LIBRARIES)
	cd $(ZLIB_SRC) && make check

$(ZLIB_SRC): $(ZLIB_ARCHIVE)
	tar xzf $(ZLIB_ARCHIVE)

$(ZLIB_ARCHIVE):
	wget $(ZLIB_URL)

$(LZMA_OBJ): $(LZMA_SRC)
	cd $(LZMA_SRC) && ./configure $(LZMA_CONFIGURE_FLAGS)
	cd $(LZMA_SRC) && make $(MAKEFLAGS_BUNDLED_LIBRARIES)
	cd $(LZMA_SRC) && make check

$(LZMA_SRC): $(LZMA_ARCHIVE)
	tar xzf $(LZMA_ARCHIVE)

$(LZMA_ARCHIVE):
	wget $(LZMA_URL)

$(ZSTD_OBJ): $(ZSTD_SRC)
	cd $(ZSTD_SRC) && make HAVE_ZLIB=0 HAVE_LZMA=0 HAVE_LZ4=0 $(MAKEFLAGS_BUNDLED_LIBRARIES)

$(ZSTD_SRC): $(ZSTD_ARCHIVE)
	tar xzf $(ZSTD_ARCHIVE)

$(ZSTD_ARCHIVE):
	wget $(ZSTD_URL)

.PHONY: clean

clean:
	rm --recursive --force $(BUILDDIR) $(BZIP2_ARCHIVE) $(BZIP2_SRC) $(ZLIB_ARCHIVE) $(ZLIB_SRC) $(LZMA_ARCHIVE) $(LZMA_SRC) $(ZSTD_ARCHIVE) $(ZSTD_SRC) *.a *.o *.so $(THIRDPARTY_DIR) $(TEST_UNPACK_OBJ) $(TEST_UNPACK_GZIP_OBJ)

$(DEMOTAR): 
	tar cf $(DEMOTAR) $(FOODIR)

.PHONY: test
.PHONY: download-test-files

download-test-files:
	rm -rf /tmp/minowebd*
	cd /tmp && wget https://github.com/Delfi-CH/minopanel/releases/download/v0.8.0/minowebd-0.8.0-amd64.tar.gz

test: all $(TEST_SRC) $(DEMOTAR) download-test-files
ifeq ($(OUTPUT), static)
	$(CC) $(TEST_UNPACK_SRC) $(PRODUCT) -o $(TEST_UNPACK_OBJ)
	$(CC) $(TEST_UNPACK_GZIP_SRC) $(PRODUCT) -o $(TEST_UNPACK_GZIP_OBJ)
else
	$(CC) $(CFLAGS) $(TEST_UNPACK_SRC) -L$(PWD) -lTeer -o $(TEST_UNPACK_OBJ)
	LD_LIBRARY_PATH=$(PWD) ./$(TEST_UNPACK_OBJ)
	$(CC) $(CFLAGS) $(TEST_UNPACK_GZIP_SRC) -L$(PWD) -lTeer -o $(TEST_UNPACK_GZIP_OBJ)
	LD_LIBRARY_PATH=$(PWD) ./$(TEST_UNPACK_GZIP_OBJ)
endif

PREFIX ?= /usr
LIBDIR = $(PREFIX)/lib
SYS_INCLUDEDIR = $(PREFIX)/include

.PHONY: install

install: all
	install -d $(DESTDIR)$(LIBDIR)
	install -d $(DESTDIR)$(SYS_INCLUDEDIR)/$(NAME)
	install -m 644 $(PRODUCT) $(DESTDIR)$(LIBDIR)/$(PRODUCT)
	install -m 644 $(INCLUDEDIR)/$(NAME)/tar.h $(DESTDIR)$(SYS_INCLUDEDIR)/$(NAME)/tar.h
ifeq ($(FEATURE_COMPRESSION), true)
	install -m 644 $(INCLUDEDIR)/$(NAME)/compress.h $(DESTDIR)$(SYS_INCLUDEDIR)/$(NAME)/compress.h
endif