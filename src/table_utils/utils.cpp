#include <ctype.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

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
        ASSERT_CALLBACK(
                buffer = (char*) calloc(buffer_size, sizeof(*buffer)),
                action_result != NULL,
                errno = ENOMEM);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        // TODO: Logs
        return -1;
    }
    SAFE_BLOCK_END

    const int input = fd;
    char* const text = buffer;

    const char* cur_word = text;
    off_t word_start = 0;
    off_t current_offset = 0;
    size_t bytes_available = 0;
    ssize_t read_result = 0;
    int is_in_word = 0;
    ssize_t words_cnt = 0;

    while ((read_result = read(input, text, buffer_size)))
    {
        if (read_result < 0)
        {
            // TODO: Logs
            errno = EACCES;
            return -1;
        }

        bytes_available = (size_t) read_result;
        for (size_t i = 0; i < bytes_available; ++i, ++current_offset)
        {
            if (!is_word_char(text[i]) && is_in_word)
            {
                SAFE_BLOCK_START
                {
                    text[i] = '\0';
                    ASSERT_ZERO(
                            hash_table_key_increment_counter(table, cur_word));
                    is_in_word = 0;
                }
                SAFE_BLOCK_HANDLE_ERRORS
                {
                    close(input);
                    free(buffer);
                    // TODO: logs
                    errno = ENOMEM;
                    return -1;
                }
                SAFE_BLOCK_END
            }
            else if (is_word_char(text[i]) && !is_in_word)
            {
                word_start = current_offset;
                cur_word = text + i;
                is_in_word = 1;
            }

            ++ words_cnt;
            if (max_words >= 0 && words_cnt == max_words)
                goto end;
        }

        if (bytes_available < buffer_size) /* File ended */
            break;

        if (is_in_word) /* Adjust position in file to continue reading word */
        {
            lseek(input, word_start, SEEK_SET);
            cur_word = text;
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
    ssize_t stored = 0;
    const size_t src_size = source->bucket_count;

    for (size_t i = 0; i < src_size; ++i) // TODO: Iterator for hash table
    {
        
        for (const HashTableEntry* entry = source->buckets[i].next;
             entry;
             entry = entry->next)
        {
            const size_t count = hash_table_get_key_count(words, entry->key);
            if (!count) continue;

            if (stored < buffer_size)
                result_buffer[stored++] = entry->key;
            else
                return -1;
        }
    }

    return stored;
}

static double get_vector_length(const HashTable* src);

double get_cosine_distance(const HashTable* src1, const HashTable* src2)
{
    double len1 = 0;
    double dot_product = 0;

    const size_t size1 = src1->bucket_count;
    for (size_t i = 0; i < size1; ++i)
    {
        const HashTableEntry* entry = src1->buckets[i].next;
        while (entry)
        {
            const size_t count = hash_table_get_key_count(src2, entry->key);
            len1 += (double)entry->count * (double)entry->count;
            if (count)
                dot_product += (double)entry->count * (double)count;

            entry = entry->next;
        }
    }

    len1 = sqrt(len1);
    double len2 = get_vector_length(src1);
    
    return dot_product / (len1 * len2);
}

static double get_vector_length(const HashTable* src)
{
    double len = 0;

    const size_t size = src->bucket_count;
    for (size_t i = 0; i < size; ++i)
    {
        const HashTableEntry* entry = src->buckets[i].next;
        while (entry)
        {
            len += entry->count * entry->count;
            entry = entry->next;
        }
    }
    return sqrt(len);
}

