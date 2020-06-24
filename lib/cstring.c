#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef WIN32
#include <Windows.h>
#endif


#include "cstring.h"


#ifndef __max
#define __max(a, b) (((a) > (b)) ? (a) : (b))
#endif


#define CSTRING_INTERNING_SIZE 32


#define FORMAT_TEMP_SIZE 1024

#define INTERNING_POOL_SIZE 1024
// HASH_START_SIZE must be 2 pow
#define HASH_START_SIZE 16

// on stack hash_size=strlen


struct string_node {
    struct string_data str;
    char buffer[CSTRING_INTERNING_SIZE];
    struct string_node *next;
};

struct string_pool {
    struct string_node node[INTERNING_POOL_SIZE];
};

struct string_interning {
    int lock;
    int size; // the hash memory size
    struct string_node **hash;
    struct string_pool *pool;
    int index;
    int total;
};

static struct string_interning g_si;


static inline void LOCK()
{
    atomic_spinlock32(&g_si.lock);
}

static inline void UNLOCK()
{
    atomic_spinunlock32(&g_si.lock);
}

char *string_char(string_t s)
{
    return s->cstr;
}

static void _insert_node(struct string_node **hash, int sz, struct string_node *n)
{
    uint32_t h = n->str.hash_size;
    int index = h & (sz - 1);
    n->next = hash[index];
    hash[index] = n;
}

static void _interning_expand(struct string_interning *si)
{
    int new_size = si->size * 2;
    new_size = __max(new_size, HASH_START_SIZE);
    assert(new_size > si->total);

    struct string_node **new_hash = calloc(1, sizeof(struct string_node *) * new_size);
    if (!new_hash)
        return;

    int i;
    for (i = 0; i < si->size; i++) {
        struct string_node *node = si->hash[i];
        while (node) {
            struct string_node *tmp = node->next;
            _insert_node(new_hash, new_size, node);
            node = tmp;
        }
    }
    free(si->hash);
    si->hash = new_hash;
    si->size = new_size;
}

static string_t _interning(struct string_interning *si, const char *cstr, size_t sz, uint32_t hash)
{
    if (si->hash == NULL) {
        return NULL;
    }
    int index = (int)(hash & (si->size - 1));
    struct string_node *n = si->hash[index];
    while (n) {
        if (n->str.hash_size == hash) {
            if (strcmp(n->str.cstr, cstr) == 0) {
                return &n->str;
            }
        }
        n = n->next;
    }
    // 80% (4/5) threshold
    if (si->total * 5 >= si->size * 4) {
        return NULL;
    }
    if (si->pool == NULL) {
        // need not free pool
        // todo: check memory alloc error
        si->pool = malloc(sizeof(struct string_pool));
        assert(si->pool);
        si->index = 0;
    }
    n = &si->pool->node[si->index++];
    memcpy(n->buffer, cstr, sz);
    n->buffer[sz] = '\0';

    string_t cs = &n->str;
    cs->cstr = n->buffer;
    cs->hash_size = hash;
    cs->type = CSTRING_INTERNING;
    cs->ref = 0;

    _insert_node(si->hash, si->size, n);
    return cs;
}

static string_t _string_interning(const char *cstr, size_t sz, uint32_t hash)
{
    string_t ret;
    LOCK();
    ret = _interning(&g_si, cstr, sz, hash);
    if (ret == NULL) {
        _interning_expand(&g_si);
        ret = _interning(&g_si, cstr, sz, hash);
    }
    ++g_si.total;
    UNLOCK();
    assert(ret);
    return ret;
}


static uint32_t _hash_blob(const char *buffer, size_t len)
{
    const uint8_t *ptr = (const uint8_t *)buffer;
    size_t h = len;
    size_t step = (len >> 5) + 1;
    size_t i;
    for (i = len; i >= step; i -= step)
        h = h ^ ((h << 5) + (h >> 2) + ptr[i - 1]);
    if (h == 0)
        return 1;
    else
        return h;
}

void string_free_persist(string_t s)
{
    if (s->type == CSTRING_PERMANENT) {
        free(s);
    }
}

static string_t _string_clone(const char *cstr, size_t sz)
{
    if (sz < CSTRING_INTERNING_SIZE) {
        return _string_interning(cstr, sz, _hash_blob(cstr, sz));
    }
    struct string_data *p = calloc(1, sizeof(struct string_data) + sz + 1);
    if (!p)
        return 0;
    assert(p);
    int8_t *ptr = (int8_t *)(p + 1);
    p->cstr = (char *)ptr;
    p->type = 0;
    p->ref = 1;
    memcpy(ptr, cstr, sz);
    ptr[sz] = '\0';
    p->hash_size = 0;
    return p;
}

string_t string_persist(const char *cstr, size_t sz)
{
    string_t s = _string_clone(cstr, sz);
    if (s->type == 0) {
        s->type = CSTRING_PERMANENT;
        s->ref = 0;
    }
    return s;
}

string_t string_grab(string_t s)
{
    if (s->type & (CSTRING_PERMANENT | CSTRING_INTERNING)) {
        return s;
    }
    if (s->type == CSTRING_ONSTACK) {
        string_t tmp = _string_clone(s->cstr, s->hash_size);
        return tmp;
    } else {
        if (s->ref == 0) {
            s->type = CSTRING_PERMANENT;
        } else {
            atomic_inc_return16(&s->ref);
        }
        return s;
    }
}

void string_release(string_t s)
{
    if (s->type != 0) {
        return;
    }
    if (s->ref == 0) {
        return;
    }

    if (atomic_dec_return16(&s->ref) == 0) {
        free(s);
    }
}

static uint32_t _string_hash(string_t s)
{
    if (s->type == CSTRING_ONSTACK) {
        return _hash_blob(s->cstr, s->hash_size);
    }
    if (s->hash_size == 0) {
        s->hash_size = _hash_blob(s->cstr, strlen(s->cstr));
    }
    return s->hash_size;
}

bool string_equal(string_t a, string_t b)
{
    if (a == b)
        return true;
    if ((a->type == CSTRING_INTERNING) && (b->type == CSTRING_INTERNING)) {
        return false;
    }
    if ((a->type == CSTRING_ONSTACK) && (b->type == CSTRING_ONSTACK)) {
        if (a->hash_size != b->hash_size) {
            return false;
        }
        return memcmp(a->cstr, b->cstr, a->hash_size) == 0;
    }
    uint32_t hasha = _string_hash(a);
    uint32_t hashb = _string_hash(b);
    if (hasha != hashb) {
        return false;
    }
    return strcmp(a->cstr, b->cstr) == 0;
}

static string_t _string_cat2(const char *a, const char *b)
{
    size_t sa = strlen(a);
    size_t sb = strlen(b);
    if (sa + sb < CSTRING_INTERNING_SIZE) {
        char tmp[CSTRING_INTERNING_SIZE];
        memcpy(tmp, a, sa);
        memcpy(tmp + sa, b, sb);
        tmp[sa + sb] = '\0';
        return _string_interning(tmp, sa + sb, _hash_blob(tmp, sa + sb));
    }
    struct string_data *p = malloc(sizeof(struct string_data) + sa + sb + 1);
    // todo: memory alloc error
    assert(p);
    char *ptr = (char *)(p + 1);
    p->cstr = ptr;
    p->type = 0;
    p->ref = 1;
    memcpy(ptr, a, sa);
    memcpy(ptr + sa, b, sb);
    ptr[sa + sb] = '\0';
    p->hash_size = 0;
    return p;
}

string_t string_cat(string_buffer_t sb, const char *str)
{
    string_t s = sb->str;
    if (s->type == CSTRING_ONSTACK) {
        int i = (int)s->hash_size;
        while (i < CSTRING_STACK_SIZE - 1) {
            s->cstr[i] = *str;
            if (*str == '\0') {
                return s;
            }
            ++s->hash_size;
            ++str;
            ++i;
        }
        s->cstr[i] = '\0';
    }
    string_t tmp = s;
    sb->str = _string_cat2(tmp->cstr, str);
    string_release(tmp);
    return sb->str;
}


static string_t _string_format(const char *format, va_list ap)
{
    // 两种情况下不使用这个 static 缓冲区
    //  1 有一个线程在使用这个 又来了一个线程 则需要重新申请内存
    //  2 这个缓冲区大小不够
    static char _s_cache[FORMAT_TEMP_SIZE];
    static char *cache = _s_cache;

    char *result = 0;
    char *temp = cache;
    bool temp_use_cache = false;
    string_t ret = 0;
    int n;


    for (;;) {
        // read cache buffer atomic
        // 这里有技术 为什么是 if 之后才加锁拿地址呢 因为 NULL 的时候一定是被拿了
        if (temp) {
            // cache=NULL;
            temp = (char *)atomic_var_compare_and_swap_pointer(&cache, temp, NULL);
            if (temp) {
                temp_use_cache = true;
            }
        }

        if (!temp) {
            temp = (char *)calloc(1, FORMAT_TEMP_SIZE);
            if (!temp)
                break;
        }

        va_list ap2;
        va_copy(ap2, ap);
        n = vsnprintf(temp, FORMAT_TEMP_SIZE, format, ap2);

        if (n <= 0)
            break;

        if (n >= FORMAT_TEMP_SIZE) {
            // clear
            temp[0] = 0;

            int sz = FORMAT_TEMP_SIZE * 2;
            for (; sz <= n; sz *= 2)
                ;

            result = (char *)calloc(1, sz);
            if (!result)
                break;

            va_copy(ap2, ap);
            n = vsnprintf(result, sz, format, ap2);

            if (!(n > 0 && n < sz))
                break;

            // temp is invalid
            if (!temp_use_cache && temp) {
                free(temp);
                temp = 0;
            }
        } else {
            result = temp;
        }
        string_t r = (string_t)malloc(sizeof(struct string_data) + n + 1);
        if (!r) {
            if (result != temp) {
                free(result);
                result = 0;
            }
            break;
        }

        r->cstr = (char *)(r + 1);
        r->type = 0;
        r->ref = 1;
        r->hash_size = 0; // TODO ?
        memcpy(r->cstr, result, n + 1);
        if (result != temp) {
            free(result);
            result = 0;
        }
        ret = r;
        break;
    }

    if (temp_use_cache && temp) {
        // save temp atomic  cache=temp
        while (!atomic_bool_compare_and_swap_pointer(&cache, NULL, temp)) {
        }
    } else if (temp) {
        free(temp);
    }

    return ret;
}

string_t string_printf(string_buffer_t sb, const char *format, ...)
{
    string_t s = sb->str;
    va_list ap;
    va_start(ap, format);
    if (s->type == CSTRING_ONSTACK) {
        int n = vsnprintf(s->cstr, CSTRING_STACK_SIZE, format, ap);
        if (n >= CSTRING_STACK_SIZE) {
            va_end(ap);
            va_start(ap, format);
            s = _string_format(format, ap);
            sb->str = s;
        } else if (n > 0) {
            s->hash_size = n;
        } else {
            sb->str = 0;
            va_end(ap);
            return 0;
        }
    } else {
        string_release(sb->str);
        s = _string_format(format, ap);
        sb->str = s;
    }
    va_end(ap);
    return s;
}

void memory_pool_release()
{
    LOCK();
    free(g_si.hash);
    g_si.hash = 0;
    free(g_si.pool);
    g_si.pool = 0;
    g_si.index = 0;
    g_si.size = 0;
    g_si.total = 0;


    UNLOCK();
}
