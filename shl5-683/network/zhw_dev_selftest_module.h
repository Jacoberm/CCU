/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_dev_selftest_module.h
 * 文件标识：
 * 摘    要： 设备自检模块主函数头文件
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2013-03-26
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */
#ifndef ZHW_DEV_SELFTEST_MODULE_H_
#define ZHW_DEV_SELFTEST_MODULE_H_
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h> // sleep()
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <cyg/kernel/kapi.h>
#define HEART_PRIMARY_SERVER

#include "zhw_network_config.h"
#include "zhw_dev_config_info.h"
#include "zhw_buffer_init.h"
#include "zhw_middle_forward.h"
#include "zhw_socket_init.h"
//#include "common_type.h"
#include "data_struct.h"
#include "zhw_apply_common_func.h"
#ifdef PCU
#include "pcu_main.h"
#endif

#ifdef BCU
#include "bcu_main.h"
#endif

#ifdef CCU
#include "ccu_main.h"
#endif

#ifdef EAMP
#include "eamp_main.h"
#endif

#define DEV_NAME_LONG 20

#define HEART_TIME 50
//#define SEND_TO_BCU_TIME HEART_TIME*3

#define EAMP_NAME "EAMP"
#define BCU_NAME "BCU"
#define CCU_NAME "CCU"
#define PCU_NAME "PCU"
#define CMU_NAME "CMU"

#define OTHER_HEART_TYPE 2
#define FAIL_TIME_HEART_TYPE 3
//#define PCU_HEART_TYPE 4
#define SEND_TO_BCU_TYPE 26
#define MAX_TYPE SEND_TO_BCU_TYPE

#define SEND_TO_BCU 2

//add by zhw-2014-11-24 ,start to set online status
#ifndef DEV_STATUS_SET
#define DEV_STATUS_SET 2
#endif


//#ifndef UPTEST
//#define UPTEST
//#endif

//设备自检信息结构--start 5.23---zhw
//extern int heart_beat_other_ccu_no;
extern dev_status_info_t dev_status_info;
extern g_dev_fault_info_t g_dev_fault_info;
extern current_time_t g_time_info;

extern void zhw_ping_test(char *local_ip,char *server_ip);

int ZhwSetClientNS(int);
int ZhwGetClientNS(void); ///<0:off line  1:on line

int ZhwGetPrimaryServerNS(void);
int ZhwSetPrimaryServerNS(int);

void ZhwAlarmFunction(cyg_handle_t alarm,cyg_addrword_t data);

void ZhwAlarmInit(void);

void ZhwServiceSendBufferInit(char *dst_dev_name,int dst_dev_number,char *src_dev_name,int src_dev_number,
		int pkg_type,int heart_pkg_type,int heart_pkg_number);

int ZhwDevStatusPrint(char *dev_name,void *dev_list_head);

void * ZhwDevStatusBufferInit(char *dev_name,int dev_number);

int ZhwDevStatusUpdate(network_heart_package_t heart_recv_buffer,dev_status_info_t dev_status_info);

void ZhwHeartPrimaryServer(int recv_buffer,int send_buffer,struct global_info dev_info);

void ZhwHeartBackupServer(int recv_buffer,int send_buffer,struct global_info dev_info);

#ifdef UPTEST
void ZhwLocalDevInfoInit(char *dev_name,int dev_status);
#endif

void ZhwSendDevStatusToBCUOrCCU(int dev_fault_send_buffer_id,struct global_info dev_info,char *dev_name);

void ZhwHeartClient(int recv_buffer,int send_buffer,struct global_info dev_info);

//int ZhwHeartBufferInit();

void ZhwHeartModule(struct global_info dev_info);

int ZhwDevTestModule(udp_heart_socket_buffer_t udp_heart_buffer);

#endif /* ZHW_DEV_SELFTEST_MODULE_H_ */
