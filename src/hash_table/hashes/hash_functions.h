/**
 * @file hash_functions.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-18
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_FUNCTIONS_H
#define __HASH_FUNCTIONS_H

#include <stdint.h>

uint64_t hash_always_one(const char* str);

uint64_t hash_first_char(const char* str);

uint64_t hash_strlen    (const char* str);

uint64_t hash_sum_char  (const char* str);

uint64_t hash_ror_xor   (const char* str);

uint64_t hash_rol_xor   (const char* str);

inline uint64_t __attribute__((always_inline)) hash_murmur(const char* str)
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
    const size_t    len   = 64;
    const uint64_t* data = (const uint64_t *)str;

    uint64_t hash = seed ^ (len * mult);
    uint64_t cur_sym = 0;

    #define PROCESS_CHARS(offset) \
    "        mov    %[cur_sym],     [%[data] + " #offset "]\n"\
    "        imul   %[cur_sym],     %[mult]\n"\
    "        mov    rsi,            %[cur_sym]\n"\
    "        shr    rsi,            47\n"\
    "        xor    %[cur_sym],     rsi\n"\
    "        imul   %[cur_sym],     %[mult]\n"\
    "        xor    %[hash],        %[cur_sym]\n"\
    "        imul   %[hash],        %[mult]\n"
    
    asm inline(
        ".intel_syntax noprefix\n"
        PROCESS_CHARS(0)
        PROCESS_CHARS(8)
        PROCESS_CHARS(16)
        PROCESS_CHARS(24)
        PROCESS_CHARS(32)
        PROCESS_CHARS(40)
        PROCESS_CHARS(48)
        PROCESS_CHARS(56)
        ".att_syntax prefix\n"
        : [cur_sym] "=&r"(cur_sym), [hash] "+r"(hash)
        : [mult] "r"(mult), [data]"r"(data)
        : "cc", "rsi");
    #undef PROCESS_CHARS
    
    return hash;
}


#endif /* hash_functions.h */
