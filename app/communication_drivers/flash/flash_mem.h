/*
 * flash.h
 *
 *  Created on: 14/07/2015
 *      Author: joao.rosa
 */

#ifndef FLASH_MEM_H_
#define FLASH_MEM_H_

#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>


//**********************************************************************************************************************************
//        Comandos para o gerenciamento de mem�ria
//**********************************************************************************************************************************
#define READ_BLOCK			0
#define WRITE_BLOCK			1
#define READ_BYTE			2
#define READ_ID				3
#define EREASE_BLOCK		4

#define INIT_DATAFLASH		255
//**********************************************************************************************************************************

 typedef struct
 {
	 uint8_t ucDataRx[256]; // Buffer de recp��o para ser gravado da FLASH
	 uint8_t ucDataTx[256]; // Buffer de transmiss�o para ler os dados da FLASH
	 uint8_t PageVector; // Posicionamento da mem�ria
	 uint8_t Sector; // Posicionamento da mem�ria
	 bool NewDataRx; // Flag de indica��o de novo dado no buffer Rx
	 bool NewDataTx; // Flag de indica��o de novo dado no buffer Tx
	 bool Cancel; // Flag de indica��o para cancelamento da rotina
	 bool WriteProcess; // Flag de indica��o para cancelamento da rotina
	 bool ReadProcess; // Flag de indica��o para cancelamento da rotina
	 bool BlockReadFull; // Flag que indica que o Bloco foi escrito por completo
	 bool BlockWriteFull; // Flag que indica que o Bloco foi escrito por completo
	 uint8_t Block; // Posicionamento da mem�ria
 }dataflash_t;

 extern dataflash_t DataFlash;

 //extern void ManagFlashMemory(uint8_t Command);

extern void FlashMemInit(void);

extern uint64_t FlashDeviceIDRead(void);

extern void FlashMemReadSerialNumber(void);

#endif /* _FLASH_MEM_H_ */
