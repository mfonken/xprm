
/*
 * get_frame.s
 *
 * Created: 9/17/2016 1:41:04 PM
 *  Author: Matthew Fonken
 */

 // get_frame.s

 	.globl	get_frame
	.type   get_frame, %function

get_frame:
		ldr		r1, =0x41004420	// PORTA IN
		ldr		r2, =0x20000000	// Memory pointer
		movs	r3, #0x40 		// VSYNC-PA22(<< 2)
		movs	r4, #0x80 		// HREF-PA23(<< 2)
		movs	r5, #0x08 		// PCLK-PA11(<< 1)
		mov		r6, #0

vsync_loop:	
		ldrb	r0, [r1, #2]    // Get PORTA[23..16]
		tst		r0, r3          // Is VSYNC set? 
		bne		vsync_loop      // No, keep waiting

nvsync_loop:	
		ldrb	r0, [r1, #2]    // Get PORTA[23..16]
		tst		r0, r3          // Is VSYNC set? 
		bne		nvsync_loop     // Yes, keep waiting

href_loop:	
		ldrb	r0, [r1, #2]    // Get PORTA[23..16]
		tst		r0, r4          // Is HREF set? 
		beq		pclk_loop       // Yes, jump to PCLK
		tst		r0, r3          // No, is VSYNC set? 
		beq		end             //  Yes, frame is done
		bne		href_loop       //  No, keep waiting for HREF

pclk_loop:	
		ldrb	r0, [r1, #1]    // Get PORTA[15..8]
		tst		r0, r5          // Is PCLK set?
		bne		pclk_loop       // No, keep waiting
                                // Yes, read pixel
		/* ---------- */
		lsl		r0, #2			// Shift port output to end
		add		r6, r6, #1
		/* ---------- */
		                        // After pixel is read...
		ldrb	r0, [r1, #2]    // Get PORTA[23..16]
		tst		r0, r4          // Is HREF still set?
		beq     pclk_loop       // Yes, loop to read next pixel
		bne		href_loop       // No, branch and wait for next line
end:	mov		r0, r6
		bx		lr				// End
						
	


