/*
 * camera_sp.c
 *
 *  Created on: Jan 8, 2017
 *      Author: Matthew Fonken
 */

#include "cam_controller.h"

#define	diff( a, b ) ( a - b > 0 ) ? a - b : b - a

beacon_t 	beacons[MAX_CENTROIDS];
uint8_t  	num_tracked;
uint8_t	 	map[MAX_CENTROIDS];
buffer_t 	camera_buffer;
centroids_t	centroids;

uint32_t	beacon_vector[2];

uint8_t		centroids_to_buffer;

uint8_t     last_index;

void printBuffer( int index, int end )
{
    uint8_t n;
    printf("Buffer: ");
    for(int i = 0; i < BUFF_SIZE; i++)
    {
        n = bufferRead( &camera_buffer, i );
        if(i == index) printf("i");
        if(i == end) printf("e");
        printf("%d ", (int)n);
    }
    printf("\n");
}

void Map_Print( void )
{
    printf("\t\tMap: ");
    for(int i = 0; i < num_tracked; i++)
    {
        printf("%d|%d ", map[i], i);
    }
    printf("\n");
}

void Beacon_Print( void )
{
    printf("Beacons:\n");
    for(int i = 0; i < num_tracked; i++)
    {
        printf("\t#%d - (%d, %d)|%d p(%d)\n", i, beacons[map[i]].centroid.x, beacons[map[i]].centroid.y, map[i], beacons[map[i]].persistence);
    }
    printf("\n");
}

uint8_t Camera_Buffer( uint8_t in )
{
    printf("Camera Buffer\n");
	return bufferAdd( &camera_buffer, in );
}

uint8_t Camera_Check( void )
{
    Beacon_Print();
    uint8_t index = last_index;
    printf("\t\t\tnum_tracked - %d\n", num_tracked);
    printf("Camera Check\n");
    uint8_t end = camera_buffer.index;//( index - 1 ) & BUFF_SIZE_MASK;
    printf("\tChecking buffer\n\t\t");
    printBuffer(index, end);
	while( index != end )
	{
		if( bufferRead( &camera_buffer, index ) == CENTROID_HEAD )
		{
            printf("\tHeader found at %d.\n", index);
			uint8_t n = bufferRead( &camera_buffer, index + 1 );
            printf("\tCount is %d.\n", n);
            if( n == 1 || n == 255 )
            {
                last_index = index + 1;
                break;
            }
            if( n == 0 ) goto invalid;
			centroids.count = n;
            uint8_t index1,index2;
			for( uint8_t i = 0 ; i < n; i ++ )
			{
				index1 = index + ( i * 2 ) + 2;
                index2 = index1 + 1;
                index1 &= BUFF_SIZE_MASK;
                index2 &= BUFF_SIZE_MASK;
                printf("\t\tRead index is %d|%d\n", index1, index2);
				if ( index1 == camera_buffer.index || index2 == camera_buffer.index)
				{
                    printf("\t\tReached end\n");
					centroids.count = 0;
                    last_index = index;
					return CAM_NULL_CMD;
				}
				centroids.centroid[i].x = bufferRead( &camera_buffer, index1 );
				centroids.centroid[i].y = bufferRead( &camera_buffer, index2 );
                printf("\tCentroid %d - (%d, %d)\n",i, centroids.centroid[i].x, centroids.centroid[i].y);
			}
            printf("=====================CHECKING=====================\n");
			Beacon_Check();
            printf("==================================================\n");
            bufferReset( &camera_buffer );
			return centroids.count;
		}
    invalid:
        index++;
        index &= BUFF_SIZE_MASK;
	}
	return CAM_NULL_CMD;
}

void Beacon_Add( centroid_t * a )
{
	Beacon_Copy( &beacons[num_tracked].centroid, a );
	beacons[num_tracked].persistence = 1;
    beacons[num_tracked].timestamp = 0;//timestamp();
	map[num_tracked] = num_tracked;
    printf("Beacon Add\n");
    printf("\t\tAdding beacon %d at index %d\n", map[num_tracked], num_tracked);
	num_tracked++;
    printf("\t\t\tnum_tracked incremented to %d\n", num_tracked);
    Map_Print();
}

void Beacon_Check( void )
{
    printf("Beacon Check\n");
	for( int i = 0; i < centroids.count; i++ )
	{
		bool claimed = false;
		for( int j = 0; j < num_tracked; j++ )
		{
            printf("Beacon Compare\n");
            printf("\t\tComparing Centroid %d (%d, %d) with beacon %d (%d,%d) at index %d\n", i, centroids.centroid[i].x, centroids.centroid[i].y, map[j], beacons[map[j]].centroid.x, beacons[map[j]].centroid.y, j);
			if( Beacon_Compare( &beacons[map[j]].centroid, &centroids.centroid[i] ) )
			{
				claimed = true;
				Beacon_Update( j, &centroids.centroid[i]);
                j++;
			}
		}
		if( !claimed )
		{
			Beacon_Add( &centroids.centroid[i] );
		}
	}
}

bool Beacon_Compare( centroid_t * a, centroid_t * b )
{
	/*TODO: Complete this compare check */
	uint8_t c = diff( a->x, b->x ) ;
	uint8_t d = diff( a->y, b->y ) ;
	if( c <= MAX_X_DIFF &&
		d <= MAX_Y_DIFF  )
    {
        printf("\t\tSimilar...\n");
		return true;
	}
    printf("\t\tNot similar...\n");
	return false;
}

/* Call this on sync */
bool Beacon_Compose( cartesian2_t vis[2] )
{
    printf("Beacon Compose\n");
	if( centroids.count >= 2 )
	{
		vis[0].x = beacons[map[0]].centroid.x;
		vis[0].y = beacons[map[0]].centroid.y;
		vis[1].x = beacons[map[1]].centroid.x;
		vis[1].y = beacons[map[1]].centroid.y;
		centroids.count = 0;
		return true;
	}
	return false;
}

void Beacon_Copy( centroid_t * a, centroid_t * b)
{
    printf("Beacon Copy\n");
	a->x = b->x;
	a->y = b->y;
}

void Beacon_Sort( void )
{
	/* Sudo resort by persistence */
    for( int i = 0; i < num_tracked-1; i++ )
    {
        if( beacons[map[i]].persistence < beacons[map[i+1]].persistence )
        {
            printf("*********************SORTING**********************\n");
            Map_Print();
            printf("Swapping %d|%d - %d and %d|%d - %d\n", map[i], i, beacons[map[i]].persistence, map[i+1], i+1, beacons[map[i+1]].persistence);
            /* Swap index in map */
            uint8_t temp = map[i];
            map[i] = map[i+1];
            map[i+1] = temp;
            Map_Print();
            printf("**************************************************\n");
        }
    }
}

void Beacon_Perge( void )
{
    printf("Beacon Perge\n");
	uint8_t perged = 0;
	for( int i = 0; i < num_tracked; i++ )
	{
		if( ( 0 - beacons[map[i]].timestamp ) > MAX_TRACK_AGE )
		{
            printf("\t\tPerging Beacon %d, replacing with %d\n", i, i + 1 + perged);
			map[i] = map[i + 1 + perged];
			perged++;
		}
	}
    printf("\t\tPerged %d beacons\n", perged);
	num_tracked -= perged;
}

void Beacon_Update( uint8_t index, centroid_t * b )
{
    printf("Beacon Update\n");
	Beacon_Copy( &beacons[map[index]].centroid, b );
    beacons[map[index]].timestamp = 0;//timestamp();
	beacons[map[index]].persistence++;

	Beacon_Perge();
	Beacon_Sort();
}
