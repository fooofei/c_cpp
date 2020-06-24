
#include <stdio.h>
#include <string>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#include "whereami/whereami.h"
#endif
#ifdef __cplusplus

}
#endif

#include "print_chs/print_chs.h"

#ifdef WIN32
#include <windows.h>
#ifndef WINAPI
// #define WINAPI __stdcall // even extern "C" cannot resolve name mangling
#define WINAPI
#endif // WINAPI

#define CPP_EXPORT __declspec(dllexport)
#define GNUC_DLL_LOAD
#else
#ifndef WINAPI
#define WINAPI
#endif // WINAPI
#define CPP_EXPORT __attribute__((visibility("default")))
#define GNUC_DLL_LOAD __attribute__((constructor))
#endif


/*
Found a bug on posix:
When `executable` and `library` all linked `whereami.cpp`,
at the runtime and debug, `executable` and `library` 's get_library_fullpath() symbol 
all from `executable`, so the get_library_fullpath() in the `library` is get the 
`executable`'s fullpath, it's not right.

To fix this, add compile option to executable/library, any of two is ok.
#target_compile_options(${PROJECT_NAME} PRIVATE -fvisibility=hidden)

*/



void
test_w()
{
    int err;
    std::wstring s1;
    wchar_t * p = 0;
    size_t  size = 0;

    err = get_executable_fullpath_w(&p, &size);
    if (p)
    {
        s1.assign(p, size);
        free(p);
        p = 0; size = 0;
    }

    printf("library->get_executable_fullpathw() return:%d, addr:%p, size:%zu\n", err, s1.c_str(), s1.size());
    s1.append(L"\n");
    print_chs_w(s1);

    s1.clear();
    err = get_library_fullpath_w(&p, &size);
    if (p)
    {
        s1.assign(p, size);
        free(p);
        p = 0; size = 0;
    }

    printf("library->get_library_fullpathw() return:%d, addr:%p, size:%zu\n", err, s1.c_str(), s1.size());
    s1.append(L"\n");
    print_chs_w(s1);
}

void 
test()
{
    int err;
    std::string s1;
    char * p = 0;
    size_t  size = 0;

    err = get_executable_fullpath(&p, &size);
    if (p)
    {
        s1.assign(p, size);
        free(p);
        p = 0; size = 0;
    }

    printf("library->get_executable_fullpath() return:%d, addr:%p, size:%zu\n", err, s1.c_str(), s1.size());
    s1.append("\n");
    print_chs(s1);

    s1.clear();
    err = get_library_fullpath(&p, &size);
    if (p)
    {
        s1.assign(p, size);
        free(p);
        p = 0; size = 0;
    }

    printf("library->get_library_fullpath() return:%d, addr:%p, size:%zu\n", err, s1.c_str(), s1.size());
    s1.append("\n");
    print_chs(s1);

}

extern "C" CPP_EXPORT int
    run(void)
{
    
    test();
    test_w();
    return 0;
}

/*
    Function pointer type.
*/
typedef int( *PFNrun)(void);

GNUC_DLL_LOAD
static int load()
{
    // Called when the library was load.
    return 0;
}
