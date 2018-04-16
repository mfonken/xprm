/*
 * sensor_data_types.h
 *
 *  Created on: Jan 15, 2017
 *      Author: Matthew Fonken
 */

#ifndef SENSORS_SENSOR_DATA_TYPES_H_
#define SENSORS_SENSOR_DATA_TYPES_H_

#include "kinetic_types.h"

#define MAX_CENTROIDS	6

typedef struct
{
	uint8_t 	x;
	uint8_t 	y;
} centroid_t;

typedef struct
{
	centroid_t 	centroid[MAX_CENTROIDS];
	uint8_t 	count;
} centroids_t;

typedef struct
{
	centroid_t	centroid;
	uint8_t		persistence;
	uint32_t 	timestamp;
} beacon_t;



#endif /* SENSORS_SENSOR_DATA_TYPES_H_ */
