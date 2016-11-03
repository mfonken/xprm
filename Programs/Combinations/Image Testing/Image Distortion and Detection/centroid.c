#define MAX_BLOBS       10
#define MAX_GAP         50

using namespace std;



blob blobs[MAX_BLOBS];
int b_id = 0;
double a_x_last = 0;                  // Global last X and Y averages

static void cma(double new_val, double *avg, int num)
{
    *avg += (new_val - *avg)/(num + 1);
}

static int getBlobId(int x, int y, int n_c, int *num_blobs)
{
    int id = -1;                                // Null id (no adjacents)
    int i, j;
    for(i = 0; i < *num_blobs; i++)
    {
        double x_l = blobs[i].X;                   // Current average
        double n_l = blobs[i].w_last;              // Last row width
        if((y - blobs[i].y_last) <= MAX_GAP)    // Ensure blob hasn't expired
        {
            if( ( ( x + n_c/2 + MAX_GAP) >= ( x_l - n_l/2) ) && // Check overlap of lower bound of blob and upper (with gap tolerance) of new
                ( ( x - n_c/2 ) <= ( x_l + n_l/2 + MAX_GAP) ) ) // and of upper bound of blob and lower (with gap tolerance) of new
            {
                if(id == -1)                    // If new blob is not claimed
                {
                    id = i;                     // Claim it under current id
                }
                else                            // Otherwise combine
                {
                    cma(blobs[i].X, &blobs[id].X, 2);
                    cma(blobs[i].Y, &blobs[id].Y, 2);
                    (*num_blobs)--;             // Decrement blob counter
                    for(j = i; j < *num_blobs; j++)
                    {
                        blobs[i] = blobs[i+1];  // and pack blobs down
                    }
                    blobs[*num_blobs].w_last = 0; // Recent remains (muahahah...)
                    blobs[*num_blobs].height = 0;
                }
            }
        }
    }
    return id;                                  // Return id: Valid if claimed, null if not
}

int getCentroids(char *image_line, int line_length, int line_number )
{
    int gap = -1, num_adj = 0,  temp_id = 0;   // Global variables

    int x;
    for(x = 0; x < line_length; x++)            // Traverse all columns
    {
        if((int)image_line[x])                             // Check if pixel is on
        {
            gap = 0;                                // Reset gap counter
            cma(x,&a_x_last,num_adj);               // Average adjacent pixels
            num_adj++;                              // Increment adjacent pixels
        }
        else if(gap != -1)                          // Otherwise, if gap counter is counting (i.e. there was a recent pixel
        {
            gap++;                                  // Increment the gap counter
            if(gap ==  MAX_GAP          ||          // If max gap reached
                 x == (line_length - 1))            // or end of row
            {                                               // Include last pixel into a blob
                temp_id = getBlobId(a_x_last, line_number, num_adj, &b_id);   // Get a blob to add to by coordinates and adjacent pixel width
                if(temp_id == -1)                   // If no blob return
                {
                    if(b_id == MAX_BLOBS)           // Check if max blobs have already been filled
                    {
                        return -1;                   // If so, end and output
                    }
                    temp_id = b_id++;               // Otherwise make a new id for the blob and increment the id counter
                }
                cma(a_x_last, &blobs[temp_id].X, blobs[temp_id].w_last); // Cumulate the new pixels into the blob's X average
                cma(line_number, &blobs[temp_id].Y, blobs[temp_id].height); // Cumulate the new row into the blob's Y average
                blobs[temp_id].w_last = num_adj;    // Update blob with: New last row width
                blobs[temp_id].x_last = a_x_last;   // last row average
                blobs[temp_id].height++;            // height
                blobs[temp_id].y_last = line_number;// last row

                num_adj = 0;                        // Reset number of adjacent pixels
                a_x_last = 0;                       // Reset adjacent pixel average
                gap = -1;                           // Reset the gap to null

            }
        }
    }
    
    return b_id;
}



/* *****************************************************************************
 End of File
 */
