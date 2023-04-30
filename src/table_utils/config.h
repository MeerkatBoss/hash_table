/**
 * @file config.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-23
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TABLE_UTILS_CONFIG_H
#define __TABLE_UTILS_CONFIG_H

#include <stdio.h>

#include "meerkat_args/argparser.h"

static const size_t hash_table_bucket_count = 7019;

struct ProgramConfig
{
    const char* filename1;
    const char* filename2;

    FILE* output;
    int print_verbose;
    ssize_t max_words;
};

/**
 * @brief Load program configuration
 *
 * @param[in]  argc	    - Argument vector length
 * @param[in]  argv	    - Program argument vector
 * @param[out] config	- Loaded program configuration
 *
 * @return 0 upon success, -1 upon loading error
 */
int configure_program(int argc, const char* const* argv, ProgramConfig* config);

/**
 * @brief Dispose the `ProgramConfig` object
 *
 * @param[inout] config	- `ProgramConfig` instance to be disposed
 */
void unload_config(ProgramConfig* config);

/**
 * @brief Output help information on parameter tags and terminate program
 *
 */
__attribute__((noreturn))
int config_get_help(const char* const* str, void* params);

/**
 * @brief Set output file for program
 * 
 * @param[in]    str    - Input arguments
 * @param[inout] params - `ProgramConfig` instance
 *
 * @return 1 on successful parse, -1 otherwise
 */
int config_set_output_file(const char* const* str, void* params);

/**
 * @brief Set verbose output for program
 * 
 * @param[in]    str    - Input arguments
 * @param[inout] params - `ProgramConfig` instance
 *
 * @return 0
 */
int config_set_verbose(const char* const* str, void* params);

/**
 * @brief Set max number of words to be read from file
 * 
 * @param[in]    str    - Input arguments
 * @param[inout] params - `ProgramConfig` instance
 *
 * @return 1 on successful parse, -1 otherwise
 */
int config_set_max_words(const char* const* str, void* params);

/**
 * @brief Add input file for program
 * 
 * @param[in]    str    - Input arguments
 * @param[inout] params - `ProgramConfig` instance
 *
 * @return 1 on successful parse, -1 otherwise
 */
int config_add_input_file(const char* const* str, void* params);

static const arg_tag PARAMETER_TAGS[] = {
    {
        .short_tag = 'h',
        .long_tag = "help",
        .callback = config_get_help,
        .description = 
            "Print help message and exit program"
    },
    {
        .short_tag = 'o',
        .long_tag = "output-file",
        .callback = config_set_output_file,
        .description = 
            "Print output to specified file instead of stdout"
    },
    {
        .short_tag = 'v',
        .long_tag = "verbose-diff",
        .callback = config_set_verbose,
        .description = 
            "Output each differing entry, instead of only the count"
    },
    {
        .short_tag = 'n',
        .long_tag = NULL,
        .callback = config_set_max_words,
        .description = 
            "Read only first <n> words from both files"
    }
};

static const arg_info PROGRAM_ARGS = {
    .help_message = 
        "hash_table [-o <file> | --output-file <file>]\n"
        "\t[-v | --verbose-diff] <file1> <file2>\t- Compare text files\n"
        "hash_table [-h | --help]\t- Get help on program usage",
    .name_handler = NULL,
    .plain_handler = config_add_input_file,
    .tags = PARAMETER_TAGS,
    .tag_cnt = sizeof(PARAMETER_TAGS) / sizeof(*PARAMETER_TAGS)
};

#endif /* config.h */
