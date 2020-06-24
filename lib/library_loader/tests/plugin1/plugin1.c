#include <string.h>
#include <stdlib.h>

#include "../api.h"


struct plugin1_internal_struct {
    uint32_t size;
    char name[0];
};


int WINAPI plugin1_create(void **handle)
{
    if (!handle) {
        return -1;
    }

    struct plugin1_internal_struct *p;
    const char *v = "plugin1_sig";
    size_t size = strlen(v);

    /* also called memset */
    p = (struct plugin1_internal_struct *)calloc(1, sizeof(struct plugin1_internal_struct) + sizeof(char) * (size + 1));

    if (!p) {
        return -1;
    }
    p->size = (uint32_t)size;
    memcpy(p->name, v, size);

    *handle = p;
    return 0;
}

/* append text with arg1, and write to arg2.  pass capacity by size, and return the used size. */
int WINAPI plugin1_dowork(void *handle, const char *arg1, char *arg2, uint32_t *size)
{
    if (!handle) {
        return -1;
    }

    if (!size) {
        return -1;
    }

    uint32_t need_size;
    struct plugin1_internal_struct *ins;

    ins = (struct plugin1_internal_struct *)handle;
    need_size = (uint32_t)strlen(arg1) + ins->size;


    if (*size <= need_size) {
        if (arg2) {
            return -1;
        }
        *size = need_size + 1;
    } else {
        memcpy(arg2, arg1, strlen(arg1));
        memcpy(arg2 + strlen(arg1), ins->name, ins->size);
        arg2[need_size] = 0;
        *size = need_size;
    }
    return 0;
}

int WINAPI plugin1_release(void **handle)
{
    if (!handle) {
        return -1;
    }

    free(*handle);
    *handle = 0;
    return 0;
}


#ifdef _cplusplus
extern "C"
#endif
    API_EXPORT int ABI_CALL
    init_export_functions(struct export_functions_table *p)
{
    uint32_t function_count = 0;
    uint32_t cb;
    enum {
        version1_function_count = 3,
    };

    cb = p->cb;
    if (!(cb >= 4)) {
        return -1;
    }
    cb -= 4;
    function_count = cb / sizeof(void *);


    switch (function_count) {
            // larger up
        case version1_function_count:
            p->pfn_plugin_create = plugin1_create;
            p->pfn_plugin_dowork = plugin1_dowork;
            p->pfn_plugin_release = plugin1_release;

            /* write back the size, to match the version. */
            p->cb = 4 + version1_function_count * sizeof(void *);

            return 0;
        default:
            break;
    }


    return -1;
}
