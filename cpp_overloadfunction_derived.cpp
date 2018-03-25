/*
注意到 Base 是基类，有同名的方法，这是在重载，同时作为子类的 Derived 覆盖了基类的 Base::error() 方法, 但是父类的另一个方法 Base::error(int ) 却没有继承下来，

我们在 Derived 类中无法以 <Derived对象>.error(1) 的形式调用。

这个页面有一个解释：

http://stackoverflow.com/questions/1628768/why-does-an-overridden-function-in-the-derived-class-hide-other-overloads-of-the

他解释说，如果有以下代码：
```cpp
struct Base
{

void foo(void *)
{

}
};

struct Derived : public Base
{

void foo(int )
{

}
};
```
如果使用 foo(NULL)，因为 NULL 被定为为 0， 会产生不知道调用哪个方法的歧义，所以， C++ 采取 "name hiding"（名字隐藏）

我认为这么解释太牵强了。同一个类中同名方法也是有可能定义的。


*/


#include <stdio.h>
#include <string.h>

struct base1
{
    virtual void error(int)
    {
        fprintf(stdout, "base1 error(int)\n");
    }
};

struct derived1 : public base1
{
    void error()
    {
        fprintf(stdout, "derived1 error() \n");
    }
};

void test1()
{
    struct derived1 d;
    d.error(); // call derived1:error()
    // d.error(1); // compile error
    /*
    错误	C2660	“derived1::error”: 函数不接受 1 个参数
*/
   
}


struct base2
{
    virtual void error()
    {
        fprintf(stdout, "base2 error()\n");
    }
    virtual void error(int)
    {
        fprintf(stdout, "base2 error(int)\n");
    }
};


struct derived2 : public base2
{
    void error()
    {
        fprintf(stdout, "derived2 error()\n");
    }
};

void test2()
{
    derived2 d;
    d.error();
    // d.error(1);
    // error C2660: “derived2::error”: 函数不接受 1 个参数
}

// 破解

struct base3
{
    virtual void error()// 是否是 virtual 结果一样
    {
        fprintf(stdout, "base3 error()\n");
    }
    virtual void error(int)
    {
        fprintf(stdout, "base3 error(int)\n");
    }
};

struct derived3 : public base3
{
    using base3::error;  // 注意

    // 如何没有这个，就会自动调用到 base3::error
    void error()
    {
        fprintf(stdout, "derived3 error()\n");
    }
};
void test3()
{
    derived3 d;
    d.error(); //  derived3 error()
        
    d.error(1); //base3 error(int)
}

int main()
{
    test3();
    return 0;
}
