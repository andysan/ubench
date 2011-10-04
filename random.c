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
static int fix_conflict = 0;
static int fix_conflict_conservative = 0;

static size_t way_size;
static size_t page_limit;
static size_t alloc_size;

#define ACCESS access_rd8

static inline char *
fixed_addr_from_offset(size_t offset)
{
    size_t page, page_offset;
    page = offset / page_limit;
    page_offset = offset % page_limit;

    return data + page * MEM_HUGE_SIZE + page_offset;
}

static inline size_t
next_offset()
{
    lcg_state = rnd_lcg64(lcg_state);

    return lcg_state % bench_size;
}

static inline void
bench_iteration()
{
    const long line_size = bench_settings.line_size;

    if (fix_conflict)
        for (long i = 0; i < bench_size; i += line_size)
            ACCESS(fixed_addr_from_offset(next_offset()));
    else
        for (long i = 0; i < bench_size; i += line_size)
            ACCESS(data + next_offset());
}

RUN_BENCH(run_bench, bench_iteration);

static void
init()
{
    EXPECT_ERRNO(bench_pin_cpu() != -1);

    way_size = bench_settings.cache_shared / bench_settings.assoc_shared;
    if (fix_conflict) {
        if (way_size > MEM_HUGE_SIZE) {
            fprintf(stderr, "Error: Cache way size is bigger than page size\n");
            exit(1);
        }
        page_limit = fix_conflict_conservative ?
            way_size : (MEM_HUGE_SIZE / way_size) * way_size;
    } else
        page_limit = MEM_HUGE_SIZE;

    alloc_size = ((bench_size / page_limit) + 
                  ((bench_size % page_limit) ? 1 : 0)
        ) * MEM_HUGE_SIZE;

    data = mem_huge_alloc(alloc_size);

    EXPECT_ERRNO(data != NULL);

    /** Write something to the first byte in every page, this makes
     * sure that we get backing storage for the entire allocation */
    for (int i = 0; i < alloc_size; i += MEM_HUGE_SIZE)
	data[i] = i & 0xFF;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state);

enum {
    KEY_FIX_CONFLICT = -1,
    KEY_FIX_CONFLICT_CONSERVATIVE = -2,
};

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

    case KEY_FIX_CONFLICT:
        fix_conflict = 1;
        break;

    case KEY_FIX_CONFLICT_CONSERVATIVE:
        fix_conflict = 1;
        fix_conflict_conservative = 1;
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
    { "fix-conflict", KEY_FIX_CONFLICT, NULL, 0,
      "Try to avoid cache conflicts", 0 },
    { "fix-conflict-conservative", KEY_FIX_CONFLICT_CONSERVATIVE, NULL, 0,
      "Try to avoid cache conflicts. Only use one way per page.", 0 },
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
    if (fix_conflict) {
        fprintf(stderr, "Conflict fix active:\n");
        fprintf(stderr, "  Page Limit: %zu\n", page_limit); 
        fprintf(stderr, "  Alloc Size: %zu\n", alloc_size);
    }
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
