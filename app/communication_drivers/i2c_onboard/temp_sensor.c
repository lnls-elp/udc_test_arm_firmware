/*
 * temp_sensor.c
 *
 *  Created on: 24/05/2017
 *      Author: ali.faraj
 */


#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"


#include "temp_sensor.h"
#include "i2c_onboard.h"
#include "hardware_def.h"

#include <stdint.h>


#define I2C_SLV_ADDR_TEMP	0x48 // Endereço 7 bits

uint8_t datatemp[10];

uint16_t temp;

void
ReadTemperature(void){
	datatemp[0] = 0x00;
	ReadI2C(I2C_SLV_ADDR_TEMP, SINGLE_ADDRESS, 0x02, datatemp);
	temp = datatemp[0];
	temp = temp << 8;
	temp = temp | datatemp[1];
	temp = temp >> 5;
}

float
TemperatureCelsius(void){
	return temp*(0.125);
}
