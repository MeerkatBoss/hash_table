#include <stdlib.h>
#include <stdio.h>

#include "meerkat_assert/asserts.h"

#include "hash_table/hash_table.h"
#include "table_utils/utils.h"

#include "histogram.h"

static void dump_contents(FILE* output, const HashTable* table);

#define STR(x) __BASIC_STR(x)
#define __BASIC_STR(x) #x

int run_test_histogram(int argc, const char* const* argv,
                       const TestConfig* config)
{
    HistogramConfig params = {-1, NULL};
    FILE *output = NULL;
    HashTable table = {};

    int parsed = parse_args(argc, argv, &HISTOGRAM_ARGS, &params);

    SAFE_BLOCK_START
    {
        ASSERT_EQUAL_MESSAGE(
            parsed, argc, "Invalid arguments");
        ASSERT_POSITIVE_MESSAGE(
            params.table_size, "Table size not specified");
        ASSERT_TRUE_MESSAGE(
            params.filename != NULL, "Input file not specified");

        if (config->filename)
        {
            ASSERT_MESSAGE(
                output = fopen(config->filename,
                                config->append_to_file ? "a" : "w"),
                action_result != NULL,
                "Failed to open output file");
        }
        else output = stdout;

        ASSERT_ZERO_MESSAGE(
            hash_table_ctor(&table, (size_t) params.table_size),
            "Invalid table size (not a prime number)");
        ASSERT_ZERO_MESSAGE(
            fill_hash_table(&table, params.filename, -1),
            "Failed to read input file");
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fprintf(stderr, "Error: %s\n", assertion_info.message);
        return 1;
    }
    SAFE_BLOCK_END
    
    dump_contents(output, &table);
    hash_table_dtor(&table);
    fclose(output);
    
    return 0;
}

static void dump_contents(FILE* output, const HashTable* table)
{
    fputs(STR(HASH_FUNCTION), output);

    for (size_t i = 0; i < table->bucket_count; ++i)
    {
        HashTableEntry* entry = &table->buckets[i];
        fprintf(output, ",%zu", entry->count);
    }
    fputc('\n', output);
}

int histogram_next_arg(const char* const* str, void* params)
{
    HistogramConfig* config = (HistogramConfig*) params;

    if (config->table_size >= 0)
    {
        config->filename = *str;   
        return 1;
    }

    char* end = NULL;
    long size = strtol(*str, &end, 10);

    if (*end != '\0' || size <= 0)
    {
        fprintf(stderr, "Error: '%s' is not a valid table size\n", *str);
        return -1;
    }

    config->table_size = size;
    return 1;
}

