/*
 * rs485_bkp.c
 *
 *  Created on: 17/06/2015
 *      Author: joao.rosa
 */

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.c"
#include "driverlib/interrupt.h"

//#include "set_pinout_udc_v2.0.h"
//#include "set_pinout_ctrl_card.h"
#include "../board_drivers/hardware_def.h"

#include "rs485_bkp.h"

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

//*****************************************************************************

#define SERIAL_HEADER           1   // Destination
#define SERIAL_CSUM             1

#define SERIAL_ADDRESS          1   // My Address
#define SERIAL_MASTER_ADDRESS   0   // Master Address

#define SERIAL_BUF_SIZE         (SERIAL_HEADER+256+SERIAL_CSUM)


//*****************************************************************************
struct serial_buffer
{
    uint8_t data[20];
    uint16_t index;
    uint8_t csum;
};

static struct serial_buffer recv_buffer = {.index = 0};
static struct serial_buffer send_buffer = {.index = 0};

//*****************************************************************************

void ReadBufferRS485BKP(char *s){
	int i;
	for(i=0;i<recv_buffer.index;i++){
		s[i] = recv_buffer.data[i];
	}
	s[recv_buffer.index] = '\0';
	recv_buffer.index = 0;
	recv_buffer.csum  = 0;
}

void
RS485BKPIntHandler(void)
{
	long lChar;
	unsigned char ucChar;
	unsigned long ulStatus;

	// Get the interrrupt status.
	ulStatus = UARTIntStatus(RS485_BKP_UART_BASE, true);

	// Clear the asserted interrupts.
	UARTIntClear(RS485_BKP_UART_BASE, ulStatus);

	if(UARTRxErrorGet(RS485_BKP_UART_BASE)) UARTRxErrorClear(RS485_BKP_UART_BASE);

	// Receive Interrupt Mask
	if(UART_INT_RX == ulStatus || UART_INT_RT == ulStatus)
	{
		// Loop while there are characters in the receive FIFO.
		while(UARTCharsAvail(RS485_BKP_UART_BASE))
		{
			lChar = UARTCharGet(RS485_BKP_UART_BASE);
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
		while(UARTBusy(RS485_BKP_RD_BASE));
		// Put IC in the reception mode
		GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, OFF);
	}
}
void
InitRS485BKP(void)
{
	// Configura UART0 com baud de 8Mbps, operação 8-N-1 devido as limitações do conversor usb/serial controle
	UARTConfigSetExpClk(RS485_BKP_UART_BASE, SysCtlClockGet(SYSTEM_CLOCK_SPEED), 115200,
						(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
						UART_CONFIG_PAR_NONE));

	UARTEnable(RS485_BKP_UART_BASE);

	UARTFIFOEnable(RS485_BKP_UART_BASE);
	UARTFIFOLevelSet(RS485_BKP_UART_BASE,UART_FIFO_TX1_8,UART_FIFO_RX1_8);
/*
	//Habilita interrupção pela UART (RS-485 BKP)
	IntRegister(RS485_BKP_INT, RS485BKPIntHandler);
	UARTIntEnable(RS485_BKP_UART_BASE, UART_INT_RX | UART_INT_TX | UART_INT_RT);
	//EOT - End of Transmission
	UARTTxIntModeSet(RS485_BKP_UART_BASE, UART_TXINT_MODE_EOT);

	//Seta níveis de prioridade entre as interrupções
	IntPrioritySet(RS485_BKP_INT, 1);

	IntEnable(RS485_BKP_INT);
	*/
}
