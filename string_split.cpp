

#include <stdio.h>
#include <string>
#include <vector>
#include <list>


//
// Split argument s use deimiter, push back to container v.
// delimiter can be substring.
// container_type's element is string_type.
//
// container_type must implement push_back(), end().
// value_type must implement empty(), size(), 
// find(const value_type&, size_t), assign(), c_str().

template<typename container_type>
void split(container_type & v
    , const typename container_type::value_type & s
    , const typename container_type::value_type & delimiter)
{
    // Will infinit loop.
    if (delimiter.empty()) return;

    typedef typename container_type::value_type value_type;
    
    size_t off = 0, off_begin = 0;

    for (; off_begin < s.size()
        // find_first_of() cannot find substring
        && (off=s.find(delimiter,off_begin)) <s.size()   
        ;)
    {
        // Many implements not notice it, if off == off_begin, 
        // it will push empty value to container.
        // implement such as not notice :
        // https://stackoverflow.com/questions/5167625/splitting-a-c-stdstring-using-tokens-e-g
        // https://stackoverflow.com/questions/236129/split-a-string-in-c
        if (off>off_begin) 
        {
            v.push_back(value_type());
            value_type & ref = *(--v.end());
            ref.assign(s.c_str() + off_begin, off - off_begin);
        }

        off_begin = off + delimiter.size();
    }

    if (off_begin<s.size())
    {
        v.push_back(value_type());
        value_type & ref = *(--v.end());
        ref.assign(s.c_str() + off_begin, s.size() - off_begin);
    }


}


#ifndef EXPECT
#define EXPECT(expr) \
    do { \
    if(!(expr)) \
        { \
        fprintf(stderr, "unexpect-> %s:%d\n", __FILE__, __LINE__); \
        exit(1); \
        } \
    } while (0)
#endif


int test_split()
{
    std::vector<std::string > v;

    split(v, std::string("hello world"), std::string(" "));
    EXPECT(v.size() == 2);
    v.clear();

    split(v, std::string("hello  world"), std::string(" "));
    EXPECT(v.size() == 2);
    v.clear();

    split(v, std::string(" hello "), std::string(" "));
    EXPECT(v.size() == 1);
    v.clear();


    std::list<std::string > v2;
    split(v2, std::string(" hello  world"), std::string(" "));
    EXPECT(v2.size() == 2);
    v2.clear();

    fprintf(stderr,"pass test_split()\n");
    return 0;
}



int main()
{
    test_split();


    return 0;
}