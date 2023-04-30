#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "meerkat_assert/asserts.h"

#include "utils.h"

__always_inline
static int is_word_char(const char c)
{
    return !isspace(c) && !ispunct(c) && !isdigit(c);
}

int fill_hash_table(HashTable* table, const char* filename, ssize_t max_words)
{
    SAFE_BLOCK_START
    {
        ASSERT_TRUE(table != NULL);
        ASSERT_TRUE(table->buckets != NULL);
        ASSERT_TRUE(filename);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        errno = EINVAL;
        return -1;
    }
    SAFE_BLOCK_END

    int fd = 0;
    const size_t buffer_size = (size_t) sysconf(_SC_PAGE_SIZE);
    char* buffer = NULL;

    SAFE_BLOCK_START
    {
        ASSERT_POSITIVE_CALLBACK(
                fd = open(filename, O_RDONLY),
                errno = EACCES);
        ASSERT_ZERO_CALLBACK(
                posix_memalign((void**)&buffer,
                                max_word_length, sizeof(*buffer)*buffer_size),
                errno = ENOMEM);
        memset(buffer, 0, sizeof(*buffer)*buffer_size);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        return -1;
    }
    SAFE_BLOCK_END

    const int input = fd;
    char* const text = buffer;

    ssize_t read_result = 0;
    ssize_t words_cnt = 0;

    while ((read_result = read(input, text, buffer_size)))
    {
        if (read_result < 0)
        {
            // TODO: Logs
            errno = EACCES;
            return -1;
        }

        const size_t bytes_available = (size_t) read_result;

        for (size_t i = 0;
             i + max_word_length <= bytes_available;
             i += max_word_length)
        {
            if (max_words >= 0 && words_cnt >= max_words)
                goto end;
            hash_table_key_increment_counter(table, text + i);
            ++ words_cnt;
        }
    }

end:
    close(input);
    free(text);

    return 0;
}

ssize_t get_table_diff(const HashTable* source, const HashTable* words,
                   const char** result_buffer, size_t buffer_size)
{
    size_t stored = 0;

    HashTableIterator it = {};
    if (hash_table_get_iterator(source, &it) < 0)
        return 0;

    do
    {
        const size_t count = hash_table_get_key_count(words, it.key);
        if (count) continue;

        if (result_buffer && stored < buffer_size)
            result_buffer[stored] = it.key;
        else if (result_buffer)
            return -1;
        stored++;
    } while (hash_table_iterator_get_next(&it) == 0);

    return (ssize_t) stored;
}

static double get_vector_length(const HashTable* src);

double get_cosine_similarity(const HashTable* src1, const HashTable* src2)
{
    double len1 = 0;
    double dot_product = 0;

    HashTableIterator it = {};
    if (hash_table_get_iterator(src1, &it) < 0)
        return 0;

    do
    {
        const size_t count = hash_table_get_key_count(src2, it.key);

        len1 += (double)it.count * (double)it.count;
        if (count)
            dot_product += (double)it.count * (double)count;

    } while (hash_table_iterator_get_next(&it) == 0);

    len1 = sqrt(len1);
    double len2 = get_vector_length(src2);
    
    return dot_product / (len1 * len2);
}

static double get_vector_length(const HashTable* src)
{
    double len = 0;
    HashTableIterator it = {};
    if (hash_table_get_iterator(src, &it) < 0)
        return 0;

    do
    {
        len += (double)it.count * (double)it.count;

    } while (hash_table_iterator_get_next(&it) == 0);

    return sqrt(len);
}

