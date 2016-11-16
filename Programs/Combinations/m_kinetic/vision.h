//
//  vision.h
//  
//
//  Created by Matthew Fonken on 11/15/16.
//
//

#ifndef vision_h
#define vision_h

#define VCP_HEADER_LENGTH 3

uint8_t[VCP_HEADER_LENGTH] VCP_STD_H = { 0x56, 0x43, 0x50 }

typedef struct _vcp_header
{
    uint8_t length : 8;
    uint8[] data   : VCP_HEADER_LENGTH * 8;
} vcp_header;

typedef struct _vcp
{
    vcp_header header       : sizeof( vcp_header ) * 8;
    uint8_t    command      : 8;
    uint32_t   timeout      : 32;
    uint8_t    certainty    : 8;
    uint8_t    data_length  : 8;
    uint8_t   *data         : sizeof( data_length ) * 8;
} vcp;

void initVision()
{
    vcp vision;
    vision.header.data   = VCP_STD_H;
    vision.header.length = VCP_HEADER_LENGTH;
}

/*! Get data from vision module */
void getVision()
{
    
    return;
}

/*! Vision module event handler */
void visionEvent()
{
    getVision(); // Update vis[] array
}

#endif /* vision_h */
