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

//#define INTEGER
//#define OUTPUT

struct blob
{
#ifdef INTEGER
    int     X;
    int     Y;
#else
    double  X;
    double  Y;
#endif
    int     height;
    int     w_last;
    int     x_last;
    int     y_last;
};

struct blob blobs[MAX_BLOBS];


char image[IMAGE_WIDTH][IMAGE_HEIGHT] = {
    {1,1,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,1,0,0,0,1,0},
    {0,0,0,0,0,1,1,1,1,0, 0,0,0,0,1,0,0,0,1,0},
    {0,0,0,0,0,1,1,1,1,0, 0,0,0,0,1,0,0,0,1,0},
    {0,0,0,0,0,1,0,0,0,0, 0,0,0,0,1,0,0,0,1,0},
    {0,0,0,0,0,1,0,0,0,0, 0,0,0,0,1,0,0,0,1,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,1,0,0,1,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,1,1,0,0},
    {0,0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    
    {0,0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,1,0,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,1,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,1,0,0,1,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,1,0,0,1,0},
    {1,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,1,0,0,1,0},
};

/*
 char image[IMAGE_WIDTH][IMAGE_HEIGHT] = {
    {1,0,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 1,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,1,0,0,0,0,0,0,0},
    
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {1,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,1,1,1,0,0, 0,0,0,0,0,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
    {0,0,1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0},
};
*/

/* Without Combination
char getBlobId(int x, int y)
{
    for(int i = 0; i < MAX_BLOBS; i++)
    {
#ifdef OUTPUT
        std::cout << "Checking blob #" << i << " at (" << blobs[i].x_last << ", " << blobs[i].y_last << ") and pixel at (" << x << ", " << y << ")\n";
        std::cout << "Y distance " << (y - blobs[i].y_last) << "\n";
        std::cout << "X distance " << abs(blobs[i].x_last - x) << "\n";
#endif
        if((y - blobs[i].y_last) <= MAX_GAP)
        {
            if(abs(blobs[i].x_last - x) <= MAX_GAP)
            {
#ifdef OUTPUT
                std::cout << "Current is adjacent to blob " << i << "\n";
#endif
                return i;
            }
        }
    }
    return -1;
}
 */

/* Cumulative Moving Average */
#ifdef INTEGER
void cma(int new_val, int *avg, int num)
{
    *avg += (new_val - *avg)/(num + 1);
}
#else
void cma(double new_val, double *avg, int num)
{
    *avg += (new_val - *avg)/(num + 1);
}
#endif


int getBlobId(int x, int y, int n_c, int *num_blobs)
{
#ifdef OUTPUT
    std::cout << "Old number of blobs is " << *num_blobs << "\n";
#endif
    int id = -1;                                // Null id (no adjacents)
    for(int i = 0; i < *num_blobs; i++)
    {
#ifdef INTEGER
        int x_l = blobs[i].X;                   // Current average
        int n_l = blobs[i].w_last;              // Last row width
#else
        double x_l = blobs[i].X;                   // Current average
        double n_l = blobs[i].w_last;              // Last row width
#endif
        
#ifdef OUTPUT
        std::cout << "\tChecking blob #" << i << " at (" << x_l << ", " << blobs[i].y_last << ") with width " << n_l << " and pixel at (" << x << ", " << y << ") with width " << n_c << "\n";
        std::cout << "\t\tY distance " << (y - blobs[i].y_last) << "\n";
        std::cout << "\t\tX distance " << abs(blobs[i].x_last - x) << "\n";
#endif
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
#ifdef OUTPUT
                    std::cout << "\tCombining blob #" << i << " to #" << id << "\n";
                    std::cout << "\t\tNew X average is "  << blobs[id].X << "\n";
                    std::cout << "\t\tNew Y average is "  << blobs[id].Y << "\n";
#endif
                    (*num_blobs)--;             // Decrement blob counter
                    for(int j = i; j < *num_blobs; j++)
                    {
                        blobs[i] = blobs[i+1];  // and pack blobs down
                    }
                    blobs[*num_blobs].w_last = 0; // Recent remains (muahahah...)
                    blobs[*num_blobs].height = 0;
                }
                
#ifdef OUTPUT
                std::cout << "\tCurrent is adjacent to blob " << i << "\n";
#endif
            }
        }
    }
#ifdef OUTPUT
    std::cout << "New number of blobs is " << *num_blobs << "\n";
#endif
    return id;                                  // Return id: Valid if claimed, null if not
}

void generateImage(char image[20][20], int w, int h, char t)
{
    switch(t)
    {
        case 0:
            for(int y = 0; y < h; y++)
            {
                for(int x = 0; x < w;x++)
                {
                    image[y][x] = 0;
                }
            }
            break;
        case 1:
            for(int y = 0; y < h; y++)
            {
                for(int x = 0; x < w;x++)
                {
                    image[y][x] = 1;
                }
            }
            break;
        case 2:
            for(int y = 0; y < h; y++)
            {
                for(int x = 0; x < w;x++)
                {
                    image[y][x] = x % 2;
                }
            }
            break;
        default:
            //*image = *default_image;
            break;
    }
}

int main(int argc, char * args[])
{
    int w = IMAGE_WIDTH;
    int h = IMAGE_HEIGHT;
    char t = -1;
    if(argc == 4)
    {
        w = atoi(args[1]);
        h = atoi(args[2]);
        t = *args[3];
    }
#ifdef OUTPUT
    std::cout << "Generating an image of size " << w << "x" << h << " of type " << t << "\n";
#endif
    //generateImage(default_image, w, h, t);
    
    int gap = -1, num_adj = 0, b_id = 0, temp_id = 0;   // Global variables
#ifdef INTEGER
    int a_x_last = 0, a_y_last = 0;                  // Global last X and Y averages
#else
    double a_x_last = 0, a_y_last = 0;                  // Global last X and Y averages
#endif
    
#ifdef OUTPUT
    std::cout << "-------------------=================(START)=================-------------------\n";
#endif
    for(int y = 0; y < IMAGE_HEIGHT; y++)               // Traverse all rows
    {
#ifdef OUTPUT
        std::cout << "---------------------Line #" << y << "---------------------\n";
#endif
        for(int x = 0; x < IMAGE_WIDTH; x++)            // Traverse all columns
        {
            if(image[y][x])                             // Check if pixel is on
            {
#ifdef OUTPUT
                std::cout << "\t--(" << x << ", " << y << ")--\n";
#endif
                
                gap = 0;                                // Reset gap counter
                cma(x,&a_x_last,num_adj);               // Average adjacent pixels
#ifdef OUTPUT
                std::cout << "\tNew a_x_last is " << a_x_last << "\n";
#endif
                num_adj++;                              // Increment adjacent pixels
            }
            else if(gap != -1)                          // Otherwise, if gap counter is counting (i.e. there was a recent pixel
            {
                gap++;                                  // Increment the gap counter
                if(gap ==  MAX_GAP          ||          // If max gap reached
                     x == (IMAGE_WIDTH - 1))            // or end of row
                {
#ifdef OUTPUT
                    std::cout << "\tOld a_x_last is " << a_x_last << "\n";
#endif                                                  // Include last pixel into a blob
                    temp_id = getBlobId(a_x_last, y, num_adj, &b_id);   // Get a blob to add to by coordinates and adjacent pixel width
                    if(temp_id == -1)                   // If no blob return
                    {
                        if(b_id == MAX_BLOBS)           // Check if max blobs have already been filled
                        {
                            goto out;                   // If so, end and output
                        }
                        temp_id = b_id++;               // Otherwise make a new id for the blob and increment the id counter
#ifdef OUTPUT
                        std::cout << "\tNew blob #" << temp_id << "\n";
#endif
                    }
                    cma(a_x_last, &blobs[temp_id].X, blobs[temp_id].w_last); // Cumulate the new pixels into the blob's X average
#ifdef OUTPUT
                    std::cout << "\tNew X for Blob " << temp_id << " is " << blobs[temp_id].X << "\n";
#endif
                    cma(y, &blobs[temp_id].Y, blobs[temp_id].height); // Cumulate the new row into the blob's Y average
                    blobs[temp_id].w_last = num_adj;    // Update blob with: New last row width
                    blobs[temp_id].x_last = a_x_last;   // last row average
                    blobs[temp_id].height++;            // height
                    blobs[temp_id].y_last = y;          // last row
#ifdef OUTPUT
                    std::cout << "Blob " << temp_id << " has been updated:\n\t";
                    std::cout << "Last width:\t" << blobs[temp_id].w_last << "\n\t";
                    std::cout << "Last avg:\t"   << blobs[temp_id].x_last << "\n\t";
                    std::cout << "Last row:\t"   << blobs[temp_id].y_last << "\n\t";
                    std::cout << "Curr height:\t"<< blobs[temp_id].height << "\n";
#endif
                    num_adj = 0;                        // Reset number of adjacent pixels
                    a_x_last = 0;                       // Reset adjacent pixel average
                    gap = -1;                           // Reset the gap to null
                    
                }
#ifdef OUTPUT
                std::cout << "\t\t\t\tGap is " << gap << "\n";
#endif
            }
        }
        gap = -1;                                       // Reset the gap to null
    }
out:
    for(int i = 0; i < b_id; i++)
    {
        std::cout << i << "> (" << blobs[i].X+.5  << ", " << blobs[i].Y+.5 << ")\n";
    }

    
    return 0;
}

