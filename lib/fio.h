#ifndef F_IO_H
#define F_IO_H

/*
support >4GB files

Why use this?
In C library, fopen to open a file, on posix is fopen, on Windows is _tfopen_s.
on posix fopen's argument name is encoding utf8, on Windows is mbcs(ASCII).
so we need a compatible.

*/


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

//
// pathname is encoding utf-8 bytes
// int is errno_t
// read and write memory's size is size_t type.
//
// invalid file pos is -1
int io_fopen(FILE **, const char *name, const char *mode);
int io_fclose(FILE *);
// off default is (uint64_t)-1
int io_fread(FILE *, void *, size_t *read, uint64_t off);
// off default is  (uint64_t)-1
int io_fwrite(FILE *, const void *, size_t *, uint64_t off);
int io_fseek(FILE *, uint64_t off, int);
int io_ftell(FILE *, uint64_t *off);
int io_fflush(FILE *);


struct file_reader {
    FILE *f;
    uint64_t off;
};

void file_reader_clear(struct file_reader *);
// open utf-8 encoding bytes filename
int file_reader_open(struct file_reader *, const char *);

/* read one char, return char if successful,
else return EOF
only read, not move cursor */
int file_reader_read(struct file_reader *);
void file_reader_next(struct file_reader *); /* move cursor */
uint64_t file_reader_off(struct file_reader *);
bool file_reader_eof(struct file_reader *);
uint64_t file_reader_size(struct file_reader *); /* invalid size is -1 */

void test_fio();


#endif // F_IO_H
