/*
 * Copyright (C) 2011, Andreas Sandberg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <argp.h>
#include <errno.h>

#include "expect.h"
#include "memory.h"
#include "bench_argp.h"
#include "argp_utils.h"
#include "access.h"
#include "bench_common.h"

static size_t bench_size = 0;

static char *data;

#define ACCESS access_rd8


#if defined(__x86_64__)
static inline void
bench_iteration()
{
    asm("xor %%rcx, %%rcx\n"
        "1:"
        "mov 0(%[base], %%rcx, 1), %%rax\n"

        "add %[line_size], %%rcx\n"
        "cmp %[size], %%rcx\n"
        "jl 1b\n"

        :
        : [size] "r"(bench_size),
          [line_size] "r"(bench_settings.line_size),
          [base] "r"(data)
        : "%rcx", "%rax"
        );
}
#else
static inline void
bench_iteration()
{
    const long line_size = bench_settings.line_size;
    for (long i = 0; i < bench_size; i += line_size)
        ACCESS(data + i);
}
#endif

RUN_BENCH(run_bench, bench_iteration);

static void
init()
{
    if (!bench_size)
        bench_size = 2 * bench_settings.cache_shared;

    EXPECT_ERRNO(bench_pin_cpu() != -1);

    data = mem_huge_alloc(bench_size);
    EXPECT_ERRNO(data != NULL);
    for (int i = 0; i < bench_size; i++)
	data[i] = i & 0xFF;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    long l_arg;

    switch (key)
    {
    case 's':
        bench_size = argp_parse_size(state, "size", arg);
        break;

    case ARGP_KEY_ARG:
	argp_usage(state);
        break;

    case ARGP_KEY_END:
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

const char *argp_program_version =
    "block";

const char *argp_program_bug_address =
    "andreas.sandberg@it.uu.se";

static struct argp_option arg_options[] = {
    { "size", 's', "SIZE", 0, "Override dataset size", 0 },
    { 0 }
};

static struct argp_child arg_children[] = {
    { &bench_argp, 0, "Common options:", 0 },
    { 0 }
};

static struct argp argp = {
    .options = arg_options,
    .parser = parse_opt,
    .args_doc = "",
    .doc = "Simple streaming access pattern"
    "\v"
    "This microbenchmark generates a simple streaming access pattern with "
    "one access stream. The stream uses touches one byte on every cache line "
    "in a data set of 2x the shared cache by default.",
    .children = arg_children,
};

int
main(int argc, char *argv[])
{
    argp_parse (&argp, argc, argv, 0, 0, NULL);

    init();

    printf("Data size: %zu\n", bench_size);
    printf("Iterations: %u\n", bench_settings.iterations);

    run_bench();
    return 0;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * c-file-style: "k&r"
 * End:
 */
