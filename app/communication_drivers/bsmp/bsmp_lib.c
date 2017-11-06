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
//                  Uart Ping BSMP Function
//*****************************************************************************
// OK
uint8_t TestBkpI2C (uint8_t *input, uint8_t *output)
{
    *output = TestI2cBkpSignal(input[0], input[1]);
    return *output;
}

static struct bsmp_func testbkpi2c_func = {
    .func_p           = TestBkpI2C,
    .info.input_size  = 2,       // Nothing is read from the input parameter
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
    bsmp_register_function(&bsmp, &testbkpi2c_func);                    // Function ID 13


    //*****************************************************************************
}

void
BSMPprocess(struct bsmp_raw_packet *recv_packet, struct bsmp_raw_packet *send_packet)
{
    bsmp_process_packet(&bsmp, recv_packet, send_packet);

}



