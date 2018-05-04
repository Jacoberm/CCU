/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_tcms.h
 * 文件标识：
 * 摘    要： TCMS相关功能处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-04-09
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef CCU_TMS_H_
#define CCU_TMS_H_
#include <stdio.h>
#include <stdlib.h>
#include "ccu_led.h"
#include "buffer_read_write_fun.h"
#include "ccu_function.h"
#include "alarm_list_fun.h"
#include "manual_ann.h"

#define PIS_TCMS_LOG_NAME "/nand/log/mingxiao_pis_tcms_log.dat"

#define LOG(fmt, para...) do{if(ccu_state.is_this_server == 1){\
		tcms_log[0]='C';tcms_log[1]='C';tcms_log[2]='U';tcms_log[3]='L';\
		sprintf(&tcms_log[6], "%s\n\t[记录]:" fmt "\n",ccu_state.tcms_log_head,##para);\
		tcms_log[4]=0xFF;tcms_log[5]=0xFF;\
		if(BlockBufferWrite(ccu_state.ccu_videolink_cmd_send_buff_id, (char *)&tcms_log[0],strlen(tcms_log)) < 0 )\
		{diag_printf("write CCU log error!!! \n");}\
		memset(tcms_log,'\0',sizeof(tcms_log));}}while(0)

#define LOG_FAULT(fmt, para...) do{if(ccu_state.is_this_server == 1){\
		tcms_log[0]='C';tcms_log[1]='C';tcms_log[2]='U';tcms_log[3]='L';\
		sprintf(&tcms_log[6], "%s\n\t[记录]:" fmt "\n",ccu_state.tcms_log_head,##para);\
		tcms_log[4]=0xFF;tcms_log[5]=0xFF;\
		if(BlockBufferWrite(ccu_state.ccu_videolink_cmd_send_buff_id, (char *)&tcms_log[0],strlen(tcms_log)) < 0 )\
		{diag_printf("write CCU log error!!! \n");}\
		memset(tcms_log,'\0',sizeof(tcms_log));}}while(0)

#define LOG_debug(fmt, para...) do{if(fp_tcms_log != NULL)fprintf(fp_tcms_log, "%s\n\t[记录]:" fmt "\n",ccu_state.tcms_log_head,##para);fflush(fp_tcms_log);} while(0)



typedef struct
{

	unsigned char FrameHead;
	unsigned char Addr;
	unsigned char DataSize;
	unsigned char Data[48];
	unsigned char SumCheck;
	unsigned char End;
}RecvNetComMpuData_t;
typedef struct
{
	unsigned char FrameHead;
	unsigned char PakgNo;
	unsigned char DataSize;
	unsigned char Data[79];
	unsigned char SumCheck;
	unsigned char End;
}MvbData_t;
typedef struct
{
	int flag;
	unsigned char time[4];
	unsigned char mpu1_state;
	unsigned char mpu2_state;
	unsigned char res[2];
}log_type;


extern char ccu_tcms_log_enable;
extern FILE *fp_tcms_log;
extern network_send_package_t tms_effective_cmd;
extern unsigned char whether_send_to_emerg;
extern unsigned char tcms_current_ann_mode;
extern unsigned char tcms_current_up_or_down;
extern unsigned char current_station_open_door_side;
extern unsigned char current_ddu_running_direction;
extern unsigned char tcms_timer_flag[7];
extern unsigned char tcms_current_is_skip_station;
extern char pis_recv_tcms_info_backup_buffer[100];
extern unsigned char whether_arrive_station;
extern common_package_t tcms_volume_info_to_bcu,tcms_volume_info_to_eamp,tcms_volume_info_to_other_ccu;

extern char tcms_skip_station_info_backup[100];
extern unsigned char whe_want_to_handle_tcms_skip_station;
extern unsigned char skip_station_setting_is_effective;
extern unsigned char tcms_should_running_times;
extern sem_t sem_mvb_new_cmd; ///< wake up handle thread

/** CcuTmsHandleThreadEntry function description
 *  TMS线程处理函数
 *
 *  @param   arg
 *  @return  null
 */
void *CcuTmsHandleThreadEntry(void *);
void *CcuMvbRecvThreadEntry(void *);

/** TCMSCMUStateInit function description
 *  TMS连挂信息初始化
 *
 *  @param   连挂信息包地址
 *  @return  null
 */

///<add by wds 20160407 begin
//create a timer for receiving data from MVB regularly
void alarm_func_handle_mvb_recv(cyg_handle_t counter_handle, cyg_addrword_t data);
int CreateMvbRecvTimer();
int DeleteMvbRecvTimer();
int SetMvbRecvTimeInterval(int time_interval);
int OpenMvbRecvTimer();
int CloseMvbRecvTimer();
void StartMvbRecvTimer();
void ResetMvbRecvTimer();
///<add by wds 20160407 end
void SetLineInfo(unsigned char tmp_current_station_no,unsigned char tmp_terminal_station_no,unsigned char temp_current_mission_number);
//获取下一站信息
unsigned char GetTCMSNextStation(unsigned char cur_station_no,unsigned char ter_station_no);
void CalculateCurrentDateInfo(unsigned int  seconds);
void ShowDoorStateAndStrategy(unsigned char tcms_door_1,unsigned char tcms_door_2);
void TCMSCMUStateInit(common_package_t *p_tcms_cmu_state);

/** TCMSDeviceVolumeInit function description
 *  TMS设备音量信息初始化
 *
 *  @param   发送给BCU音量信息包地址
 *  @param   发送给EAMP音量信息包地址
 *  @param   发送给CCU音量信息包地址
 *  @return  null
 */
void TCMSDeviceVolumeInit(common_package_t *p_to_bcu,common_package_t *p_to_eamp,common_package_t *p_to_other_ccu);

/** TMSATOAnnPackageInit function description
 *  ATO信息包初始化
 *
 *  @param   发送给EAMP信息包地址
 *  @return  null
 */
void TMSATOAnnPackageInit(network_send_package_t *p_ato_ann);

/** TMSSkipStationPackageInit function description
 *  越站信息包初始化
 *
 *  @param   发送给EAMP信息包地址
 *  @return  null
 */
void TMSEmergCMDInfoInit(network_send_package_t *p_skip_station);

/** TMSSkipStationPackageInit function description
 *  紧急广播包信息包初始化
 *
 *  @param   发送给EAMP信息包地址
 *  @return  null
 */
void TMSEmergCMDInfoInit(network_send_package_t *p_emerg_cmd);

/** TMSEffectiveCMDInfoInit function description
 *  TCMS状态包信息包初始化
 *
 *  @param   发送给BCU、EAMP信息包地址
 *  @return  null
 */
void TMSEffectiveCMDInfoInit(network_send_package_t *p_effective_cmd);

/** UpdatePASoftwareVersionInfoToTMS function description
 *  更新PA版本号态给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdatePASoftwareVersionInfoToTMS(int flag);

/** UpdatePISDeviceVolumeInfoToTMS function description
 *  更新PA音量信息态给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateBroadcastModeToTMS(int flag);

/** UpdatePISDeviceVolumeInfoToTMS function description
 *  更新当前状态给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateBroadcastModeToTMS(int flag);

/** UpdatePARunningStationInfoToTMS function description
 *  更新当前运营信息给TCMS
 *
 *  @param   起点站
 *  @param   终点站
 *  @param   当前站
 *  @param   下一站
 *  @return  null
 */
void UpdatePARunningStationInfoToTMS(unsigned char begin_station_code,unsigned char terminal_station_code,
										unsigned char current_station_code,unsigned char next_station_code);

/** UpdateKeyInfoToTMS function description
 *  更新钥匙信号给TCMS
 *
 *  @param   钥匙信号
 *  @return  null
 */
void UpdateKeyInfoToTMS(int flag);

/** UpdateMediaFailureToTMS function description
 *  更新媒体故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateMediaFailureToTMS();

/** UpdateMonitorFailureToTMS function description
 *  更新监控故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateMonitorFailureToTMS();

/** UpdateDMFailureToTMS function description
 *  更新动态地图故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateDMFailureToTMS();

/** UpdateNDFailureToTMS function description
 *  更新噪检故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateNDFailureToTMS();

/** UpdateLEDFailureToTMS function description
 *  更新LED故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateLEDFailureToTMS();

/** UpdateEAMPFailureToTMS function description
 *  更新EAMP故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateEAMPFailureToTMS();

/** UpdateCCUFailureToTMS function description
 *  更新CCU故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateCCUFailureToTMS();

/** UpdateBCUFailureToTMS function description
 *  更新BCU故障给TCMS
 *
 *  @param   null
 *  @return  null
 */
void UpdateBCUFailureToTMS();

/** SendTCMSEffectivateFlagToBCU function description
 *  发送TCMS有效信息标志
 *
 *  @param   TCMS有效信息包
 *  @param   TCMS标志位
 *  @return  线路号
 */
void SendTCMSEffectivateFlagToBCU(network_send_package_t tms_effective_cmd,unsigned char tms_effective_flag);

/** SendTCMSLineAndRegionInfo function description
 *  发送当前TCMS线路号信息
 *
 *  @param   null
 *  @return  null
 */
void SendTCMSLineAndRegionInfo();

/** SendCurrentCCULineInfo function description
 *  发送当前CCU线路号信息
 *
 *  @param   null
 *  @return  null
 */
void SendCurrentCCULineInfo();

/** SendHardSignalManual function description
 *  硬信号报站
 *
 *  @param   开么侧
 *  @param   到站还是预到站
 *  @return  null
 */
void SendHardSignalManual(unsigned char open_door_side,unsigned char stop_or_leave);

/** InitTCMSPackage function description
 *  初始化TCMS相关包信息
 *
 *  @param   null
 *  @return  null
 */
void InitTCMSPackage();

/** TCMSToPISInfoHandle function description
 *  TCMS信息包处理
 *
 *  @param   接受到TCMS信息
 *  @return  if success return 0
 */
void TCMSToPISInfoHandle(unsigned char ccu_recv_tms_info[]);

/** SendTCMSVolumeToAllDevice function description
 *  发送TCMS音量信息给所有的EAMP
 *
 *  @param   null
 *  @return  null
 */
void SendTCMSVolumeToAllDevice();

/** ShowTCMSSkipStationInfo function description
 *  显示TCMS越站信息设置
 *
 *  @param   null
 *  @return  null
 */
void ShowTCMSSkipStationInfo();

/** SendATCCMD function description
 *  发送ATC报站控制命令包
 *
 *  @param   null
 *  @return  null
 */
void SendATCCMD();

/** SendDDUCMDAgain function description
 *  再次发送DDU报站控制命令包
 *
 *  @param   null
 *  @return  null
 */
void SendDDUCMDAgain();

/** SendDDUCMD function description
 *  发送DDU报站控制命令包
 *
 *  @param   null
 *  @return  null
 */
void SendDDUCMD();

/** SendTCMSEmergInfo function description
 *   发送TCMS紧急广播信息
 *
 *  @param   null
 *  @param   null
 *  @return  null
 */
void SendTCMSEmergInfo();


/** SendCMUStateToBCUAndCMU function description
 *   发送连挂状态网络命令包给BCU和CMU
 *
 *  @param   当前连挂状态
 *  @return  null
 */
void SendCMUStateToBCUAndCMU(unsigned char current_cmu_state);

void SendTCMSNetworkToPA();

void SendTCMSManualInfo();

void SetManualInfo(unsigned char s_no,unsigned char c_no,unsigned char t_no, unsigned char up_down, unsigned char stop_or_leave);

void SetAutoInfo();

void SetSemiAutoInfo();

void SendSyncSignal();

void CountButtonClick(unsigned char array[]);

void GetDoorState(unsigned char ccu_recv_tms_info[]);

void InitStationDistance();

unsigned long CountDistanceToNext(unsigned char current_no, unsigned char next_no);

unsigned char GetIndexFromStation(unsigned char station_no);
 

#endif



