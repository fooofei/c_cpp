

#include <math.h>
#include <stdio.h>

/* 2 = sqrt1(4) */
double 
sqrt1(double value)
{
  double t = 0;
  double t2 = 0;
  for (t = 0; t2 <= value; t += 0.0001, t2 = t * t)
  {
    double x = value - t2;
    if (x > 0 && x <= 0.00001)
    {
      break;
    }
    else if ((-x >= 0) && (-x <= 0x00001))
    {
      break;
    }
  }
  return t;
}

//
double 
sqrt_newton(double value)
{
  double root = 1;
  double t = 0;
  for (;;)
  {
    t = root;
    root = (root + value / root) / 2;
    if (fabs(root-t)<1e-11)
    {
      break;
    }
  }
  return root;
}

// the algorithm come from a schoolmates of colleage

double 
sqrt_jinmengyao(double value)
{
  double delta = 1;
  double root = 0;

  for (; delta > 1e-11;)
  {
    for (;;)
    {
      double t = root * root;
      if (t == value)
      {
        return root;
      }
      if (t < value && (root + delta) * (root + delta) > value)
      {
        break;
      }
      root = root + delta;
    }
    delta = delta / 10;
  }
  return root;
}

typedef double (*pfn_sqrt)(double);

int 
test(pfn_sqrt fn, const char *label)
{
  int ar[] = {9, 10, 8, 5, 6, 7, 0};

  for (int *p = ar; *p; ++p)
  {
    printf("%s:%d ->%.10f\n", label, *p, fn(*p));
  }
  return 0;
}

int 
main()
{
  test(sqrt1, "sqrt1");
  printf("\n");
  test(sqrt_newton, "sqrt_newton");
  printf("\n");
  test(sqrt_jinmengyao, "sqrt_jinmengyao");
  return 0;
}

// output
/*
sqrt1:9 ->3.0000000000
sqrt1:10 ->3.1623000000
sqrt1:8 ->2.8285000000
sqrt1:5 ->2.2361000000
sqrt1:6 ->2.4495000000
sqrt1:7 ->2.6458000000

sqrt_newton:9 ->3.0000000000
sqrt_newton:10 ->3.1622776602
sqrt_newton:8 ->2.8284271247
sqrt_newton:5 ->2.2360679775
sqrt_newton:6 ->2.4494897428
sqrt_newton:7 ->2.6457513111

sqrt_jinmengyao:9 ->3.0000000000
sqrt_jinmengyao:10 ->3.1622776602
sqrt_jinmengyao:8 ->2.8284271247
sqrt_jinmengyao:5 ->2.2360679775
sqrt_jinmengyao:6 ->2.4494897428
sqrt_jinmengyao:7 ->2.6457513111
*/
