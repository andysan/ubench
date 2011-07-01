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

#include "argp_utils.h"

#include <stdlib.h>
#include <stdint.h>
#include <argp.h>

long long
argp_parse_long_long(struct argp_state *state,
		     const char *name, const char *arg)
{
    char *endptr;
    long value;

    errno = 0;
    value = strtoll(arg, &endptr, 0);
    if (errno)
        argp_failure(state, EXIT_FAILURE, errno,
                     "Invalid %s", name);
    else if (*arg == '\0' || *endptr != '\0')
        argp_error(state, "Invalid %s: '%s' is not a number.\n", name, arg);

    return value;
}

#define PARSE_INTTYPEX(type, fname, min, max)				\
    type								\
    argp_parse_ ## fname(struct argp_state *state,			\
			 const char *name, const char *arg)		\
    {									\
	long long ll = argp_parse_long_long(state, name, arg);		\
	if (ll > max || ll <  min)					\
	    argp_error(state,						\
		       "Invalid %s: '%lli' out of range for " # type ".\n", \
		       name, ll);					\
									\
	return (type)ll;						\
    }


#define PARSE_INTTYPE(type, limit)					\
    PARSE_INTTYPEX(type ## _t, type, limit ## _MIN, limit ## _MAX)

#define PARSE_UINTTYPE(type, limit)			\
    PARSE_INTTYPEX(type ## _t, type, 0, limit ## _MAX)

PARSE_INTTYPEX(long, long, LONG_MIN, LONG_MAX)
PARSE_INTTYPEX(int, int, INT_MIN, INT_MAX)
PARSE_INTTYPE(int64, INT64)
PARSE_INTTYPE(int32, INT32)
PARSE_INTTYPE(int16, INT16)
PARSE_INTTYPE(int8, INT8)

PARSE_UINTTYPE(size, SIZE)
PARSE_INTTYPEX(unsigned int, uint, 0, UINT_MAX)
PARSE_UINTTYPE(uint64, UINT64)
PARSE_UINTTYPE(uint32, UINT32)
PARSE_UINTTYPE(uint16, UINT16)
PARSE_UINTTYPE(uint8, UINT8)
