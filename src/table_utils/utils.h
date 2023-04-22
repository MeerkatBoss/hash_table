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
 * @param[in] 	 max_words  - Maximum number of words to read from file.
 *                              -1 means all words will be read.
 *
 * @return 0 upon success, -1 upon error
 *
 * @exception EINVAL    - table is NULL or uninitialized of filename is NULL
 * @exception ENOMEM    - not enough memory to store words in table
 *                          or not enough memory for buffered input
 * @exception EACCES    - failed to open file
 */
int fill_hash_table(HashTable* table, const char* filename, ssize_t max_words);

/**
 * @brief Find all words in `source`, which are NOT in `words` and
 * store them in `result_buffer`
 *
 * @param[in]  source	        - Table of words to be selected from
 * @param[in]  words	        - Table of words to be compared against
 * @param[out] result_buffer	- Buffer to store the result in
 * @param[in]  buffer_size      - Maximum number of words to be stored in
 *                                  `result_buffer`
 *
 * @return number of entries stored, -1 if `buffer_size`
 * was too small
 */
ssize_t get_table_diff(const HashTable* source, const HashTable* words,
                   const char** result_buffer, size_t buffer_size);

/**
 * @brief Get cosine similarity between multisets of words
 *
 * @param[in] src1  - First multiset
 * @param[in] src2  - Second multiset
 *
 * @return Value of cosine of angle between multiset vectors
 */
double get_cosine_similarity(const HashTable* src1, const HashTable* src2);

#endif /* utils.h */
