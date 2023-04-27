#include <stdio.h>

#include "hash_table/hash_table.h"
#include "table_utils/utils.h"

#include "test_utils/config.h"
#include "test_cases/histogram.h"

int main(int argc, char** argv)
{
    TestConfig config = {};
    int parsed = configure_tests(argc, argv, &config);

    if (parsed < 0 || config.had_error)
    {
        fprintf(stderr, "Invalid arguments");
        return 1;
    }

    argc -= parsed - 1;
    argv += parsed - 1;

    switch (config.test_case)
    {
    case TEST_HISTOGRAM:
        return run_test_histogram(argc, argv, &config);
    case TEST_BENCHMARK_FULL:
        return 2;
    case TEST_NONE:
    default:
        fprintf(stderr, "Invalid test case");
        return 1;
    }
}

