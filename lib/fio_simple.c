// You can embeded this code to other .c files
// not handle the filename's encoding


int io_fopen(FILE **f, const char *name, const char *mode);
int io_fread(FILE *f, void *buffer, size_t *read, uint64_t off);
int io_fwrite(FILE *f, const void *buffer, size_t *write, uint64_t off);
int io_fseek(FILE *f, uint64_t off, int origin);
int io_ftell(FILE *f, uint64_t *off);
int io_fsize(FILE *f, uint64_t *size);


int io_fopen(FILE **f, const char *name, const char *mode)
{
#ifdef WIN32
    // return 0 if successful
    return fopen_s(f, name, mode);
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

int io_fsize(FILE *f, uint64_t *size)
{
    int ret;
    uint64_t s = 0;

    ret = io_fseek(f, 0, SEEK_END);
    if (ret != 0) {
        return -1;
    }
    ret = io_ftell(f, &s);
    if (ret != 0) {
        return -1;
    }
    *size = s;
    return 0;
}
