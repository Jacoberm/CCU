/*
 * Copyright (c) 2012��MIT
 * All right reserved.
 *
 * �ļ����ƣ� ccu_tcms_test.h
 * �ļ���ʶ��
 * ժ    Ҫ�� TCMS��ع��ܲ��Ժ���
 *
 * ��ǰ�汾�� V1.0.0
 * ��    �ߣ� wilson
 * ������ڣ�2014-04-16
 *
 * ȡ���汾��
 * ԭ����  ��
 * ������ڣ�
 */

#ifndef CCU_TCMS_TEST_H_
#define CCU_TCMS_TEST_H_

#include "ccu_tms.h"

/** TestTCMSEmerg function description
 *  ����TCMS�����㲥����
 *
 *  @param   null
 *  @return  null
 */
void TestTCMSEmerg();
void TestTCMSEmergOk(unsigned char emerg_sequence,unsigned char emerg_times);

/** TestDDU function description
 *  ����TCMS��DDU�㲥����
 *
 *  @param   null
 *  @return  null
 */
void TestDDU();

/** TestLineInfo function description
 *  ����TCMS����·����Ϣ����
 *
 *  @param   null
 *  @return  null
 */
void TestLineInfo();

/** ShowTCMSInfo function description
 *  ��ʾ���յ���TCMS��Ϣ
 *
 *  @param   ���յ���TCMS�źŰ���ַ
 *  @return  null
 */
void ShowTCMSInfo(unsigned char recv_tcms_info[]);

/** SetPISInfoToTCMS function description
 *  ���÷��͸�TCMS����Ϣ
 *
 *  @param   ���͸�TCMS��Ϣ����ַ
 *  @return  null
 */
void SetPISInfoToTCMS(char pc_info[]);


#endif /* CCU_TCMS_TEST_H_ */
