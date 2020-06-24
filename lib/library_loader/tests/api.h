#include <stdint.h>


#ifdef WIN32
#include <windows.h>
#define WINAPI __stdcall
#define ABI_CALL __cdecl
#define API_EXPORT __declspec(dllexport)
#else
#define WINAPI
#define ABI_CALL
#define API_EXPORT __attribute__((visibility("default")))
#endif


#pragma pack(push, 1)

struct export_functions_table {
    uint32_t cb;

    int(WINAPI *pfn_plugin_create)(void **handle);

    /* append text with arg1, and write to arg2.  pass capacity by size, and return the used size. */
    int(WINAPI *pfn_plugin_dowork)(void *handle, const char *arg1, char *arg2, uint32_t *size);

    int(WINAPI *pfn_plugin_release)(void **handle);

    /* extend functions here */
};
#pragma pack(pop)


#ifdef _cplusplus
extern "C"
#endif
    API_EXPORT int ABI_CALL
    init_export_functions(struct export_functions_table *);

typedef int(ABI_CALL *PFN_init_export_functions)(struct export_functions_table *);
