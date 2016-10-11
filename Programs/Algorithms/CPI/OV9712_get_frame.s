/*
 * get_frame.s
 *
 * Created: 9/17/2016 1:41:04 PM
 *  Author: Matthew Fonken
 */

 // get_frame.s

#define VSYNC		1ul << 22
#define HREF		1ul << 23
#define PCLK		1ul << 11

.globl	get_frame
.type   get_frame, %function
 
img_buf =0x20000000
porta	=0x41004420	
vsync	=VSYNC
href	=HREF
pclk	=PCLK

/* START */
get_frame:
		ldr		r1, =porta
		ldr		r2, =img_bufz
        ldr     r3, =vsync
        ldr     r4, =href
        ldr     r5, =pclk

/* VSYNC */
vsync_loop:	
		ldr     r0, [r1]        // Get PORTA
		tst     r0, r3          // Is VSYNC set?
		bne		vsync_loop      // No, keep waiting

nvsync_loop:
		ldr     r0, [r1]        // Get PORTA
		tst     r0, r3          // Is VSYNC set?
		beq		nvsync_loop     // Yes, keep waiting

/* HREF */
href_loop:	
		ldr     r0, [r1]        // Get PORTA
		tst     r0, r4          // Is HREF set?
		beq		pclk_loop       // Yes, jump to PCLK
        tst     r0, r3          // No, is VSYNC set?
        beq		end             //  Yes, frame is done
        bne     href_loop       //  No, keep waiting for HREF

/* PCLK */
pclk_loop:
		ldr     r0, [r1]        // Get PORTA
		tst     r0, r5          // Is PCLK set?
		bne		pclk_loop       // No, keep waiting
                                // Yes, read pixel
/* Read pixel */
		lsl		r0, #2			// Shift port output to end
        strb    [r2], r0
/* ---------- */
                                // After pixel is read...
        ldr     r0, [r1]        // Get PORTA
		tst     r0, r4          // Is HREF still set?
        beq     pclk_loop       // Yes, loop to read next pixel
		bne		href_loop       // No, branch and wait for next line
/* END */
end:	bx		lr