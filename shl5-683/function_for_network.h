/*
 * Copyright (c) 2012��MIT
 * All right reserved.
 *
 * �ļ����ƣ� function_for_network.h
 * �ļ���ʶ��
 * ժ    Ҫ�� �������̹߳��ú���
 *
 * ��ǰ�汾�� V1.0.0
 * ��    �ߣ� wilson
 * ������ڣ�2014-5-21
 *
 * ȡ���汾��
 * ԭ����  ��
 * ������ڣ�
 */

#ifndef FUNCTION_FOR_NETWORK_H_
#define FUNCTION_FOR_NETWORK_H_

#include "ccu_function.h"

#define CCU_DEV

extern unsigned char fdu_failure_flag;
extern unsigned char check_dev_state;

/** GetCurrentTCMSTime function description
*  ��ȡ��ǰTCMSʱ��
*
*  @param   null
*  @return  ��ǰtcmsʱ��
*/
current_time_t GetCurrentTCMSTime();

/** CCUGetAllPerDevFailureInfo function description
*  CCU��ȡ������Χ�豸������Ϣ
*
*  @param   null
*  @return  null
*/
void CCUGetAllPerDevFailureInfo();

/** InitializeHeartBeatPackage function description
*  ��ʼ����Χ�豸������
*
*  @param   null
*  @return  null
*/
void InitializeHeartBeatPackage();

/** SetFailureTimeFlag function description
*  ���÷�����Χ�豸��������־
*
*  @param   null
*  @return  null
*/
void SetFailureTimeFlag();

/** SendHeartBetaToPeripheral function description
 *  send heart beta to peripheral
 *
 *  @param   null
 *  @return  null
 */
void SendHeartBetaToPeripheral();

/** SendHeartBetaToFDU function description
*  ���÷���FDU������
*
*  @param   null
*  @return  null
*/
void SendHeartBetaToFDU();


#endif /* FUNCTION_FOR_NETWORK_H_ */
