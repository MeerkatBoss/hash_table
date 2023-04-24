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

uint64_t hash_custom    (const char* str);

#endif /* hash_functions.h */
