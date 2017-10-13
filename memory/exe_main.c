
#include <stdio.h>
#include <stdlib.h>

#include <Windows.h>



int main()
{
  HMODULE h=0;


  h = LoadLibraryA("mdll.dll");

  if (h)
  {
    void * p = 0;
    size_t l = 0;
    void * p1 = 0;
    size_t l1 = 0;
    
    typedef int(*pfn_foo)(void **, size_t *);

    pfn_foo fn = (pfn_foo)GetProcAddress(h, "foo");
    if (fn)
    {
      fn(&p, &l);

      __asm{_emit 0xcc}


      p1 = malloc(20);
      free(p1);

      free(p);

      printf("freed\n");
     
    }
    FreeLibrary(h);
  }
  return 0;
}