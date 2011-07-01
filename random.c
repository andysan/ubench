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

#define _GNU_SOURCE
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <argp.h>
#include <errno.h>

#include "expect.h"
#include "memory.h"
#include "timing.h"
#include "cyclecounter.h"
#include "bench_argp.h"
#include "argp_utils.h"
#include "access.h"
#include "rnd_lcg.h"

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

static inline void
bench_iteration()
{
    const long line_size = bench_settings.line_size;

    for (long i = 0; i < bench_size; i += line_size)
        ACCESS(next_address());
}

static void __attribute__((noinline))
run_bench()
{
    timing_t t;
    uint64_t cycles_start;
    uint64_t cycles_stop;

    timing_init(&t);

    timing_start(&t);
    cycles_start = cycles_get();
    for (unsigned int i = 0; i < bench_settings.iterations; i++)
	bench_iteration();
    cycles_stop = cycles_get();
    timing_stop(&t);

    printf("Wall clock time: %.4f\n"
	   "Cycles: %" PRIu64 "\n",
	   t.acc, cycles_stop - cycles_start);
}

static void
init()
{
    if (bench_settings.cpu != -1) {
	cpu_set_t cpu_set;

	CPU_ZERO(&cpu_set);
	CPU_SET(bench_settings.cpu, &cpu_set);
	EXPECT_ERRNO(sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) != -1);
    }

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
