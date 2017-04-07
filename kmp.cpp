// demo.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <windows.h>
#include <tchar.h>

using std::cout;
using std::endl;


// KMP 算法太复杂了 还是记住 sunday 算法好使

//
// KMP 进化 
//

void test()
{

    const char * base = "abcabc";
    size_t l = strlen(base);

    std::vector<int> prefix;
    prefix.resize(l);

    prefix[0] = 0;
    for (size_t i =1; i< l; ++i)
    {
        // ab  
        prefix[i] = 0;
        for (int j=i-1;j>=0;--j)
        {
            std::string s1(base,j+1);
            std::string s2(base+i-j,j+1);
            if (s1 ==s2)
            {
                prefix[i]=j+1;
                break;
            }
        }
    }

    for (size_t i = 0 ; i< l; ++ i)
    {
        printf("%d ",prefix[i]);
    }

}


void test2()
{
    const char * base = "bcbcabcbcb";//"abcabc";
    size_t l = strlen(base);

    std::vector<BYTE> prefix;
    prefix.resize(l);

    prefix[0] = 0;
    for (size_t i =1; i< l; ++i)
    {
        prefix[i] = 0;
        for (int j=prefix[i-1];j>=0;--j)
        {
            std::string s1(base,j+1);
            std::string s2(base+i-j,j+1);
            if (s1 ==s2)
            {
                prefix[i]=j+1;
                break;
            }
        }
    }

    for (size_t i = 0 ; i< l; ++ i)
    {
        printf("%d ",prefix[i]);
    }
}



void test3()
{

    const char * base = "bcbcabcbcb";//"abcabc";
    size_t l = strlen(base);

    std::vector<BYTE> prefix;
    prefix.resize(l);

    prefix[0] = 0;
    for (size_t i =1; i< l; ++i)
    {
        prefix[i] = 0;
        for (int j=prefix[i-1];j>=0;)
        {
            std::string s1(base,j+1);
            std::string s2(base+i-j,j+1);
            if (s1 ==s2)
            {
                prefix[i]=j+1;
                break;
            }

            if (j)
            {
                j = prefix[j-1];
            }
            else
            {
                break;
            }
            
        }
    }

    for (size_t i = 0 ; i< l; ++ i)
    {
        printf("%d ",prefix[i]);
    }
}



void test4()
{

    const char * base = "bcbcabcbcb";//"abcabc";
    size_t l = strlen(base);

    std::vector<BYTE> prefix;
    prefix.resize(l);

    prefix[0] = 0;
    for (size_t i =1; i< l; ++i)
    {
        prefix[i] = 0;

        int j = i ;

        do 
        {
            j = prefix[j-1];
            std::string s1(base,j+1);
            std::string s2(base+i-j,j+1);
            if (s1 ==s2)
            {
                prefix[i]=j+1;
                break;
            }
        } while (j>0);
    }

    for (size_t i = 0 ; i< l; ++ i)
    {
        printf("%d ",prefix[i]);
    }

    // 0 0 1 2 0 1 2 3 4 3

}

int main()
{
    test4();

    return 0;
}