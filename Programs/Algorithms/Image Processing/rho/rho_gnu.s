/* REGISTERS: */
//  r0-r3 - Reserved
//.set x,   r4  // Column index
//.set th,  r5  // Threshold
//.set tg,  r6  // Green toggle
//.set wi,  r7  // Write index
//.set ri,  r8  // Read index
//.set cp,  r9  // Camera Port
//.set rx,  r10 // Read col index
//.set ry,  r11 // Read row index
//.set QS,  r12 // Quadrant selection
//.set Cx,  r13 // Quadrant center x value
//.set Cy,  r14 // Quadrant centroid y value

;/* System Constants */
.set W,                 300
.set H,                 300
.set Y_DEL,             0xaa
.set C_FRAME_SIZE,      10
.set IS_GREEN,          0x01

    .section .text,"ax"    // 5.7 Sections
    .balign 4

;/* RAM types - Pre-assigned */
CAMERA_PORT:
    .word   0x20100000 ;/* GPIO_PORTA_L */
sram:
    .word   0x20000000
Dx:
    .word   sram
Dy:
    .word   Dx + W
Q:
    .word   Dy + H
QT:
    .word   Q  + 20
QN_:
    .word   QT + 4
Ce:  ;/* Capture end */
    .word   QN_ + 4
Cf:  ;/* Capture frame */
    .word   Ce + 4
Cm:  ;/* Capture max */
    .word   Cf + C_FRAME_SIZE
Cx:
    .word   Cm
Cy:
    .word   Cx + 4
THRESH_ADDR:
    .word   Cy + 4

ram_clear:
    .word   Cm - Dx

rho_init:
    mov r0, #CAMERA_PORT

frame_start:
    ldr r0, Dx                  ;;/* Clear RAM */
    mov r1, #0
    ldr r2, ram_clear
    bl  memset
    ldr r7, Cf                  ;;/* wi = Cf; */
    ldr r8, Cf                  ;;/* ri = Cf; */
    ldr r5, THRESH_ADDR         ;;/* th = *THRESH_ADDR; */
    mov r6, #0                  ;;/* Reset operation variables */
                ;b   rho_process             ;;/* Start processing loop */
    bx  lr

frame_end:
    str r0, Ce
    str r7, [r0]
    ldr r0, Q                   ;/* int * Qp = &Q; */
    ldr r2, [r0], #4            ;/* QT += *(Qp++); x4 */
    ldr r1, [r0], #4
    add r2, r2, r1
    ldr r1, [r0], #4
    add r2, r2, r1
    ldr r1, [r0]
    add r2, r2, r1
//    mov r0, QT
    str r2, QT
    bx  lr

row_int:
    mov r0, #Y_DEL
    str r0, [r7]                ;/* (*(wi) = Y_DEL)++; */
    add r7, r7, #4
    eor r6, r6, #0x01           ;/* tg ^= 0x01; */
    mov r4, r6                  ;/* x = tg; */
    bx  lr

pclk_int_low:
    add r4, r4, #1              ;/* x++ */
    bx  lr

pclk_int_high:
    tst r4, #0x01               ;/* if( x & 0x01 ) burn */
    beq .burn_pixel
    ldr r3, [r9]                ;/* if( cp > th ) */
    cmp r3, r5
    strge r4, [r7], #1          ;/* (*(wi) = x)++; */
burn_pixel:
    bx  lr

//pixel_process
//    //    export pixel_process
//    ldr c0, [cp]        ;/* if( cp > th ) */
//    cmp r5, c0
//    strbls r4, [wi] #1   ;/* (*(wi) = x)++; */
////    strb r7, [wi] #1
//    add r4, r4, #1        ;/* x++; */
//
rho_process:
    mov r10, #0                 ;/* int rx = 0, ry = 0; */
    mov r11, #0
    mov r12, #0
read_loop:                      ;/* while( ri != Ce ) */
    cmp r8, r7                  ;/* if( ri < wi ) */
    bcs .read_loop
    ldr r10, [r8], #4           ;/* rx = *(ri++); */
    cmp r10, #Y_DEL             ;/* if( rx == Y_DEL ) */
    bne .no_del
    ldr r0, Cy                  ;/* if( ry < *Cy ) */
    cmp r11, r0
    bge .higher
    and r12, r12, #0xfd         ;/* QS &= 0xfd; */
    ldr r0, Q                   ;/* QN = *(&Q) + *(&Q+1); */
    ldr r1, [r0], #4
    ldr r2, [r0]
    add r0, r1, r2
    b .dx_store
higher:
    orr r12, r12, #0x02         ;/* QS |= 0x02; */
    ldr r0, Q                   ;/* QN = *(&Q+2) + *(&Q+3); */
    add r0, r0, #8
    ldr r1, [r0], #4
    ldr r2, [r0]
    add r0, r1, r2
dx_store:                        ;/* (*(&Dx+ry++)) = QN - QN_; */
    ldr r1, QN_
    sub r1, r0, r1
    mov r2, #Dx
    str r1, [r11, r2]
    add r11, r11, #4
    str r0, [r2]                ;/* QN_ = QN; */
no_del:                         ;/* else */
    ldr r0, Cx                  ;/* if( rx < *Cx ) */
    cmp r10, r0
    bge .right
    and r12, r12, #0xfe         ;/* QS &= 0xfe; */
    b .dy_store                 ;/* else */
right:
    orr r12, r12, #0x01         ;/* QS |= 0x01; */
dy_store:
    mov r1, #Dy
    ldr r0, [r1, r10]           ;/* (*(&Dy+rx))++; */
    add r0, r0, #1
    str r0, [r1, r10]
    mov r1, #Q
    ldr r0, [r1, r12]           ;/* (*(&Q+QS))++; */
    add r0, r0, #1
    str r0, [r1, r12]
    ldr r0, Ce                  ;/* if( ri < *C_FRAME_END ) */
    ldr r0, [r0]
    cmp r8, r0
    blt .read_loop
    bx  lr

    .end
