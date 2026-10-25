#ifndef LIBTAR_TAR_H
#define LIBTAR_TAR_H

// Data structures

typedef struct {
    char data[512];
} Record;

// Functions

void append();
void merge();
void pack();
void unpack();

#endif