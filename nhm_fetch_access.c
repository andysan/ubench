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

#define CACHE_PRIV ((32 + 256) * 1024)
#define LINE_SIZE (64)

static size_t bench_size = 16*1024*1024;
static int bench_size_mask = 16*1024*1024 - 1;
static size_t bench_priv_size = CACHE_PRIV;
static size_t bench_line_size = LINE_SIZE;

static long bench_distance = CACHE_PRIV * 1.5;
static long bench_streams = 3;
static long bench_iterations = 1000;

static int param_cpu = -1;

static char *data;

static inline char __attribute__((always_inline))
access(const char *d)
{
    char c;
    asm volatile ("mov %1, %0"
	 : "=r"(c)
	 : "m"(*d));
    return c;
}

static inline void
bench_iteration()
{

    for (long i = 0; i < bench_size; i += bench_line_size) {
	for (long j = 0; j < bench_streams; j++)
	    access(data +
		   ((i + bench_distance * j) & bench_size_mask));
    }
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
    for (long i = 0; i < bench_iterations; i++)
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
    if (param_cpu != -1) {
	cpu_set_t cpu_set;

	CPU_ZERO(&cpu_set);
	CPU_SET(param_cpu, &cpu_set);
	EXPECT_ERRNO(sched_setaffinity(0, sizeof(cpu_set_t), &cpu_set) != -1);
    }

    data = mem_huge_alloc(bench_size);
    EXPECT_ERRNO(data != NULL);
    for (int i = 0; i < bench_size; i++)
	data[i] = i & 0xFF;
}

static long
arg_long(struct argp_state *state, const char *arg, const char *errmsg)
{
    long ret;
    char *endptr;

    errno = 0;
    ret = strtol(arg, &endptr, 0);
    if (arg == endptr || *endptr || errno != 0)
	argp_error(state, "%s", errmsg);

    return ret;
}

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
    long l_arg;

    switch (key)
    {
    case 'c':
	l_arg = arg_long(state, arg, "Invalid CPU specified");
        if (l_arg > INT_MAX || l_arg < INT_MIN)
            argp_error(state, "CPU number out of range");

	param_cpu = (int)l_arg;
	break;

    case 'i':
        bench_iterations = arg_long(state, arg, "Number of iterations is invalid");
        break;

    case 's':
        bench_streams = arg_long(state, arg, "Number of streams is invalid");
        break;

    case 'd':
        bench_distance = arg_long(state, arg, "Distance is invalid");
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
    "nhm_fetch_access";

const char *argp_program_bug_address =
    "andreas.sandberg@it.uu.se";

static struct argp_option arg_options[] = {
    { "cpu", 'c', "CPU", 0, "Pin process to CPU", 0 },
    { "iterations", 'i', "NUM", 0, "Run NUM iterations", 0 },
    { "streams", 's', "NUM", 0, "Use NUM streams", 0 },
    { "distance", 'd', "NUM", 0, "Stream distance in bytes", 0 },
    { 0 }
};

static struct argp argp = {
    .options = arg_options,
    .parser = parse_opt,
    .args_doc = "",
    .doc = "Generate a parameterizable access stream to L3"
    "\v"
    "This microbenchmark accesses memory in multiple streams sequential "
    "streams. Each stream is has a distance of 1.5x the private cache size "
    "of a core by default.",
};

int
main(int argc, char *argv[])
{
    argp_parse (&argp, argc, argv, 0, 0, NULL);

    init();
    run_bench();
    return 0;
}
