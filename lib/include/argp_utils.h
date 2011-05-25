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

#ifndef ARGP_UTILS_H
#define ARGP_UTILS_H

#include <stdint.h>
#include <argp.h>

long long argp_parse_long_long(struct argp_state *state,
			       const char *name, const char *arg);

long long argp_parse_long(struct argp_state *state,
			  const char *name, const char *arg);
long long argp_parse_int(struct argp_state *state,
			 const char *name, const char *arg);

int64_t argp_parse_int64(struct argp_state *state,
			 const char *name, const char *arg);
int32_t argp_parse_int32(struct argp_state *state,
			 const char *name, const char *arg);
int16_t argp_parse_int16(struct argp_state *state,
			 const char *name, const char *arg);
int8_t argp_parse_int8(struct argp_state *state,
		       const char *name, const char *arg);

uint64_t argp_parse_uint64(struct argp_state *state,
			   const char *name, const char *arg);
uint32_t argp_parse_uint32(struct argp_state *state,
			   const char *name, const char *arg);
uint16_t argp_parse_uint16(struct argp_state *state,
			   const char *name, const char *arg);
uint8_t argp_parse_uint8(struct argp_state *state,
			 const char *name, const char *arg);

size_t argp_parse_size(struct argp_state *state,
		       const char *name, const char *arg);
ssize_t argp_parse_ssize(struct argp_state *state,
			 const char *name, const char *arg);

#endif
