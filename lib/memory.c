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

#ifndef MEM_NO_HUGE
#include <sys/mman.h>
#else
#include <stdlib.h>
#endif

#include "memory.h"

/* Round size (upwards) to the nearest multiple of 2 MiB */
#define ROUND_U(x) (((x) + MEM_HUGE_SIZE) & ~(MEM_HUGE_SIZE - 1))

#ifndef MAP_HUGETLB
/* MAP_HUGETLB is supported for kernels newer than 2.6.32 (might have
 * been supported earlier). Define MAP_HUGETLB in case it wasn't
 * defined by the system headers. */
#define MAP_HUGETLB     0x40000
#endif

#ifndef MEM_NO_HUGE

void *
mem_huge_alloc(size_t size)
{
    void *ptr;
    ptr = mmap(NULL, ROUND_U(size),
	       PROT_READ | PROT_WRITE,
	       MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
	       -1, 0);

    return ptr == MAP_FAILED ? NULL : ptr;
}

void
mem_huge_free(void *addr, size_t size)
{
    munmap(addr, ROUND_U(size));
}

#else

void *
mem_huge_alloc(size_t size)
{
    return malloc(size);
}

void
mem_huge_free(void *addr, size_t size)
{
    free(addr);
}

#endif

/*
 * Local Variables:
 * mode: c
 * c-basic-offset: 4
 * indent-tabs-mode: nil
 * c-file-style: "k&r"
 * End:
 */
