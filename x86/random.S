/* define SIZE, ITERATIONS */

#define SIZE (SIZE_k * 1024)
        
        .text
        .global _start

_start:
        xor %rax, %rax
1:
        mov %rax, mem(%rax)
        add $4096, %rax
        cmp $SIZE, %rax
        jl 1b

        mov $(ITERATIONS*SIZE/64), %rcx
        
        mov $6364136223846793005, %r10 /* LCG A */
        mov $1442695040888963407, %r11 /* LCG C */
        mov $42, %r12 /* Seed the random number generator */
1:      
        imul %r10, %r12 /* LCG_A * cur */
        add %r11, %r12 /* LCG_A * cur + LCG_C */
        mov %r12, %rax
        and $(SIZE-1), %rax
        mov mem(%rax), %rax

        dec %rcx
        jnz 1b

        
        mov $1, %rax /* SYS_EXIT */
        mov $0, %rbx /* exit code */
        int $0x80

        .data
mem:
        .space SIZE
