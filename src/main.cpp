#include <stdio.h>
#include <stdlib.h>

#include "meerkat_assert/asserts.h"

#include "hash_table/hash_table.h"
#include "table_utils/utils.h"

const size_t hash_table_bucket_count = 997;

__always_inline
static size_t max(size_t x, size_t y)
{
    return x > y ? x : y;
}

int main()
{
    HashTable tolstoy = {}, pushkin = {};

    SAFE_BLOCK_START
    {
        ASSERT_ZERO(
                hash_table_ctor(&tolstoy, hash_table_bucket_count));
        ASSERT_ZERO(
                hash_table_ctor(&pushkin, hash_table_bucket_count));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        perror("Initial allocations");
        return 1;
    }
    SAFE_BLOCK_END

    SAFE_BLOCK_START
    {
        ASSERT_ZERO(
                fill_hash_table(&tolstoy, "assets/war_and_peace.txt.data", -1));
        ASSERT_ZERO(
                fill_hash_table(&pushkin, "assets/pushkin_vol1-6.txt.data", -1));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        perror("Filling hash tables");
        hash_table_dtor(&tolstoy);
        hash_table_dtor(&pushkin);
        return 1;
    }
    SAFE_BLOCK_END
    
    const char** difference = NULL;
    SAFE_BLOCK_START
    {
        ASSERT_SIMPLE(
                difference = (const char**)calloc(
                                max(tolstoy.distinct_count,
                                    pushkin.distinct_count),
                                sizeof(*difference)),
                action_result != NULL);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        perror("Differrence buffer allocation");
        hash_table_dtor(&tolstoy);
        hash_table_dtor(&pushkin);
        return 1;
    }
    SAFE_BLOCK_END

    const double cosine = get_cosine_distance(&tolstoy, &pushkin);
    printf("Cosine similarity: %lf\n\n", cosine);

    ssize_t cnt = get_table_diff(&tolstoy, &pushkin,
                                 difference, tolstoy.distinct_count);
    puts("Tolstoy - Pushkin");
    
    for (ssize_t i = 0; i < cnt; ++ i)
        puts(difference[i]);

    cnt = get_table_diff(&pushkin, &tolstoy,
                                 difference, tolstoy.distinct_count);
    puts("Pushkin - Tolstoy");
    
    for (ssize_t i = 0; i < cnt; ++ i)
        puts(difference[i]);

    hash_table_dtor(&tolstoy);
    hash_table_dtor(&pushkin);
    free(difference);
    return 0;
}
