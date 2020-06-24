#include <string>

#include "datetime.h"


int strftime2(const struct tm *t, const char *fmt, std::string &s)
{
    size_t r = 0;
    std::string temps;

    // strftime() if success, return writed size, not include '\0'
    //   else return 0
    for (temps.resize(0x10); (r = strftime(&temps[0], temps.size(), fmt, t)) == 0 && temps.size() < 0x10000;
        temps.resize(temps.size() * 2))
        ;

    if (r > 0) {
        s.assign(temps.c_str(), r);
        return 0;
    }
    return -1;
}


int main()
{
    return 0;
}
