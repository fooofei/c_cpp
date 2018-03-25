
#include <string.h>
#include <stdio.h>
#include <ctype.h>

#define EXPECT(expr) \
    do { \
    if(!(expr)) \
        { \
        fprintf(stderr, "unexpect %s  (%s:%d)\n",#expr, __FILE__, __LINE__); \
        fflush(stderr);\
        } \
    } while (0)

char n_tolower(char c)
{
    return c | 0x20;
}

char n_toupper(char c)
{
    // MUST have this
    if (c >= 'a' && c <= 'z')
    {
        return c & 0xdf;
    }
    return c;
}


void test_upper_lower()
{
    char * s_upper = "12345ABCDEFG\0";
    char * s_lower = "12345abcdefg\0";

    char * p_u;
    char * p_l;
 
    for (p_u = s_upper, p_l =s_lower;*p_u; p_u+=1, p_l += 1)
    {

        EXPECT(*p_u == toupper(*p_l));
        EXPECT(*p_l == tolower(*p_u));

        EXPECT(*p_u == n_toupper(*p_l));
        EXPECT(*p_l == n_tolower(*p_u));


        EXPECT(*p_l == n_tolower(*p_l));
        EXPECT(*p_u == n_toupper(*p_u));

        // for add breakpoint
        if (*p_u != n_toupper(*p_l))
        {
            printf("");
        }
        if (*p_l != n_tolower(*p_u))
        {
            printf("");
        }
      
    }

    printf("%s() pass\n", __FUNCTION__);

}


int main()
{
    test_upper_lower();
    return 0;
}
