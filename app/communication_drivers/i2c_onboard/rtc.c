/*
 * rtc.c
 *
 *  Created on: 06/07/2015
 *      Author: joao.rosa
 *
 *      Routines working fine!!
 *      Tested 15/07/2015
 */


#include "rtc.h"
#include "i2c_onboard.h"

#include <stdint.h>

#define I2C_SLV_ADDR_RTC	0x68 // Endereço 7 bits

uint8_t data[10];

uint64_t	DataHour;

uint8_t rtc_access_sts;

uint64_t
DataHourRead(void)
{
	return DataHour;
}

void
RTCWriteDataHour(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayweek, uint8_t day, uint8_t month, uint8_t year)
{
	data[0] = 0x01; //Register
	data[1] = seconds;
	data[2] = minutes;
	data[3] = hours;
	data[4] = dayweek;
	data[5] = day;
	data[6] = month;
	data[7] = year;

	WriteI2C(I2C_SLV_ADDR_RTC, 0x08, data);
}

void
RTCReadDataHour(void)
{
	data[0] = 0x01;  // Register
	ReadI2C(I2C_SLV_ADDR_RTC, SINGLE_ADDRESS, 0x07, data);

	DataHour = data[6]; // Year
	DataHour = DataHour << 8;

	DataHour |= data[5]; // Month
	DataHour = DataHour << 8;

	DataHour |= data[4]; // Day
	DataHour = DataHour << 8;

	DataHour |= data[3]; // Day week
	DataHour = DataHour << 8;

	DataHour |= data[2]; // Hours
	DataHour = DataHour << 8;

	DataHour |= data[1]; // Minutes
	DataHour = DataHour << 8;

	DataHour |= data[0]; // Seconds

	// 0x00 00 00 00 00 00 00 00  64bits
	//     |Y |M |D |DW|H |M |S
}

void
RTCClearHT(void)
{
	uint8_t tst;
	data[0] = 0x0C;  // Register
	ReadI2C(I2C_SLV_ADDR_RTC, SINGLE_ADDRESS, 0x02, data);

	tst = data[0];
	tst &= 0b01000000;

	if(tst)
	{
		data[1] = data[0] & 0b10111111;
		data[0] = 0x0C;

		WriteI2C(I2C_SLV_ADDR_RTC, 0x02, data); // Send command to clear the HT bit

	}
}

void
RTCStopClock(void)
{
	data[0] = 0x01;
	data[1] = 0x80;
	WriteI2C(I2C_SLV_ADDR_RTC, 0x02, data); // Send command to stop the RTC

	data[0] = 0x01;
	data[1] = 0x00;
	WriteI2C(I2C_SLV_ADDR_RTC, 0x02, data); // Send command to start the RTC
}

uint8_t
RTCStatusOF(void)
{
	uint8_t tst;
	data[0] = 0x0F; // Register
	ReadI2C(I2C_SLV_ADDR_RTC, SINGLE_ADDRESS, 0x02, data);

	tst = data[0];
	tst &= 0b00000100;
	return tst;
}

void
RTCCheckOF(void)
{
	// Check RTC Oscillator

	unsigned long ulLoop;

	if(RTCStatusOF())
	{
		// Send a message to the system to indicate corruption in the RTC data

		for (ulLoop=0;ulLoop<80000000;ulLoop++){}; // Wait 4 seconds

		data[1] = data[0] & 0b11111011;
		data[0] = 0x0F;

		WriteI2C(I2C_SLV_ADDR_RTC, 0x02, data); // Send command to clear the OF bit

		// Test if the RTC is working
		if(RTCStatusOF())
		{
			RTCStopClock(); // Stop and start the RTC oscillator to try turn on the RTC
			if(RTCStatusOF()) ; // Test RTC to check the oscillator, if a problem is found than a message needs to be send to indicate a problem with RTC
		}

	}
}

uint8_t
RTCBatteryCheck(void)
{
	// Check if the battery is OK, Low Battery (LB) bit
	// The battery is cehcked once a day at 23:59:59 in continuous work or during power-up process

	uint8_t tst;

	data[0] = 0x0F; // Register
	ReadI2C(I2C_SLV_ADDR_RTC, SINGLE_ADDRESS, 0x02, data);

	tst = data[0];
	tst &= 0b00010000;

	return tst;
}

void
RTCInit(void)
{
	RTCClearHT();
	RTCCheckOF();
}

void
RTCSramWrite(uint8_t byte_addr, uint8_t data_value)
{
    if(byte_addr > 6) byte_addr = 6;

    byte_addr += 0x19;

    data[0] = byte_addr; //SRAM Register
    data[1] = data_value;

    WriteI2C(I2C_SLV_ADDR_RTC, 0x02, data);
}

uint8_t
RTCSramRead(uint8_t byte_addr)
{
    if(byte_addr > 6) byte_addr = 6;

    byte_addr += 0x19;

    data[0] = byte_addr; // Register
    ReadI2C(I2C_SLV_ADDR_RTC, SINGLE_ADDRESS, 0x02, data);

    return data[0];
}

void
RTCAccessTest(void)
{
    uint8_t var_test, var_error = 0;

    RTCSramWrite(0x00, 0x01);
    RTCSramWrite(0x01, 0x02);
    RTCSramWrite(0x02, 0x03);
    RTCSramWrite(0x03, 0x04);
    RTCSramWrite(0x04, 0x05);
    RTCSramWrite(0x05, 0x06);
    RTCSramWrite(0x06, 0x07);

    var_test = RTCSramRead(0x00);
    if(var_test != 0x01) var_error = 1;

    var_test = RTCSramRead(0x01);
    if(var_test != 0x02) var_error = 1;

    var_test = RTCSramRead(0x02);
    if(var_test != 0x03) var_error = 1;

    var_test = RTCSramRead(0x03);
    if(var_test != 0x04) var_error = 1;

    var_test = RTCSramRead(0x04);
    if(var_test != 0x05) var_error = 1;

    var_test = RTCSramRead(0x05);
    if(var_test != 0x06) var_error = 1;

    var_test = RTCSramRead(0x06);
    if(var_test != 0x07) var_error = 1;

    rtc_access_sts = var_error;

}

uint8_t
RTCAccessStatus(void)
{
    return rtc_access_sts;
}
