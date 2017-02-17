/*! \file centroid.h
    \brief Centroid Detection
 
 Detects object persistence from each line input
 one at a time and returns the centroid coordinates
 for each object.
 */

#ifndef centroid_h
#define centroid_h

#define MAX_CENTROIDS   100          /**< Maximun number of centroids to track */
#define MAX_GAP         1          /**< Maximun gap between detected pixels */

using namespace std;

typedef struct _centroid
{
    double  X;                      /**< X center value */
    double  Y;                      /**< Y center value */
    
    int     mass;
    
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
                    printf("Combining centroid %d:m%d(%.3f, %.3f) with %d:m%d(%.3f, %.3f)\n",id,centroids[id].mass,centroids[id].X,centroids[id].Y,i,centroids[i].mass,centroids[i].X,centroids[i].Y);
                    cma(centroids[i].X, &centroids[id].X, 1);//centroids[id].height);
                    cma(centroids[i].Y, &centroids[id].Y, 1);//centroids[id].height);
                    centroids[id].mass += centroids[i].mass;
                    
                    printf("New centroid is %d:m%d(%.3f, %.3f)\n",id,centroids[id].mass,centroids[id].X,centroids[id].Y);
                    
                    /* Pack number of objects down */
                    for(j = i; j < *num_centroids-1; j++)
                    {
                        printf("Packing centroid %d to %d\n", j+1, j);
                        centroids[j] = centroids[j+1];
                    }
                    int e = *num_centroids-1;
                    printf("Erasing centroid %d\n", e);
                    centroids[e].w_last = 0;
                    centroids[e].height = 0;
                    centroids[e].mass = 0;
                    (*num_centroids)--;
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
        if(image_line[x])
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
                    if(b_id <= MAX_CENTROIDS) temp_id = b_id++;
                }
                if(temp_id != -1)
                {
                    /* Cumulate the new pixels into the centroid's X average and
                        new row into the centroid's Y average */
                    cma(a_x_last, &centroids[temp_id].X, centroids[temp_id].height);
                    cma(line_number, &centroids[temp_id].Y, centroids[temp_id].height);
                    
                    /* Update centroid information */
                    centroids[temp_id].mass  += num_adj;
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
    }
    
    return b_id;
}

void resetBlobs( void )
{
    int i;
    for( i = 0; i < MAX_CENTROIDS; i++ )
    {
        centroids[i].X = 0;
        centroids[i].Y = 0;
        centroids[i].mass   = 0;
        centroids[i].height = 0;
        centroids[i].w_last = 0;
        centroids[i].x_last = 0;
        centroids[i].y_last = 0;
    }
}

#endif /* centroid_h */
