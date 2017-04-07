
#include <string.h>
#include <stdio.h>
#include <iostream>

// С���� , ������ĳһ�����е����� k ��ֵ
// ͨ���������� �Ѷ��е���Сֵ�ŵ����ϣ� ��������֤
// parent(i) -> left(2*i) , right(2*)

// ע�⣬��������/��С�ѵĹ�����Ҫѭ���³��� ����/��С�� �Ǹ��ڵ㶼��Ҷ�Ӵ�/С�Ķѡ�����ĺ��������Ķѣ����������Ҫ��
void heapify(int * begin, int * end)
{
    for (int n = end-begin-1;n>1;--n)
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
    int ar_top[k+1]={}; // 0 �������ã����㸸�ӹ�ϵ�ļ���

    // ����� k ��ֵ
    size_t j = 0;
    for (size_t i=1;i<_countof(ar_top);++i,++j)
    {
        ar_top[i] = ar[j];
    }

    // ������
    heapify(ar_top,ar_top+_countof(ar_top));

    for (;j<_countof(ar);++j)
    {
        // ������Ҫ�����Ǵ��ڻ���С��
        if (ar[j] > ar_top[1] )
        {
            // �������еļ�ֵ�� ��Ҫ�����ҵ����еļ�ֵ
            ar_top[1] = ar[j];
            heapify(ar_top,ar_top+_countof(ar_top));
        }
    }
    // 86 96 108 99 102
    print_array(ar_top+1,ar_top+_countof(ar_top));
    return 0;
}

