/**
 * @file hash_table.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-18
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __HASH_TABLE_H
#define __HASH_TABLE_H

#include <stddef.h>

struct HashTableEntry;

struct HashTableEntry
{
    char* key;
    size_t count; 

    HashTableEntry* next;
    int is_free;
};

struct HashTable
{
    HashTableEntry* buckets;
    size_t bucket_count;

    HashTableEntry* free;

    size_t capacity;
    size_t distinct_count;
    size_t total_count;
};

/**
 * @brief Create and initialize new hash table
 *
 * @param[out] table	- Hash table instance to be initialized
 *
 * @return 0 upon success, -1 upon error. Check `errno` for error description
 *
 * @exception EINVAL    - table was NULL or bucket_count is not a prime number
 * @exception ENOMEM    - failed to allocate memory for table
 */
int hash_table_ctor(HashTable* table, size_t bucket_count);

/**
 * @brief Destroy hash table and free all associated resources
 *
 * @param[inout] table	- Hash table to be deinitialized
 *
 * @return 0 upon success, -1 upon invalid parameter
 */
int hash_table_dtor(HashTable* table);

/**
 * @brief Increment counter on entry associated with given key
 *
 * @param[in] key	- Counted key
 *
 * @return 0 upon success, -1 upon error
 *
 * @exception EINVAL    - table or key is NULL
 * @exception ENOMEM    - failed to allocate memory for entry
 */
int hash_table_key_increment_counter(HashTable* table, const char* key);

/**
 * @brief Decrement counter on entry associated with given key
 *
 * @param[in] key	- Counted key
 *
 * @return 0 upon success, -1 upon invalid parameter
 * (table is NULL or count of key is already 0)
 */
int hash_table_key_decrement_counter(HashTable* table, const char* key);

/**
 * @brief Get value of counter on entry associated with given key
 *
 * @param[in] key	- Counted key
 *
 * @return Counter value
 */
size_t hash_table_get_key_count(const HashTable* table, const char* key);

#endif /* hash_table.h */
