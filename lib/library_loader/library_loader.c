// common includes
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h> // calloc
#include <string.h> // memset memcpy strlen

#ifdef WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

// self includes
#include "library_loader.h"


#ifdef WIN32

static int utf8_2_utf16le_windows(const char *src, size_t src_size, wchar_t *dst, size_t *dst_size)
{
    int r = MultiByteToWideChar(CP_UTF8, 0, src, (int)(src_size), dst, (int)(*dst_size));
    if (r <= 0) {
        return -1;
    } else {
        *dst_size = (size_t)(r);
        return 0;
    }
}

static int utf8_2_utf16le_windows2(const char *src, size_t src_size, wchar_t **out_dst, size_t *out_dst_size)
{
    int ret;
    wchar_t *dst = 0;
    size_t dst_size = 0;
    size_t rsize = 0;

    ret = utf8_2_utf16le_windows(src, src_size, 0, &dst_size);
    if (0 == ret && dst_size < 0x10000) {
        // remember to call free()
        dst_size += 1;
        dst = (wchar_t *)calloc(1, sizeof(wchar_t) * dst_size);
        if (!dst)
            return -ENOMEM;
        rsize = dst_size;
        utf8_2_utf16le_windows(src, src_size, dst, &rsize);
        if (rsize >= dst_size) {
            free(dst);
            return -1;
        }
        *out_dst = dst;
        dst[rsize] = 0;
        *out_dst_size = rsize;
        return 0;
    }
    return -1;
}

struct library_loader {
    HMODULE handle;
    char fullpath[1];
};

int library_open(struct library_loader **outins, const char *path)
{
    int ret;
    struct library_loader *ins = 0;
    wchar_t *pathw = 0;
    size_t pathw_size = 0;
    size_t path_size = strlen(path);
    size_t ins_size = 0;


    for (;;) {
        ret = utf8_2_utf16le_windows2(path, path_size, &pathw, &pathw_size);

        if (0 != ret) {
            ret = -1;
            break;
        }

        ins_size = sizeof(*ins) + path_size + 1;
        ins = calloc(1, ins_size);
        if (!ins) {
            ret = -4;
            break;
        }

        memcpy(ins->fullpath, path, path_size);
        ins->fullpath[path_size] = 0;

        ins->handle = LoadLibraryW(pathw);
        if (!ins->handle) {
            ret = -1;
            break;
        }

        *outins = ins;
        free(pathw);
        return 0;

        break;
    }

    // errors
    if (pathw) {
        free(pathw);
    }
    if (ins) {
        free(ins);
    }

    return ret;
}

void library_close(struct library_loader **ins)
{
    if ((*ins)->handle) {
        FreeLibrary((*ins)->handle);
    }
    free(*ins);
    *ins = 0;
}

int library_sym(struct library_loader *ins, const char *symbol, void **proc)
{
    *proc = (void *)GetProcAddress(ins->handle, symbol);
    return (*proc != 0) ? 0 : -1;
}

const char *library_fullpath(struct library_loader *ins)
{
    return ins->fullpath;
}

#else


struct library_loader {
    void *handle;
    char fullpath[1];
};

// #define RTLD_LAZY 1
// #define RTLD_NOW 2
// #define RTLD_GLOBAL 4
// #define RTLD_LOCAL 8

int library_open(struct library_loader **outins, const char *path)
{
    size_t path_size = strlen(path);
    struct library_loader *ins = 0;
    int ret = 0;


    for (;;) {
        ins = calloc(1, sizeof(*ins) + path_size + 1);
        if (!ins) {
            ret = -1;
            break;
        }

        memcpy(ins->fullpath, path, path_size);
        ins->fullpath[path_size] = 0;

        ins->handle = dlopen(path, RTLD_NOW);
        if (!ins->handle) {
            ret = -2;
            break;
        }
        *outins = ins;
        return 0;

        break;
    }


    return ret;
}

void library_close(struct library_loader **ins)
{
    dlclose((*ins)->handle);
    free(*ins);
    *ins = 0;
}

int library_sym(struct library_loader *ins, const char *symbol, void **proc)
{
    *proc = dlsym(ins->handle, symbol);
    return (*proc != 0) ? 0 : -1;
}

const char *library_fullpath(struct library_loader *ins)
{
    return ins->fullpath;
}


#endif
