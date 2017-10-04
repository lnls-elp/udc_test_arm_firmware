/*
 * rs485.c
 *
 *  Created on: 29/05/2015
 *      Author: joao.rosa
 */


#include "inc/hw_sysctl.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"

#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "driverlib/debug.h"

//#include "set_pinout_udc_v2.0.h"
//#include "set_pinout_ctrl_card.h"
#include "../board_drivers/hardware_def.h"

#include "rs485.h"

#include "../i2c_onboard/eeprom.h"

#include "../i2c_onboard/exio.h"

#include "../system_task/system_task.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "../bsmp/bsmp_lib.h"

#pragma DATA_SECTION(recv_buffer, "SERIALBUFFER")
#pragma DATA_SECTION(send_buffer, "SERIALBUFFER")

//*****************************************************************************

#define SERIAL_HEADER           1   // Destination
#define SERIAL_CSUM             1

static uint8_t SERIAL_ADDRESS = 1;   // My Address
static uint8_t BCAST_ADDRESS  = 255; // Broadcast Adress
#define SERIAL_MASTER_ADDRESS   0   // Master Address

#define SERIAL_BUF_SIZE         (SERIAL_HEADER+3+3+16834+SERIAL_CSUM)


//*****************************************************************************
struct serial_buffer
{
    uint8_t data[20];
    uint16_t index;
    uint8_t csum;
};

static struct serial_buffer recv_buffer = {.index = 0};
static struct serial_buffer send_buffer = {.index = 0};

static struct bsmp_raw_packet recv_packet =
                             { .data = recv_buffer.data + 1 };
static struct bsmp_raw_packet send_packet =
                             { .data = send_buffer.data + 1 };

//*****************************************************************************

struct serial_buffer
ReadBufferRS485(void){
	return recv_buffer;
}

void ResetBufferRS485(void){
	recv_buffer.index = 0;
	recv_buffer.csum  = 0;
}

void
RS485IntHandler(void)
{
	long lChar;
	unsigned char ucChar;
	unsigned long ulStatus;
	long read;

	// Get the interrrupt status.
	ulStatus = UARTIntStatus(RS485_UART_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(RS485_UART_BASE, ulStatus);

	if(UARTRxErrorGet(RS485_UART_BASE)) UARTRxErrorClear(RS485_UART_BASE);

	// Receive Interrupt Mask
	if(UART_INT_RX == ulStatus || UART_INT_RT == ulStatus)
	{
		// Loop while there are characters in the receive FIFO.
		while(UARTCharsAvail(RS485_UART_BASE))
		{
			lChar = UARTCharGet(RS485_UART_BASE);
			if(!(lChar & ~0xFF))
			{
				ucChar = (unsigned char)(lChar & 0xFF);
				//UARTCharPutNonBlocking(FT230_UART_BASE, ucChar);
				recv_buffer.data[recv_buffer.index] = ucChar;
				recv_buffer.csum += recv_buffer.data[recv_buffer.index++];
			}
		}
	}
    // Transmit Interrupt Mask
	else if(UART_INT_TX == ulStatus) // TX interrupt
	{
		while(UARTBusy(RS485_RD_BASE));
		// Put IC in the reception mode
		GPIOPinWrite(RS485_RD_BASE, RS485_RD_PIN, OFF);
	}
}


void
InitRS485(void)
{

	if(HARDWARE_VERSION == 0x21) Rs485TermCtrl(0);

	UARTConfigSetExpClk(RS485_UART_BASE, SysCtlClockGet(SYSTEM_CLOCK_SPEED), 115200,
						(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));

	UARTFIFOEnable(RS485_UART_BASE);
	UARTFIFOLevelSet(RS485_UART_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);

	/*
	//Habilita interrupo pela UART1 (RS-485)
	IntRegister(RS485_INT, RS485IntHandler);
	UARTIntEnable(RS485_UART_BASE, UART_INT_RX | UART_INT_TX | UART_INT_RT);

	//EOT - End of Transmission
	UARTTxIntModeSet(RS485_UART_BASE, UART_TXINT_MODE_EOT);

	//Seta nveis de prioridade entre as interrupes
	IntPrioritySet(RS485_INT, 0);

	// Enable the UART
	UARTEnable(RS485_UART_BASE);

	IntEnable(RS485_INT);

	*/
}


