#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "meerkat_assert/asserts.h"

#include "utils.h"
#include "config.h"

int configure_program(int argc, const char* const* argv, ProgramConfig* config)
{
    config->filename1 = NULL;
    config->filename2 = NULL;
    config->output = stdout;
    config->print_verbose = 0;
    config->max_words = -1;

    SAFE_BLOCK_START
    {
        ASSERT_EQUAL_MESSAGE(
            parse_args(argc, argv, &PROGRAM_ARGS, config), argc,
            "Invalid arguments\n");
        ASSERT_TRUE_MESSAGE(
            config->filename1 != NULL,
            "No input file provided\n");
        ASSERT_TRUE_MESSAGE(
            config->filename2 != NULL,
            "Only one input file provided (two expected)\n");
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fputs(assertion_info.message, stderr);
        return -1;
    }
    SAFE_BLOCK_END

    return 0;
}

void unload_config(ProgramConfig* config)
{
    if (config->output != stdout)
        fclose(config->output);

    memset(config, 0, sizeof(*config));
}

int config_get_help([[maybe_unused]] const char* const* str,
                                     void* params)
{
    ProgramConfig* config = (ProgramConfig*) params;

    print_help(&PROGRAM_ARGS);

    unload_config(config);

    exit(0);
}

int config_set_output_file(const char* const* str, void* params)
{
    ProgramConfig* config = (ProgramConfig*) params;

    SAFE_BLOCK_START
    {
        ASSERT_EQUAL_MESSAGE(
            config->output, stdout,
            "Output file can only be specified once\n");
        ASSERT_TRUE_MESSAGE(
            str[0] != NULL,
            "Output file name not specified\n");
        FILE* file = NULL;
        ASSERT_MESSAGE(
            file = fopen(str[0], "w"),
            action_result != NULL,
            "Failed to open output file\n");
        config->output = file;
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fputs(assertion_info.message, stderr);
        return -1;
    }
    SAFE_BLOCK_END

    return 1;
}

int config_set_verbose([[maybe_unused]] const char* const* str,
                                        void* params)
{
    ProgramConfig* config = (ProgramConfig*) params;
    config->print_verbose = 1;
    return 0;
}

int config_set_max_words(const char* const* str, void* params)
{
    ProgramConfig* config = (ProgramConfig*) params;
    SAFE_BLOCK_START
    {
        ASSERT_NEGATIVE_MESSAGE(
            config->max_words,
            "Maximum number of words can only be specified once");
        ASSERT_TRUE_MESSAGE(
            str[0] != NULL,
            "Expected an integer");

        char* endptr = NULL;
        long number = strtol(str[0], &endptr, 10);
        ASSERT_TRUE_MESSAGE(
            *str[0] != '\0' && *endptr == '\0',
            "Invalid number");
        ASSERT_POSITIVE_MESSAGE(
            number, "Expected positive number");
        config->max_words = number;
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fputs(assertion_info.message, stderr);
        return -1;
    }
    SAFE_BLOCK_END

    return 1;
}

int config_add_input_file(const char* const* str, void* params)
{
    ProgramConfig* config = (ProgramConfig*) params;
    if (config->filename1 && config->filename2)
    {
        fputs("Too many input files: only two are allowed\n", stderr);
        return -1;
    }
    if (!config->filename1)
        config->filename1 = str[0];
    else
        config->filename2 = str[0];

    return 1;
}

