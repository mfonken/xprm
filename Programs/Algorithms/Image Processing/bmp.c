//
//  bmp.h
//
//
//  Created by Matthew Fonken on 10/28/16.
//
//

#ifndef bmp_h
#define bmp_h

#include <stdint.h>

uint8_t bmp_header[54] =
{ 42, 4d,         // BMP type signature
	36, 30, 00, 00, // Image size (unreliable)
	00, 00, // Reserved
	00, 00, // Reserved
	36, 00, 00, 00, // Image offset
	28, 00, 00, 00, // Size of BITMAPINFOHEADER structure (must be 0x28)
	40, 00, 00, 00, // Image width
	40, 00, 00, 00, // Image Height
	01, 00,         // Number of planes (must be 1)
	18, 00,         // Number of bits per pixel (1, 4, 8, or 0x18)
	00, 00, 00, 00, // Compression type
	00, 30, 00, 00, // Size of image data in bytes + padding
	00, 00, 00, 00, // Horizontal resolution (unreliable)
	00, 00, 00, 00, // Vertical resolution (unreliable)
	00, 00, 00, 00, // Number of colors in image or zero
	00, 00, 00, 00  // Number of important colors or zero
}

typedef struct _bmp
{
	22
} bmp;

#endif /* bmp_h */
