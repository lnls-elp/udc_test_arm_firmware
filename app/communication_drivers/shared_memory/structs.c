/*
 * structs.c
 *
 *  Created on: 23/07/2015
 *      Author: joao.rosa
 */

#include "structs.h"

//shm_m2c_param_ctrl_t shm_m2c_param_ctrl;
//shm_c2m_output_var_t shm_c2m_output_var;

tDP_Framework DP_Framework_MtoC;

c28_values_t c28_values;

arm_controls_t arm_controls;


//#pragma DATA_SECTION(DP_Framework_MtoC,"SHARERAMS0");
#pragma DATA_SECTION(arm_controls,"SHARERAMS0");

//#pragma DATA_SECTION(shm_c2m_output_var,"SHARERAMS2");
#pragma DATA_SECTION(c28_values,"SHARERAMS2");



tDP_Framework DP_Framework;
//#pragma DATA_SECTION(DP_Framework,"SHARERAMS1");
