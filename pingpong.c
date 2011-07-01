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

static size_t bench_size = 4*1024*1024;
static char *data;

#define ACCESS access_rd8

static inline void
bench_iteration()
{
    const long line_size = bench_settings.line_size;

    for (long i = 0; i < bench_size; i += line_size)
        ACCESS(data + i);

    for (long i = bench_size - line_size; i >= 0; i -= line_size)
        ACCESS(data + i);
}

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
    { "size", 's', "SIZE", 0, "Data set size", 0 },
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
    .doc = "Generate a ping-pong access pattern"
    "\v"
    "This microbenchmark accesses memory in a ping-pong fashion, reading an "
    "array of a specific size from left to right and then from right to left.",
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
