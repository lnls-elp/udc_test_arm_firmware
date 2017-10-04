/*
 * bsmp_jiga.h
 *
 *  Created on: 12/05/2017
 *      Author: ali.faraj
 */

#ifndef BSMP_JIGA_H_
#define BSMP_JIGA_H_

#include "bsmp/include/server.h"

extern void BSMPprocess(struct bsmp_raw_packet *recv_packet, struct bsmp_raw_packet *send_packet);

extern void BSMPInit(void);


#endif /* BSMP_JIGA_H_ */
