//
//  sercom.cpp
//  combine_core
//
//  Created by Matthew Fonken on 12/7/17.
//  Copyright © 2017 Marbl. All rights reserved.
//

#include "sercom.h"

int Init_SERCOM_Default( void )
{
    SERCOM_Channel chan = SERCOM_Channel_Default;
    chan.filestream = open(chan.port, O_RDWR | O_NOCTTY | O_NDELAY);		//Open in non blocking read/write mode
    if (chan.filestream == -1)
    {
        printf("Trying alternate port at %s\n", chan.port_alt);
        chan.filestream = open(chan.port_alt, O_RDWR | O_NOCTTY | O_NDELAY);
        if (chan.filestream == -1)
            printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
        else
            printf("Initializing SERCOM at %s: success on alternate.\n", chan.port);
    }
    else printf("Initializing SERCOM at %s(%d): success on main.\n", chan.port, chan.filestream);
    
    tcgetattr(chan.filestream, &chan.options);
    chan.options.c_cflag = chan.baud | chan.bits | CLOCAL | CREAD;
    chan.options.c_iflag = IGNPAR;
    chan.options.c_oflag = 0;
    chan.options.c_lflag = 0;
    tcflush(chan.filestream, TCIFLUSH);
    tcsetattr(chan.filestream, TCSANOW, &chan.options);
    return chan.filestream;
}

void Write_SERCOM_Byte( int filestream, char b )
{
    int byte_written = -1;
    while(byte_written <= 0) byte_written = (int)write(filestream, &b, 1);
}

void Write_SERCOM_Bytes( int filestream, const char * a, int l )
{
    int byte_written = -1;
    while(byte_written <= 0) byte_written = (int)write(filestream, a, (size_t)l);
}

int Read_SERCOM_Bytes( int filestream, char * a, int l )
{
    return (int)read(filestream, a, (size_t)l);
}

int Read_SERCOM_Byte( int filestream, char * b )
{
    return (int)read(filestream, b, 1);
}

int Test_SERCOM( int filestream )
{
    char t;
    return (int)read(filestream, &t, 1);
}
