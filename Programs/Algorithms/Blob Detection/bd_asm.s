//
//  bd_asm.s
//  
//
//  Created by Matthew Fonken on 10/3/16.
//
//


.globl	bd_v1
.type   bd_v1, %function

bd_v1:
    pop     {r1,r2,r3,r4,r5,r6,r7}
    ldr     r1, #1
    ldr     r2, #0  // gap
    ldr     r4, #0  // x
    ldr     r5, #0  // y

y_loop:
    add     r5, r5, #1

x_loop:
    add     r4, r4, #1
    tst     r0, r1
    beq     no_pixels

no_pixels:
    ldr     r2, #0

    bl      cma

end:
    push    {r0,r1,r2,r3,r4,r5,r6,r7}
    bx      lr

cma:
