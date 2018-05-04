/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：zhw_network_config.h
 * 文件标识：
 * 摘    要：网络配置文件头文件
 *
 * 当前版本：V1.0.0
 * 作    者：周魏
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */

#ifndef ZHW_MIDDLE_FORWARD_H_
#define ZHW_MIDDLE_FORWARD_H_

#include "zhw_network_config.h"
#include "zhw_dev_config_info.h"
#include "zhw_dev_selftest_module.h"

int ZhwNetworkRecvForward(int recv_buffer_id,int buffer_type);
int ZhwNetworkSendForward(int send_buffer_id,int send_big_buffer_type);
#endif /* ZHW_MIDDLE_FORWARD_H_ */
