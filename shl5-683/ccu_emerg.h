/*
 * Copyright (c) 2012��MIT
 * All right reserved.
 *
 * �ļ����ƣ� ccu_emerg.h
 * �ļ���ʶ��
 * ժ    Ҫ�� �����㲥����
 *
 * ��ǰ�汾�� V1.0.0
 * ��    �ߣ� wilson
 * ������ڣ�2013-11-18
 *
 * ȡ���汾��
 * ԭ����  ��
 * ������ڣ�
 */
#ifndef CCU_EMERG_H_
#define CCU_EMERG_H_

#include "data_struct.h"
#include "ccu_function.h"

extern emerg_table_items_t *p_ccu_emerg_info;
extern ccu_emerg_info_t ccu_emerg_info;

/** CreateCCUEmergSpace function description
 *  ����CCU�йؽ����㲥�ռ�
 *
 *  @param   �����㲥��Ŀ��
 *  @return  null
 */
void CreateCCUEmergSpace(int param_emerg_total_items);

/** InitCCUEmergInfo function description
 *  ��ʼ�������㲥��Ϣ
 *
 *  @param   null
 *  @return  null
 */
void InitCCUEmergInfo();

/** GetEmergDisplayMessage function description
 *  ��ȡ�����㲥��Ϣ
 *
 *  @param   �����㲥���к�
 *  @param   �����㲥����
 *  @return  �����㲥LED��ʾ�ļ�
 */
char *GetEmergDisplayMessage(unsigned char param_emerg_sequence,unsigned short *len);

#endif /* CCU_EMERG_H_ */
