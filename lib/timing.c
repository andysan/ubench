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

#include "timing.h"
#include "expect.h"
#include <stdio.h>

#if defined(CLOCK_HIGHRES)
#define CLOCK_ID CLOCK_HIGHRES
#elif defined(CLOCK_MONOTONIC)
#define CLOCK_ID CLOCK_MONOTONIC
#else
#error No suitable clock type found.
#endif

#define SECONDS(ts) (ts.tv_sec + ts.tv_nsec * 1E-9)

double
timing_precision()
{
    struct timespec ts;

    EXPECT_ERRNO(clock_getres(CLOCK_ID, &ts) == 0);

    return SECONDS(ts);
}

void
timing_init(timing_t *t)
{
    t->acc = 0.0;
}

void
timing_start(timing_t *t)
{
    EXPECT_ERRNO(clock_gettime(CLOCK_ID, &t->start) == 0);
}

void
timing_stop(timing_t *t)
{
    struct timespec ts;

    EXPECT_ERRNO(clock_gettime(CLOCK_ID, &ts) == 0);

    EXPECT(ts.tv_sec > t->start.tv_sec ||
	   (ts.tv_sec == t->start.tv_sec &&
	    ts.tv_nsec >= t->start.tv_nsec));

    t->acc += ts.tv_sec - t->start.tv_sec;
    if (t->start.tv_nsec < ts.tv_nsec)
	t->acc += (ts.tv_nsec - t->start.tv_nsec) * 1E-9;
    else
	t->acc -= (t->start.tv_nsec - ts.tv_nsec) * 1E-9;
}

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * c-file-style: "k&r"
 * End:
 */
