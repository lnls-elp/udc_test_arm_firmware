/*
 * rs485.h
 *
 *  Created on: 29/05/2015
 *      Author: joao.rosa
 */

#include <stdint.h>
#include <stdarg.h>
#include <string.h>

#ifndef RS485_H_
#define RS485_H_

extern void InitRS485(void);
extern struct serial_buffer ReadBufferRS485(void);
extern void ResetBufferRS485(void);

#endif /* RS485_H_ */
