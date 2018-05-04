/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： media_and_monitor.h
 * 文件标识：
 * 摘    要： 主函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-09-05
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef MEDIA_AND_MONITOR_H_
#define MEDIA_AND_MONITOR_H_


#include "ccu_led.h"
#include "ccu_function.h"
#include "manual_ann.h"

extern int whether_send_serial_fdu_data;
extern unsigned char whether_show_line4_cycle_terminal_station;
extern unsigned char media_failure_flag;
extern unsigned char monitor_failure_flag;

//add by zhw 2015-12-23
//typedef struct {
//	unsigned char
//}pecu_status_t;

typedef struct {
	unsigned char time_of_year;
	unsigned char time_of_month;
	unsigned char time_of_day;
	unsigned char time_of_hour;
	unsigned char time_of_minute;
	unsigned char time_of_second;
	unsigned char car_number_low;
	unsigned char car_number_high;
	struct {
		unsigned char local_PA:1;
		unsigned char TC1_pcu1_ask:1;
		unsigned char TC1_pcu2_ask:1;
		unsigned char MP1_pcu1_ask:1;
		unsigned char MP1_pcu2_ask:1;
		unsigned char M1_pcu1_ask:1;
		unsigned char M1_pcu2_ask:1;
		unsigned char M2_pcu1_ask:1;

		unsigned char M2_pcu2_ask:1;
		unsigned char MP2_pcu1_ask:1;
		unsigned char MP2_pcu2_ask:1;
		unsigned char TC2_pcu1_ask:1;
		unsigned char TC2_pcu2_ask:1;
		unsigned char TC1_pcu1_calling:1;
		unsigned char TC1_pcu2_calling:1;
		unsigned char MP1_pcu1_calling:1;

		unsigned char MP1_pcu2_calling:1;
		unsigned char M1_pcu1_calling:1;
		unsigned char M1_pcu2_calling:1;
		unsigned char M2_pcu1_calling:1;
		unsigned char M2_pcu2_calling:1;
		unsigned char MP2_pcu1_calling:1;
		unsigned char MP2_pcu2_calling:1;
		unsigned char TC2_pcu1_calling:1;

		unsigned char TC2_pcu2_calling:1;
		unsigned char persist:7;
	}pecu_status;

	struct {
		unsigned char persist:1;
		unsigned char TC1_active:1;
		unsigned char TC2_active:1;
		unsigned char door_close:1;//1:all close;0:at lease one door is not close.
		unsigned char open_a_door:1;
		unsigned char open_b_door:1;
		unsigned char ATC_active:1;
		unsigned char TC1_1_door_open:1;
	}car_status;

	struct {
		unsigned char TC1_2_door_open:1;
		unsigned char TC1_3_door_open:1;
		unsigned char TC1_4_door_open:1;
		unsigned char TC1_5_door_open:1;
		unsigned char TC1_6_door_open:1;
		unsigned char TC1_7_door_open:1;
		unsigned char TC1_8_door_open:1;
		unsigned char TC1_9_door_open:1;

		unsigned char TC1_10_door_open:1;
		unsigned char MP1_1_door_open:1;
		unsigned char MP1_2_door_open:1;
		unsigned char MP1_3_door_open:1;
		unsigned char MP1_4_door_open:1;
		unsigned char MP1_5_door_open:1;
		unsigned char MP1_6_door_open:1;
		unsigned char MP1_7_door_open:1;

		unsigned char MP1_8_door_open:1;
		unsigned char MP1_9_door_open:1;
		unsigned char MP1_10_door_open:1;
		unsigned char M1_1_door_open:1;
		unsigned char M1_2_door_open:1;
		unsigned char M1_3_door_open:1;
		unsigned char M1_4_door_open:1;
		unsigned char M1_5_door_open:1;

		unsigned char M1_6_door_open:1;
		unsigned char M1_7_door_open:1;
		unsigned char M1_8_door_open:1;
		unsigned char M1_9_door_open:1;
		unsigned char M1_10_door_open:1;
		unsigned char M2_1_door_open:1;
		unsigned char M2_2_door_open:1;
		unsigned char M2_3_door_open:1;

		unsigned char M2_4_door_open:1;
		unsigned char M2_5_door_open:1;
		unsigned char M2_6_door_open:1;
		unsigned char M2_7_door_open:1;
		unsigned char M2_8_door_open:1;
		unsigned char M2_9_door_open:1;
		unsigned char M2_10_door_open:1;
		unsigned char MP2_1_door_open:1;

		unsigned char MP2_2_door_open:1;
		unsigned char MP2_3_door_open:1;
		unsigned char MP2_4_door_open:1;
		unsigned char MP2_5_door_open:1;
		unsigned char MP2_6_door_open:1;
		unsigned char MP2_7_door_open:1;
		unsigned char MP2_8_door_open:1;
		unsigned char MP2_9_door_open:1;

		unsigned char MP2_10_door_open:1;
		unsigned char TC2_1_door_open:1;
		unsigned char TC2_2_door_open:1;
		unsigned char TC2_3_door_open:1;
		unsigned char TC2_4_door_open:1;
		unsigned char TC2_5_door_open:1;
		unsigned char TC2_6_door_open:1;
		unsigned char TC2_7_door_open:1;

		unsigned char TC2_8_door_open:1;
		unsigned char TC2_9_door_open:1;
		unsigned char TC2_10_door_open:1;
		unsigned char TC1_escape_door_open:1;
		unsigned char TC2_escape_door_open:1;
		unsigned char TC1_link_door_open:1;
		unsigned char TC2_link_door_open:1;
		unsigned char smoke_1_ask:1;
	}door_open_status;

	struct {
		unsigned char smoke_2_ask:1;
		unsigned char smoke_3_ask:1;
		unsigned char smoke_4_ask:1;
		unsigned char smoke_5_ask:1;
		unsigned char smoke_6_ask:1;
		unsigned char smoke_7_ask:1;
		unsigned char smoke_8_ask:1;
		unsigned char smoke_9_ask:1;

		unsigned char smoke_10_ask:1;
		unsigned char smoke_11_ask:1;
		unsigned char smoke_12_ask:1;
		unsigned char smoke_13_ask:1;
		unsigned char smoke_14_ask:1;
		unsigned char smoke_15_ask:1;
		unsigned char smoke_16_ask:1;
		unsigned char smoke_17_ask:1;

		unsigned char smoke_18_ask:1;
		unsigned char smoke_19_ask:1;
		unsigned char smoke_20_ask:1;
		unsigned char smoke_21_ask:1;
		unsigned char smoke_22_ask:1;
		unsigned char smoke_23_ask:1;
		unsigned char smoke_24_ask:1;
		unsigned char smoke_25_ask:1;

		unsigned char smoke_26_ask:1;
		unsigned char smoke_27_ask:1;
		unsigned char smoke_28_ask:1;
		unsigned char smoke_29_ask:1;
		unsigned char smoke_30_ask:1;
		unsigned char smoke_31_ask:1;
		unsigned char smoke_32_ask:1;
		unsigned char smoke_33_ask:1;

		unsigned char smoke_34_ask:1;
		unsigned char smoke_35_ask:1;
		unsigned char smoke_36_ask:1;
		unsigned char smoke_37_ask:1;
		unsigned char smoke_38_ask:1;
		unsigned char smoke_39_ask:1;
		unsigned char smoke_40_ask:1;
		unsigned char smoke_41_ask:1;

		unsigned char smoke_42_ask:1;
		unsigned char smoke_43_ask:1;
		unsigned char smoke_44_ask:1;
		unsigned char smoke_45_ask:1;
		unsigned char smoke_46_ask:1;
		unsigned char smoke_47_ask:1;
		unsigned char smoke_48_ask:1;
		unsigned char smoke_49_ask:1;

		unsigned char smoke_50_ask:1;
		unsigned char smoke_51_ask:1;
		unsigned char smoke_52_ask:1;
		unsigned char persist:5;
	}smoke_ask_status;
}data_t;

typedef struct {
	unsigned char message_head;//0xfe
	unsigned char message_type;//0x32
	unsigned char data_long_forth;
	unsigned char data_long_third;
	unsigned char data_long_second;
	unsigned char data_long_first;
	data_t data;
	unsigned char message_end;//0xff
	unsigned char check_sum_high;
	unsigned char check_sum_low;
}message_to_monitor_t;

//end of add

/** FDUCanSendThreadEntry function description
 *	 FDU线程处理函数
 *
 *  @param  arg
 *  @return  null
 */
void *FDUCanSendThreadEntry(void *arg);

/** MediaAndMonitorSerialRecvThreadEntry function description
 *	 媒体与监控发送线程处理函数
 *
 *  @param  arg
 *  @return  null
 */
void *MediaAndMonitorSerialRecvThreadEntry(void *arg);

/** MediaAndMonitorSerialRecvThreadEntry function description
 *	 媒体与监控数据解析线程处理函数
 *
 *  @param  arg
 *  @return  null
 */
void *MediaAndMonitorSerialCMDParseThreadEntry(void *arg);

/** CreateFduShowTimer function description
 *	创建FDU显示定时器
 *
 *  @param  null
 *  @return  null
 */
void CreateFduShowTimer();

/** RestartFduShowTimer function description
 *	重新启动FDU显示定时器
 *
 *  @param  null
 *  @return  null
 */
void RestartFduShowTimer();

/** CreateTestTimer function description
 *	创建临时测试定时器
 *
 *  @param  null
 *  @return  null
 */
void CreateTestTimer();

/** TransmitBoradcastMessageToMedia function description
 *	CCU给媒体发送紧急广播信息
 *
 *  @param  紧急广播状态
 *  @param  紧急广播序列号
 *  @return  null
 */
void TransmitBoradcastMessageToMedia(char emerg_state,char emerg_no);

/** TransmitDevVolumeToMedia function description
 *	CCU给媒体发送音量信息
 *
 *  @param  null
 *  @return  null
 */
void TransmitDevVolumeToMedia();

/** TransmitBoradcastMessageToMonitor function description
 *	CCU给监控发送信息
 *
 *  @param null
 *  @return  null
 */
void TransmitBoradcastMessageToMonitor();

/** ReTransmitBoradcastMessageToMedia function description
 *	重新发送给媒体的数据包
 *
 *  @param   null
 *  @return  null
 */
void ReTransmitBoradcastMessageToMedia();

/** ReTransmitBoradcastMessageToMonitor function description
 *	重新发送给监控的数据包
 *
 *  @param   null
 *  @return  null
 */
void ReTransmitBoradcastMessageToMonitor();

/** ReceiveControlMessageFromMonitor function description
 *	处理监控给CCU发送信息
 *
 *  @param   null
 *  @return  null
 */
void ReceiveControlMessageFromMonitor();

/** SendFDUInfo function description
 *	发送FDU显示数据
 *
 *  @param   是否发送标志
 *  @return  null
 */
void SendFDUInfo(int *p_param_whether_send_serial_fdu_data);


/** UpdatePCUGlobalInfoToTCMSAndMonitor function description
 *	发送PCU状态给TCMS和监控
 *
 *  @param   PCU 设备号
 *  @param   设备状态
 *  @return  null
 *  pcu_state: 0:over  1:request 2:connecting
 */
void UpdatePCUGlobalInfoToTCMSAndMonitor(unsigned char src_dev_no,unsigned char pcu_state);

/** UpdatePCURequestLinkage function description
 *	处理PCU状态联动
 *
 *  @param   从网络上接收到的PCU相关命令
 *  @return  null
 */
void UpdatePCURequestLinkage(network_send_package_t recv_network_info_from_network);

int CreateManualPackageTimer(unsigned int msecond);

void restartManualPackageTimer();

void CloseManualPackageTimer();

void JudgeManualPackage();


#endif /* MEDIA_AND_MONITOR_H_ */
