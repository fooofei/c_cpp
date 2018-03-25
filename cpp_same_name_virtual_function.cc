
/*

 有一次一个新人来到项目组 给他了新的任务 他发现现在的公共接口中有个方法名字他可以用 
 比如名字叫 foo(int), 参数为一个 int
 但是参数不符合他的要求  于是他就在原有接口类的最后增加了一个同名的重载方法
 foo(const char *) ，参数为 const char *

 这个方法他增加了后， 我发现原来的功能不正常了 调用到 foo(int)的地方开始去调用
 他的 foo(const char *)了 

 当然了，我们前提是不能更改公共接口的，既然更改了，这个现象我做了研究。



*/

#include <stdio.h>


struct test
{
    virtual void func1(void *) {}
    virtual void func1(int) {}
};

/*

Vistual studio 2012, 2015 版本验证: 在虚函数表中，是 `void func1(int)` 在 `void func1(void *)` 的前面。
甚至是倒序，但在 gcc 中不是这样的。
*/


struct IA
{
    virtual void show_a() = 0;
};

struct  IB
{
    virtual void show_b() = 0;
};

struct A : public IA, public IB
{
    A()
    {
        printf("A\n");
    }
    ~A()
    {
        printf("~A\n");
    }
    void show_a()
    {
        printf("A:show_a\n");
    }
    void show_b()
    {
        printf("A:show_b\n");
    }
};


int main(int argc, const char ** argv)
{
    /*
    dynamic_cast 可用于`父1`指针转到`父2`指针。
    pa pb 地址相差 4 ， 在 x86 platform.
    因此，在实现 `QueryInterface()` 时，应该使用 *pv = static_cast<父类指针>(子类指针-this));
    */
    A * p = new A;
    IB * pb = p;
    IA * pa = p;
    ((IB*)pa)->show_b();  // 从这里看出 我们要使用 IA 指针 ， 不能把 IB 指针地址强转，这样做不行
    reinterpret_cast<IB*>(pa)->show_b();
    dynamic_cast<IB*>(pa)->show_b();
    // output
    // A:show_a
    // A:show_a
    // A:show_b
    delete p;
    return 0;
}

