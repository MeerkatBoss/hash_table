#include <math.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <unistd.h>

#include "meerkat_assert/asserts.h"

#include "benchmark.h"

struct ExecTime
{
    double user_ms;
    double sys_ms;
};

static const size_t repeat_count = 10;

static int get_execution_time(int argc, char** argv, ExecTime* exec_time);

int run_test_benchmark(int argc, const char* const* argv,
                       const TestConfig* config)
{
    FILE *output = NULL;
    SAFE_BLOCK_START
    {
        if (config->filename)
        {
            ASSERT_MESSAGE(
                output = fopen(config->filename,
                                config->append_to_file ? "a" : "w"),
                action_result != NULL,
                "Failed to open output file");
        }
        else output = stdout;

        ASSERT_GREATER_MESSAGE(
            argc, 1, "No program to benchmark");
    }
    SAFE_BLOCK_HANDLE_ERRORS
    {
        fprintf(stderr, "Error: %s\n", assertion_info.message);
        return 1;
    }
    SAFE_BLOCK_END

    double sys_time[10], user_time[10];
    double mean_sys = 0, mean_user = 0;
    -- argc;
    ++ argv;

    for (size_t i = 0; i < 10; ++i)
    {
        ExecTime exec_time = {};
        if (get_execution_time(argc, (char**)argv, &exec_time) < 0)
            return 1;

        fprintf(output, "%lf %lf\n", exec_time.user_ms, exec_time.sys_ms);

        mean_sys  += exec_time.sys_ms;
        mean_user += exec_time.user_ms;
        sys_time[i]  = exec_time.sys_ms;
        user_time[i] = exec_time.user_ms;
    }

    mean_sys /= 10;
    mean_user /= 10;

    double var_sys = 0, var_user = 0;
    for (size_t i = 0; i < 10; ++i)
    {
        var_sys  += (sys_time[i]  - mean_sys)  * (sys_time[i]  - mean_sys);
        var_user += (user_time[i] - mean_user) * (user_time[i] - mean_user);
    }

    var_sys  = sqrt(var_sys / 9);
    var_user = sqrt(var_user / 9);

    fprintf(output, "Total: %.3lf (~%.3lf)\n"
                    "User:  %.3lf (~%.3lf)\n"
                    "Sys:   %.3lf (~%.3lf)\n",
                    mean_sys + mean_user, var_sys + var_user,
                    mean_user, var_user,
                    mean_sys,  var_sys);

    return 0;
}

static int get_execution_time(int argc, char** argv, ExecTime* exec_time)
{
    pid_t child = fork();

    if (child < 0)
    {
        perror("Failed to spawn proccess");
        return -1;
    }

    if (child == 0) /* In child */
    {
        int dev_null = open("/dev/null", O_WRONLY);

        if (dev_null < 0)   // On Windows
            dev_null = open("nul", O_WRONLY);

        // Redirect output to /dev/null or nul
        dup2(dev_null, STDOUT_FILENO);

        if (execvp(argv[0], (char**)argv) < 0)
        {
            perror("Failed to start program");
            return -1;
        }

        /* Unreachable */
        return 0;
    }

    rusage usage = {};
    double sum_sys = 0, sum_user = 0;

    getrusage(RUSAGE_CHILDREN, &usage);
    sum_user = (double) usage.ru_utime.tv_sec * 1000.0
                        + (double) usage.ru_utime.tv_usec / 1000.0;
    sum_sys = (double) usage.ru_stime.tv_sec * 1000.0
                        + (double) usage.ru_stime.tv_usec / 1000.0;
    
    int status = 0;
    if (waitpid(child, &status, 0) < 0)
    {
        perror("Failed to wait for child");
        return -1;
    }

    getrusage(RUSAGE_CHILDREN, &usage);

    exec_time->user_ms = (double) usage.ru_utime.tv_sec  * 1000.0
                       + (double) usage.ru_utime.tv_usec / 1000.0
                       - sum_user;
    exec_time->sys_ms  = (double) usage.ru_stime.tv_sec  * 1000.0
                       + (double) usage.ru_stime.tv_usec / 1000.0
                       - sum_sys;

    return 0;
}

