#ifndef _CENTROID_H
#define _CENTROID_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define MAX_BLOBS       30
#define MAX_GAP         50
    
static inline void cma( double new_val, double *avg, uint16_t num )
{
    *avg += ( new_val - *avg ) / ( num + 1 );
}
    
typedef struct
{
    double      X;
    double      Y;

    uint16_t    height;
    uint16_t    w_last;
    uint16_t    x_last;
    uint16_t    y_last;
} blob_t;

typedef struct
{
    uint8_t     numBlobs;
    blob_t      blobs[MAX_BLOBS];
} centroids_t;

typedef struct
{
    centroids_t centroids;
    uint16_t    CENTROIDS_WIDTH;
    uint16_t    CENTROIDS_HEIGHT;
    uint8_t     CENTROIDS_SKIP;
}centroids_test_t;
    
uint8_t getBlobId( centroids_test_t * c, uint16_t x, uint16_t y, uint16_t n_c, uint8_t *num_blobs);
void    getCentroids( centroids_test_t * c, uint8_t *image_line, uint16_t line_number, uint8_t skip );
void    initCentroids( centroids_test_t * c, uint16_t width, uint16_t height );

#ifdef __cplusplus
}
#endif
    
#endif
