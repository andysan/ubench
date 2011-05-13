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

#ifndef _TSC_H
#define _TSC_H

#include <stdint.h>

/**
 * Read the local TSC counter
 *
 * This function is a wrapper around the x86 rdtsc instruction. In
 * order to get useful results, make sure that the CPU supports the
 * "invariant TSC" feature. CPUs without this feature may have an
 * unstable TSC frequency as a result of power save features. Also,
 * remember that the TSC may not be the same across all cores in a
 * multi-socket system and that the rdtsc instruction is not serializing.
 */
static inline uint64_t x86_tsc_read();

/**
 * Serialize memory operations and read the local TSC counter
 *
 * This function is a wrapper around the x86 mfence and rdtsc
 * instructions. In order to get useful results, make sure that the
 * CPU supports the "invariant TSC" feature. CPUs without this feature
 * may have an unstable TSC frequency as a result of power save
 * features.  Also, remember that the TSC may not be the same across
 * all cores in a system.
 *
 * Note that the mfence instruction will only serialize memory
 * accesses, other instructions may still be reordered across the
 * mfence and the rdtsc instruction.
 */
static inline uint64_t x86_tsc_read_mfenced();

#if defined(__x86_64__)

static inline uint64_t
x86_tsc_read()
{
    uint64_t tsc;
    asm volatile ("rdtsc\n\t"
                  "shl $32, %%rdx\n\t"
                  "or %%rdx, %%rax"
                  : "=a"(tsc)
                  :
                  : "rdx");
    return tsc;
}

static inline uint64_t
x86_tsc_read_mfenced()
{
    uint64_t tsc;
    asm volatile ("mfence\n\t"
                  "rdtsc\n\t"
                  "shl $32, %%rdx\n\t"
                  "or %%rdx, %%rax"
                  : "=a"(tsc)
                  :
                  : "rdx");
    return tsc;
}

#elif defined(__i386__)

static inline uint64_t
x86_tsc_read()
{
    uint32_t eax, edx;
    asm volatile ("rdtsc"
                  : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
}

static inline uint64_t
x86_tsc_read_mfenced()
{
    uint32_t eax, edx;
    asm volatile ("mfence\n\t"
                  "rdtsc"
                  : "=a"(eax), "=d"(edx));
    return ((uint64_t)edx << 32) | eax;
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
