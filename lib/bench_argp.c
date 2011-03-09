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

#include "bench_argp.h"

#include <stdlib.h>

enum {
    KEY_CACHE_PRIVATE = -1,
    KEY_CACHE_SHARED = -2,
    KEY_LINE_SIZE = -3,
};

static struct argp_option options[] = {
    { NULL, 0, NULL, 0, "Ubench common:", 1 },
    { "cpu", 'c', "CPU", 0, "Pin to CPU", 1 },
    { "iterations", 'i', "NUM", 0, "Run NUM iterations", 1 },

    { NULL, 0, NULL, 0, "Cache settings:", 2 },
    { "cache-pri", KEY_CACHE_PRIVATE, "SIZE", 0, "Shared cache size", 2 },
    { "cache-sha", KEY_CACHE_SHARED, "SIZE", 0, "Shared cache size", 2 },
    { "line-size", KEY_LINE_SIZE, "SIZE", 0, "Line size", 2 },

    { 0 }
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key) {
    case 'c':
        bench_settings.cpu = bench_argp_parse_int(state, "cpu", arg);
	break;

    case 'i':
        bench_settings.iterations =
            bench_argp_parse_long(state, "iterations", arg);
	break;

    case KEY_CACHE_PRIVATE:
        bench_settings.cache_private =
            bench_argp_parse_long(state, "private cache size", arg);
	break;

    case KEY_CACHE_SHARED:
        bench_settings.cache_private =
            bench_argp_parse_long(state, "shared cache size", arg);
	break;

    case KEY_LINE_SIZE:
        bench_settings.line_size =
            bench_argp_parse_long(state, "line size", arg);
	break;

    case ARGP_KEY_END:
        break;
     
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

long
bench_argp_parse_long(struct argp_state *state, const char *name, const char *arg)
{
    char *endptr;
    long value;

    errno = 0;
    value = strtol(arg, &endptr, 0);
    if (errno)
        argp_failure(state, EXIT_FAILURE, errno,
                     "Invalid %s", name);
    else if (*arg == '\0' || *endptr != '\0')
        argp_error(state, "Invalid %s: '%s' is not a number.\n", name, arg);

    return value;
}

int
bench_argp_parse_int(struct argp_state *state, const char *name, const char *arg)
{
    long value;

    value = bench_argp_parse_long(state, name, arg);
    if (value > INT_MAX || value < INT_MIN)
        argp_error(state, "Invalid %s: '%s' is out of range", name, arg);

    return (int)value;
}


struct argp bench_argp = {
    .options = options,
    .parser = parse_opt,
};

bench_settings_t bench_settings = {
    .cpu = -1,
    .iterations = 1000,
    .cache_private = (32 + 256) * 1024,
    .cache_shared = 12 * 1024 * 1024,
    .line_size = 64,
};

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * c-file-style: "k&r"
 * End:
 */
