/*
 * rtc.h
 *
 *  Created on: 06/07/2015
 *      Author: joao.rosa
 */

#include <stdint.h>

#ifndef RTC_H_
#define RTC_H_

extern void RTCInit(void);
extern void RTCReadDataHour(void);
extern void RTCWriteDataHour(uint8_t seconds, uint8_t minutes, uint8_t hours, uint8_t dayweek, uint8_t day, uint8_t month, uint8_t year);
extern uint8_t RTCBatteryCheck(void);

extern uint64_t DataHourRead(void);

extern void RTCSramWrite(uint8_t byte_addr, uint8_t data_value);
extern uint8_t RTCSramRead(uint8_t byte_addr);
extern void RTCAccessTest(void);
extern uint8_t RTCAccessStatus(void);

#endif /* RTC_H_ */
