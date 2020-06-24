#include <stdio.h>
#include <string.h>


#include "crt_dbg_leak.h"

#ifdef WIN32
_crt_dbg_leak_t::_crt_dbg_leak_t(long break_alloc)
{
    memset(&ins, 0, sizeof(ins));
    crt_dbg_leak_lock(&ins);
    if (break_alloc != 0) {
        _CrtSetBreakAlloc(break_alloc);
    }
}
_crt_dbg_leak_t::~_crt_dbg_leak_t()
{
    crt_dbg_leak_unlock(&ins);
}

#else
_crt_dbg_leak_t::_crt_dbg_leak_t(long break_alloc)
{
    (void)break_alloc;
}
_crt_dbg_leak_t::~_crt_dbg_leak_t() {}

#endif // WIN32
