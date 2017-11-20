 #include "centroid.h"

#define NULL_ 0xff

#define THRESH  0

uint8_t getBlobId(centroids_test_t * c, uint16_t x, uint16_t y, uint16_t n_c, uint8_t *num_blobs)
{
    n_c /= 2;
    uint8_t id = 0xff;                                // NULL_ id (no adjacents)
    uint8_t i, j;
    for( i = 0; i < *num_blobs; i++ )
    {
        if( ( y - c->centroids.blobs[i].y_last ) <= MAX_GAP )    // Ensure blob hasn't expired
        {
            double x_l = c->centroids.blobs[i].X;                   // Current average
            double n_l = c->centroids.blobs[i].w_last/2;            // Last row width
        
            if( ( ( x + n_c + MAX_GAP) >= ( x_l - n_l) )  && // Check overlap of lower bound of blob and upper (with gap tolerance) of new
                ( ( x - n_c - MAX_GAP ) <= ( x_l + n_l ) ) ) // and of upper bound of blob and lower (with gap tolerance) of new
            {
                if( id == 0xff )                    // If new blob is not claimed
                {
                    id = i;                     // Claim it under current id
                }
                else                            // Otherwise combine
                {
                    cma( c->centroids.blobs[i].X, &c->centroids.blobs[id].X, 2 );
                    cma( c->centroids.blobs[i].Y, &c->centroids.blobs[id].Y, 2 );
                    ( *num_blobs )--;             // Decrement blob counter
                    for( j = i; j < *num_blobs; j++ )
                    {
                        c->centroids.blobs[i] = c->centroids.blobs[i+1];  // and pack blobs down
                    }
                    c->centroids.blobs[*num_blobs].w_last = 0;
                    c->centroids.blobs[*num_blobs].height = 0;
                }
            }
        }
    }
    return id;                                  // Return id: Valid if claimed, NULL_ if not
}

void getCentroids( centroids_test_t * c, uint8_t *image_line, uint16_t line_number, uint8_t skip )
{
    uint8_t gap = NULL_, num_adj = 0, temp_id = 0;   // Global variables
    double a_x_last = 0;                  // Global last X and Y averages
    uint16_t x;
    for( x = 0; x < c->CENTROIDS_WIDTH; x += skip )            // Traverse all columns
    {
        if(image_line[x] > THRESH)                             // Check if pixel is on
        {
            gap = 0;                                // Reset gap counter
            cma( x, &a_x_last, num_adj );               // Average adjacent pixels
            num_adj++;                              // Increment adjacent pixels
        }
        else if( gap != NULL_ )                          // Otherwise, if gap counter is counting (i.e. there was a recent pixel
        {
            gap++;                                  // Increment the gap counter
            if( gap == MAX_GAP ||
                  x == (c->CENTROIDS_HEIGHT - 1))                    // If max gap reached
            {                                               // Include last pixel into a blob
                temp_id = getBlobId( c, a_x_last, line_number, num_adj, &c->centroids.numBlobs );   // Get a blob to add to by coordinates and adjacent pixel width
                if( temp_id == NULL_ )                   // If no blob return
                {
                    if( c->centroids.numBlobs == MAX_BLOBS )           // Check if max blobs have already been filled
                    {
                        return;                   // If so, end and output
                    }
                    temp_id = c->centroids.numBlobs++;               // Otherwise make a new id for the blob and increment the id counter
                }
                cma( a_x_last, &c->centroids.blobs[temp_id].X, c->centroids.blobs[temp_id].w_last ); // Cumulate the new pixels into the blob's X average
                cma( line_number, &c->centroids.blobs[temp_id].Y, c->centroids.blobs[temp_id].height ); // Cumulate the new row into the blob's Y average
                c->centroids.blobs[temp_id].w_last = num_adj;    // Update blob with: New last row width
                c->centroids.blobs[temp_id].x_last = a_x_last;   // last row average
                c->centroids.blobs[temp_id].height++;            // height
                c->centroids.blobs[temp_id].y_last = line_number;// last row

                num_adj = 0;                        // Reset number of adjacent pixels
                a_x_last = 0;                       // Reset adjacent pixel average
                gap = 0xff;                           // Reset the gap to NULL_
            }
        }
    }
}

void initCentroids( centroids_test_t * c, uint16_t width, uint16_t height )
{
    c->CENTROIDS_WIDTH  = width;
    c->CENTROIDS_HEIGHT = height;
    c->centroids.numBlobs = 0;
}
