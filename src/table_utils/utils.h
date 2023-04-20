/**
 * @file utils.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-20
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TABLE_UTILS_UTILS_H
#define __TABLE_UTILS_UTILS_H

#include "hash_table/hash_table.h"

/**
 * @brief Fill table with words from file
 *
 * @param[inout] table	    - Hash table to work with
 * @param[in]    filename   - Path to text file
 *
 * @return 0 upon success, -1 upon error
 *
 * @exception EINVAL    - table is NULL or uninitialized of filename is NULL
 * @exception ENOMEM    - not enough memory to store words in table
 *                          or not enough memory for buffered input
 * @exception EACCES    - failed to open file
 */
int fill_hash_table(HashTable* table, const char* filename);

#endif /* utils.h */
