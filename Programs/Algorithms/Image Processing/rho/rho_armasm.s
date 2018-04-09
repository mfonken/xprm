/* REGISTERS: */
//  r0-r3 - Reserved
#define x   r4  // Column index
#define th  r5  // Threshold
#define tg  r6  // Green toggle
#define wr  r7  // Write index
#define rd  r8  // Read index
#define cp  r9  // Camera Port
#define rx  r10 // Read col index
#define ry  r11 // Read row index

/* System Constants */
#define W                   300
#define H                   300
#define Y_DEF               0xaa
#define MAX_COVERAGE        0.5 // %
#define C_FRAME_SIZE        ((MAX_COVERAGE * ( W * H )))
#define IS_GREEN           0x01

    area    rho, code, readonly
    entry

/* RAM types - Pre-assigned */
CAMERA_PORT
    .word   0x /* GPIO_PORTA_L */
sram
    .word   0x2000_0000
Dx
    .word   sram
Dy
    .word   Dx + W
Q
    .word   Dy + H
QT
    .word   Q  + #20
QN_
    .word   QT + #4
Ce  /* Capture end */
    .word   QN_ + #4
Cf  /* Capture frame */
    .word   Ce + #4
Cm  /* Capture max */
    .word   Cf + #C_FRAME_SIZE
THRESH_ADDR
    .word   Cm

ram_clear
    .word   Cm - Dx

rho_init
    function
    export rho_init

    mov cp, CAMERA_PORT

    endp

frame_start
    function
    export frame_start

    ldr r0, Dx                  /* Clear RAM */
    mov r1, #0
    ldr r2, ram_clear
    bl  memset
    ldr wr, Cf                  /* wr = Cf; */
    ldr rd, Cf                  /* rd = Cf; */
    ldr th, [THRESH_ADDR]       /* th = THRESH_ADDR; */
    mov tg, #0                  /* Reset operation variables */
    b   rho_process             /* Start processing loop */
    br  lx
    endp

frame_end
    function
    export frame_end
    str wr, [Ce]
    ldr r0, Q                   /* int * Qp = &Q; */
    ldr r2, [r0] #4             /* QT += *(Qp++); x4 */
    ldr r1, [r0] #4
    add r2, r2, r1
    ldr r1, [r0] #4
    add r2, r2 r1
    ldr r1, [r0]
    add r2, r2, r1
    str r2, [QT]
    endp

row_int
    function
    export row_int

    str Y_DEL, [wr]     /* (*(wr) = Y_DEL)++; */
    add wr, wr, #4
    eor tg, tg, 0x01    /* tg ^= 0x01; */
    mov x, tg           /* x = tg; */
    endp

pclk_int_low
    function
    export pclk_int_low
    add x, x, #1        /* x++ */
    endp

pclk_int_high
    function
    export pclk_int_high
    tst x, 0x01         /* if( x & 0x01 ) burn */
    beq .burn_pixel
    ldr r3, [cp]        /* if( cp > th ) */
    cmp r3, th
    strbge x, [wr] #1   /* (*(wr) = x)++; */
.burn_pixel
    endp

//pixel_process
//    function
//    export pixel_process
//    ldr c0, [cp]        /* if( cp > th ) */
//    cmp th, c0
//    strbls x, [wr] #1   /* (*(wr) = x)++; */
////    strb wr, [wr] #1
//    add x, x, #1        /* x++; */
//    endp

rho_process
    function
    export rho_process
/* int rx = 0, ry = 0; */
    mov rx, #0
    mov ry, #0
/* while( rd != Ce ) */
.read_loop
    cmp rd, wr              /* if( rd < wr ) */
    bcs .read_loop
    ldr rx, [rd] #4         /* rx = *(rd++); */
    cmp rx, Y_DEL           /* if( rx == Y_DEL ) */
    bne .no_del
    cmp ry, Cy              /* if( ry < Cy ) */
    bge .higher
    and QS, QS, 0b11111101  /* QS &= 0xfd; */
    ldr r0, Q               /* QN = *(&Q) + *(&Q+1); */
    ldr r1, [r0], #4
    ldr r2, [r0]
    add r0, r1, r2
    b .dx_store
.higher
    orr QS, QS, 0b00000010  /* QS |= 0x02; */
    ldr r0, Q               /* QN = *(&Q+2) + *(&Q+3); */
    add r0, r0, #8
    ldr r1, [r0], #4
    ldr r2, [r0]
    add r0, r1, r2
.dx_store                   /* (*(&Dx+ry++)) = QN - QN_; */
    ldr r1, [QN_]
    sub r1, r0, r1
    str r1, [Dx, ry]
    add ry, ry, #4
    str r0, [QN_]           /* QN_ = QN; */
.no_del                     /* else */
    cmp rx, Cx              /* if( rx < Cx ) */
    bge .right
    and QS, QS, 0b11111110  /* QS &= 0xfe; */
    b .dy_store             /* else */
.right
    orr QS, QS, 0b00000001  /* QS |= 0x01; */
.dy_store
    ldr r0, [Dy, rx]        /* (*(&Dy+rx))++; */
    add r0, r0, #1
    str r0, [Dy, rx]
    ldr r0, [Q, QS]         /* (*(&Q+QS))++; */
    add r0, r0, #1
    str r0, [Q, QS]

    ldr r0, [Ce]   /* if( rd < C_FRAME_END ) */
    cmp rd, r0
    blt .read_loop
    endp

    end
