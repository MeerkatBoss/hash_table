#include <stdio.h>
#include <stdlib.h>

#include "meerkat_assert/asserts.h"

#include "hash_table/hash_table.h"
#include "table_utils/utils.h"

#include "program.h"

int main(int argc, char** argv)
{
    ProgramState program = {};
    ProgramConfig config = {};

    SAFE_BLOCK_START
    {
        ASSERT_ZERO(
            configure_program(argc, argv, &config));
        ASSERT_ZERO(
            program_init(&program, &config));
        ASSERT_ZERO(
            program_compare_files(&program, &config));
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        unload_config(&config);
        program_finish(&program);
        return 1;
    }
    SAFE_BLOCK_END
    
    unload_config(&config);
    program_finish(&program);
    return 0;
}
