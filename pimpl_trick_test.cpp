
// A version of Pimpl implemetation by cloudwu

// ref https://gist.github.com/cloudwu/0e530864e5dcb77021fb9dd550390e84
// ref http://en.cppreference.com/w/cpp/language/pimpl


// I change some.




// interface (widget.h)
class widget {
    struct impl; // please use struct not use class
public:
    static widget* create(int);  // replacement of new, MUST, cannot use ctor, will stackoverflow
    void release() const;  // replacement of delete, MUST, cannot use dtor, will stackoverflow


    void draw() const; // public API that will be forwarded to the implementation
    void draw();
    bool shown() const { return true; } // public API that implementation has to call
};

// implementation (widget.cpp)


// #include "widget.h"
#include <iostream>
#define DESC_SELF impl * self = static_cast<impl *>(this);
#define CONST_SELF const impl * self = static_cast<const impl *>(this);

struct widget::impl : public widget {
    int n; // private data
    impl(int n) : n(n) {}
};

widget * widget::create(int n) {
    return new impl(n);
}

void widget::release() const {
    CONST_SELF
    delete self;
}

void widget::draw() const {
    CONST_SELF
    if(self->shown()) { // this call to public member function requires the back-reference
        std::cout << "drawing a const widget " << self->n << '\n';
    }
}

void widget::draw() {
    DESC_SELF
    if(self->shown()) {
        std::cout << "drawing a non-const widget " << self->n << '\n';
    }
}


// user (main.cpp)
int main()
{
    widget *w = widget::create(7);
    const widget *w2 = w;
    w->draw();
    w2->draw();
    w->release();

}

/*
drawing a non-const widget 7
drawing a const widget 7
*/
