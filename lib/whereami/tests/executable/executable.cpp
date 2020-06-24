
#include <stdio.h>
#include <string.h>
#include <string>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "whereami/whereami.h"
#include "library_loader.h"


#ifdef __cplusplus
}
#endif

#include <crt_dbg_leak.h>
#include "print_chs/print_chs.h"


typedef int(*PFNrun)(void);
_crt_dbg_leak_t g_cdl(0);

int main()
{

    int err;
    std::wstring s1;
    std::string s2;
    library_loader_t handle=0;
    wchar_t * wp = 0;
    size_t wsize = 0;
    char * p = 0;
    size_t size = 0;

    

    err = get_executable_fullpath_w(&wp,&wsize);
    if (wp)
    {
        s1.assign(wp, wsize);
        free(wp);
        wp = 0; wsize = 0;
    }

    printf("executable->get_executable_fullpathw() return:%d, addr:%p, size:%zu\n", err, s1.c_str(), s1.size());
    s1.append(L"\n");
    print_chs_w(s1);

    // test utf-8 print
    err = get_executable_fullpath(&p,&size);
    if (p)
    {
        s2.assign(p, size);
        free(p);
        p = 0; size = 0;
    }


    printf("executable->get_executable_fullpath() return:%d, addr:%p, size:%zu\n", err, s2.c_str(), s2.size());
    s2.append("\n");
    print_chs(s2);

    printf("\n");

    s1.clear();
    err = get_library_fullpath_w(&wp,&wsize);
    if (wp)
    {
        s1.assign(wp, wsize);
        free(wp);
        wp = 0; wsize = 0;
    }

    printf("executable->get_library_fullpathw() return:%d, addr:%p, size:%zu\n", err, s1.c_str(), s1.size());
    s1.append(L"\n");
    print_chs_w(s1);

    s2.clear();
    err = get_library_fullpath(&p,&size);
    if (p)
    {
        s2.assign(p, size);
        free(p);
        p = 0; size = 0;
    }
    printf("executable->get_library_fullpath() return:%d, addr:%p, size:%zu\n", err, s2.c_str(), s2.size());
    s2.append("\n");
    print_chs(s2);

    printf("\n");


    

#ifdef __linux__
    library_open(&handle, "./libwhereamitestlib.so");
#elif defined(__APPLE__)
    library_open(&handle, "./libwhereamitestlib.dylib");
#elif defined(WIN32)
#ifdef _DEBUG
    // visual studio's relative path
    library_open(&handle, "../../bin/whereamitestlib.dll");
#else
    library_open(&handle, "whereamitestlib.dll");
#endif
    
#else
#pragma error('not support')
#endif
    if (handle)
    {
        PFNrun fn = 0;
        library_sym(handle, "run", (void**)&fn);
        if (fn)
        {
            fn();
        }
        else
        {
            printf("executable->false dlsym\n");
            err = -1;
        }

        library_close(&handle);
        handle = 0;
    }
    else
    {
        printf("executable->false dlopen\n");
        err = -1;
    }

    return err;
}
