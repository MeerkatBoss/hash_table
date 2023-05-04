#include <string.h>

#include "hash_functions.h"

static constexpr size_t max_len = 64;

uint64_t hash_always_one(const char* str __attribute((unused)))
{
    return 1;
}

uint64_t hash_first_char(const char* str)
{
    return (uint64_t) *str;
}

uint64_t hash_strlen(const char* str)
{
    return strnlen(str, 64);
}

uint64_t hash_sum_char  (const char* str)
{
    uint64_t hash = 0;
    size_t cnt = 0;

    do
    {
        hash += (uint64_t) *str;
    } while(*(str++) && ++cnt < max_len);

    return hash;
}

__always_inline
static uint64_t rotate_right(uint64_t x)
{
    return (x << 63) | (x >> 1);
}

__always_inline
static uint64_t rotate_left(uint64_t x)
{
    return (x >> 63) | (x << 1);
}

uint64_t hash_ror_xor(const char* str)
{
    uint64_t hash = 0;
    size_t cnt = 0;
    while (*str && cnt++ < max_len)
        hash = rotate_right(hash) ^ (uint64_t) *(str++);
    return hash;
}

uint64_t hash_rol_xor(const char* str)
{
    uint64_t hash = 0;
    size_t cnt = 0;
    while (*str && cnt++ < max_len)
        hash = rotate_left(hash) ^ (uint64_t) *(str++);
    return hash;
}

