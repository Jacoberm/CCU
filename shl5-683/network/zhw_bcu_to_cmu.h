/*
 * zhw_bcu_to_cmu.h
 *
 *  Created on: 2014-4-4
 *      Author: benty
 */

#ifndef ZHW_BCU_TO_CMU_H_
#define ZHW_BCU_TO_CMU_H_
#include "data_struct.h"
#include "zhw_dev_config_info.h"

extern cyg_flag_t bcu_cmd_to_cmu;

#if 1
#define cmu_debug_printf(s) //diag_printf s
//#define cmu_debug_printf(s)  ///< printf s       ///<debug for wilson,on the platform of Beta1.0
#endif

void ZhwSendCmdInfo(int ,int );
void ZhwBcuToCmuEntry(int );

#endif /* ZHW_BCU_TO_CMU_H_ */
