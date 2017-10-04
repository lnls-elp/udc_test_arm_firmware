/*
 * usb_to_serial.h
 *
 *  Created on: 23/01/2017
 *      Author: joao.rosa
 */

#include <stdint.h>

#ifndef USB_TO_SERIAL_USB_TO_SERIAL_H_
#define USB_TO_SERIAL_USB_TO_SERIAL_H_

extern void InitUsb2Serial(void);

extern void Usb2SerialProcessData(void);

extern uint8_t ReadUsb2SerialAddress(void);

extern void SetUsb2SerialAddress(uint8_t addr);


#endif /* USB_TO_SERIAL_USB_TO_SERIAL_H_ */
