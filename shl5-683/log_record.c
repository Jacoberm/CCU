/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： log_record.c
 * 文件标识：
 * 摘    要：The source file of log record
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#include "log_record.h"

mapping_table_t mapping_table[256];
log_mic_simple_t log_mic_simple[LOG_NUMBER];

int InitLogTime(log_mic_simple_t log_mic_simple[])
{///<初始化LOG时间
	int i = 0;
	for(i = 0; i < LOG_NUMBER; i ++)
	{
		log_mic_simple[i].first_free_number = 0;
		memset((void *)&log_mic_simple[i].log_time,0,sizeof(log_mic_simple[i].log_time));
	}
	return 0;
}

int AddNewDataToLogTime(log_mic_simple_t log_mic_simple[],int type,int tv_sec,int tv_usec,int audio_data_length)
{///<将新的数据写入到LOG中，测试使用
	int free_number = log_mic_simple[0].first_free_number;
	int ret = 0;
	if(free_number >= LOG_NUMBER)
	{
		ret = -1;
	}
	else
	{
		if(type == 0)//begin
		{
			log_mic_simple[free_number].log_time.tv_sec_begin = tv_sec;
			log_mic_simple[free_number].log_time.tv_usec_begin = tv_usec;
		}
		else//over
		{
			log_mic_simple[free_number].log_time.tv_sec_over = tv_sec;
			log_mic_simple[free_number].log_time.tv_usec_over = tv_usec;
			log_mic_simple[free_number].log_time.len_of_read_audio_data = audio_data_length;
			log_mic_simple[0].first_free_number += 1;
		}
	}
	return ret;
}

int ShowDataOfLogTime(log_mic_simple_t log_mic_simple[])
{///<显示时间LOG信息
	int i = 0;
	int temp_begin_time = 0;
	int temp_over_time = 0;
	int temp_dec_time = 0;

	for(i = 0;i < log_mic_simple[0].first_free_number - 1 && i < LOG_NUMBER;i ++)
	{
		temp_begin_time = (log_mic_simple[i].log_time.tv_sec_begin * 1000000 + log_mic_simple[i].log_time.tv_usec_begin)/1000;
		temp_over_time = (log_mic_simple[i].log_time.tv_sec_over * 1000000 + log_mic_simple[i].log_time.tv_usec_over)/1000;
		temp_dec_time =  temp_over_time - temp_begin_time;
		debug_print(("begin:%5d	over:%5d		dec:%5d\n",temp_begin_time,temp_over_time,temp_dec_time));
	}
	return 0;
}

/*show the detail of cmd information*/
int DisplayNetworkCmd(network_send_package_t  param_cmd_info)
{
	debug_print(("destination device:%s-%d\n",param_cmd_info.dst_devices_name,param_cmd_info.dst_devices_no));
	debug_print(("source device:%s%d\n",param_cmd_info.send_information.src_devices_name,param_cmd_info.send_information.src_devices_no));

	debug_print(("event_type_ann = %s\n",mapping_table[param_cmd_info.send_information.event_type_ann].event_name));
	debug_print(("event_type_intercom = %s\n",mapping_table[param_cmd_info.send_information.event_type_intercom].event_name));
	debug_print(("event_type_line_no = %d\n",param_cmd_info.send_information.event_info_ann.line_info.line_no));
	debug_print(("event_ptt_state = %d\n",param_cmd_info.send_information.event_ptt_state));

	if(param_cmd_info.send_information.event_type_ann == MANUAL_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  manual ann:package number %d\n",param_cmd_info.send_information.sequence));
		debug_print(("manual_active = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.manual_active));
		debug_print(("manual_begin_or_over = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.manual_begin_or_over));
		debug_print(("manual_pre_read = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.manual_pre_read));

       debug_print(("manual_response = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.manual_response));
		debug_print(("line_number = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.line_number));
		debug_print(("current_station_code = %d,\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.current_station_code));
		debug_print(("door_side = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.door_side));
		debug_print(("stop_or_leave = %d\n",param_cmd_info.send_information.event_info_ann.manual_annnounce.stop_or_leave));
	}
	else if(param_cmd_info.send_information.event_type_ann == EMERG_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  emerg ann:package number %d\n",param_cmd_info.send_information.sequence));
		debug_print(("emerg_active = %d\n",param_cmd_info.send_information.event_info_ann.emerg_announce.emerg_active));
		debug_print(("emerg_begin_or_over = %d\n",param_cmd_info.send_information.event_info_ann.emerg_announce.emerg_begin_or_over));
		debug_print(("emerg_response = %d\n",param_cmd_info.send_information.event_info_ann.emerg_announce.emerg_response));
		debug_print(("emerg_sequence = %d\n",param_cmd_info.send_information.event_info_ann.emerg_announce.emerg_sequence));
		debug_print(("emerg_times = %d\n",param_cmd_info.send_information.event_info_ann.emerg_announce.emerg_times));
	}
	else if(param_cmd_info.send_information.event_type_ann == LIVE_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  live ann:package number %d\n",param_cmd_info.send_information.sequence));
		debug_print(("live_active = %d\n",param_cmd_info.send_information.event_info_ann.live_announce.live_active));
		debug_print(("live_begin_or_over = %d\n",param_cmd_info.send_information.event_info_ann.live_announce.live_begin_or_over));
		debug_print(("live_response = %d\n",param_cmd_info.send_information.event_info_ann.live_announce.live_response));
	}
	else if(param_cmd_info.send_information.event_type_ann == TMS_AUTO_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  tms auto ann:package number %d\n",param_cmd_info.send_information.sequence));
		debug_print(("tms_auto_active = %d\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.tms_auto_active));
		debug_print(("tms_auto_begin_or_over = %d\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over));
       debug_print(("tms_auto_response = %d\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.tms_auto_response));
		debug_print(("line_number = %d\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.line_number));
		debug_print(("current_station_code = %d,\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.current_station_code));
		debug_print(("door_side = %d\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.door_side));
		debug_print(("stop_or_leave = %d\n",param_cmd_info.send_information.event_info_ann.tms_auto_announce.stop_or_leave));
	}
	else if(param_cmd_info.send_information.event_type_ann == ANN_IDLE)
	{
		if(param_cmd_info.send_information.event_type_intercom == D2P_INTERCOMM_EVENT)
		{
			debug_print(("D2P active = %d\n",param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active));
			debug_print(("D2P dst bcu no = %d\n",param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no));
			debug_print(("D2P dst pcu no = %d\n",param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no));
			debug_print(("D2P pending = %d\n",param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pending));
			debug_print(("D2P request or over = %d\n",param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over));
			debug_print(("D2P response = %d\n",param_cmd_info.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_response));
		}
		else if(param_cmd_info.send_information.event_type_intercom == D2D_INTERCOMM_EVENT)
		{
			debug_print(("D2D active = %d\n",param_cmd_info.send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_active));
			debug_print(("D2D dst bcu no = %d\n",param_cmd_info.send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no));
			debug_print(("D2D ppt state = %d\n",param_cmd_info.send_information.event_info_intercom.d2d_intercomm.d2d_ppt_state));
			debug_print(("D2D request or over = %d\n",param_cmd_info.send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over));
			debug_print(("D2D response = %d\n",param_cmd_info.send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_response));
		}
		else
		{
			debug_print(("Debug cmd info:  idle ann:package number %d\n",param_cmd_info.send_information.sequence));
		}
	}
	else
	{
		debug_print(("This is a error package\n"));
	}
	return 0;
}

/*show the detail of cmd information*/
int DisplaySendCmd(send_infomation_t  param_cmd_info)
{

	debug_print(("event_type_ann = %s\n",mapping_table[param_cmd_info.event_type_ann].event_name));
	debug_print(("event_type_intercom = %s\n",mapping_table[param_cmd_info.event_type_intercom].event_name));
	debug_print(("event_type_line_no = %d\n",param_cmd_info.event_info_ann.line_info.line_no));
	debug_print(("event_ptt_state = %d\n",param_cmd_info.event_ptt_state));

	if(param_cmd_info.event_type_ann == SKIP_STATION)
	{
		debug_print(("Debug cmd info:  skip station\n"));
		debug_print(("Debug cmd info:  skip station:line_number %d\n",param_cmd_info.event_info_ann.station_information.line_number));
		debug_print(("Debug cmd info:  skip station:station_no %d\n",param_cmd_info.event_info_ann.station_information.station_no));
		debug_print(("Debug cmd info:  skip station:effective %d\n",param_cmd_info.event_info_ann.station_information.effective));
	}
	else if(param_cmd_info.event_type_ann == MANUAL_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  manual ann:package number %d\n",param_cmd_info.sequence));
		debug_print(("manual_active = %d\n",param_cmd_info.event_info_ann.manual_annnounce.manual_active));
		debug_print(("manual_begin_or_over = %d\n",param_cmd_info.event_info_ann.manual_annnounce.manual_begin_or_over));
		debug_print(("manual_pre_read = %d\n",param_cmd_info.event_info_ann.manual_annnounce.manual_pre_read));

       debug_print(("manual_response = %d\n",param_cmd_info.event_info_ann.manual_annnounce.manual_response));
		debug_print(("line_number = %d\n",param_cmd_info.event_info_ann.manual_annnounce.line_number));
		debug_print(("current_station_code = %d,\n",param_cmd_info.event_info_ann.manual_annnounce.current_station_code));
		debug_print(("door_side = %d\n",param_cmd_info.event_info_ann.manual_annnounce.door_side));
		debug_print(("stop_or_leave = %d\n",param_cmd_info.event_info_ann.manual_annnounce.stop_or_leave));
	}
	else if(param_cmd_info.event_type_ann == EMERG_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  emerg ann:package number %d\n",param_cmd_info.sequence));
		debug_print(("emerg_active = %d\n",param_cmd_info.event_info_ann.emerg_announce.emerg_active));
		debug_print(("emerg_begin_or_over = %d\n",param_cmd_info.event_info_ann.emerg_announce.emerg_begin_or_over));
		debug_print(("emerg_response = %d\n",param_cmd_info.event_info_ann.emerg_announce.emerg_response));
		debug_print(("emerg_sequence = %d\n",param_cmd_info.event_info_ann.emerg_announce.emerg_sequence));
		debug_print(("emerg_times = %d\n",param_cmd_info.event_info_ann.emerg_announce.emerg_times));
	}
	else if(param_cmd_info.event_type_ann == LIVE_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  live ann:package number %d\n",param_cmd_info.sequence));
		debug_print(("live_active = %d\n",param_cmd_info.event_info_ann.live_announce.live_active));
		debug_print(("live_begin_or_over = %d\n",param_cmd_info.event_info_ann.live_announce.live_begin_or_over));
		debug_print(("live_response = %d\n",param_cmd_info.event_info_ann.live_announce.live_response));
	}
	else if(param_cmd_info.event_type_ann == TMS_AUTO_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  tms auto ann:package number %d\n",param_cmd_info.sequence));
		debug_print(("tms_auto_active = %d\n",param_cmd_info.event_info_ann.tms_auto_announce.tms_auto_active));
		debug_print(("tms_auto_begin_or_over = %d\n",param_cmd_info.event_info_ann.tms_auto_announce.tms_auto_begin_or_over));
       debug_print(("tms_auto_response = %d\n",param_cmd_info.event_info_ann.tms_auto_announce.tms_auto_response));
		debug_print(("line_number = %d\n",param_cmd_info.event_info_ann.tms_auto_announce.line_number));
		debug_print(("current_station_code = %d,\n",param_cmd_info.event_info_ann.tms_auto_announce.current_station_code));
		debug_print(("door_side = %d\n",param_cmd_info.event_info_ann.tms_auto_announce.door_side));
		debug_print(("stop_or_leave = %d\n",param_cmd_info.event_info_ann.tms_auto_announce.stop_or_leave));
	}
	else if(param_cmd_info.event_type_ann == ANN_IDLE)
	{
		if(param_cmd_info.event_type_intercom == D2P_INTERCOMM_EVENT)
		{
			debug_print(("D2P active = %d\n",param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_active));
			debug_print(("D2P dst bcu no = %d\n",param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no));
			debug_print(("D2P dst pcu no = %d\n",param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no));
			debug_print(("D2P pending = %d\n",param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_pending));
			debug_print(("D2P request or over = %d\n",param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over));
			debug_print(("D2P response = %d\n",param_cmd_info.event_info_intercom.d2p_intercomm.d2p_intercomm_response));
		}
		else if(param_cmd_info.event_type_intercom == D2D_INTERCOMM_EVENT)
		{
			debug_print(("D2D active = %d\n",param_cmd_info.event_info_intercom.d2d_intercomm.d2d_intercomm_active));
			debug_print(("D2D dst bcu no = %d\n",param_cmd_info.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no));
			debug_print(("D2D ppt state = %d\n",param_cmd_info.event_info_intercom.d2d_intercomm.d2d_ppt_state));
			debug_print(("D2D request or over = %d\n",param_cmd_info.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over));
			debug_print(("D2D response = %d\n",param_cmd_info.event_info_intercom.d2d_intercomm.d2d_intercomm_response));
		}
		else
		{
			debug_print(("Debug cmd info:  idle ann:package number %d\n",param_cmd_info.sequence));
		}
	}
	else
	{
		debug_print(("This is a error package\n"));
	}
	return 0;
}
void InitMappingTable()
{///<设置映射表，方便阅读，比如LIVE_ANN_EVENT显示为“LIVE_ANN_EVENT”
 	 int i = 0;
 	 for(i = 0; i < 256;i ++)
 	 {
 	  	 strcpy(mapping_table[i].event_name,"NULL");
 		 mapping_table[i].event_no = i;
 	 }

 	 strcpy(mapping_table[1].event_name,"OCC_EVENT");
 	 mapping_table[1].event_no = 1;

 	 strcpy(mapping_table[2].event_name,"LIVE_ANN_EVENT");
 	 mapping_table[2].event_no = 2;

 	 strcpy(mapping_table[3].event_name,"EMERG_ANN_EVENT");
 	 mapping_table[3].event_no = 3;

 	 strcpy(mapping_table[4].event_name,"MANUAL_ANN_EVENT");
 	 mapping_table[4].event_no = 4;

 	 strcpy(mapping_table[5].event_name,"TMS_MANUAL_ANN_EVENT");
 	 mapping_table[5].event_no = 5;

 	 strcpy(mapping_table[6].event_name,"TMS_AUTO_ANN_EVENT");
 	 mapping_table[6].event_no = 6;

 	  strcpy(mapping_table[7].event_name,"D2D_INTERCOMM_EVENT");
 	 mapping_table[7].event_no = 7;

 	 strcpy(mapping_table[8].event_name,"D2P_INTERCOMM_EVENT");
 	 mapping_table[8].event_no = 8;

 	 strcpy(mapping_table[9].event_name,"D2D_HANGUP_D2P_EVENT");
 	 mapping_table[9].event_no = 9;

 	 strcpy(mapping_table[10].event_name,"D2P_INTERCOM_PENDING");
 	 mapping_table[10].event_no = 10;

 	 strcpy(mapping_table[11].event_name,"LINE_NUMBER_CHANGE_EVENT");
 	 mapping_table[11].event_no = 11;

 	 strcpy(mapping_table[254].event_name,"ANN_IDLE");
 	 mapping_table[254].event_no = 254;

 	 strcpy(mapping_table[255].event_name,"INTERCOM_IDLE");
 	 mapping_table[255].event_no = 1;
	return ;
}
void WriteNetworkErrorToLog(char *param_error_tip_info)
{
	debug_print(("%s\n",param_error_tip_info));
}


