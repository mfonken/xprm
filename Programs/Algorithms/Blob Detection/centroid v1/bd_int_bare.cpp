//
//  bd.cpp
//  
//
//  Created by Matthew Fonken on 9/30/16.
//
//

#include <stdio.h>
#include <iostream>

#define IMAGE_WIDTH     20
#define IMAGE_HEIGHT    20
#define MAX_BLOBS       10
#define MAX_GAP         3
#define FIXED_BASE      3

#define OUTPUT

using namespace std;
struct blob
{
    int     X;
    int     Y;
    int     height;
    int     w_last;
    int     x_last;
    int     y_last;
};

struct blob blobs[MAX_BLOBS];

char image[IMAGE_WIDTH][IMAGE_HEIGHT] = {
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,1, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,1,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
};

int f(int num)
{
    int pow = 1;
    for(int i = 0; i < FIXED_BASE; i++, pow *= 10);
    return num*pow;
}

int u(int num)
{
    int pow = 1;
    for(int i = 0; i < FIXED_BASE; i++, pow *= 10);
    return num/pow;
}

int d(int a, int b)
{
    return a/b;
}

/* Cumulative Moving Average */
void cma(int new_val, int *avg, int num)
{
    //*avg += (new_val - *avg)/(num + 1);
    *avg += d((new_val - *avg),(num + 1));
}

int getBlobId(int x, int y, int n_c, int *num_blobs)
{
    n_c = f(n_c);
    int id = -1;                                // Null id (no adjacents)
    for(int i = 0; i < *num_blobs; i++)
    {
        int x_l = blobs[i].X;                   // Current average
        int n_l = blobs[i].w_last;              // Last row width

        if((y - blobs[i].y_last) <= MAX_GAP)    // Ensure blob hasn't expired
        {
            if( ( ( x + n_c/2 + f(MAX_GAP)) >= ( x_l - n_l/2) ) && // Check overlap of lower bound of blob and upper (with gap tolerance) of new
                ( ( x - n_c/2 ) <= ( x_l + n_l/2 + f(MAX_GAP)) ) ) // and of upper bound of blob and lower (with gap tolerance) of new
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
                    for(int j = i; j < *num_blobs; j++)
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
int main(int argc, char * args[])
{
    int gap = -1, num_adj = 0, b_id = 0, temp_id = 0;   // Global variables
    int a_x_last = 0, a_y_last = 0;                     // Global last X and Y averages
    
    for(int y = 0; y < IMAGE_HEIGHT; y++)               // Traverse all rows
    {
        for(int x = 0; x < IMAGE_WIDTH; x++)            // Traverse all columns
        {
            if(image[y][x])                             // Check if pixel is on
            {
                gap = 0;                                // Reset gap counter
                cma(f(x),&a_x_last,num_adj);               // Average adjacent pixels
                num_adj++;                              // Increment adjacent pixels
            }
            else if(gap != -1)                          // Otherwise, if gap counter is counting (i.e. there was a recent pixel
            {
                gap++;                                  // Increment the gap counter
                if(gap ==  MAX_GAP          ||          // If max gap reached
                     x == (IMAGE_WIDTH - 1))            // or end of row
                {                                               // Include last pixel into a blob
                    temp_id = getBlobId(a_x_last, y, num_adj, &b_id);   // Get a blob to add to by coordinates and adjacent pixel width
                    if(temp_id == -1)                   // If no blob return
                    {
                        if(b_id == MAX_BLOBS)           // Check if max blobs have already been filled
                        {
                            goto out;                   // If so, end and output
                        }
                        temp_id = b_id++;               // Otherwise make a new id for the blob and increment the id counter
                    }
                    cma(a_x_last, &blobs[temp_id].X, blobs[temp_id].w_last); // Cumulate the new pixels into the blob's X average
                    cma(f(y), &blobs[temp_id].Y, blobs[temp_id].height); // Cumulate the new row into the blob's Y average
                    blobs[temp_id].w_last = num_adj;    // Update blob with: New last row width
                    blobs[temp_id].x_last = a_x_last;   // last row average
                    blobs[temp_id].height++;            // height
                    blobs[temp_id].y_last = y;          // last row
                    num_adj = 0;                        // Reset number of adjacent pixels
                    a_x_last = 0;                       // Reset adjacent pixel average
                    gap = -1;                           // Reset the gap to null
                    
                }
            }
        }
        gap = -1;                                       // Reset the gap to null
    }
out:
    for(int i = 0; i < b_id; i++)
    {
        blobs[i].X += f(10/2)/10;
        blobs[i].Y += f(10/2)/10;
        
        cout << i << "> (" << blobs[i].X << ", " << blobs[i].Y << ")\n";
    }

    
    return 0;
}

