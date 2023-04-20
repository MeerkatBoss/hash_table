#include <stdlib.h>
#include <string.h>

#include "meerkat_assert/asserts.h"

#include "hashes/hash_functions.h"

#ifndef HASH_FUNCTION
#define HASH_FUNCTION(key) hash_always_one(key)
#endif

#include "hash_table.h"

static void mark_free(HashTableEntry* entries, size_t entry_count);
static int try_grow(HashTable* table);

__always_inline
static size_t round_to_pow2(size_t x)
{
    size_t result = 1;
    while (result < x)
        result <<= 1;
    return result;
}

__always_inline
static int is_prime(size_t x)
{
    if (x < 2) return 0;

    for (size_t div = 2; div <= x / div; div++)
        if (x % div == 0)
            return 0;
    return 1;
}


int hash_table_ctor(HashTable* table, const size_t bucket_count)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(is_prime(bucket_count));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    size_t capacity = round_to_pow2(2*bucket_count);
    HashTableEntry* buffer = NULL;

    SAFE_BLOCK_START
    {
        ASSERT_SIMPLE(
            buffer = (HashTableEntry*)calloc(capacity, sizeof(*buffer)),
            action_result != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = ENOMEM;
        return -1;
    }
    SAFE_BLOCK_END

    mark_free(buffer + bucket_count, capacity - bucket_count);

    table->buckets = buffer;
    table->bucket_count = bucket_count;
    table->free = buffer + bucket_count;

    table->capacity = capacity;
    table->distinct_count = 0;
    table->total_count = 0;

    return 0;
}

int hash_table_dtor(HashTable* table)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(table->buckets != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        HashTableEntry* entry = table->buckets[i].next;
        while (entry)
        {
            free(entry->key);
            entry = entry->next;
        }
    }
    free(table->buckets);

    memset(table, 0, sizeof(*table));

    return 0;
}

int hash_table_key_increment_counter(HashTable* table, const char* key)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(table->buckets != NULL);
        ASSERT_TRUE(key   != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    size_t key_hash = HASH_FUNCTION(key) % table->bucket_count;

    HashTableEntry* key_entry = table->buckets[key_hash].next;

    while (key_entry && strcmp(key_entry->key, key) != 0)
        key_entry = key_entry->next;

    if (key_entry)
    {
        ++ key_entry->count;
        ++ table->total_count;
        return 0;
    }

    SAFE_BLOCK_START
    {
        ASSERT_ZERO(
                try_grow(table));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = ENOMEM;
        return -1;
    }
    SAFE_BLOCK_END

    key_entry = table->free;
    table->free = table->free->next;
    
    key_entry->key = strdup(key);
    key_entry->count = 1;
    key_entry->next = table->buckets[key_hash].next;
    
    table->buckets[key_hash].next = key_entry;
    ++ table->buckets[key_hash].count;
    ++ table->distinct_count;
    ++ table->total_count;

    return 0;
}

int hash_table_key_decrement_counter(HashTable* table, const char* key)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(table->buckets != NULL);
        ASSERT_TRUE(key   != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    size_t key_hash = HASH_FUNCTION(key) % table->bucket_count;

    HashTableEntry* lst_entry = &table->buckets[key_hash];
    HashTableEntry* key_entry = lst_entry->next;

    while (key_entry && strcmp(key_entry->key, key) != 0)
    {
        lst_entry = key_entry;
        key_entry = lst_entry->next;
    }

    SAFE_BLOCK_START
    {
        ASSERT_TRUE(key_entry != NULL);
        ASSERT_POSITIVE(key_entry->count);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    -- key_entry->count;
    -- table->total_count;

    if (key_entry->count)
        return 0;

    lst_entry->next = key_entry->next;
    -- table->buckets[key_hash].count;
    -- table->distinct_count;

    free(key_entry->key);
    key_entry->key = NULL;
    key_entry->count = 0;

    key_entry->next = table->free;
    key_entry->is_free = 1;

    table->free = key_entry;
    
    return 0;
}

size_t hash_table_get_key_count(const HashTable* table, const char* key)
{
    /* If there is no table, it does not contain any keys */
    if (!table || !table->buckets) return 0;

    /* If there is no key, no table contains it */
    if (!key) return 0;

    size_t key_hash = HASH_FUNCTION(key) % table->bucket_count;

    HashTableEntry* key_entry = table->buckets[key_hash].next;

    while (key_entry && strcmp(key_entry->key, key) != 0)
        key_entry = key_entry->next;
    
    return key_entry ? key_entry->count : 0;
}

static void mark_free(HashTableEntry* entries, size_t entry_count)
{
    for (size_t i = 0; i < entry_count; ++i)
    {
        entries[i].is_free = 1;
        entries[i].next = i + 1 < entry_count
                            ? entries + i + 1
                            : NULL;
    }
}


static int try_grow(HashTable* table)
{
    const size_t cap_growth = 2;
    if (table->free) return 0;

    const intptr_t old_addr = (intptr_t) table->buckets;

    const size_t old_cap = table->capacity;
    const size_t new_cap = old_cap * cap_growth;
    HashTableEntry* data = NULL;

    SAFE_BLOCK_START
    {
        ASSERT_SIMPLE(
                data = (HashTableEntry*)
                        realloc(table->buckets, new_cap*sizeof(*data)),
                action_result != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        return -1;
    }
    SAFE_BLOCK_END

    const intptr_t new_addr = (intptr_t) data;
    const intptr_t addr_offset = new_addr - old_addr;

    /* Update addresses */
    if (addr_offset)
        for (size_t i = 0; i < old_cap; ++i)
            if (data[i].next)
                data[i].next = (HashTableEntry*)
                                        ((intptr_t)data[i].next + addr_offset);

    mark_free(data + old_cap, new_cap - old_cap);

    table->buckets = data;
    table->free = data + old_cap;
    table->capacity = new_cap;

    return 0;
}
