/**
 * @file asm_hash.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-05-02
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_TABLE_HASHES_ASM_HASH_H
#define __HASH_TABLE_HASHES_ASM_HASH_H

#include <stdint.h>

extern "C" uint64_t asm_hash_murmur(const char* str);

#endif /* asm_hash.h */
