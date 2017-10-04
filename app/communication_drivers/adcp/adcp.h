/*
 * adcp.h
 *
 *  Created on: 14/07/2015
 *      Author: joao.rosa
 */

#ifndef ADCP_H_
#define ADCP_H_

#include <stdint.h>
#include <stdarg.h>


extern void AdcpInit(void);

extern void AdcpRead(void);

extern float AdcpReadChannel(uint8_t sel_channel);

//extern void ReadAdcP(adcpvar_t *ReadAd);
//extern void ClearAdcFilter(void);

#endif /* ADCP_H_ */
