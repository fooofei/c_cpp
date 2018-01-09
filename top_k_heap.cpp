
#include <string.h>
#include <stdio.h>
#include <iostream>

// 小顶堆 , 用于求某一数列中的最大的 k 个值
// 通过自下向上 把堆中的最小值放到顶上， 其他不保证
// parent(i) -> left(2*i) , right(2*)

// 注意，建立最大堆/最小堆的过程需要循环下沉， 最大堆/最小堆 是父节点都比叶子大/小的堆。下面的函数建立的堆，不符合这个要求。
void heapify(int * begin, int * end)
{
    for (int n = int(end-begin-1);n>1;--n)
    {
        if(*(begin+n)<*(begin+n/2))
        {
            std::swap(*(begin+n),*(begin+n/2));
        }
    }
}

void print_array(int * begin, int * end)
{
    for (;begin<end;++begin)
    {
        printf("%d ",*begin);
    }
    printf("\n");
}


void insert_sort(int * begin, int * end)
{
    for (;begin<end; ++begin)
    {
        for (int * b2 = begin+1;b2<end;++b2)
        {
            if(*begin > *b2)
            {
                std::swap(*begin,*b2);
            }
        }
    }
}

int main()
{

    int ar[] = {99, 11, 13, 61, 36, 64, 74, 70, 63, 46, 102, 22, 84, 76, 96, 108, 86, 53, 49};

    const int k = 5;
    int ar_top[k+1]={}; // 0 索引不用，方便父子关系的计算

    // 先填充 k 个值
    size_t j = 0;
    for (size_t i=1;i<_countof(ar_top);++i,++j)
    {
        ar_top[i] = ar[j];
    }

    // 调整堆
    heapify(ar_top,ar_top+_countof(ar_top));

    for (;j<_countof(ar);++j)
    {
        // 这里需要考虑是大于还是小于
        if (ar[j] > ar_top[1] )
        {
            // 更换堆中的极值后 需要重新找到堆中的极值
            ar_top[1] = ar[j];
            heapify(ar_top,ar_top+_countof(ar_top));
        }
    }
    // 86 96 108 99 102
    print_array(ar_top+1,ar_top+_countof(ar_top));
    return 0;
}

