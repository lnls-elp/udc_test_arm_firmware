/*
 * temp_sensor.h
 *
 *  Created on: 24/05/2017
 *      Author: ali.faraj
 */

#include <stdint.h>

#ifndef TEMP_SENSOR_H_
#define TEMP_SENSOR_H_

//extern void TempInit(void);
extern void ReadTemperature(void);
extern float TemperatureCelsius(void);

#endif /* TEMP_SENSOR_H_ */
