/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_emerg.h
 * 文件标识：
 * 摘    要： 紧急广播操作
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-11-18
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef CCU_EMERG_H_
#define CCU_EMERG_H_

#include "data_struct.h"
#include "ccu_function.h"

extern emerg_table_items_t *p_ccu_emerg_info;
extern ccu_emerg_info_t ccu_emerg_info;

/** CreateCCUEmergSpace function description
 *  创建CCU有关紧急广播空间
 *
 *  @param   紧急广播条目数
 *  @return  null
 */
void CreateCCUEmergSpace(int param_emerg_total_items);

/** InitCCUEmergInfo function description
 *  初始化紧急广播信息
 *
 *  @param   null
 *  @return  null
 */
void InitCCUEmergInfo();

/** GetEmergDisplayMessage function description
 *  获取紧急广播信息
 *
 *  @param   紧急广播序列号
 *  @param   紧急广播长度
 *  @return  紧急广播LED显示文件
 */
char *GetEmergDisplayMessage(unsigned char param_emerg_sequence,unsigned short *len);

#endif /* CCU_EMERG_H_ */
