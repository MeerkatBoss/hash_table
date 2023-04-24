#include <stdio.h>

#include "hash_table/hash_table.h"
#include "table_utils/utils.h"

static void dump_contents(const HashTable* table);

#define STR(x) __BASIC_STR(x)
#define __BASIC_STR(x) #x

int main()
{
    HashTable table = {};

    hash_table_ctor(&table, 19);

    fill_hash_table(&table, "assets/war_and_peace.txt.data", -1);

    dump_contents(&table);

    hash_table_dtor(&table);
}

static void dump_contents(const HashTable* table)
{
    FILE* results = fopen("results/hash_functions.csv", "a");
    fputs(STR(HASH_FUNCTION), results);

    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        HashTableEntry* entry = &table->buckets[i];
        fprintf(results, ",%zu", entry->count);
    }
    fputc('\n', results);
}

