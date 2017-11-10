/*
 * jiga.c
 *
 *  Created on: 08/05/2017
 *      Author: ali.faraj
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

#include "../board_drivers/hardware_def.h"
#include "../communication_drivers/i2c_onboard/i2c_onboard.h"
#include "../communication_drivers/i2c_onboard/rtc.h"
#include "../communication_drivers/i2c_onboard/eeprom.h"
#include "../communication_drivers/i2c_onboard/temp_sensor.h"
#include "../communication_drivers/i2c_onboard/exio.h"
#include "../communication_drivers/i2c_offboard_isolated/i2c_offboard_isolated.h"
#include "../communication_drivers/rs485/rs485.h"
#include "../communication_drivers/rs485_bkp/rs485_bkp.h"
#include "../system_task/system_task.h"
#include "../communication_drivers/flash/flash_mem.h"
#include "../epi/sdram_mem.h"
#include "../shared_memory/structs.h"
#include "../adcp/adcp.h"

#include <stdlib.h>
#include "jiga.h"

#define I2C_SLV_ADDR_EEPROM	0x50 // Endereço 7 bits

#define I2C_SLV_ADDR_BKP    0b0101000

#define	SINGLE_ADDRESS	0x01
#define	DOUBLE_ADDRESS	0x02

// Loopback
#define LOOPBACK_1      0X01
#define LOOPBACK_2      0x02
#define LOOPBACK_3      0x03
#define LOOPBACK_4      0x04
#define LOOPBACK_5      0x05
#define LOOPBACK_6      0x06
#define LOOPBACK_7      0x07
#define LOOPBACK_8      0x08
#define LOOPBACK_9      0x09
#define LOOPBACK_10     0x0A
#define LOOPBACK_11     0x0B
#define LOOPBACK_12     0x0C
#define LOOPBACK_13     0x0D
#define LOOPBACK_14     0x0E
#define LOOPBACK_15     0x0F
#define LOOPBACK_16     0x10
#define LOOPBACK_17     0x11
#define LOOPBACK_18     0x12
#define LOOPBACK_19     0x13
#define LOOPBACK_20     0x14
#define LOOPBACK_21     0x15
#define LOOPBACK_22     0x16
#define LOOPBACK_23     0x17
#define LOOPBACK_24     0x18
#define LOOPBACK_25     0x19
#define LOOPBACK_26     0x1A
#define LOOPBACK_27     0x1B
#define LOOPBACK_28     0x1C
#define LOOPBACK_29     0x1D
#define LOOPBACK_30     0x1E
#define LOOPBACK_31     0x1F
#define LOOPBACK_32     0x20

volatile uint8_t teste = 0;

//***********************************************************************************

// IO Expander Test Routine (OK)
uint8_t
TestIoExp(uint8_t function_type)
{
    if(function_type == 0)return TaskSetNew(EXIO_TEST);

    else if(function_type == 1)return ExIoTestResult();
}

//***********************************************************************************

// Led Test (OK)
uint8_t
TestLedTurnOn(void)
{
    //Config BLUE LED
    TaskSetNew(LED_STS_TURN_ON);

    //Config RED LED
    TaskSetNew(LED_ITLK_TURN_ON);

    return 0;

}

uint8_t
TestLedTurnOff(void)
{
    //Turn Leds Off
    LedStsCtrl(0x00);
    LedItlkCtrl(0x00);

    return 0;
}

//***********************************************************************************

// Buzzer Test (OK)
uint8_t
TestBuzzerTurnOn(void)
{
    //Buzzer Turn On
    //BuffersCtrl(0x01);
    return TaskSetNew(SOUND_ITLK_TURN_ON);
}

uint8_t
TestBuzzerTurnOff(void)
{

    //Buzzer Turn Off
    //BuffersCtrl(0x00);
    return TaskSetNew(SOUND_ITLK_TURN_OFF);

}

//***********************************************************************************

// Implementar a leitura!!
// EEPROM Test Routine
uint8_t
TestEepromWriteSerialNumber(uint8_t function_type, uint8_t serial_number0, uint8_t serial_number1, uint8_t serial_number2, uint8_t serial_number3, uint8_t serial_number4, uint8_t serial_number5, uint8_t serial_number6, uint8_t serial_number7, uint8_t serial_number8, uint8_t serial_number9)
{

    if(function_type == 0) return SaveSerialNumber(serial_number0, serial_number1, serial_number2, serial_number3, serial_number4, serial_number5, serial_number6, serial_number7, serial_number8, serial_number9);

    else if (function_type == 1) return ReadEepromSts();
}

//***********************************************************************************

volatile uint8_t flash_result = 0;

// FLASH Test Routines (OK)
uint8_t
TestFlashMem(uint8_t function_type)
{
    if(function_type == 0) return TaskSetNew(FLASH_TEST);
    else if(function_type == 1) return flash_result;
}

uint8_t
TestFlashRoutine(void)
{
    uint64_t test_mem = 0x0000000000000000;

    //Write memory test sequence
    test_mem = FlashDeviceIDRead();

    if((test_mem > 0x0000000000000000) && (test_mem < 0xFFFFFFFFFFFFFFFF))
    {
        // No error
        flash_result = 0;
    }
    // Error
    else flash_result = 1;
}

//***********************************************************************************

volatile uint8_t ram_result = 0;

// RAM Test Routines (OK)
uint8_t
TestRamMem(uint8_t function_type)
{
    if(function_type == 0) return TaskSetNew(RAM_TEST);
    else if(function_type == 1) return ram_result;
}

uint8_t
TestRamRoutine(void)
{
    uint8_t test_mem = 0x00;

    test_mem = sdram_read_write();

    // Error
    if(test_mem) ram_result = 1;

    // No Error
    else ram_result = 0;

    return 0;
}

//***********************************************************************************

//Não implementar
// SD Card Test Routines
uint8_t
TestSdCardMen(void)
{
    // Test card is connected

    // if previous test is ok, run memory check

    return;
}

//***********************************************************************************

// RTC Test Routine (OK)
uint8_t
TestRtcMod(uint8_t function_type)
{
    if(function_type == 0)return TaskSetNew(RTC_TEST);

    else if(function_type == 1)return RTCAccessStatus();

}

//***********************************************************************************

volatile uint8_t temp_sensor_result = 0;

// Temperature Sensor Test (OK)
uint8_t
TestTempSense(uint8_t function_type)
{
    if(function_type == 0) return TaskSetNew(TEMP_SENSE_TEST);
    else if(function_type == 1) return ram_result;
}

uint8_t
TestTempSenseRoutine(void)
{
    float temp_var = 0.0;

    // Read temperature
    temp_var = TemperatureCelsius();

    // Test temperature value
    if((temp_var > 10.0) && (temp_var < 40.0)) temp_sensor_result = 0; // Successfully test
    else temp_sensor_result = 1; // Error

    return 0;
}



//***********************************************************************************

// ERRO DCDC NAO DESLIGA !!!!
// Isolated Power Supply Test Routine

volatile uint8_t iso_ps_lb_result = 0;

uint8_t
IsoPowerSupplyLoopbackResult(void)
{
   return iso_ps_lb_result;
}

uint8_t
IsoPowerSupplyLoopBackTest(void)
{
    uint8_t gpdi_sts = 0;

    // Read Isolated Power Supply Status
    gpdi_sts = DcdcSts();

    //
    if(gpdi_sts == 1)
    {
        iso_ps_lb_result = 0;
    }
    else iso_ps_lb_result = 1; // Error

    return 0;
}

uint8_t
TestIsoPowerSup(uint8_t function_type)
{
    if(function_type == 0)return TaskSetNew(ISO_PS_TEST);
    else if(function_type == 1)return IsoPowerSupplyLoopbackResult();
}

//***********************************************************************************
// OK

// ADC Tests Routines
volatile uint8_t adcp_1_result = 0;
volatile uint8_t adcp_2_result = 0;
volatile uint8_t adcp_3_result = 0;
volatile uint8_t adcp_4_result = 0;
volatile uint8_t adcp_5_result = 0;
volatile uint8_t adcp_6_result = 0;
volatile uint8_t adcp_7_result = 0;
volatile uint8_t adcp_8_result = 0;

volatile uint8_t adcp_test_channel = 0;

uint8_t
TestAdcpRange(uint8_t function_type, uint8_t channel_number)
{
    uint8_t result;
    // Execute Test Routines
    if(function_type == 0)
    {
        // Configure the channel that need to be tested
        adcp_test_channel = channel_number;
        TaskSetNew(ADCP_TEST);
        return 0;
    }
    // Read Test Result
    else if(function_type == 1)
    {
        switch(channel_number)
        {
        case 0x01:
            result = adcp_1_result;
            break;
        case 0x02:
            result = adcp_2_result;
            break;
        case 0x03:
            result = adcp_3_result;
            break;
        case 0x04:
            result = adcp_4_result;
            break;
        case 0x05:
            result = adcp_5_result;
            break;
        case 0x06:
            result = adcp_6_result;
            break;
        case 0x07:
            result = adcp_7_result;
            break;
        case 0x08:
            result = adcp_8_result;
            break;
        }
    }

    return result;
}

uint8_t
AdcpFunctionTest(void)
{
    unsigned long ulLoop;
    float adcp_value = 0.0;

    switch(adcp_test_channel)
    {
    case 1:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM1 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(1);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM1 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(1);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM1 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(1);

                if(adcp_value < -7.0)
                {
                    adcp_1_result = 0;
                }
                else adcp_1_result = 1;
            }
            else adcp_1_result = 1;
        }
        else adcp_1_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM1 = 50;

        break;
    case 2:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM2 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(2);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM2 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(2);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM2 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(2);

                if(adcp_value < -7.0)
                {
                    adcp_2_result = 0;
                }
                else adcp_2_result = 1;
            }
            else adcp_2_result = 1;
        }
        else adcp_2_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM2 = 50;
        break;

    case 3:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM3 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(3);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM3 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(3);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM3 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(3);

                if(adcp_value < -7.0)
                {
                    adcp_3_result = 0;
                }
                else adcp_3_result = 1;
            }
            else adcp_3_result = 1;
        }
        else adcp_3_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM3 = 50;
        break;

    case 4:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM4 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(4);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM4 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(4);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM4 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(4);

                if(adcp_value < -7.0)
                {
                    adcp_4_result = 0;
                }
                else adcp_4_result = 1;
            }
            else adcp_4_result = 1;
        }
        else adcp_4_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM4 = 50;
        break;

    case 5:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM5 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(5);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM5 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(5);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM5 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(5);

                if(adcp_value < -7.0)
                {
                    adcp_5_result = 0;
                }
                else adcp_5_result = 1;
            }
            else adcp_5_result = 1;
        }
        else adcp_5_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM5 = 50;
        break;

    case 6:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM6 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(6);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM6 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(6);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM6 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(6);

                if(adcp_value < -7.0)
                {
                    adcp_6_result = 0;
                }
                else adcp_6_result = 1;
            }
            else adcp_6_result = 1;
        }
        else adcp_6_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM6 = 50;
        break;

    case 7:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM7 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(7);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM7 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(7);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM7 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(7);

                if(adcp_value < -7.0)
                {
                    adcp_7_result = 0;
                }
                else adcp_7_result = 1;
            }
            else adcp_7_result = 1;
        }
        else adcp_7_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM7 = 50;
        break;

    case 8:
        // Adjust setpoint to 0% = 10V
        arm_controls.PWM8 = 0;

        // wait
        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read adcp
        adcp_value = AdcpReadChannel(8);

        if(adcp_value > 9.0)
        {
            // Adjust setpoint to 50%
            arm_controls.PWM8 = 50;

            // wait
            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read adcp
            adcp_value = AdcpReadChannel(8);

            if(adcp_value > -1.5 && adcp_value < 2.0)
            {
                // Adjust setpoint to 100% = -10V
                arm_controls.PWM8 = 100;

                // wait
                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read adcp
                adcp_value = AdcpReadChannel(8);

                if(adcp_value < -7.0)
                {
                    adcp_8_result = 0;
                }
                else adcp_8_result = 1;
            }
            else adcp_8_result = 1;
        }
        else adcp_8_result = 1;

        // Set a setpoint to 50%
        arm_controls.PWM8 = 50;
        break;
    }
}

//***********************************************************************************

volatile uint8_t rs485_test_result = 0;

// Rs485 Test Routines (OK)
uint8_t
TestRs485Loop(uint8_t function_type)
{
    // Put RS485_M3 on reception mode
    if(function_type == 0)return TaskSetNew(RS485_TEST);
    else if(function_type == 1)return rs485_test_result;
}

uint8_t
Rs485Loopbacktest(void)
{
    unsigned long ulLoop;

    uint8_t rs485_bkp_rx_sts = 0;
    uint8_t rs485_rx_sts = 0;
    uint8_t sci_rx_sts = 0;

    // RS485 frontal TX test
    // Put transceiver in transmission mode
    GPIOPinWrite(RS485_RD_BASE, RS485_RD_PIN, ON); //Turn on RS-485_RD
    // Put the others in reception mode
    GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, OFF); //Turn off UART_M3_RD
    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_5, OFF); //Turn off SCI_RD

    // Put tx pin in low level
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, OFF); //Turn off RS-485_TX

    for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

    // Read RX pins
    rs485_bkp_rx_sts = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_6); //UART_M3_RX
    rs485_bkp_rx_sts = rs485_bkp_rx_sts >> 6;

    sci_rx_sts = GPIOPinRead(GPIO_PORTQ_BASE,GPIO_PIN_7); //Read SCIRXDA pin
    sci_rx_sts = sci_rx_sts >> 7;

    if(rs485_bkp_rx_sts == 0 && sci_rx_sts == 0)
    {

        // Put tx pin in high level
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, ON); //Turn on RS-485_TX

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read RX pins
        rs485_bkp_rx_sts = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_6); //UART_M3_RX
        rs485_bkp_rx_sts = rs485_bkp_rx_sts >> 6;

        sci_rx_sts = GPIOPinRead(GPIO_PORTQ_BASE,GPIO_PIN_7); //Read SCIRXDA pin
        sci_rx_sts = sci_rx_sts >> 7;

        if(rs485_bkp_rx_sts == 1 && sci_rx_sts == 1)
        {
            // Put transceiver in reception mode
            GPIOPinWrite(RS485_RD_BASE, RS485_RD_PIN, OFF); //Turn off RS-485_RD

            // Put transceiver in transmission mode
            GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, ON); //Turn on UART_M3_RD

            // Put tx pin in low level
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_7, OFF); //Turn off UART_M3_TX

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read RX pins
            rs485_rx_sts = GPIOPinRead(GPIO_PORTN_BASE,GPIO_PIN_2); //RS-485_RX
            rs485_rx_sts = rs485_rx_sts >> 2;

            sci_rx_sts = GPIOPinRead(GPIO_PORTQ_BASE,GPIO_PIN_7); //Read SCIRXDA pin
            sci_rx_sts = sci_rx_sts >> 7;

            if(rs485_rx_sts == 0 && sci_rx_sts == 0)
            {
                // Put tx pin in high level
                GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_7, ON); //Turn on UART_M3_TX

                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                // Read RX pins
                rs485_rx_sts = GPIOPinRead(GPIO_PORTN_BASE,GPIO_PIN_2); //RS-485_RX
                rs485_rx_sts = rs485_rx_sts >> 2;

                sci_rx_sts = GPIOPinRead(GPIO_PORTQ_BASE,GPIO_PIN_7); //Read SCIRXDA pin
                sci_rx_sts = sci_rx_sts >> 7;

                if(rs485_rx_sts == 1 && sci_rx_sts == 1)
                {
                    // Put transceiver in reception mode
                    GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, OFF); //Turn off UART_M3_RD

                    // Put transceiver in transmission mode
                    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_5, ON); //Turn on SCI_RD

                    // Put tx pin in low level
                    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_6, OFF); //Turn off SCITXDA

                    for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                    // Read RX pins
                    rs485_rx_sts = GPIOPinRead(GPIO_PORTN_BASE,GPIO_PIN_2); //RS-485_RX
                    rs485_rx_sts = rs485_rx_sts >> 2;

                    rs485_bkp_rx_sts = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_6); //UART_M3_RX
                    rs485_bkp_rx_sts = rs485_bkp_rx_sts >> 6;

                    if(rs485_rx_sts == 0 && rs485_bkp_rx_sts == 0)
                    {
                        // Put tx pin in high level
                        GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_6, ON); //Turn off SCITXDA

                        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                        // Read RX pins
                        rs485_rx_sts = GPIOPinRead(GPIO_PORTN_BASE,GPIO_PIN_2); //RS-485_RX
                        rs485_rx_sts = rs485_rx_sts >> 2;

                        rs485_bkp_rx_sts = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_6); //UART_M3_RX
                        rs485_bkp_rx_sts = rs485_bkp_rx_sts >> 6;

                        if(rs485_rx_sts == 1 && rs485_bkp_rx_sts == 1)
                        {
                            rs485_test_result = 0;
                        }
                        else rs485_test_result = 1;
                    }
                    else rs485_test_result = 1;


                }
                else rs485_test_result = 1;
            }
            else rs485_test_result = 1;

        }
        else rs485_test_result = 1;

    }
    else rs485_test_result = 1;

    //Turn off all output pins
    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_3, OFF); //Turn off RS-485_TX
    GPIOPinWrite(RS485_RD_BASE, RS485_RD_PIN, OFF); //Turn off RS-485_RD

    GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_7, OFF); //Turn off UART_M3_TX
    GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, OFF); //Turn off UART_M3_RD

    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_6, OFF); //Turn off SCITXDA
    GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_5, OFF); //Turn off SCI_RD
}

//***********************************************************************************

// AVALIAR SE É NECESSÁRIO!!!!!!
// Ethernet Init Phy Tests
uint8_t
TestEthernetPhy(void)
{
    // Init Phy


    // Read Phy status

    return;

}

//***********************************************************************************

//*************************************************************************************
//         Loopback functions
//*************************************************************************************

// OK

volatile uint8_t test_channel = 0;

volatile uint8_t loopback_1_result = 0;
volatile uint8_t loopback_2_result = 0;
volatile uint8_t loopback_3_result = 0;
volatile uint8_t loopback_4_result = 0;
volatile uint8_t loopback_5_result = 0;
volatile uint8_t loopback_6_result = 0;
volatile uint8_t loopback_7_result = 0;
volatile uint8_t loopback_8_result = 0;
volatile uint8_t loopback_9_result = 0;
volatile uint8_t loopback_10_result = 0;
volatile uint8_t loopback_11_result = 0;
volatile uint8_t loopback_12_result = 0;
volatile uint8_t loopback_13_result = 0;
volatile uint8_t loopback_14_result = 0;
volatile uint8_t loopback_15_result = 0;
volatile uint8_t loopback_16_result = 0;
volatile uint8_t loopback_17_result = 0;
volatile uint8_t loopback_18_result = 0;
volatile uint8_t loopback_19_result = 0;
volatile uint8_t loopback_20_result = 0;
volatile uint8_t loopback_21_result = 0;
volatile uint8_t loopback_22_result = 0;
volatile uint8_t loopback_23_result = 0;
volatile uint8_t loopback_24_result = 0;
volatile uint8_t loopback_25_result = 0;
volatile uint8_t loopback_26_result = 0;
volatile uint8_t loopback_27_result = 0;
volatile uint8_t loopback_28_result = 0;
volatile uint8_t loopback_29_result = 0;
volatile uint8_t loopback_30_result = 0;
volatile uint8_t loopback_31_result = 0;
volatile uint8_t loopback_32_result = 0;

//
uint8_t
TestLoopBackSignal(uint8_t function_type, uint8_t channel_number)
{
    uint8_t result = 0;
    // Execute Test Routines
    if(function_type == 0)
    {
        // Configure the channel that need to be tested
        test_channel = channel_number;
        TaskSetNew(LOOPBACK_TEST);
        return 0;
    }
    // Read Test Result
    else if(function_type == 1)
    {
        switch(channel_number)
        {
        case LOOPBACK_1:
            result = loopback_1_result;
            break;
        case LOOPBACK_2:
            result = loopback_2_result;
            break;
        case LOOPBACK_3:
            result = loopback_3_result;
            break;
        case LOOPBACK_4:
            result = loopback_4_result;
            break;
        case LOOPBACK_5:
            result = loopback_5_result;
            break;
        case LOOPBACK_6:
            result = loopback_6_result;
            break;
        case LOOPBACK_7:
            result = loopback_7_result;
            break;
        case LOOPBACK_8:
            result = loopback_8_result;
            break;
        case LOOPBACK_9:
            result = loopback_9_result;
            break;
        case LOOPBACK_10:
            result = loopback_10_result;
            break;
        case LOOPBACK_11:
            result = loopback_11_result;
            break;
        case LOOPBACK_12:
            result = loopback_12_result;
            break;
        case LOOPBACK_13:
            result = loopback_13_result;
            break;
        case LOOPBACK_14:
            result = loopback_14_result;
            break;
        case LOOPBACK_15:
            result = loopback_15_result;
            break;
        case LOOPBACK_16:
            result = loopback_16_result;
            break;
        case LOOPBACK_17:
            result = loopback_17_result;
            break;
        case LOOPBACK_18:
            result = loopback_18_result;
            break;
        case LOOPBACK_19:
            result = loopback_19_result;
            break;
        case LOOPBACK_20:
            result = loopback_20_result;
            break;
        case LOOPBACK_21:
            result = loopback_21_result;
            break;
        case LOOPBACK_22:
            result = loopback_22_result;
            break;
        case LOOPBACK_23:
            result = loopback_23_result;
            break;
        case LOOPBACK_24:
            result = loopback_24_result;
            break;
        case LOOPBACK_25:
            result = loopback_25_result;
            break;
        case LOOPBACK_26:
            result = loopback_26_result;
            break;
        case LOOPBACK_27:
            result = loopback_27_result;
            break;
        case LOOPBACK_28:
            result = loopback_28_result;
            break;
        case LOOPBACK_29:
            result = loopback_29_result;
            break;
        case LOOPBACK_30:
            result = loopback_30_result;
            break;
        case LOOPBACK_31:
            result = loopback_31_result;
            break;
        case LOOPBACK_32:
            result = loopback_32_result;
            break;
        }
    }

    return result;
}



// Loop Back Signals Test Routines
uint8_t
LoopBackFunctionTest(void)
{
    unsigned long ulLoop;
    uint8_t gpdi_sts = 0;

    switch(test_channel)
    {
    // PWM_OF16 -> GPDI16_ISO (C28)
    case LOOPBACK_1:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI16;

        if(gpdi_sts != 0) loopback_1_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI16;

            if(gpdi_sts != 1) loopback_1_result = 1;// Error
            else
            {
                loopback_1_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_7, OFF); //Turn off
        }
        break;

    // PWM_OF15 -> GPDI15_ISO (C28)
    case LOOPBACK_2:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI15;

        if(gpdi_sts != 0) loopback_2_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI15;

            if(gpdi_sts != 1) loopback_2_result = 1;// Error
            else
            {
                loopback_2_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6, OFF); //Turn off
        }
        break;

    // PWM_OF14 -> GPDI14_ISO (C28)
    case LOOPBACK_3:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI14;

        if(gpdi_sts != 0) loopback_3_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI14;

            if(gpdi_sts != 1) loopback_3_result = 1;// Error
            else
            {
                loopback_3_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, OFF); //Turn off
        }
        break;

    // PWM_OF13 -> GPDI13_ISO (C28)
    case LOOPBACK_4:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI13;

        if(gpdi_sts != 0) loopback_4_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI13;

            if(gpdi_sts != 1) loopback_4_result = 1;// Error
            else
            {
                loopback_4_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, OFF); //Turn off
        }
        break;

    // PWM_OF12 -> GPDI12_ISO
    case LOOPBACK_5:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTQ_BASE, GPIO_PIN_0); //GPDI12_ISO

        if(gpdi_sts != 0) loopback_5_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTQ_BASE, GPIO_PIN_0); //GPDI12_ISO

            if(gpdi_sts != 1) loopback_5_result = 1;// Error
            else
            {
                loopback_5_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_3, OFF); //Turn off
        }
        break;

    // PWM_OF11 -> GPDI11_ISO
    case LOOPBACK_6:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTQ_BASE, GPIO_PIN_1); //GPDI11_ISO
        gpdi_sts = gpdi_sts >> 1;

        if(gpdi_sts != 0) loopback_6_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTQ_BASE, GPIO_PIN_1); //GPDI11_ISO
            gpdi_sts = gpdi_sts >> 1;

            if(gpdi_sts != 1) loopback_6_result = 1;// Error
            else
            {
                loopback_6_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_2, OFF); //Turn off
        }
        break;

    // PWM_OF10 -> GPDI10_ISO
    case LOOPBACK_7:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_6); //GPDI10_ISO
        gpdi_sts = gpdi_sts >> 6;

        if(gpdi_sts != 0) loopback_7_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_6); //GPDI10_ISO
            gpdi_sts = gpdi_sts >> 6;

            if(gpdi_sts != 1) loopback_7_result = 1;// Error
            else
            {
                loopback_7_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1, OFF); //Turn off
        }
        break;

    // PWM_OF9 -> GPDI9_ISO
    case LOOPBACK_8:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_5); //GPDI9_ISO
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_8_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTP_BASE, GPIO_PIN_5); //GPDI9_ISO
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_8_result = 1;// Error
            else
            {
                // Write 0 to GPIO
                GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, OFF); //Turn off
                loopback_8_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0, OFF); //Turn off
        }
        break;

    // PWM_SOC -> GPDI8_ISO (C28)
    case LOOPBACK_9:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, OFF); //Turn off PWM_SOC

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI8;

        if(gpdi_sts != 0) loopback_9_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, ON); //Turn on PWM_SOC

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI8;

            if(gpdi_sts != 1) loopback_9_result = 1;// Error
            else
            {
                loopback_9_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_0, OFF); //Turn off PWM_SOC
        }
        break;

    // GPIO_CONFIG -> GPDI7_ISO (C28)
    case LOOPBACK_10:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_3, OFF); //Turn off GPIO_CONFIG

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI7;

        if(gpdi_sts != 0) loopback_10_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_3, ON); //Turn on GPIO_CONFIG

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI7;

            if(gpdi_sts != 1) loopback_10_result = 1;// Error
            else
            {
                loopback_10_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_3, OFF); //Turn off GPIO_CONFIG
        }
        break;

    // GPIO_CS2 -> GPDI6_ISO
    case LOOPBACK_11:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_1, OFF); //Turn off GPIO_CS2

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTQ_BASE, GPIO_PIN_4); //GPDI6_ISO
        gpdi_sts = gpdi_sts >> 4;

        if(gpdi_sts != 0) loopback_11_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_1, ON); //Turn on GPIO_CS2

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTQ_BASE, GPIO_PIN_4); //GPDI6_ISO
            gpdi_sts = gpdi_sts >> 4;

            if(gpdi_sts != 1) loopback_11_result = 1;// Error
            else
            {
                loopback_11_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_1, OFF); //Turn off GPIO_CS2
        }
        break;

    // GPIO_CS1 -> GPDI5_ISO (C28)
    case LOOPBACK_12:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_2, OFF); //Turn off GPIO_CS1

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = c28_values.GPDI5;

        if(gpdi_sts != 0) loopback_12_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_2, ON); //Turn on GPIO_CS1

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = c28_values.GPDI5;

            if(gpdi_sts != 1) loopback_12_result = 1;// Error
            else
            {

                loopback_12_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTS_BASE, GPIO_PIN_2, OFF); //Turn off GPIO_CS1
        }
        break;

    // GPDO4 -> GPDI4_ISO
    case LOOPBACK_13:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, OFF); //Turn off GPDO4

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_5); //GPDI4_ISO
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_13_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, ON); //Turn on GPDO4

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_5); //GPDI4_ISO
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_13_result = 1;// Error
            else
            {
                loopback_13_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, OFF); //Turn off GPDO4
        }
        break;

    // GPDO3 -> GPDI3_ISO
    case LOOPBACK_14:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_2, OFF); //Turn off GPDO3

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_4); //GPDI3_ISO
        gpdi_sts = gpdi_sts >> 4;

        if(gpdi_sts != 0) loopback_14_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_2, ON); //Turn on GPDO3

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_4); //GPDI3_ISO
            gpdi_sts = gpdi_sts >> 4;

            if(gpdi_sts != 1) loopback_14_result = 1;// Error
            else
            {
                loopback_14_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_2, OFF); //Turn off GPDO3
        }
        break;

    // GPDO2 -> GPDI2_ISO
    case LOOPBACK_15:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, OFF); //Turn off GPDO2

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_7); //GPDI2_ISO
        gpdi_sts = gpdi_sts >> 7;

        if(gpdi_sts != 0) loopback_15_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, ON); //Turn on GPDO2

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_7); //GPDI2_ISO
            gpdi_sts = gpdi_sts >> 7;

            if(gpdi_sts != 1) loopback_15_result = 1;// Error
            else
            {
                loopback_10_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, OFF); //Turn off GPDO2
        }
        break;

    // GPDO1 -> GPDI1_ISO
    case LOOPBACK_16:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_3, OFF); //Turn off GPDO1

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_6); //GPDI1_ISO
        gpdi_sts = gpdi_sts >> 6;

        if(gpdi_sts != 0) loopback_16_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_3, ON); //Turn on GPDO1

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTR_BASE, GPIO_PIN_6); //GPDI1_ISO
            gpdi_sts = gpdi_sts >> 6;

            if(gpdi_sts != 1) loopback_16_result = 1;// Error
            else
            {
                loopback_16_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_3, OFF); //Turn off GPDO1
        }
        break;

    // BP_I2C_SDA -> STATUS_ADC0
    case LOOPBACK_17:

        // Write 0 to GPIO
        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, OFF); //Turn off BP_I2C_SDA

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2); //STATUS_ADC0
        gpdi_sts = gpdi_sts >> 2;

        if(gpdi_sts != 0) loopback_17_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, ON); //Turn on BP_I2C_SDA

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_2); //STATUS_ADC0
            gpdi_sts = gpdi_sts >> 2;

            if(gpdi_sts != 1) loopback_17_result = 1;// Error
            else
            {
                loopback_17_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, OFF); //Turn off BP_I2C_SDA
        }
        break;

    // BP_I2C_SCL -> STATUS_ADC1
    case LOOPBACK_18:

        // Write 0 to GPIO
        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, OFF); //Turn off BP_I2C_SCL

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTS_BASE, GPIO_PIN_5); //STATUS_ADC1
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_18_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, ON); //Turn on BP_I2C_SCL

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTS_BASE, GPIO_PIN_5); //STATUS_ADC1
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_18_result = 1;// Error
            else
            {

                loopback_18_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, OFF); //Turn off BP_I2C_SCL
        }
        break;

    // BP_I2C_SDA_ISO -> STATUS_ADC2
    case LOOPBACK_19:

        // Write 0 to GPIO
        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, OFF); //Turn off BP_I2C_SDA_ISO

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_5); //STATUS_ADC2
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_19_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, ON); //Turn on BP_I2C_SDA_ISO

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTG_BASE, GPIO_PIN_5); //STATUS_ADC2
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_19_result = 1;// Error
            else
            {
                loopback_19_result = 0;
            }
            // Write 0 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, OFF); //Turn off BP_I2C_SDA_ISO
        }
        break;

    // BP_I2C_SCL_ISO -> STATUS_ADC3
    case LOOPBACK_20:

        // Write 0 to GPIO
        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, OFF); //Turn off BP_I2C_SCL_ISO

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTS_BASE, GPIO_PIN_7); //STATUS_ADC3
        gpdi_sts = gpdi_sts >> 7;

        if(gpdi_sts != 0) loopback_20_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, ON); //Turn on BP_I2C_SCL_ISO

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTS_BASE, GPIO_PIN_7); //STATUS_ADC3
            gpdi_sts = gpdi_sts >> 7;

            if(gpdi_sts != 1) loopback_20_result = 1;// Error
            else
            {
                loopback_20_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, OFF); //Turn off BP_I2C_SCL_ISO
        }
        break;

    // MCBSP_MDX -> MCBSP_MDR
    case LOOPBACK_21:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, OFF); //Turn off MCBSP_MDX

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_5); //MCBSP_MDR
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_21_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, ON); //Turn on MCBSP_MDX

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_5); //MCBSP_MDR
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_21_result = 1;// Error
            else
            {
                loopback_21_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_4, OFF); //Turn off MCBSP_MDX
        }
        break;

    // MCBSP_MCLKX -> MCBSP_MFSX
    case LOOPBACK_22:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, OFF); //Turn off MCBSP_MCLKX

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7); //MCBSP_MFSX
        gpdi_sts = gpdi_sts >> 7;

        if(gpdi_sts != 0) loopback_22_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, ON); //Turn on MCBSP_MCLKX

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTD_BASE, GPIO_PIN_7); //MCBSP_MFSX
            gpdi_sts = gpdi_sts >> 7;

            if(gpdi_sts != 1) loopback_22_result = 1;// Error
            else
            {

                loopback_22_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, OFF); //Turn off MCBSP_MCLKX
        }
        break;

    // EPWMSYNCO -> EPWMSYNCI
    case LOOPBACK_23:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, OFF); //Turn off EPWMSYNCO

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_6); //EPWMSYNCI
        gpdi_sts = gpdi_sts >> 6;

        if(gpdi_sts != 0) loopback_23_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ON); //Turn on EPWMSYNCO

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_6); //EPWMSYNCI
            gpdi_sts = gpdi_sts >> 6;

            if(gpdi_sts != 1) loopback_23_result = 1;// Error
            else
            {
                loopback_23_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, OFF); //Turn off EPWMSYNCO
        }
        break;

    // CAN_TX -> CAN_RX
    case LOOPBACK_24:

        // Write 0 to GPIO
        GPIOPinWrite(CAN_BASE, CAN_TX_PIN, OFF); //Turn off CAN_TX

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(CAN_BASE, CAN_RX_PIN); //CAN_RX
        gpdi_sts = gpdi_sts >> 6;

        if(gpdi_sts != 0) loopback_24_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(CAN_BASE, CAN_TX_PIN, ON); //Turn on CAN_TX

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(CAN_BASE, CAN_RX_PIN); //CAN_RX
            gpdi_sts = gpdi_sts >> 6;

            if(gpdi_sts != 1) loopback_24_result = 1;// Error
            else
            {
                loopback_24_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(CAN_BASE, CAN_TX_PIN, OFF); //Turn off CAN_TX
        }
        break;

    // HRADC_RESET -> INT_GENERAL
    case LOOPBACK_25:

        // Write 0 to GPIO
        HradcRstCtrl(0); //Turn off HRADC_RESET

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_7); //INT_GENERAL
        gpdi_sts = gpdi_sts >> 7;

        if(gpdi_sts != 0) loopback_25_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            HradcRstCtrl(1); //Turn on HRADC_RESET

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTH_BASE, GPIO_PIN_7); //INT_GENERAL
            gpdi_sts = gpdi_sts >> 7;

            if(gpdi_sts != 1) loopback_25_result = 1;// Error
            else
            {
                loopback_25_result = 0;
            }

            // Write 0 to GPIO
            HradcRstCtrl(0); //Turn off HRADC_RESET
        }
        break;

    // UART_M3_RD -> INT_ARM
    case LOOPBACK_26:

        // Write 0 to GPIO
        GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, OFF); //Turn off UART_M3_RD

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(INT_ARM_BASE, INT_ARM_PIN); //INT_ARM
        gpdi_sts = gpdi_sts >> 4;

        if(gpdi_sts != 0) loopback_26_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, ON); //Turn on UART_M3_RD

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(INT_ARM_BASE, INT_ARM_PIN); //INT_ARM
            gpdi_sts = gpdi_sts >> 4;

            if(gpdi_sts != 1) loopback_26_result = 1;// Error
            else
            {

                loopback_26_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(RS485_BKP_RD_BASE, RS485_BKP_RD_PIN, OFF); //Turn off UART_M3_RD
        }
        break;

    // SCI_RD -> INT_C28
    case LOOPBACK_27:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_5, OFF); //Turn off SCI_RD

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_5); //INT_C28
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_27_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_5, ON); //Turn on SCI_RD

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_5); //INT_C28
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_27_result = 1;// Error
            else
            {

                loopback_27_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_5, OFF); //Turn off SCI_RD
        }
        break;

    // DISPLAY_TX -> DISPLAY_RX
    case LOOPBACK_28:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, OFF); //Turn off DISPLAY_TX

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_5); //DISPLAY_RX
        gpdi_sts = gpdi_sts >> 5;

        if(gpdi_sts != 0) loopback_28_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, ON); //Turn on DISPLAY_TX

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTN_BASE, GPIO_PIN_5); //DISPLAY_RX
            gpdi_sts = gpdi_sts >> 5;

            if(gpdi_sts != 1) loopback_28_result = 1;// Error
            else
            {
                loopback_28_result = 0;
            }
            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_4, OFF); //Turn off DISPLAY_TX
        }
        break;

    // 3V3 -> SPI_STE_C28
    case LOOPBACK_29:

        // Read GPDI status
        gpdi_sts = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_3); //SPI_STE_C28
        gpdi_sts = gpdi_sts >> 3;

        if(gpdi_sts != 1) loopback_29_result = 1;// Error
        else
        {
            loopback_29_result = 0;
        }
        break;

    // SPI_CLK_C28 -> FOFB_EPWMSYNCI
    case LOOPBACK_30:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_2, OFF); //Turn off SPI_CLK_C28

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_6); //FOFB_EPWMSYNCI/EPWMSYNCI
        gpdi_sts = gpdi_sts >> 6;

        if(gpdi_sts != 0) loopback_30_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_2, ON); //Turn on SPI_CLK_C28

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_6); //FOFB_EPWMSYNCI/EPWMSYNCI
            gpdi_sts = gpdi_sts >> 6;

            if(gpdi_sts != 1) loopback_30_result = 1;// Error
            else
            {
                loopback_30_result = 0;
            }
            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_2, OFF); //Turn off SPI_CLK_C28
        }
        break;

    // SPI_MISO_C28 -> SPI_MOSI_C28
    case LOOPBACK_31:

        // Write 0 to GPIO
        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, OFF); //Turn off SPI_MISO_C28

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        // Read GPIO Input pin status, it needs to be 0. If different, return error
        gpdi_sts = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0); //SPI_MOSI_C28

        if(gpdi_sts != 0) loopback_31_result = 1; // Error
        else
        {
            // Write 1 to GPIO
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, ON); //Turn on SPI_MISO_C28

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            // Read GPDI status
            gpdi_sts = GPIOPinRead(GPIO_PORTK_BASE, GPIO_PIN_0); //SPI_MOSI_C28

            if(gpdi_sts != 1) loopback_31_result = 1;// Error
            else
            {
                loopback_31_result = 0;
            }

            // Write 0 to GPIO
            GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, OFF); //Turn off SPI_MISO_C28
        }
        break;

    // BP_I2C_SDA -> HRADC_INT_STS
    // BP_I2C_SCL -> HRADC_INT_STS
    // BP_I2C_SDA -> HRADC_INT_STS
    // BP_I2C_SCL -> HRADC_INT_STS
    case LOOPBACK_32:

        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, OFF); //Turn off
        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, OFF); //Turn off

        for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

        gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_7); //HRADC_INT_STS
        gpdi_sts = gpdi_sts >> 7;

        // If 0 than continue the test
        if(gpdi_sts == 0)
        {
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, ON); //Turn on
            GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, ON); //Turn on

            for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

            gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_7); //HRADC_INT_STS
            gpdi_sts = gpdi_sts >> 7;

            // If 1 than continue the test
            if(gpdi_sts == 1)
            {
                GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, OFF); //Turn off

                for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_7); //HRADC_INT_STS
                gpdi_sts = gpdi_sts >> 7;

                // If 0 than continue the test
                if(gpdi_sts == 0)
                {
                    GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, ON); //Turn on
                    GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, OFF); //Turn off

                    for (ulLoop=0;ulLoop<100000;ulLoop++){}; // wait 5ms

                    gpdi_sts = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_7); //HRADC_INT_STS
                    gpdi_sts = gpdi_sts >> 7;

                    // If 0 than continue the test
                    if(gpdi_sts == 0)
                    {
                        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, ON); //Turn off
                        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, ON); //Turn off
                        loopback_32_result = 0; // Successful test
                    }
                    else loopback_32_result = 1; // Error

                }
                else loopback_32_result = 1;

            }
            else loopback_32_result = 1; // Error

        }
        else loopback_32_result = 1; // Error

        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SDA_PIN, ON); //Turn off
        GPIOPinWrite(I2C_OFFBOARD_ISO_BASE, I2C_OFFBOARD_ISO_SCL_PIN, ON); //Turn off

        break;
    }

}

//***********************************************************************************

uint8_t i2c_bkp_result[2];
volatile uint8_t write_val;

uint8_t TestI2cBkpSignal(uint8_t function_type, uint8_t val)
{
    uint8_t result = 0;

    if (function_type == 0)
    {
        write_val = val;
        TaskSetNew(BKP_I2C_TEST);
        result =  0;
    }
    else if (function_type == 1)
    {
        result = write_val;
    }
    return result;
}

void TestI2cBkpRoutine()
{
    //i2c_bkp_result[0] = 0x02;
    i2c_bkp_result[0] = 0x00;
    i2c_bkp_result[1] = write_val;

    teste++;
    WriteI2COffboardIsolated(0b0101000, 2, i2c_bkp_result);
}
