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

#ifndef _CYCLECOUNTER_H
#define _CYCLECOUNTER_H

#include <stdint.h>

/**
 * Read the local cycle counter
 *
 * This function will read architecture dependent cycle counter. The
 * usefulness of this counter depends on the hardware
 * implementation. For example, certain x86-implementation have a
 * non-constant frequency. It is most likely a bad idea to compare
 * cycle counter values from different cores, especially if they
 * belong to CPUs on different sockets.
 *
 * This function does not guarantee that the instruction stream is
 * serialized across, which may lead to unexpected results.
 */
static inline uint64_t cycles_get();

/**
 * Serialize memory operations read the local cycle counter
 *
 * This function will serialize the memory instruction stream and read
 * architecture dependent cycle counter. The usefulness of this
 * counter depends on the hardware implementation. For example,
 * certain x86-implementation have a non-constant frequency. It is
 * most likely a bad idea to compare cycle counter values from
 * different cores, especially if they belong to CPUs on different
 * sockets.
 *
 * Note that this function may not serialize the entire instruction
 * stream, only memory accesses are serialized.
 */
static inline uint64_t cycles_get_mfenced();


#if defined(__i386__) || defined(__x86_64__)

#include "x86/tsc.h"

static inline uint64_t
cycles_get()
{
    return x86_tsc_read();
}


static inline uint64_t
cycles_get_mfenced()
{
    return x86_tsc_read_mfenced();
}


#else

#error Unsupported architecture

#endif

#endif

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * c-file-style: "k&r"
 * End:
 */
