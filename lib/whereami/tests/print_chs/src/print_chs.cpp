
#include <stdarg.h>
#include <stdio.h>

#ifdef WIN32
#include <Windows.h>
#endif

#include "../include/print_chs/print_chs.h"

int print_chs_w(const wchar_t *ptr, size_t size)
{
#ifdef WIN32
    const char *local = "chs";
#else
    const char *local = "zh_CN.UTF-8";
#endif

    char *restore = setlocale(LC_ALL, local);
    size_t bytes_size = size * sizeof(wchar_t);
    // printf("[addr:0x%p - size:%u]value:(%.*ls)\n",(const void*)ptr,size,p_size,ptr);
    printf("%.*ls", (int)bytes_size, ptr);
    setlocale(LC_ALL, restore);
    return size;
}

int print_chs_w(const std::wstring &s)
{
    return print_chs_w(s.c_str(), s.size());
}

int print_chs_w(const wchar_t *format, ...)
{
    std::wstring s;
    int ret;
    s.resize(4096);

    va_list args;
    va_start(args, format);
    ret = vswprintf(&s[0], s.size(), format, args);
    va_end(args);

    if (ret > 0)
    {
        return print_chs_w(s.c_str(), ret);
    }
    return ret;
}

int print_chs(const char *format, ...)
{
    std::string s;
    int ret;
    s.resize(4096);

    va_list args;
    va_start(args, format);
    ret = vsnprintf(&s[0], s.size(), format, args);
    va_end(args);

    if (ret > 0)
    {
        return print_chs(s.c_str(), ret);
    }
    return ret;
}
int print_chs(const char *ptr, size_t size)
{
    /*
    try on Windows:
    _setmode(_fileno(stdout), _O_U8TEXT); // crash, debug assert
    SetConsoleOutputCP(CP_UTF8); // not work
    
    have not found the way to print utf-8 bytes string on Windows.

    */
    
#ifdef WIN32
    // found a way to print utf-8 bytes on Windows.
    // !!! Do not use printf, it cannot.
    UINT __cp;
    BOOL r;
    DWORD written;
    static HANDLE __std_out_handle = GetStdHandle(STD_OUTPUT_HANDLE);

    fflush(stdout);
    __cp = GetConsoleOutputCP();
    // This function have side effect, it will change the console's size.
    r = SetConsoleOutputCP(CP_UTF8);
    if (r != TRUE) return -1;
    // WriteConsoleA() cannot use redirect to file(">").
    //r = WriteConsoleA(__std_out_handle, ptr, size, &written, NULL);
    r = WriteFile(__std_out_handle, ptr, size, &written, NULL);
    SetConsoleOutputCP(__cp);
    if (r == TRUE) return size;
    else return -1;

#else
    const char *local = "zh_CN.UTF-8";
    char *restore = setlocale(LC_ALL, local);
    printf("%.*s", (int)size, ptr);
    setlocale(LC_ALL, restore);
    return size;
#endif // !WIN32

}
int print_chs(const std::string &s)
{
    return print_chs(s.c_str(), s.size());
}