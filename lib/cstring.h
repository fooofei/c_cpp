#ifndef CORE_CSTRING_H
#define CORE_CSTRING_H

#include <stdbool.h>
#include <stdint.h>

#include "atomic.h"

// ref https://github.com/cloudwu/cstring


#define CSTRING_STACK_SIZE 128

enum {
    CSTRING_PERMANENT = 1,
    CSTRING_INTERNING = 2,
    CSTRING_ONSTACK = 4
};


struct string_data {
    char *cstr;
    uint32_t hash_size;
    uint16_t type;
    uint16_t ref;
};

typedef struct string_data *string_t;

bool string_equal(string_t a, string_t b);
char *string_char(string_t self);
void string_release(string_t self);
void string_free_persist(string_t self);
string_t string_grab(string_t s);
string_t string_persist(const char *cstr, size_t sz);

typedef struct {
    string_t str;
} string_buffer_t[1];


string_t string_printf(string_buffer_t sb, const char *format, ...);
string_t string_cat(string_buffer_t sb, const char *str);

void memory_pool_release();

#define string_buffer_create(var) char var##string_memory[CSTRING_STACK_SIZE]={0};                                 \
    struct string_data var##string_data = { var##string_memory, 0, CSTRING_ONSTACK, 0 }; \
    string_buffer_t var;                                                                 \
    var->str = &var##string_data

#define string_buffer_close(var) if ((var)->str->type != 0) {    \
    } else {                        \
        string_release((var)->str); \
    }


#define string_literal(var, cstr) static string_t var = 0;                                                       \
    if (var) {                                                                     \
    } else {                                                                       \
        string_t tmp = string_persist("" cstr, (sizeof(cstr) / sizeof(char)) - 1); \
        if (!atomic_bool_compare_and_swap_pointer(&var, NULL, tmp)) {              \
            string_free_persist(tmp);                                              \
        }                                                                          \
    }


#define string_buffer_tostring(var) ((var)->str)

#endif
