/*
 * Cross platform file IO. Windows x86, Windows x64, linux ... etc.
 *
 */


/* Set off_t to 64bit. */
#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE

#include <string.h>
#include <errno.h>

#include "fio.h"

/* Or gcc with -D_FILE_OFFSET_BITS=64. */

#ifdef WIN32
#include <Windows.h>
#include <share.h>
// return 0 for success
static int _utf8_2_utf16le_windows(const char *src, size_t src_size, wchar_t *dst, size_t *dst_size)
{
    int r = MultiByteToWideChar(CP_UTF8, 0, src, (int)(src_size), dst, (int)(*dst_size));
    if (r <= 0) {
        return -1;
    } else {
        *dst_size = (size_t)(r);
        return 0;
    }
}

static int _utf8_2_utf16le(const char *src, wchar_t **dst)
{
    size_t src_size;
    size_t dst_size;
    int ret;
    wchar_t *p = 0;

    for (;;) {
        src_size = strlen(src);
        dst_size = 0;
        ret = _utf8_2_utf16le_windows(src, src_size, NULL, &dst_size);
        if (!(ret == 0 && dst_size > 0)) {
            break;
        }
        p = 0;
        dst_size += 1;
        p = (wchar_t *)malloc(dst_size * sizeof(wchar_t));
        if (!p) {
            break;
        }

        ret = _utf8_2_utf16le_windows(src, src_size, p, &dst_size);
        if (!(ret == 0 && dst_size > 0)) {
            break;
        }
        p[dst_size] = 0;
        *dst = p;
        return 0;
    }

    if (p) {
        free(p);
    }

    return -1;
}

#endif

int io_fopen(FILE **f, const char *name, const char *mode)
{
#ifdef WIN32

    wchar_t *wname = 0;
    wchar_t *wmode = 0;
    int ret;
    for (;;) {
        ret = _utf8_2_utf16le(name, &wname);
        if (ret != 0) {
            break;
        }
        ret = _utf8_2_utf16le(mode, &wmode);
        if (ret != 0) {
            break;
        }

        // this open can share read, which is useful when in log file
        // _SH_DENYWR is to share read
        // see the map https://stackoverflow.com/questions/9738960/what-is-difference-between-sh-secure-and-sh-denywr
        // return _wfopen_s(f, ws_name.c_str(), ws_mode.c_str());
        *f = _wfsopen(wname, wmode, _SH_DENYWR);
        if (*f) {
            free(wname);
            free(wmode);
            return 0;
        }

        break;
    }
    if (wname) {
        free(wname);
    }
    if (wmode) {
        free(wmode);
    }
    return EIO;

#else
    // fopen(<name>,"wb") is the share read mode by default on posxi.
    // we can use `tail -f <name>` to see the log file line
    *f = fopen(name, mode);
    if (*f) {
        return 0;
    } else {
        return EIO;
    }
#endif
}


int io_fclose(FILE *f)
{
    return fclose(f);
}

int io_fread(FILE *f, void *buffer, size_t *read, uint64_t off)
{
    int err;
    size_t readbk = *read;
    if (off != (-1)) {
        err = io_fseek(f, off, SEEK_SET);
        if (err != 0)
            return err;
    }


#ifdef WIN32
    *read = fread_s(buffer, readbk, 1, readbk, f);
    return 0;
#else
    *read = fread(buffer, 1, readbk, f);
    return 0;
#endif
}

int io_fwrite(FILE *f, const void *buffer, size_t *write, uint64_t off)
{
    int err;

    if (off != (-1)) {
        err = io_fseek(f, off, SEEK_SET);
        if (err) {
            return err;
        }
    }

    *write = fwrite(buffer, 1, *write, f);
    return 0;
}

int io_fseek(FILE *f, uint64_t off, int origin)
{
#ifdef WIN32
    return _fseeki64(f, off, origin);
#else
    return fseeko(f, off, origin);
#endif
}

int io_ftell(FILE *f, uint64_t *off)
{
#ifdef WIN32
    *off = _ftelli64(f);
#else
    *off = ftello(f);
#endif
    return 0;
}

int io_fflush(FILE *f)
{
    return fflush(f);
}

void file_reader_clear(struct file_reader *f)
{
    if (f->f) {
        io_fclose(f->f);
    }
    memset(f, 0, sizeof(*f));
}
// open utf-8 encoding bytes filename
int file_reader_open(struct file_reader *f, const char *name)
{
    file_reader_clear(f);
    return io_fopen(&f->f, name, "rb");
}

/* read one char, return char if successful,
else return EOF
only read, not move cursor */
int file_reader_read(struct file_reader *f)
{
    int r = EOF;
    int ret;
    uint8_t v = 0;
    size_t read = 1;

    if (!(f->f)) {
        return EOF;
    }
    ret = io_fread(f->f, &v, &read, f->off);
    if (!(0 == ret && read == 1)) {
        return EOF;
    }
    return (int)(v);
}

void file_reader_next(struct file_reader *f) /* move cursor */
{
    f->off += 1;
}

uint64_t file_reader_off(struct file_reader *f)
{
    return f->off;
}

bool file_reader_eof(struct file_reader *f)
{
    uint64_t s;

    s = file_reader_size(f);
    if (s == (uint64_t)-1) {
        return true;
    }
    return f->off >= s;
}

uint64_t file_reader_size(struct file_reader *f) /* invalid size is -1 */
{
    int ret;
    uint64_t s = 0;

    if (f->f == 0) {
        return -1;
    }
    ret = io_fseek(f->f, 0, SEEK_END);
    if (ret != 0) {
        return -1;
    }
    ret = io_ftell(f->f, &s);
    if (ret != 0) {
        return -1;
    }
    return s;
}


void test_fio()
{
    const char *filename1 = "utf8-filename.txt";
    FILE *f1 = 0;
    char *filename2 = 0;
    FILE *f2 = 0;
    int ret;
    char buffer[0x100] = { 0 };
    char buffer2[0x100] = { 0 };
    size_t read;
    uint64_t size;
    struct file_reader fr;
    int8_t byte;
    bool test_ok = false;

    for (;;) {
        ret = io_fopen(&f1, filename1, "r");
        if (0 != ret) {
            break;
        }

        read = 100;
        ret = io_fread(f1, buffer, &read, -1);
        if (ret != 0) {
            break;
        }
        if (read == 0) {
            break;
        }

        ret = io_fopen(&f2, buffer, "r");
        if (0 != ret) {
            break;
        }

        read = 100;
        ret = io_fread(f2, buffer2, &read, -1);
        if (ret != 0) {
            break;
        }
        buffer2[read] = 0;

        printf("%s fio_test()\n", (0 == memcmp(buffer2, "hello", 5) ? "pass" : "fail"));


        memset(&fr, 0, sizeof(fr));
        file_reader_clear(&fr);
        ret = file_reader_open(&fr, buffer);
        size = file_reader_size(&fr);

        printf("get file size %llu\n", size);

        for (; !file_reader_eof(&fr);) {
            byte = file_reader_read(&fr);
            if (byte == EOF) {
                break;
            }
            printf("%c", (char)byte);
            file_reader_next(&fr);
        }
        printf("\n");

        file_reader_clear(&fr);
        test_ok = true;

        break;
    }

    if (f1) {
        io_fclose(f1);
    }
    if (f2) {
        io_fclose(f2);
    }

    printf("%s test_fio()\n", (test_ok ? "pass" : "fail"));
}

int main()
{
    test_fio();
    return 0;
}
