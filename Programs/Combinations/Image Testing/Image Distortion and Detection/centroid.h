/*! \file centroid.h
    \brief Centroid Detection
 
 Detects object persistence from each line input
 one at a time and returns the centroid coordinates
 for each object.
 */

#ifndef centroid_h
#define centroid_h

#define MAX_CENTROIDS   10          /**< Maximun number of centroids to track */
#define MAX_GAP         50          /**< Maximun gap between detected pixels */

using namespace std;

typedef struct _centroid
{
    double  X;                      /**< X center value */
    double  Y;                      /**< Y center value */
    
    int     height;                 /**< Height of tracked object */
    int     w_last;                 /**< Width of previous line of object */
    int     x_last;                 /**< Average of previous line of object */
    int     y_last;                 /**< Line number of previous line of object */
} centroid;

centroid centroids[MAX_CENTROIDS];  /**< Global array of detected object */
int b_id = 0;                       /**< Global current object id */
double a_x_last = 0;                /**< Global previous X average */


/*! Cumulative Moving Average\r\n
        Averages a moving set using:
        \f[
            {\mu}_{new}=\frac{x_{new} - {\mu}_{curr}}{n+1}
        \f]
 */
void cma(double new_val, double *avg, int num)
{
    *avg += (new_val - *avg)/(num + 1);
}

/*! Get proper id of pixel at (x,y)
 */
int getcentroidId(int x, int y, int n_c, int *num_centroids)
{
    /* Null id (no adjacents) */
    int id = -1;
    int i, j;
    for(i = 0; i < *num_centroids; i++)
    {
        /* Get current average and previous row width */
        double x_l = centroids[i].X;
        double n_l = centroids[i].w_last;
        
        /* Ensure centroid hasn't expired */
        if((y - centroids[i].y_last) <= MAX_GAP)
        {
            /* Check overlap of lower bound of centroid and upper (with gap tolerance) of new
                         and of upper bound of centroid and lower (with gap tolerance) of new */
            if( ( ( x + n_c/2 + MAX_GAP) >= ( x_l - n_l/2) ) &&
                ( ( x - n_c/2 ) <= ( x_l + n_l/2 + MAX_GAP) ) )
            {
                /* If object is unclaimed, claim it, otherwise combine it */
                if(id == -1)
                {
                    id = i;
                }
                else
                {
                    cma(centroids[i].X, &centroids[id].X, 2);
                    cma(centroids[i].Y, &centroids[id].Y, 2);
                    (*num_centroids)--;
                    /* Pack number of objects down */
                    for(j = i; j < *num_centroids; j++)
                    {
                        centroids[i] = centroids[i+1];
                    }
                    centroids[*num_centroids].w_last = 0;
                    centroids[*num_centroids].height = 0;
                }
            }
        }
    }
    /* Valid if object was claimed, null if not */
    return id;
}

/*! Main Centroid Detection
 */
int getCentroids(char *image_line, int line_length, int line_number )
{
    /* Global variables */
    int gap = -1, num_adj = 0,  temp_id = 0, x;

    /* Traverse all columns */
    for(x = 0; x < line_length; x++)
    {
        /* Check if pixel is on */
        if((int)image_line[x])
        {
            /* Reset gap counter, add to average of adjacent pixels, and increment */
            gap = 0;
            cma(x,&a_x_last,num_adj);
            num_adj++;
        }
        /* Otherwise, if gap counter is counting (i.e. there was a recent pixel) */
        else if(gap != -1)
        {
            /* Check if the max gap reached or the row has ended */
            gap++;
            if(gap ==  MAX_GAP ||
                 x == (line_length - 1))
            {
                /* Check for adjacent centroid to add to */
                temp_id = getcentroidId(a_x_last, line_number, num_adj, &b_id);
                /* If a null id is return and max centroids is not reached, 
                    get a new id and increment id counter */
                if(temp_id == -1)
                {
                    if(b_id == MAX_CENTROIDS)
                    {
                        return -1;
                    }
                    temp_id = b_id++;
                }
                /* Cumulate the new pixels into the centroid's X average and
                    new row into the centroid's Y average */
                cma(a_x_last, &centroids[temp_id].X, centroids[temp_id].w_last);
                cma(line_number, &centroids[temp_id].Y, centroids[temp_id].height);
                
                /* Update centroid information */
                centroids[temp_id].w_last = num_adj;
                centroids[temp_id].x_last = a_x_last;
                centroids[temp_id].height++;
                centroids[temp_id].y_last = line_number;

                /* Reset global variables */
                num_adj = 0;
                a_x_last = 0;
                gap = -1;

            }
        }
    }
    
    return b_id;
}

#endif /* centroid_h */
