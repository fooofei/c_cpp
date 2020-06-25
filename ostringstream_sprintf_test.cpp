#include <sstream>
#include <stdio.h>
int main()
{
    std::ostringstream oss;

    oss << "123"
        << " " << 4 << 5;

    printf("%s\n", oss.str().c_str());

    // reset the stream
    oss.str(std::string());
    oss.clear();

    oss << 1.345f;

    printf("%s\n", oss.str().c_str());

    printf("hello\n");
    return 0;
}
