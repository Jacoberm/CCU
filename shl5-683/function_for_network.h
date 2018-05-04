/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： function_for_network.h
 * 文件标识：
 * 摘    要： 与网络线程共用函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-5-21
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef FUNCTION_FOR_NETWORK_H_
#define FUNCTION_FOR_NETWORK_H_

#include "ccu_function.h"

#define CCU_DEV

extern unsigned char fdu_failure_flag;
extern unsigned char check_dev_state;

/** GetCurrentTCMSTime function description
*  获取当前TCMS时间
*
*  @param   null
*  @return  当前tcms时间
*/
current_time_t GetCurrentTCMSTime();

/** CCUGetAllPerDevFailureInfo function description
*  CCU获取所有外围设备故障信息
*
*  @param   null
*  @return  null
*/
void CCUGetAllPerDevFailureInfo();

/** InitializeHeartBeatPackage function description
*  初始化外围设备心跳包
*
*  @param   null
*  @return  null
*/
void InitializeHeartBeatPackage();

/** SetFailureTimeFlag function description
*  设置发送外围设备心跳包标志
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
*  设置发送FDU心跳包
*
*  @param   null
*  @return  null
*/
void SendHeartBetaToFDU();


#endif /* FUNCTION_FOR_NETWORK_H_ */
