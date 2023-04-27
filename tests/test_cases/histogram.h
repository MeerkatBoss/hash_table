/**
 * @file histogram.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-27
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __TESTS_TEST_CASES_HISTOGRAM_H
#define __TESTS_TEST_CASES_HISTOGRAM_H

#include <stddef.h>

#include "meerkat_args/argparser.h"

#include "test_utils/config.h"


struct HistogramConfig
{
    ssize_t table_size;
    const char* filename;
};

/**
 * @brief Load file into hash table and print its bucket sizes
 *
 * @param[in] argc	    - Argument vector length
 * @param[in] argv	    - Argument vector
 * @param[in] config	- Test configuration
 *
 * @return Exit status
 */
int run_test_histogram(int argc, const char* const* argv,
                       const TestConfig* config);

/**
 * @brief Load next test argument
 *
 * @param[in]    str    Parameter array
 * @param[inout] params HistogramConfig instance
 *
 * @return 1 upon success, -1 otherwise
 */
int histogram_next_arg(const char* const* str, void* params);

const arg_info HISTOGRAM_ARGS = {
    .help_message = 
        "histogram <TABLE SIZE> <INPUT FILE> - Load file and calculate "
                                                            "bucket sizes",
    .name_handler = NULL,
    .plain_handler = histogram_next_arg,
    .tags = NULL,
    .tag_cnt = 0
};

#endif /* histogram.h */
