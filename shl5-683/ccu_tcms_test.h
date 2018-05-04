/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_tcms_test.h
 * 文件标识：
 * 摘    要： TCMS相关功能测试函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-04-16
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef CCU_TCMS_TEST_H_
#define CCU_TCMS_TEST_H_

#include "ccu_tms.h"

/** TestTCMSEmerg function description
 *  测试TCMS紧急广播功能
 *
 *  @param   null
 *  @return  null
 */
void TestTCMSEmerg();
void TestTCMSEmergOk(unsigned char emerg_sequence,unsigned char emerg_times);

/** TestDDU function description
 *  测试TCMS的DDU广播功能
 *
 *  @param   null
 *  @return  null
 */
void TestDDU();

/** TestLineInfo function description
 *  测试TCMS的线路号信息功能
 *
 *  @param   null
 *  @return  null
 */
void TestLineInfo();

/** ShowTCMSInfo function description
 *  显示接收到的TCMS信息
 *
 *  @param   接收到的TCMS信号包地址
 *  @return  null
 */
void ShowTCMSInfo(unsigned char recv_tcms_info[]);

/** SetPISInfoToTCMS function description
 *  设置发送给TCMS的信息
 *
 *  @param   发送给TCMS信息包地址
 *  @return  null
 */
void SetPISInfoToTCMS(char pc_info[]);


#endif /* CCU_TCMS_TEST_H_ */
