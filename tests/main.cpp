#include <stdio.h>

#include "hash_table/hash_table.h"
#include "table_utils/utils.h"

static void dump_contents(const HashTable* table);

int main()
{
    HashTable table = {};

    hash_table_ctor(&table, 17);

    fill_hash_table(&table, "assets/war_and_peace.txt");

    dump_contents(&table);

    hash_table_dtor(&table);
}

static void dump_contents(const HashTable* table)
{
    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        HashTableEntry* entry = &table->buckets[i];
        printf("[%zu]: %zu\n", i, entry->count);
    }
}


