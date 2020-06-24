#ifdef WIN32
#include <windows.h>
#else
#endif

#include "fmem.h"


// for no warning
// static void __test_43C80FF_D479_4463_8DB3_F119829CAFC8() {}

#ifdef WIN32

// this implemention is not as expected.
// the file writed not back to buffer
int fmemopen2(FILE **self, void *ptr, size_t size, const char *mode)
{
    const int PATH_SIZE1 = MAX_PATH - 13;
    const int PATH_SIZE2 = MAX_PATH + 1;

    wchar_t temppath[PATH_SIZE1] = { 0 };
    DWORD r;
    int ret = -1;
    wchar_t filename[PATH_SIZE2] = { 0 };
    FILE *f = 0;
    wchar_t modew[0x10] = { 0 };
    size_t i;

    r = GetTempPathW(PATH_SIZE1, temppath);
    if (r == 0) {
        return ret;
    }


    r = GetTempFileNameW(temppath, L"fmem_", 0, filename);
    if (r == 0) {
        return ret;
    }

    for (i = 0; *mode && i < 0x0F; mode += 1, i += 1) {
        modew[i] = *mode;
    }

    ret = _wfopen_s(&f, filename, L"wb");
    if (!(ret == 0 && f)) {
        return -1;
    }

    fwrite(ptr, size, 1, f);
    fclose(f);

    ret = _wfopen_s(self, filename, modew);
    if (ret == 0 && *self) {
        // http://zh.cppreference.com/w/c/io/setvbuf
        ret = setvbuf(*self, ptr, _IOFBF, size);
        if (0 != ret) {
            fclose(*self);
            *self = 0;
        }
        // the file capacity is more,
        // when the buffer is no more, then it will write to file
        // the buffer only contains the last size bytes
    }
    return ret;
}
#else


int fmemopen2(FILE **self, void *ptr, size_t size, const char *mode)
{
    *self = fmemopen(ptr, size, mode);
    return (*self ? 0 : -1);
}

#endif // WIN32
