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
#include "rnd_lcg.h"
#include "bench_common.h"

static size_t bench_size = 4*1024*1024;
static char *data;
static uint64_t lcg_state = 42ULL;

#define ACCESS access_rd8

static inline char *
next_address()
{
    lcg_state = rnd_lcg64(lcg_state);

    return data + (lcg_state % bench_size);
}

#if defined(__x86_64__)
static inline void
bench_iteration()
{
    const uint64_t lcg_a = 6364136223846793005ULL;
    const uint64_t lcg_b = 1442695040888963407ULL;

    asm("mov %[i], %%rcx\n"
        "1:"
        "imul %[lcg_a], %[lcg_state]\n"
        "add %[lcg_b], %[lcg_state]\n"

        /* Divide the LCG state by the size to get reminder in rdx */
        "xor %%rdx, %%rdx\n"
        "mov %[lcg_state], %%rax\n"
        "divq %[size]\n"

        "mov 0(%[base], %%rdx), %%rax\n"

        "dec %%rcx\n"
        "jnz 1b\n"

        : [lcg_state] "+r"(lcg_state)
        : [lcg_a] "r"(lcg_a),
          [lcg_b] "r"(lcg_b),
          [size] "r"(bench_size),
          [i] "r"(bench_size / bench_settings.line_size),
          [base] "r"(data)
        : "rax", "rdx", "rcx"
        );
}
#else
static inline void
bench_iteration()
{
    const long line_size = bench_settings.line_size;

    for (long i = 0; i < bench_size; i += line_size)
        ACCESS(next_address());
}
#endif

RUN_BENCH(run_bench, bench_iteration);

static void
init()
{
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

    case 'r':
        lcg_state = argp_parse_uint64(state, "num", arg);
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
    "random";

const char *argp_program_bug_address =
    "andreas.sandberg@it.uu.se";

static struct argp_option arg_options[] = {
    { "size", 's', "SIZE", 0, "Data set size", 0 },
    { "random-seed", 'r', "NUM", 0, "Random seed", 0 },
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
    .doc = "Generate a random access pattern"
    "\v"
    "This microbenchmark accesses memory in a random fashion, reading one "
    "byte from a random cacheline in an array of a specific size. The number "
    "of random accesses per iteration is the data_size / line_size.",
    .children = arg_children,
};

int
main(int argc, char *argv[])
{
    argp_parse (&argp, argc, argv, 0, 0, NULL);

    init();

    printf("Data size: %zu\n", bench_size);
    printf("Seed: %" PRIu64 "\n", lcg_state);
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
