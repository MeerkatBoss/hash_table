#include <string.h>

#include "hash_functions.h"

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
    return strlen(str);
}

uint64_t hash_sum_char  (const char* str)
{
    uint64_t hash = 0;

    do
    {
        hash += (uint64_t) *str;
    } while(*(str++));

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
    while (*str)
        hash = rotate_right(hash) ^ (uint64_t) *(str++);
    return hash;
}

uint64_t hash_rol_xor(const char* str)
{
    uint64_t hash = 0;
    while (*str)
        hash = rotate_left(hash) ^ (uint64_t) *(str++);
    return hash;
}

uint64_t hash_murmur(const char* str)
{
    /*
     * MurmurHash64A by Austin Appleby
     * 64-bit hash for 64-bit platforms
     *
     * See https://github.com/aappleby/smhasher/blob/master/src/MurmurHash2.cpp
     */

    const uint64_t mult = 0xC6A4A7935BD1E995;   // This is from the original
                                                // algorithm
    const uint64_t seed = 0x8B72E9FB7FAA60FD;   // This is offline-generated
                                                // seed
    const size_t   len  = strlen(str);
    const int      shift= 47;

    uint64_t hash = seed ^ (len * mult);

    const uint64_t* data = (const uint64_t *)str;
    const uint64_t* end  = data + (len/8);

    while(data != end)
    {
        uint64_t cur_sym = 0;
        if ((intptr_t) data & 7)
            memcpy(&cur_sym, data++, sizeof(cur_sym));
        else
            cur_sym = *(data++);

        cur_sym *= mult; 
        cur_sym ^= cur_sym >> shift; 
        cur_sym *= mult; 

        hash ^= cur_sym;
        hash *= mult; 
    }

    const unsigned char* char_data = (const unsigned char*)data;

    switch(len & 7) // Loop optimization
    {
    case 7: hash ^= uint64_t(char_data[6]) << 48;
            /* fall through */
    case 6: hash ^= uint64_t(char_data[5]) << 40;
            /* fall through */
    case 5: hash ^= uint64_t(char_data[4]) << 32;
            /* fall through */
    case 4: hash ^= uint64_t(char_data[3]) << 24;
            /* fall through */
    case 3: hash ^= uint64_t(char_data[2]) << 16;
            /* fall through */
    case 2: hash ^= uint64_t(char_data[1]) << 8;
            /* fall through */
    case 1: hash ^= uint64_t(char_data[0]);
            hash *= mult;
            /* fall through */
    default: break;
    };

    hash ^= hash >> shift;
    hash *= mult;
    hash ^= hash >> shift;

    return hash;
}

