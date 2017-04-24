

#include <cstdio>
#include <cmath>


double sqrt1(double value)
{
    double t = 0;
    double t2 =0;
    for (t =0; t2 <= value; t += 0.0001, t2=t*t)
    {
        double x = value-t2;
        if ( x >0 && x <=0.00001)
        {
            break;
        }
        else if ( (-x>=0) && (-x <= 0x00001) )
        {
            break;
        }
    }
    return t;
}

// 牛顿迭代法
double sqrt_newton(double value)
{
    double root = 1;
    double t = 0;

    do 
    {
        t = root;
        root = (root+value/root)/2;
    } while (fabs(root-t)>=1e-11);
    return root;
}

// 来自金梦瑶

double sqrt_jinmengyao(double value)
{
    double delta = 1;
    double root = 0;

    for (;delta > 1e-11;)
    {
        for (;;)
        {
            double t = root*root;
            if( t == value){
                return root;
            }
            if(t < value && (root+delta)*(root+delta) > value){
                break;
            }
            root = root + delta;
        }
        delta = delta/10;
    }
    return root;
}


typedef double (*pfn_sqrt)(double);

int test(pfn_sqrt fn, const char * label)
{
    int ar[] = {9,10,8,5,6,7,0};

    for (int * p= ar; *p ; ++p)
    {
        printf("%s:%d ->%.10f\n",label,*p,fn(*p));
    }
    return 0;
}


int main()
{
    test(sqrt1,"sqrt1");
    printf("\n");
    test(sqrt_newton,"sqrt_newton");
    printf("\n");
    test(sqrt_jinmengyao,"sqrt_jinmengyao");
    return 0;
}
