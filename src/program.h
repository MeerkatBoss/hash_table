/**
 * @file program.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-23
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __PROGRAM_H
#define __PROGRAM_H

#include "hash_table/hash_table.h"
#include "table_utils/config.h"

static const size_t hash_table_bucket_count = 19;

struct ProgramState
{
    HashTable file1_words;
    HashTable file2_words;
    const char** diff_array;
    size_t diff_array_size;
};

int program_init(ProgramState* state, const ProgramConfig* config);

int program_compare_files(ProgramState* state, const ProgramConfig* config);

void program_finish(ProgramState* state);

#endif /* program.h */
