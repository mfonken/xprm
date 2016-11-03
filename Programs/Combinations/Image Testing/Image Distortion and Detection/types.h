//
//  types.h
//  
//
//  Created by Matthew Fonken on 11/1/16.
//
//

#ifndef types_h
#define types_h

typedef struct _coord
{
    double x;
    double y;
} coord;

typedef struct _blob
{
    double  X;
    double  Y;
    
    int     height;
    int     w_last;
    int     x_last;
    int     y_last;
} blob;

#endif /* types_h */
