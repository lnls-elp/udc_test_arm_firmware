/*
 * bsmp_jiga.c
 *
 *  Created on: 12/05/2017
 *      Author: ali.faraj
 */

#include "bsmp/include/server.h"
#include "../i2c_onboard/eeprom.h"
#include "../i2c_onboard/temp_sensor.h"
#include "../i2c_onboard/rtc.h"
#include "../ethernet/ethernet_uip.h"
#include "../system_task/system_task.h"
#include "../jiga/jiga.h"
#include "../rs485/rs485.h"
#include "../rs485_bkp/rs485_bkp.h"
#include "../board_drivers/hardware_def.h"


//#!
#include "inc/hw_memmap.h"
#include "driverlib/ipc.h"
#include "inc/hw_ipc.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"

#include "../ipc/ipc_lib.h"
#include "../shared_memory/structs.h"
#include "../can/can_bkp.h"
#include "../jiga/jiga.h"
//#!

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>


//Systick includes
#include "driverlib/systick.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "bsmp_lib.h"

volatile unsigned long ulTimeout;

//#pragma CODE_SECTION(BSMPprocess, "ramfuncs");

bsmp_server_t bsmp;
uint16_t TIMEOUT_VALUE = 20;

//*****************************************************************************
//                  IO Expander BSMP Function
//*****************************************************************************
// OK
uint8_t TestIoExpander (uint8_t *input, uint8_t *output)
{
    *output = TestIoExp(*input);
    return *output;
}

static struct bsmp_func testioexpander_func = {
    .func_p           = TestIoExpander,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  Leds BSMP Function
//*****************************************************************************
// OK
uint8_t TestLed (uint8_t *input, uint8_t *output)
{
    //Call led
    TestLedTurnOn();


    *output = 0;
    return *output;
}

static struct bsmp_func testled_func = {
    .func_p           = TestLed,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  Buzzer BSMP Function
//*****************************************************************************
// OK
uint8_t TestBuzzer (uint8_t *input, uint8_t *output)
{
    //Call buzzer function test
    TestBuzzerTurnOn();

    *output = 0;
    return *output;
}

static struct bsmp_func testbuzzer_func = {
    .func_p           = TestBuzzer,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  EEPROM BSMP Function
//*****************************************************************************
uint8_t TestEeprom (uint8_t *input, uint8_t *output)
{
    *output = TestEepromWriteSerialNumber(input[0],input[1], input[2], input[3], input[4], input[5], input[6], input[7], input[8], input[9], input[10]);

    return *output;
}

static struct bsmp_func testeeprom_func = {
    .func_p           = TestEeprom,
    .info.input_size  = 11,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  FLASH BSMP Function
//*****************************************************************************
// OK
uint8_t TestFlash (uint8_t *input, uint8_t *output)
{
    *output = TestFlashMem(*input);
    return *output;
}

static struct bsmp_func testflash_func = {
    .func_p           = TestFlash,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  RAM BSMP Function
//*****************************************************************************
// OK
uint8_t TestRam (uint8_t *input, uint8_t *output)
{
    *output = TestRamMem(*input);
    return *output;
}

static struct bsmp_func testram_func = {
    .func_p           = TestRam,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  RTC BSMP Function
//*****************************************************************************
// OK
uint8_t TestRtc (uint8_t *input, uint8_t *output)
{
    *output = TestRtcMod(*input);
    return *output;
}

static struct bsmp_func testrtc_func = {
    .func_p           = TestRtc,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  Temperature Sensor BSMP Function
//*****************************************************************************
// OK
uint8_t TestTemperatureSensor (uint8_t *input, uint8_t *output)
{
    *output = TestTempSense(*input);
    return *output;
}

static struct bsmp_func testtemperaturesensor_func = {
    .func_p           = TestTemperatureSensor,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  Isolated Power Supply BSMP Function
//*****************************************************************************
// OK
uint8_t TestIsoPowerSupply (uint8_t *input, uint8_t *output)
{
    *output = TestIsoPowerSup(*input);
    return *output;
}

static struct bsmp_func testisopowersupply_func = {
    .func_p           = TestIsoPowerSupply,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  ADC Channels BSMP Function
//*****************************************************************************
// OK
uint8_t TestAdcChannels (uint8_t *input, uint8_t *output)
{
    *output = TestAdcpRange(input[0], input[1]);
    return *output;
}

static struct bsmp_func testadcchannels_func = {
    .func_p           = TestAdcChannels,
    .info.input_size  = 2,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  RS485 BSMP Function
//*****************************************************************************
// OK
uint8_t TestRs485 (uint8_t *input, uint8_t *output)
{
    *output = TestRs485Loop(input[0]);
    return *output;
}

static struct bsmp_func testrs485_func = {
    .func_p           = TestRs485,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
//                  Loopback BSMP Function
//*****************************************************************************
// OK
uint8_t TestLoopback (uint8_t *input, uint8_t *output)
{
    *output = TestLoopBackSignal(input[0], input[1]);
    return *output;
}

static struct bsmp_func testloopback_func = {
    .func_p           = TestLoopback,
    .info.input_size  = 2,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};


//*****************************************************************************
//                  Uart Ping BSMP Function
//*****************************************************************************
// OK
uint8_t TestuartPing (uint8_t *input, uint8_t *output)
{
    *output = *input;
    return *output;
}

static struct bsmp_func testuartping_func = {
    .func_p           = TestuartPing,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};


//*****************************************************************************
//                          BSMP Initialization
//*****************************************************************************
void
BSMPInit(void)
{
    //********************************************
    // Initialize communications library
    bsmp_server_init(&bsmp);
    //bsmp_register_hook(&bsmp, hook);

    //*****************************************************************************
    //                      BSMP Function Register
    //*****************************************************************************
    bsmp_register_function(&bsmp, &testioexpander_func);                // Function ID 0
    bsmp_register_function(&bsmp, &testled_func);                       // Function ID 1
    bsmp_register_function(&bsmp, &testbuzzer_func);                    // Function ID 2
    bsmp_register_function(&bsmp, &testeeprom_func);                    // Function ID 3
    bsmp_register_function(&bsmp, &testflash_func);                     // Function ID 4
    bsmp_register_function(&bsmp, &testram_func);                       // Function ID 5
    bsmp_register_function(&bsmp, &testrtc_func);                       // Function ID 6
    bsmp_register_function(&bsmp, &testtemperaturesensor_func);         // Function ID 7
    bsmp_register_function(&bsmp, &testisopowersupply_func);            // Function ID 8
    bsmp_register_function(&bsmp, &testadcchannels_func);               // Function ID 9
    bsmp_register_function(&bsmp, &testrs485_func);                     // Function ID 10
    bsmp_register_function(&bsmp, &testloopback_func);                  // Function ID 11
    bsmp_register_function(&bsmp, &testuartping_func);                  // Function ID 12


    //*****************************************************************************
}

void
BSMPprocess(struct bsmp_raw_packet *recv_packet, struct bsmp_raw_packet *send_packet)
{
    bsmp_process_packet(&bsmp, recv_packet, send_packet);

}






/*

//*****************************************************************************
// 						TestUsbUart BSMP Function
//*****************************************************************************
uint8_t TestUsbUart (uint8_t *input, uint8_t *output)
{
	*output = input[0] + 1;
	return *output;
}

static struct bsmp_func usbuart_func = {
    .func_p 		  = TestUsbUart,
    .info.input_size  = 1,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
// 						WriteIPAddress BSMP Function
//*****************************************************************************
uint8_t WriteIPAddress (uint8_t *input, uint8_t *output)
{
	*output = input[0] + 1;
	IPAddressWrite(input[0],input[1],input[2],input[3]);
	return *output;
}

static struct bsmp_func writeipaddress_func = {
    .func_p 		  = WriteIPAddress,
    .info.input_size  = 4,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
// 						WriteIPMask BSMP Function
//*****************************************************************************
uint8_t WriteIPMask (uint8_t *input, uint8_t *output)
{
	*output = input[0] + 1;
	IPMaskWrite(input[0],input[1],input[2],input[3]);
	return *output;
}

static struct bsmp_func writeipmask_func = {
    .func_p 		  = WriteIPMask,
    .info.input_size  = 4,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
// 						RS485Test BSMP Function
//*****************************************************************************
uint8_t RS485Test (uint8_t *input, uint8_t *output)
{
		long lChar;
		unsigned char ucChar;
		int i = 0;
		char* sent_message;
		int len = rand()%10 + 1;
		random_string(sent_message, len);

		char* recv_message;

		UARTSendString(sent_message, RS485_UART_BASE);

		while(!UARTSpaceAvail(RS485_UART_BASE));
		// Send Byte
		UARTCharPut(RS485_UART_BASE, '\n');

		while(!UARTSpaceAvail(RS485_UART_BASE));
		// Send Byte
		UARTCharPut(RS485_UART_BASE, '\r');

		while(UARTBusy(RS485_UART_BASE));
		while(UARTCharsAvail(RS485_BKP_UART_BASE)){
			lChar = UARTCharGet(RS485_BKP_UART_BASE);
			if(!(lChar & ~0xFF))
			{
				ucChar = (unsigned char)(lChar & 0xFF);
				recv_message[i++] = ucChar;
			}
		}

		UARTSendString(recv_message, RS485_UART_BASE);

		while(UARTBusy(RS485_UART_BASE));

		//ReadBufferRS485BKP(recv_message);

		*output = 8;
		if(strcmp(sent_message,"") && strcmp(recv_message,"")){
			if(strcmp(sent_message, recv_message)){
				// strings nao sao vazias mas sao diferentes
				*output = 2;
			}
			else{
				// strings nao sao vazias e sao iguais
				*output = 3;
			}
		}
		else{
			// pelo menos uma das strings eh vazia
			if(!strcmp(sent_message, "")){
				*output = 4;
			}
			else if(!strcmp(recv_message, "")){
				*output = 5;
			}
		}

		return *output;
}

static struct bsmp_func rs485_func = {
    .func_p 		  = RS485Test,
    .info.input_size  = 0,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
// 						TemperatureTest BSMP Function
//*****************************************************************************
uint8_t TemperatureTest (uint8_t *input, uint8_t *output)
{
	float temp = 0.1;
	ReadTemperature();
	temp = TemperatureCelsius();
	if(temp > 9.9 && temp < 49.9){
		*output = 2;
	}
	else if(temp < 9.9){
		*output = 3;
	}
	else if(temp > 49.9){
		*output = 4;
	}
	return *output;
}

static struct bsmp_func temperature_func = {
    .func_p 		  = TemperatureTest,
    .info.input_size  = 0,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

//*****************************************************************************
// 						RTCTest BSMP Function
//*****************************************************************************
uint8_t RTClkTest (uint8_t *input, uint8_t *output)
{
	uint64_t	datahour1, datahour2;

	RTCWriteDataHour(0x30, 0x25, 0x14, 0x02, 0x23, 0x05, 0x17);
	RTCReadDataHour();
	datahour1 = DataHourRead();

	RTCWriteDataHour(0x45, 0x53, 0x11, 0x05, 0x13, 0x11, 0x15);
	RTCReadDataHour();
	datahour2 = DataHourRead();



	if(datahour1 == 0x17052302142530 && datahour2 == 0x15111305115345){
		*output = 3;
	}
	else{
		*output = 4;
	}
	return *output;
}

static struct bsmp_func rtclk_func = {
    .func_p 		  = RTClkTest,
    .info.input_size  = 0,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};


//*****************************************************************************
// 						EepromTest BSMP Function
//*****************************************************************************
uint8_t EepromTest (uint8_t *input, uint8_t *output)
{
	uint16_t address = 0x0240;
	uint8_t data = 0;
	data = EepromRead(address);
	uint8_t data_ok = 0xAC;
	uint8_t data_fail = 0x36;
	EepromWrite(address, data_ok);
	data = EepromRead(address);
	EepromDummyWrite(address, data_fail);
	data = EepromRead(address);
	if(data==data_ok){
		*output = 3;
	}
	else if(data==data_fail){
		*output = 4;
	}
	else{
		*output = 5;
	}
	return *output;
}

static struct bsmp_func eeprom_func = {
    .func_p 		  = EepromTest,
    .info.input_size  = 0,       // Nothing is read from the input parameter
    .info.output_size = 1,       // command_ack
};

*/



