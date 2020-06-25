//
// https://stackoverflow.com/questions/29116354/should-i-free-the-pointer-returned-by-setlocale


#include <string.h>
#include <locale.h>

int main()
{
    char *ret = setlocale(LC_ALL, 0);

    // free(ret); ? NOT free.

    return 0;
}
