
// common includes
#include <stdlib.h>
#include <string.h>


// platform includes
#ifdef WIN32
#include <Windows.h>
#else
#endif // WIN32


// self includes
#include "whereami/whereami.h"




#ifdef WIN32

// wrap the `GetModuleFileNameW()`  to return a specific fullpath
static int 
_get_module_fullpath(HMODULE m, wchar_t ** out_ptr, size_t * out_size)
{

    size_t size=0;
    wchar_t * buffer = 0;
    DWORD r = 0;


    for (size=0x10;;)
    {
        buffer = (wchar_t*)calloc(1, size*sizeof(wchar_t));
        if (!buffer) return -ENOMEM;
        
        // If buffer is NULL, not defined.
        // If buffer is small, return a smalled size.
        // If succeed, return value not include the terminating null character.
        r = GetModuleFileNameW(m, buffer, (DWORD)size);
        if (r == 0)
        {
            free(buffer);
            return -1;
        }

        if (r < size)
        {
            *out_ptr = buffer;
            *out_size = r;
            return 0;
        }

        free(buffer);
        buffer = 0;
        size *= 2;
    }
    
    return -2;
}

int 
get_library_fullpath_w(wchar_t ** out_ptr, size_t * out_size)
{
    HMODULE m = NULL;
    BOOL b = GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCTSTR)get_library_fullpath, &m);
    if (!b)
        return -1;
    if (!m)
        return -2;

    return _get_module_fullpath(m, out_ptr, out_size);
}

int get_executable_fullpath_w(wchar_t ** out_ptr, size_t * out_size)
{
    return _get_module_fullpath(NULL, out_ptr, out_size);
}


static int
utf16le_2_utf8_windows(const wchar_t *src, size_t src_size, char *dst, size_t *dst_size)
{
    int r = WideCharToMultiByte(CP_UTF8, 0, src, (int)(src_size), dst, (int)(*dst_size), NULL, NULL);
    if (r <= 0)
    {
        return -1;
    }
    else
    {
        *dst_size = (size_t)(r);
        return 0;
    }
}

static int
utf16le_2_utf8_windows2(const wchar_t *src, size_t src_size, char ** out_dst, size_t * out_dst_size)
{
    int ret;
    char * dst = 0;
    size_t dst_size = 0;
    size_t rsize = 0;

    ret = utf16le_2_utf8_windows(src, src_size, 0, &dst_size);
    if (0 == ret && dst_size < 0x10000)
    {
        // remember to call free()
        dst_size += 1;
        dst = (char *)calloc(1, dst_size);
        if (!dst) return -ENOMEM;
        rsize = dst_size;
        utf16le_2_utf8_windows(src, src_size, dst, &rsize);
        if (rsize >= dst_size)
        {
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

int 
get_library_fullpath(char ** out_ptr, size_t  * out_size)
{
    int ret;
    wchar_t * ptrw=0;
    size_t sizew=0;

    ret = get_library_fullpath_w(&ptrw, &sizew);
    if (0 != ret) return -1;

    ret = utf16le_2_utf8_windows2(ptrw, sizew, out_ptr, out_size);
    free(ptrw);
    return ret;
}

int 
get_executable_fullpath(char ** out_ptr, size_t * out_size)
{
    int ret;
    wchar_t * ptrw = 0;
    size_t sizew = 0;

    ret = get_executable_fullpath_w(&ptrw, &sizew);
    if (0 != ret) return -1;

    ret = utf16le_2_utf8_windows2(ptrw, sizew, out_ptr, out_size);
    free(ptrw);
    return ret;
}


#endif //WIN32

#ifdef __linux__

#include <stdlib.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
/* for gcc 4.1.2 -- CentOS release 5.11 */
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

static unsigned 
get_path_max_size()
{
  unsigned path_max;
  // http://man7.org/linux/man-pages/man3/realpath.3.html
#ifdef PATH_MAX
  path_max = PATH_MAX;
#else
  path_max = pathconf(path, _PC_PATH_MAX);
  if (path_max <= 0)
    path_max = 4096;
#endif
  return path_max;
}

/* string is utf-8 bytes*/
static int 
get_executable_fullpath_utf8(char ** out_ptr, size_t * out_size)
{
  const char *self = "/proc/self/exe";
  char *ret;


  // remember to call free()
  ret = realpath(self, NULL);
  if (!ret)
    return -1;
  
  *out_ptr = ret;
  *out_size = strlen(ret);
  return 0;
}

/* string is utf-8 bytes */
static int
get_library_fullpath_utf8(char ** out_ptr, size_t * out_size)
{
    FILE *fmaps = NULL;
    const char *pmaps = "/proc/self/maps";
    char line[0x800];
    char *p;
    const unsigned path_max = get_path_max_size();
    int retval = -1;
    int retval_temp;
    uint64_t addr0 = (uint64_t)(uintptr_t)(void *)"";
    (void)addr0;
    uint64_t addr1 = (uint64_t)(uintptr_t)(void *)get_library_fullpath_utf8;
    (void)addr1;
    uint64_t addr = (uint64_t)(uintptr_t)(void *)__builtin_extract_return_addr(__builtin_return_address(0));


    // line
    uint64_t low;
    uint64_t high;
    char perms[5];
    uint64_t offset;
    uint32_t major;
    uint32_t minor;
    uint32_t inode;
    enum { fmt_size = 0x40, };
    char fmt[fmt_size + 1];
    char * scanf_path = (char *)calloc(1, path_max + 1);
    *out_ptr = 0;
    *out_size = 0;

    fmaps = fopen(pmaps, "r");

    if (fmaps)
    {
        for (; !feof(fmaps) && (*out_ptr == 0);)
        {
            memset(line, 0, sizeof(line));
            p = fgets(line, sizeof(line), fmaps);

            if (p)
            {
                /* Address like 7f95d4cbd000-7f95d4e7c000 r-xp 00000000 08:01 2257311  /lib/x86_64-linux-gnu/libc-2.23.so */
                fmt[0] = 0;
                snprintf(fmt, fmt_size, "%s", "%"PRIx64"-%"PRIx64" %s %"PRIx64" %x:%x %u %s\n");

                scanf_path[0] = 0;
                retval_temp = sscanf(p, fmt,
                    &low, &high, perms, &offset, &major, &minor, &inode, scanf_path);
                scanf_path[path_max] = 0;
                if (retval_temp == 8)
                {
                    // Must be the the number of items in the argument list
                    if (addr >= low && addr <= high)
                    {
                        *out_ptr = realpath(scanf_path, NULL);
                        if (!*out_ptr)
                        {
                            retval = -1;
                            break;
                        }
                        retval = 0;
                        *out_size = strlen(*out_ptr);
                    }
                }


            }
        }

        fclose(fmaps);
    }
    free(scanf_path);

    return retval;
}

#endif // __linux__

#ifdef __APPLE__

#define _DARWIN_BETTER_REALPATH
#include <mach-o/dyld.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <dlfcn.h>

static int
get_executable_fullpath_utf8(char ** out_ptr, size_t * out_size)
{
    char primer[8];
    uint32_t size = 7;
    int retval_temp;
    const char * p=0;
    char * palloc = 0;
    uint32_t rsize = 0;

    retval_temp = _NSGetExecutablePath(primer, &size);
    // size include the '\0' .
    if (-1 == retval_temp && size < 0x10000)
    {
        //size +=1; // No need plus 1
        // If the size is large than the need size, then only return input size.
        // Not the real size.
        palloc = (char *)calloc(1, size);
        rsize = size;
        retval_temp = _NSGetExecutablePath(palloc, &rsize);
        if (rsize > size)
        {
            free(palloc);
            return -1;
        }
        size = rsize;
        if (retval_temp == 0 && palloc[size] == 0) {
            size -= 1;
        }
        p = palloc;
    }
    else {
        p = primer;
    }
    if (0 != retval_temp || p ==0) {
        free(palloc);
        return -1;
    }

    *out_ptr = realpath(p, NULL);

    if (palloc)
    {
        free(palloc);
    }
    if (*out_ptr)
    {
        *out_size = strlen(*out_ptr);
        return 0;
    }
    return -1;
}

static int 
get_library_fullpath_utf8(char ** out_ptr, size_t * out_size)
{
    Dl_info info;
    const char *cp;
    int retval;

    memset(&info, 0, sizeof(info));
    retval = dladdr((void *)"", &info);
    if (retval != 0)
    {
        cp = info.dli_fname;
        *out_ptr = realpath(cp, 0);
        if (*out_ptr)
        {
            *out_size = strlen(*out_ptr);
            return 0;
        }
    }
    return -1;
}


#endif // __APPLE__

#ifndef WIN32

int 
get_library_fullpath(char ** arg0, size_t * arg1)
{
  return get_library_fullpath_utf8(arg0,arg1);
}

int get_executable_fullpath(char ** arg0, size_t * arg1)
{
  return get_executable_fullpath_utf8(arg0, arg1);
}
int 
get_library_fullpath_w(wchar_t ** arg0, size_t * arg1)
{
    (void)arg0;
    (void)arg1;
    return -1;
}
int 
get_executable_fullpath_w(wchar_t ** arg0, size_t * arg1)
{
    (void)arg0;
    (void)arg1;
    return -1;
}

#endif // !WIN32
