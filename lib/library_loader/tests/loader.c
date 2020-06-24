#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "api.h"

#include "library_loader.h"


void _framework(const char *plugin_name)
{
    void *handle = 0;

    struct library_loader *library = 0;

    enum {
        size = 0x20,
    };
    uint32_t buffer_size = size;
    char buffer[size] = { 0 };
    struct export_functions_table exp;
    int ret;

    memset(&exp, 0, sizeof(exp));

    for (;;) {
        ret = library_open(&library, plugin_name);
        if (ret != 0) {
            fprintf(stderr, "%s:%d fail dlopen %s\n", __FILE__, __LINE__, plugin_name);
            break;
        }

        PFN_init_export_functions pfn = 0;
        ret = library_sym(library, "init_export_functions", (void **)&pfn);
        if (ret != 0 || !pfn) {
            fprintf(stderr, "%s:%d fail dlsym\n", __FILE__, __LINE__);
            break;
        }


        exp.cb = sizeof(exp);
        ret = pfn(&exp);
        if (ret != 0) {
            fprintf(stderr, "%s:%d fail pfn\n", __FILE__, __LINE__);
            break;
        }

        if (exp.cb != sizeof(exp)) {
            fprintf(stderr, "%s:%d warning: API version not match\n", __FILE__, __LINE__);
        }

        ret = exp.pfn_plugin_create(&handle);
        if (ret != 0) {
            fprintf(stderr, "%s:%d fail plugin create\n", __FILE__, __LINE__);
            break;
        }

        ret = exp.pfn_plugin_dowork(handle, "nice_", buffer, &buffer_size);

        fprintf(stdout, "buffer_size=%u, buffer=%s\n", buffer_size, buffer);

        break;
    }

    if (library && handle && exp.pfn_plugin_release) {
        exp.pfn_plugin_release(&handle);
    }
    if (library) {
        library_close(&library);
    }
}


int main()
{
#ifdef WIN32
    _framework("../bin/plugin1.dll");
#else
    _framework("../bin/libplugin1.so");
#endif

    printf("main end\n");

    return 0;
}
