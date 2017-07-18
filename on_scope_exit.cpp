
#include <stdio.h>
#include <iostream>

#include <functional>

class scope_guard_t
{
private:
    bool dismissed_;
    std::function<void()> on_exit_;
    
    scope_guard_t(const scope_guard_t &);
    void operator = (const scope_guard_t &);
public:
    explicit scope_guard_t(std::function<void()> f) :on_exit_(f), dismissed_(false) {
        ;
    }
    ~scope_guard_t() {
        if (!dismissed_) {
            on_exit_();
        }
    }

    void dismiss() {
        dismissed_ = true;
    }

};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)
#define ON_SCOPE_EXIT(callback) scope_guard_t SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)


int main()
{
    FILE * f = fopen("on_scope_exit.cpp", "rb");
    ON_SCOPE_EXIT([&]  {
        if (f)  {  printf("close file\n"); fclose(f);  f = NULL; 
        }
    });
    

    return 0;
}