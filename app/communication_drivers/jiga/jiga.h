/*
 * jiga.h
 *
 *  Created on: 08/05/2017
 *      Author: ali.faraj
 */

#ifndef JIGA_H_
#define JIGA_H_

extern uint8_t TestIoExp(uint8_t function_type);

extern uint8_t TestLedTurnOn(void);
extern uint8_t TestLedTurnOff(void);

extern uint8_t TestBuzzerTurnOn(void);
extern uint8_t TestBuzzerTurnOff(void);

extern uint8_t TestEepromWriteSerialNumber(uint8_t function_type, uint8_t serial_number0, uint8_t serial_number1, uint8_t serial_number2, uint8_t serial_number3, uint8_t serial_number4, uint8_t serial_number5, uint8_t serial_number6, uint8_t serial_number7, uint8_t serial_number8, uint8_t serial_number9);

extern uint8_t TestTempSense(uint8_t function_type);
extern uint8_t TestTempSenseRoutine(void);

extern uint8_t TestFlashMem(uint8_t function_type);
extern uint8_t TestFlashRoutine(void);

extern uint8_t TestRamMem(uint8_t function_type);
extern uint8_t TestRamRoutine(void);

extern uint8_t TestRtcMod(uint8_t function_type);

extern uint8_t IsoPowerSupplyLoopBackTest(void);
extern uint8_t TestIsoPowerSup(uint8_t function_type);

extern uint8_t TestLoopBackSignal(uint8_t function_type, uint8_t channel_number);
extern uint8_t LoopBackFunctionTest(void);

extern uint8_t AdcpFunctionTest(void);
extern uint8_t TestAdcpRange(uint8_t function_type, uint8_t channel_number);

extern uint8_t Rs485Loopbacktest(void);
extern uint8_t TestRs485Loop(uint8_t function_type);

extern uint8_t TestI2cBkpSignal(uint8_t function_type, uint8_t register_address);
extern void TestI2cBkpRoutine();

/*
void jiga_main();

extern void random_string(char *s, int len);

extern char checksum(char* s);

extern void UARTSendString();

void EepromWrite(uint16_t address, uint8_t data);

void EepromDummyWrite(uint16_t address, uint8_t data);

uint8_t EepromRead(uint16_t address);

*/


struct serial_buffer
{
    uint8_t data[20];
    uint16_t index;
    uint8_t csum;
};

#endif /* JIGA_H_ */
