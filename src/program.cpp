#include <stdlib.h>
#include <stdio.h>

#include "table_utils/utils.h"
#include "meerkat_assert/asserts.h"

#include "program.h"

__always_inline
static size_t max(size_t a, size_t b)
{
    return a > b ? a : b;
}

int program_init(ProgramState* state, const ProgramConfig* config)
{
    SAFE_BLOCK_START
    {
        ASSERT_ZERO(
            hash_table_ctor(&state->file1_words, hash_table_bucket_count));
        ASSERT_ZERO(
            hash_table_ctor(&state->file2_words, hash_table_bucket_count));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        perror("Word sets construction");
        return -1;
    }
    SAFE_BLOCK_END

    SAFE_BLOCK_START
    {
        ASSERT_ZERO_MESSAGE(
                fill_hash_table(&state->file1_words,
                    config->filename1, config->max_words),
                config->filename1);
        ASSERT_ZERO_MESSAGE(
                fill_hash_table(&state->file2_words,
                    config->filename2, config->max_words),
                config->filename2);
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fprintf(stderr, "Failed to read file '%s'\n", assertion_info.message);
        return -1;
    }
    SAFE_BLOCK_END
    
    if (!config->print_verbose)
    {
        state->diff_array = NULL;
        state->diff_array_size = 0;
        return 0;
    }

    SAFE_BLOCK_START
    {
        const size_t max_size = max(state->file1_words.distinct_count,
                                    state->file2_words.distinct_count);
        const char** array = NULL;
        ASSERT_SIMPLE(
            array = (const char**) calloc(max_size, sizeof(*array)),
            action_result != NULL);

        state->diff_array = array;
        state->diff_array_size = max_size;
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        perror("Verbose difference array allocation");
        return -1;
    }
    SAFE_BLOCK_END

    return 0;
}

int program_compare_files(ProgramState* state, const ProgramConfig* config)
{
    const double cosine = get_cosine_similarity(&state->file1_words,
                                                &state->file2_words);
    fprintf(config->output, "Cosine similarity: %lf\n", cosine);

    ssize_t cnt = get_table_diff(&state->file1_words,
                                 &state->file2_words,
                                 state->diff_array,
                                 state->diff_array_size);
    if (cnt < 0) return -1;

    fprintf(config->output,
            "\nTotal distinct words in '%s' which are not in '%s': %zd\n",
            config->filename1, config->filename2, cnt);

    if (state->diff_array)
    {
        fputs("========================================\n", config->output);

        for (ssize_t i = 0; i < cnt; ++i)
        {
            fputs(state->diff_array[i], config->output);
            fputc('\n', config->output);
        }
        
        fputs("========================================\n", config->output);
    }

    cnt = get_table_diff(&state->file2_words,
                         &state->file1_words,
                         state->diff_array,
                         state->diff_array_size);
    if (cnt < 0) return -1;

    fprintf(config->output,
            "\nTotal distinct words in '%s' which are not in '%s': %zd\n",
            config->filename2, config->filename1, cnt);

    if (state->diff_array)
    {
        fputs("========================================\n", config->output);

        for (ssize_t i = 0; i < cnt; ++i)
        {
            fputs(state->diff_array[i], config->output);
            fputc('\n', config->output);
        }
        
        fputs("========================================\n", config->output);
    }

    return 0;
}

void program_finish(ProgramState* state)
{
    hash_table_dtor(&state->file1_words);
    hash_table_dtor(&state->file2_words);
    if (state->diff_array)
        free(state->diff_array);
}

