/*
 * system_task.c
 *
 *  Created on: 20/07/2015
 *      Author: joao.rosa
 */

#include "system_task.h"

#include "../i2c_onboard/rtc.h"

#include "../i2c_offboard_isolated/temp_low_power_module.h"

#include "../signals_onboard/signals_onboard.h"

#include "../rs485_bkp/rs485_bkp.h"

#include "../rs485/rs485.h"

#include "../ihm/ihm.h"

#include "../can/can_bkp.h"

#include "../shared_memory/main_var.h"

#include "../ipc/ipc_lib.h"

#include "../i2c_onboard/eeprom.h"

#include "../usb_to_serial/usb_to_serial.h"

#include "../i2c_onboard/exio.h"

#include "../i2c_onboard/temp_sensor.h"

#include "../jiga/jiga.h"

#include <stdint.h>
#include <stdbool.h>

volatile bool READ_RTC = 0;
volatile bool READ_IIB = 0;
volatile bool ITLK_ALARM_RESET = 0;
volatile bool PROCESS_DISP_MESS = 0;
volatile bool PROCESS_ETH_MESS = 0;
volatile bool PROCESS_CAN_MESS = 0;
volatile bool PROCESS_RS485_MESS = 0;
volatile bool PROCESS_USB2SERIAL_MESS = 0;
volatile bool PROCESS_POWER_TEMP_SAMPLE = 0;
volatile bool EEPROM_WRITE_REQUEST = 0;

volatile bool LED_STS_TURN_ON_REQUEST = 0;
volatile bool LED_STS_TURN_OFF_REQUEST = 0;
volatile bool LED_ITLK_TURN_ON_REQUEST = 0;
volatile bool LED_ITLK_TURN_OFF_REQUEST = 0;
volatile bool SOUND_ITLK_TURN_ON_REQUEST = 0;
volatile bool SOUND_ITLK_TURN_OFF_REQUEST = 0;
volatile bool SAMPLE_BOARD_TEMP_REQUEST = 0;
volatile bool RTC_TEST_REQUEST = 0;
volatile bool EXIO_TEST_REQUEST = 0;
volatile bool ISO_PS_TEST_REQUEST = 0;
volatile bool LOOPBACK_TEST_REQUEST = 0;
volatile bool ADCP_TEST_REQUEST = 0;
volatile bool RS485_TEST_REQUEST = 0;
volatile bool FLASH_TEST_REQUEST = 0;
volatile bool RAM_TEST_REQUEST = 0;
volatile bool TEMP_SENSE_TEST_REQUEST = 0;

uint8_t
TaskSetNew(uint8_t TaskNum)
{
	switch(TaskNum)
	{
	case SAMPLE_RTC:
		READ_RTC = 1;
		break;

	case PROCESS_IIB_MESSAGE:
		READ_IIB = 1;
		break;

	case CLEAR_ITLK_ALARM:
		ITLK_ALARM_RESET = 1;
		break;

	case PROCESS_DISPLAY_MESSAGE:
		PROCESS_DISP_MESS = 1;
		break;

	case PROCESS_ETHERNET_MESSAGE:
		PROCESS_ETH_MESS = 1;
		break;

	case PROCESS_CAN_MESSAGE:
		PROCESS_CAN_MESS = 1;
		break;

	case PROCESS_RS485_MESSAGE:
		PROCESS_RS485_MESS = 1;
		break;

	case PROCESS_USB2SERIAL_MESSAGE:
	    PROCESS_USB2SERIAL_MESS = 1;
	    break;

	case POWER_TEMP_SAMPLE:
		PROCESS_POWER_TEMP_SAMPLE = 1;
		break;
	case EEPROM_WRITE_REQUEST_CHECK:
		EEPROM_WRITE_REQUEST = 1;
		break;
	case LED_STS_TURN_ON:
	    LED_STS_TURN_ON_REQUEST = 1;
	    break;
	case LED_STS_TURN_OFF:
	    LED_STS_TURN_OFF_REQUEST = 1;
	    break;
	case LED_ITLK_TURN_ON:
	    LED_ITLK_TURN_ON_REQUEST = 1;
	    break;
	case LED_ITLK_TURN_OFF:
	    LED_ITLK_TURN_OFF_REQUEST = 1;
	    break;
	case SOUND_ITLK_TURN_ON:
	    SOUND_ITLK_TURN_ON_REQUEST = 1;
	    break;
	case SOUND_ITLK_TURN_OFF:
	    SOUND_ITLK_TURN_OFF_REQUEST = 1;
	    break;
	case SAMPLE_BOARD_TEMP:
	    SAMPLE_BOARD_TEMP_REQUEST = 1;
	    break;
	case RTC_TEST:
	    RTC_TEST_REQUEST = 1;
	    break;
	case EXIO_TEST:
	    EXIO_TEST_REQUEST = 1;
	    break;
	case ISO_PS_TEST:
	    ISO_PS_TEST_REQUEST = 1;
	    break;
	case LOOPBACK_TEST:
	    LOOPBACK_TEST_REQUEST = 1;
	    break;
	case ADCP_TEST:
	    ADCP_TEST_REQUEST = 1;
	    break;
	case RS485_TEST:
	    RS485_TEST_REQUEST = 1;
	    break;
	case FLASH_TEST:
	    FLASH_TEST_REQUEST = 1;
	    break;
	case RAM_TEST:
	    RAM_TEST_REQUEST = 1;
	    break;
	case TEMP_SENSE_TEST:
	    TEMP_SENSE_TEST_REQUEST = 1;

	default:

		break;

	}

	return 0;
}

uint8_t
TaskCheck(void)
{

	if(PROCESS_CAN_MESS)
	{
		PROCESS_CAN_MESS = 0;
		CanCheck();
	}

	else if(PROCESS_USB2SERIAL_MESS)
	{
	    PROCESS_USB2SERIAL_MESS = 0;
	    Usb2SerialProcessData();
	}
/*
	else if(PROCESS_RS485_MESS)
	{
		PROCESS_RS485_MESS = 0;
		RS485ProcessData();
	}

*/
	else if(PROCESS_ETH_MESS)
	{
		PROCESS_ETH_MESS = 0;
		// Ethernet function
	}

	else if(PROCESS_DISP_MESS)
	{
		PROCESS_DISP_MESS = 0;
		DisplayProcessData();
	}

	else if(READ_RTC)
	{
		READ_RTC = 0;
		RTCReadDataHour();
		//HeartBeatLED();
	}

	else if(READ_IIB)
	{
		READ_IIB = 0;
	//	RS485BKPTxHandler();
	}

	else if(ITLK_ALARM_RESET)
	{
		ITLK_ALARM_RESET = 0;
		InterlockAlarmReset();
	}

	else if(PROCESS_POWER_TEMP_SAMPLE)
	{
		PROCESS_POWER_TEMP_SAMPLE = 0;

		switch(IPC_MtoC_Msg.PSModule.Model.u16)
		{
			case FBP_100kHz:
				//PowerSupply1TempRead();
				break;
			case FBPx4_100kHz:
				PowerSupply1TempRead();
				PowerSupply2TempRead();
				PowerSupply3TempRead();
				PowerSupply4TempRead();
				break;
		}
	}

	else if(EEPROM_WRITE_REQUEST)
	{
		EEPROM_WRITE_REQUEST = 0;
		EepromWriteRequestCheck();
	}

	else if(LED_STS_TURN_ON_REQUEST)
	{
	    LED_STS_TURN_ON_REQUEST = 0;
	    LedStsCtrl(1);
	}

	else if(LED_STS_TURN_OFF_REQUEST)
    {
        LED_STS_TURN_OFF_REQUEST = 0;
        LedStsCtrl(0);
    }

	else if(LED_ITLK_TURN_ON_REQUEST)
	{
	    LED_ITLK_TURN_ON_REQUEST = 0;
	    LedItlkCtrl(1);
	}

	else if(LED_ITLK_TURN_OFF_REQUEST)
    {
        LED_ITLK_TURN_OFF_REQUEST = 0;
        LedItlkCtrl(0);
    }

	else if(SOUND_ITLK_TURN_ON_REQUEST)
	{
	    SOUND_ITLK_TURN_ON_REQUEST = 0;
	    SoundSelCtrl(1);
	}

	else if(SOUND_ITLK_TURN_OFF_REQUEST)
    {
        SOUND_ITLK_TURN_OFF_REQUEST = 0;
        SoundSelCtrl(0);
    }

	else if(SAMPLE_BOARD_TEMP_REQUEST)
	{
	    SAMPLE_BOARD_TEMP_REQUEST = 0;
	    ReadTemperature();
	}

	else if(RTC_TEST_REQUEST)
	{
	    RTC_TEST_REQUEST = 0;
	    RTCAccessTest();
	}

	else if(EXIO_TEST_REQUEST)
	{
	    EXIO_TEST_REQUEST = 0;
	    ExIoTestRoutine();
	}

	else if(ISO_PS_TEST_REQUEST)
	{
	    ISO_PS_TEST_REQUEST = 0;
	    IsoPowerSupplyLoopBackTest();
	}
	else if(LOOPBACK_TEST_REQUEST)
	{
	    LOOPBACK_TEST_REQUEST = 0;
	    LoopBackFunctionTest();
	}
	else if(ADCP_TEST_REQUEST)
	{
	    ADCP_TEST_REQUEST = 0;
	    AdcpFunctionTest();
	}
	else if(RS485_TEST_REQUEST)
	{
	    RS485_TEST_REQUEST = 0;
	    Rs485Loopbacktest();
	}
	else if(FLASH_TEST_REQUEST)
	{
	    FLASH_TEST_REQUEST = 0;
	    TestFlashRoutine();
	}
	else if(RAM_TEST_REQUEST)
	{
	    RAM_TEST_REQUEST = 0;
	    TestRamRoutine();
	}
	else if(TEMP_SENSE_TEST_REQUEST)
	{
	    TEMP_SENSE_TEST_REQUEST = 0;
	    TestTempSenseRoutine();
	}

	return 0;

}
