
#include <locale.h>
#include <string>

int print_chs_w(const wchar_t *, ...);
int print_chs_w(const wchar_t *, size_t);
int print_chs_w(const std::wstring &);

/* string is utf-8 encoding */
int print_chs(const char *, ...);
int print_chs(const char *, size_t);
int print_chs(const std::string &);