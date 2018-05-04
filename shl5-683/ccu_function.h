/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� ccu_function.h
 * ���������������
 * ���    ������ CCU������������
 *
 * ��������������� V1.0.0
 * ���    ������ wilson
 * ���������������2013-06-21
 *
 * ���������������
 * ���������  ���
 * ���������������
 */
#ifndef CCU_FUNCTION_H_
#define CCU_FUNCTION_H_

#include "data_struct.h"
#include "tms_module.h"
#include "ccu_led.h"
#include "network/zhw_network_main.h"
#include "ccu_emerg.h"
#include "ccu_station.h"
#include "audio_sample.h"
#include "sqlite_middle_layer.h"
#include "ccu_log_info.h"
#include "file_operate.h"
#include "ccu_tms.h"
#include "ccu_tcms_test.h"
#include "ccu_device_volume.h"
#include "media_and_monitor.h"

typedef struct
{
	unsigned short tms_train_no;
	unsigned char tms_car_current_speed;
	current_time_t current_time;
}ccu_tms_car_running_info_t;
typedef struct
{
	unsigned char pcu_no;
	unsigned char pcu_request_state;
	unsigned char pcu_connenting_state;
}pcu_state_on_ccu_t;

typedef struct
{
	unsigned long distance;
	unsigned int pre_triger_distance[2];
	unsigned int triger_distance[2];
	unsigned int triger_dia_distance[2];
}ccu_tms_station_diatance_t;
typedef struct
{
	unsigned char ddu_ternimal_station_code;
	unsigned char ddu_current_station_code;
	unsigned char ddu_next_station_code;
	unsigned char ddu_line_no;
	unsigned char ddu_start_station_code;
	unsigned char up_or_down;
	unsigned char stop_or_leave;
}tms_ddu_info_t;
typedef struct
{
	unsigned char ato_current_station_no;
	unsigned char ato_next_station_no;
	unsigned char ato_terminal_statio_no;
	unsigned char ato_up_or_down;
	unsigned char ato_line_no;
}tms_ato_info_t;
typedef struct
{
	tms_ddu_info_t tms_ddu_info;
	tms_ato_info_t tms_ato_info;
}tms_braodcast_info_t;
typedef struct
{
	unsigned char hard_signal_current_station_no;
	unsigned char hard_signal_stop_or_leave;
}hard_signal_info_t;
typedef struct
{
	unsigned char pis_fault_tcms_simulate_state;
	unsigned char data[100];
}pis_fault_tcms_simulate;
typedef struct
{
	device_ccu_state_t ccu_info;
	int tms_cmd_send_buffer_id;
	int tms_cmd_recv_buffer_id;

	int ccu_send_cmd_buffer_id;
	int ccu_recv_cmd_buffer_id;

	int ccu_send_audio_data_buffer_id;
	int ccu_recv_audio_data_buffer_id;

	int ccu_send_common_cmd_buffer_id;
	int ccu_recv_common_cmd_buffer_id;
	
	int ccu_send_to_media_cmd_buffer_id;
	
	int shell_send_buffer_id;
	int shell_recv_buffer_id;

	unsigned char current_line_no;
	unsigned char current_section_no;

	unsigned char key_info;
	unsigned char key_info_tcms;
	ccu_tms_car_running_info_t ccu_tms_car_running_info;

	device_software_version_t bcu_software_version;
	device_software_version_t monitor_software_version;
	device_software_version_t media_software_version;

	unsigned char current_pa_running_mode;
	unsigned char current_d2d_running_mode;
	unsigned char current_media_volume[2];///<0:the volune  1:set volume
	unsigned char tcms_media_volume[2];///<0:the volune  1:set volume
	unsigned char current_pa_volume;
	unsigned char media_failure_info[3];///<���media���������������������������������
	unsigned char monitor_failure_info[7];///<���monitor���������������������������������
	unsigned char pa_failure_info[32];///<CCU���������������TCMS���������������������
	unsigned char pa_running_info_to_tms[32];
	unsigned char pa_failure_info1[32];///<CCU���������������TCMS���������������������
	unsigned char pa_running_info_to_tms1[32];
	unsigned char door_emerg_unlock_state_and_smoke_info[16];///<���������������������������TCMS���������������������������������������TCU���������������������������������
	unsigned char led_failure_info[4];///<LED������
	unsigned char nd_failure_info[3];///<������������
	unsigned char dm_failure_info[2];///<������������������
	unsigned char pcu_failure_info[2];///<PCU ������
	unsigned char eamp_failure_info[2];///<EAMP ������
	unsigned char ccu_failure_info;///<ccu ������
	unsigned char bcu_failure_info;///<bcu ������
	unsigned char pcu_state_to_monitor[7];
	unsigned char pcu_state_to_tcms[4];

	ccu_emerg_info_t *p_ccu_emerg_info;
	global_region_info_t ccu_region_info[2];
	station_information_t (*pp_skip_station_info)[MAX_STATION_NUMBER];
	station_information_t (*pp_station_info)[32]; ///< station name info
	ann_station_report_t *p_line_ann_station_info[2];

	int local_audio_file_buffer_id;
	int local_pending_buffer_id;
	unsigned char ddu_start_no;
	unsigned char led_language;
	unsigned char key_status[2];///< 0:none 1:tc1  2:tc2
	unsigned char other_ccu_no;

	int ccu_get_bcu_log_info_buffer_id;
	int tms_true_cmd_recv_buffer_id;
	int tms_true_cmd_send_buffer_id;
	int ccu_get_pcu_state_comm_buffer_id;
	int ccu_intercomm_audio_data_record_recv_buffer_id;
	int ccu_ddu_tms_simulate_id;	///<for simulate ddu

	ccu_tms_station_diatance_t ccu_tms_station_diatance[2][32];
	tms_braodcast_info_t tms_braodcast_info;
	unsigned char tms_effective_flag;
	int tcms_to_pa_timer_flag[7];
	unsigned char enable_audio_data_to_file;
	ccu_per_dev_failure_t ccu_per_dev_failure_info;
	hard_signal_info_t hard_signal_info;
	unsigned char is_this_server;
	//unsigned char current_pa_volume;
	unsigned char current_monitor_state;
	unsigned char current_monitor_volume;
	unsigned char current_d2d_volume;
	unsigned char current_d2p_volume;
	int ccu_tcms_log_info;
	current_time_t tcms_log_time;
	char tcms_log_head[512];
	unsigned char simulate_speed;	//simulate ddu
	unsigned char simulate_door_status;	//simulate ddu
	int simulate_flag;
	int software_version_buffer_id;
	pa_software_version_t pa_software_version;
	int common_big_send_buffer_id;
	unsigned char db_setting_speed;
	char ts_password[6];
	int ccu_cmu_state_buffer_id;
	unsigned char ccu_cmu_state;
	pis_fault_tcms_simulate pis_fault_tcms_simulate;

	region_station_info_t region_station_info;
	int ccu_videolink_cmd_send_buff_id;
}ccu_state_t;

typedef struct
{
	unsigned char emerg_state;
	unsigned char emerg_sequence;
}notice_ccu_send_to_media_t;

extern ccu_state_t ccu_state;

extern unsigned char send_eamp_network_state;

extern char chEmerMsg_wilson[10][192];

extern pcu_state_on_ccu_t pcu_state_on_ccu[20];

extern int whether_send_serial_fdu_data;
extern unsigned char whether_send_fdu_can_data;
extern unsigned char whether_send_fdu_can_heart_beta;

extern unsigned char whether_show_line4_cycle_terminal_station;

extern sem_t  sem_wakeup_eamp_led;
extern sem_t sem_wakeup_test;
extern sem_t sem_can_recv;

extern unsigned char wherther_send_sync_signal;

/** CcuBufferInit function description
 *  CCU ������������������
 *
 *  @param   null
 *  @return  if success return o
 */
int CcuBufferInit(void);

/** CcuGetCurrentLineNumber function description
 *  CCU ���������������������������
 *
 *  @param   null
 *  @return  if success return o
 */
int CcuGetCurrentLineNumber(void);

/** CcuGetCurrentSectionNumber function description
 *  CCU ������������������������
 *
 *  @param   null
 *  @return  if success return o
 */
int CcuGetCurrentSectionNumber(void);

/** LEDShowTerminalStation function description
 *  ���������LED������������������
 *
 *  @param   null
 *  @return  if success return o
 */
int LEDShowTerminalStation();

/** PthreadPriorityChangeForSchedRr function description
 *  ���������������������
 *
 *  @param   ������id
 *  @param   ���������������
 *  @return  if success return o
 */
int PthreadPriorityChangeForSchedRr(pthread_t  target_thread_id, int  new_prior );

/** CCUStateInit function description
 *  CCU���������������
 *
 *  @param   null
 *  @return  null
 */
void CCUStateInit();

/** InitCCUDataBaseInfo function description
 * ���������CCU���������������
 *
 *  @param   null
 *  @return  null
 */
void InitCCUDataBaseInfo();

/** GetCurrentregionPosition function description
 *   ���������������������������
 *
 *  @param   ���������
 *  @return  null
 */
void GetCurrentregionPosition(unsigned int param_region_no);

/** UpdateRegionStationInfo function description
 *   ������������������������
 *
 *  @param   ���������
 *  @param   ������������������
 *  @return  null
 */
void UpdateRegionStationInfo(unsigned char param_line_no,unsigned char param_region_position);

/** InitPCUDeviceState function description
 *   ���������PCU���������������
 *
 *  @param   null
 *  @return  null
 */
void InitPCUDeviceState();

/** GetSimulateVariableInfo function description
 *  ������������������������������
 *
 *  @param   null
 *  @return  if success return 0
 */
int GetSimulateVariableInfo();

/** GetLineInfo function description
 *  ������������������������������������
 *
 *  @param   null
 *  @return  if success return 0
 */
int GetLineInfo();

/** GetAllStationsName function description
 *  ������������������������������������������
 *
 *  @param   null
 *  @return  null
 */
void GetAllStationsName();

/** UpdateSkipStationInformation function description
 *  ���������������������
 *
 *  @param   ���������������������
 *  @return  null
 */
void UpdateSkipStationInformation(send_infomation_t *param_cmd_info);

/** ShowStationInformation function description
 *  ���������������������
 *
 *  @param   ���������
 *  @return  null
 */
void ShowStationInformation(int line_number_of_skip_station);

/** GetDefaultRegion function description
 *  ���������������������
 *
 *  @param   null
 *  @return  null
 */
void GetDefaultRegion();

/** GetRegionInfo function description
 *  ������������������������������
 *
 *  @param   null
 *  @return  null
 */
void GetRegionInfo();

/** UpdateRegionInfo function description
 *  ������������������������
 *
 *  @param   ���������
 *  @param   ���������
 *  @return  null
 */
void UpdateRegionInfo(int param_line_no,int param_region_no);

/** ShowRegionInfo���ShowRegionInfo_diag_printf function description
 *  ������������������������
 *
 *  @param   ���������
 *  @return  null
 */
void ShowRegionInfo(int param_line_no);
void ShowRegionInfo_diag_printf(int param_line_no);

/** ShowAllRegionInfo function description
 *  ������������������������
 *
 *  @param   ���������
 *  @return  null
 */
void ShowAllRegionInfo(int param_line_no);

/** GetEmergInfo function description
 *  ���������������������������������������
 *
 *  @param   null
 *  @return  null
 */
void GetEmergInfo();

/** GetHardwareKeyStatus function description
 *  ������������������������
 *
 *  @param   null
 *  @return  ������������������
 */
int GetHardwareKeyStatus();

/** GetDriverKeyStatus function description
 *  ���������������������������������������������������
 *
 *  @param   null
 *  @return  ������������������
 */
int GetDriverKeyStatus();

/** GetKeyInfoNew function description
 *  ���������������������������������������������������
 *
 *  @param   null
 *  @return  null
 */
void GetKeyInfoNew(int flag);

/** SendKeyInfo function description
 *  ������������������
 *
 *  @param   ������������
 *  @return  null
 */
void SendKeyInfo(unsigned char param_key_status);

/** UpdateGlobalkeyInfo function description
 *  ���������������������������������
 *
 *  @param   null
 *  @return  null
 */
void UpdateGlobalkeyInfo();

/** CCUControlBufferInit function description
 *  CCU������������������
 *
 *  @param   null
 *  @return  null
 */
void CCUControlBufferInit();

/** SendTMSKeyInfo function description
 *  ������TCMS������������
 *
 *  @param   null
 *  @return  null
 */
void SendTMSKeyInfo();

/** AlarmPeriDeviceThatLineOrRegionHasChanged function description
 *  ���������������������������������������
 *
 *  @param   null
 *  @return  null
 */
void AlarmPeriDeviceThatLineOrRegionHasChanged();

/** SetSkipStationToTCMS function description
 *  ������������������������������������������������TCMS
 *
 *  @param   ���������
 *  @param   ���������
 *  @param   ������������
 *  @return  null
 */
void SetSkipStationResponseToTCMS(unsigned char line_no,unsigned char station_no,unsigned char skip_station_flag);

/** UpdateIntercommAudioDataRecordingFile function description
 * ������������������������������
 *
 *  @param   ���BCU������������������������������
 *  @return  null
 */
void UpdateIntercommAudioDataRecordingFile(common_package_t common_package_recv_from_bcu);

/** UpdatePAVolumeInfo function description
 *   ������PA������������
 *
 *  @param   ������������������������
 *  @return  null
 */
void UpdatePAVolumeInfo(common_package_t common_package_recv);

/** GetPerDevFailureInfo function description
 *   ������������������������������
 *
 *  @param   ������������
 *  @param   ������������
 *  @return  null
 */
void GetPerDevFailureInfo(unsigned char failure_info_buffer[],int buffer_size);

/** ReadNumWilson function description
 *   ���������������
 *
 *  @param   null
 *  @return  ���������
 */
int ReadNumWilson(void);

/** SendHeartBetaSignalToEAMP function description
 *   ������������������EAMP
 *
 *  @param   null
 *  @return  null
 */
void SendHeartBetaSignalToEAMP();

/** CreateEAMPNetworkHeartBetaTimer function description
 *   ���������������EAMP������������������
 *
 *  @param   null
 *  @return  null
 */
void CreateEAMPNetworkHeartBetaTimer();

/** CreateSYNCTimer function description
 *   ���������������������
 *
 *  @param   null
 *  @return  null
 */
void CreateSYNCTimer();

/** RestartSYNCTimer function description
 *   ���������������������
 *
 *  @param   null
 *  @return  null
 */
void RestartSYNCTimer();

/** CloseSYNCTimer function description
 *   ���������������������
 *
 *  @param   null
 *  @return  null
 */
void CloseSYNCTimer();

/** alarm_func_handle_sync function description
 *   ���������������������������
 *
 *  @param   ���������������
 *  @param   ���������������������
 *  @return  null
 */
void alarm_func_handle_sync(cyg_handle_t counter_handle, cyg_addrword_t data);

/** SendSYNCPackageToEamp function description
 *   ���������������
 *
 *  @param   null
 *  @return  null
 */
void SendSYNCPackageToEamp();

/** eamp_read_pa_occupy_status_wilson function description
 *   ������������������������
 *
 *  @param   null
 *  @return  ������������������
 */


int eamp_read_pa_occupy_status_wilson();

/** CCUUpdateLineInfo function description
 *   ������CCU���������������
 *
 *  @param   ������������������������������
 *  @return  null
 */
void CCUUpdateLineInfo(network_send_package_t package_from_network);
void SetSimulatedduInfo(common_package_t common_package_recv_simulateddu);

/** GetCCUDeviceVersionInfo function description
 *   CCU���������������
 *
 *  @param   null
 *  @return  null
 */
void GetCCUDeviceVersionInfo();

/** SendTCMSCMUStateResponsePackage function description
 *   CCU   ������CMU������
 *
 *  @param   CMU���������������
 *  @return  null
 */
void SendTCMSCMUStateResponsePackage(common_package_t param_common_package);
#endif /* CCU_FUNCTION_H_ */
