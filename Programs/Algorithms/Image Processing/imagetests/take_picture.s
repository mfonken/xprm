//
//  take_picture.s
//  
//
//  Created by Matthew Fonken on 9/27/16.
//
//
loop_until_bit_is_clear(VPORT0_IN,CAM_VD);    //wait for a
loop_until_bit_is_set(VPORT0_IN,CAM_VD);    //new frame
TCC0.CCA = 1;    //divide the clock by 2
                //normally TCC0.CCA = 0
for (y=0; < 96; y++)
{
        asm volatile(
        "cli"                    "nt"
        "L1: in r24, 0x0012"    "nt" //read portA_in (mapped to port0) to r24
        "sbrs r24,1"            "nt" // wait for the rising edge on cam HD (new line)
        "rjmp L1"                "nt" //loop
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt" 

        "L2:nop"                "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "nop"                    "nt"
        "in r24, 0x0016"        "nt"    //read pixel info from portB_in (mapped to port1) to r24
        "st %a0+, r24"           "nt"    

        "in r24, 0x0012"        "nt" //read portA_in to r24
        "sbrc r24,1"            "nt" // wait for the falling edge on cam HD (end of line)
        "rjmp L2"                "nt" //loop
        "sei"                   "nt" //Done, turn 
        :
        : "e" (frame[y])
        : "r24"
    );
    TCC0.CCA = 0; //go back to fast clock
}