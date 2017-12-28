
#include <stdio.h>
#include <stdint.h>

struct sa_t
{
    struct common_t
    {
        uint32_t head;
    }cm;

    uint32_t data_sa;
};

struct sb_t
{
    struct sa_t::common_t cm; //struct common_t cm; // error
    uint32_t data_sb;
};



int main()
{

    printf("sizeof(sa_t) =%d, sizeof(sb_t)=%d\n"
        , sizeof(struct sa_t)
        , sizeof(struct sb_t)
        );// 8 8
   
    return 0;
}
