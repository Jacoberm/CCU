/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� ccu_function.c
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
#include "ccu_function.h"
extern unsigned char check_dev_state;

ccu_state_t ccu_state = {
    {"\0", 0},
    0			
};
//���������
sem_t  sem_wakeup_eamp_led;//LED���������������
sem_t sem_wakeup_test;//���������������
sem_t sem_can_recv;//can���������������
sem_t sem_mvb_new_cmd; ///< wake up handle thread

//���������������������
cyg_handle_t  counter_handle_sync;
cyg_handle_t alarm_handle_sync;
cyg_alarm alarm_object_sync;
unsigned char wherther_send_sync_signal = 0;

//������������������������
cyg_handle_t  counter_handle_eamp_network_heart_beta;
cyg_handle_t alarm_handle_eamp_network_heart_beta;
cyg_alarm alarm_object_eamp_network_heart_beta;
unsigned char send_eamp_network_state = 0;

char chEmerMsg_wilson[10][192] = {//���������������������������
										{""},
										{"���������������������,������������,���������������������������,������������������������,���������������������������,������������������,������������."},
										{"���������������������,���������������������,���������������,������������������������,������������."},
										{"���������,������������������,������������������,���������������������������,������������,������������������������������,������������."},
										{"���������,������������������,���������������,������������"},
										{"Emergency message 5"},
										{"Emergency message 6"},
										{"Emergency message 7"},
										{"Emergency message 8"},
										{"Emergency message 9"}
										};

pcu_state_on_ccu_t pcu_state_on_ccu[20] = {//���������PCU���������
										{0,0,0},
										{1,0,0},
										{2,0,0},
										{3,0,0},
										{4,0,0},
										{5,0,0},
										{6,0,0},
										{7,0,0},
										{8,0,0},
										{9,0,0},
										{10,0,0},
										{11,0,0},
										{12,0,0},
										{13,0,0},
										{14,0,0},
										{15,0,0},
										{16,0,0}
										};

int CcuBufferInit(void)
{//CCU ������������������
        unsigned char buffer_fail = 0;
   if( 0==BlockBufferCreate(3, sizeof(common_package_t), "ccu-ddu-tms-simulate-Rx"))
   {
		diag_printf("CcuBufferInit:  ccu-ddu-tms-simulate-Rx, fail\n");
		buffer_fail++;
   }
	if( 0==BlockBufferCreate(8, 68, "ccu-true-tms-cmd-Tx") )
	{
		diag_printf("CcuBufferInit:  ccu-true-tms-cmd-Tx, fail\n");
		buffer_fail++;
	}
	if( 0==BlockBufferCreate(8, 67, "ccu-tcms-log-info") )
	{
		diag_printf("CcuBufferInit:  ccu-tcms-log-info, fail\n");
		buffer_fail++;
	}
	if( 0==BlockBufferCreate(8, 68, "ccu-true-tms-cmd-Rx") )
	{
		diag_printf("CcuBufferInit:  ccu-true-tms-cmd-Rx, fail\n");
		buffer_fail++;
	}

	if( 0==BlockBufferCreate(8, sizeof(notice_ccu_send_to_media_t), "ccu-media-cmd-Tx") )
	{
	  diag_printf("CcuBufferInit:  ccu-media-cmd-Tx, fail\n");
	  buffer_fail++;
	}

	if( 0==BlockBufferCreate(8, sizeof(network_send_package_t), "ccu-network-cmd-Tx") )
	{
              diag_printf("CcuBufferInit:  ccu-network-cmd-Tx, fail\n");
              buffer_fail++;
	}
	if( 0==BlockBufferCreate(24, sizeof(network_send_package_t), "ccu-network-cmd-Rx") )
	{
              diag_printf("CcuBufferInit:  ccu-network-cmd-Rx, fail\n");
              buffer_fail++;			  
	}
	if( 0==BlockBufferCreate(8, sizeof(common_big_package_t), "ccu-tms-cmd-Rx") )
	{
              diag_printf("CcuBufferInit:  ccu-tms-cmd-Rx, fail\n");
              buffer_fail++;			  
	}	
	if( 0==BlockBufferCreate(8, sizeof(common_big_package_t), "ccu-tms-cmd-Tx") )
	{
              diag_printf("CcuBufferInit:  ccu-tms-cmd-Tx, fail\n");
              buffer_fail++;			  
	}	
	if( 0==BlockBufferCreate(5, sizeof(common_package_t), "network-udp-common-send") )
	{
              diag_printf("CcuBufferInit:  network-udp-common-send, fail\n");
              buffer_fail++;			  
	}
	if( 0==BlockBufferCreate(5, sizeof(common_package_t), "intercomm-audio_data-record-recv") )
	{
              diag_printf("CcuBufferInit:  network-udp-common-send, fail\n");
              buffer_fail++;
	}


	if( 0==BlockBufferCreate(2, sizeof(common_package_t), "ccu-get-pcu-state-info") )
	{
			  diag_printf("CcuBufferInit:  ccu-get-pcu-state-info, fail\n");
			  buffer_fail++;
	}
	if( 0==BlockBufferCreate(16, sizeof(common_package_t), "network-udp-common-recv") )
	{
              diag_printf("CcuBufferInit:  network-udp-common-recv, fail\n");
              buffer_fail++;			  
	}
	if( 0==BlockBufferCreate(3, sizeof(common_package_t), "network-udp-common-dev-vol-recv") )
	{
              diag_printf("CcuBufferInit:  network-udp-common-recv, fail\n");
              buffer_fail++;
	}

        if( 0==BlockBufferCreate(5, sizeof(common_big_package_t), "network-udp-common-big-send") )
        {
             diag_printf("CcuBufferInit:  network-udp-common-big-send, fail\n");
             buffer_fail++;			  
        }	
        if( 0==BlockBufferCreate(16, sizeof(common_big_package_t), "network-udp-common-big-recv") )
        {
              diag_printf("CcuBufferInit:  network-udp-common-big-recv, fail\n");
              buffer_fail++;			  
        }

	if(0==CharBufferCreate((8),"ccu-local-leave-audio-file-one"))
	{
              diag_printf("CcuBufferInit:  ccu-local-leave-audio-file-one, fail\n");
              buffer_fail++;			  
	}
	if( 0==CharBufferCreate((8), "ccu-local-stop-audio-file-one") )
	{
              diag_printf("CcuBufferInit:  ccu-local-stop-audio-file-one, fail\n");
              buffer_fail++;			  
	}
	if( 0==CharBufferCreate((8),"ccu-local-leave-audio-file-two"))
	{
              diag_printf("CcuBufferInit:  ccu-local-leave-audio-file-two, fail\n");
              buffer_fail++;			  
	}
	if( 0==CharBufferCreate((8), "ccu-local-stop-audio-file-two") )
	{
              diag_printf("CcuBufferInit:  ccu-local-stop-audio-file-two, fail\n");
              buffer_fail++;			  
	}

	if( 0==CharBufferCreate((1024 * 64), "ccu-local-audio-file") )
	{
              diag_printf("CcuBufferInit:  ccu-local-audio-file, fail\n");
              buffer_fail++;
	}

	if( 0==CharBufferCreate(((1024 * 3 + 64)), "ccu-local-pending-audio-file") )
	{
              diag_printf("CcuBufferInit:  ccu-local-pending-audio-file\n");
              buffer_fail++;
	}

        if( 0==BlockBufferCreate(3, sizeof(audio_play_control_t), "ccu-audio-play-cmd-Rx") )
	{
              diag_printf("CcuBufferInit:  ccu-audio-play-cmd-Rx, fail\n");
              buffer_fail++;			  
	}

        if( 0==BlockBufferCreate(5, sizeof(common_big_package_t), "ccu-gd-net-cmd-Rx") )
	{
              diag_printf("CcuBufferInit:  ccu-gd-net-big-cmd-Rx, fail\n");
              buffer_fail++;			  
	}	
        if( 0==BlockBufferCreate(5, sizeof(common_big_package_t), "ccu-get-bcu-log-info") )
	{
              diag_printf("CcuBufferInit:  ccu-get-bcu-log-info, fail\n");
              buffer_fail++;
	}

        ///< add begin,ach,12-21:   ccu send OCC audio to EAMP via Net
	if( 0==CharBufferCreate((1024 * 16), "ccu-network-audio-Tx") )
	{
              diag_printf("CcuBufferInit:  ccu-network-audio-Tx, fail\n");
              buffer_fail++;			  
	}
	if( 0==CharBufferCreate((1024 * 16 * 4), "ccu-network-audio-Rx") )
	{
              diag_printf("CcuBufferInit:  ccu-network-audio-Rx, fail\n");
              buffer_fail++;
	}
        ///< add end, ach, 12-21:   ccu send OCC audio to EAMP via Net	
	if ( 0 == BlockBufferCreate(3, sizeof(network_heart_package_t), "network-udp-heart-recv"))
	{
		diag_printf("CcuBufferInit:  network-udp-heart-recv, fail\n");
		 buffer_fail++;
	}
	if ( 0 == BlockBufferCreate(3, sizeof(network_heart_package_t), "network-udp-heart-send"))
	{
		diag_printf("CcuBufferInit:  network-udp-heart-recv, fail\n");
		 buffer_fail++;
	}
	if(0 == BlockBufferCreate(3, sizeof(common_big_package_t), "network-udp-common-big-recv-version-check"))
	{
		buffer_fail++;
	}
	if(0 == BlockBufferCreate(3, sizeof(common_package_t), "network-udp-common-TCMS-cmu-state"))
	{
		buffer_fail++;
	}

	//add by zhw 2015-12-23
	//if( 0==CharBufferCreate((500), "ccu-monitor-command-send") )
	if( 0==BlockBufferCreate(3, (64), "ccu-monitor-command-send") )		
	{
              diag_printf("CcuBufferInit:  ccu-monitor-command-send, fail\n");
              buffer_fail++;
	}
	if ( 0 == BlockBufferCreate(2, 32, "ccu-monitor-command-recv"))
	{
		diag_printf("CcuBufferInit:  ccu-monitor-command-recv, fail\n");
		 buffer_fail++;
	}
	//end of add

	#ifdef CONFIG_CCU_VIDEOLINK
	if ( 0 == BlockBufferCreate(3, 32, "ccu-videolink-cmd-rx"))
	{
		diag_printf("CcuBufferInit:  ccu-videolink-cmd-rx, fail\n");
		 buffer_fail++;
	}
	if ( 0 == BlockBufferCreate(5, 1024, "ccu-videolink-cmd-tx")) ///< send LED-context
	{
		diag_printf("CcuBufferInit:  ccu-videolink-cmd-tx, fail\n");
		 buffer_fail++;
	}	
	#endif

	if ( 0 == CharBufferCreate((1024*3), "ccu-mvb-cmd-rx") )
	{
		diag_printf("CcuBufferInit:  ccu-mvb-cmd-rx, fail\n");
		 buffer_fail++;
	}
	
	if ( 0 == BlockBufferCreate(2, sizeof(network_shell_package_t),"network-udp-shell-recv") )
	{
		buffer_fail++;
	}

	if ( 0 == BlockBufferCreate(2, sizeof(network_shell_package_t),"network-udp-shell-send") )
	{
		buffer_fail++;
	}
	if( buffer_fail )
	{
        return -1;
	}
	
	return 0;
}


int CcuGetCurrentLineNumber(void)
{//CCU ���������������������������
	return (int)(ccu_state.current_line_no);
}
int CcuGetCurrentSectionNumber(void)
{//CCU ������������������������
	return (int)(ccu_state.current_section_no);
}

int LEDShowTerminalStation()
{// ���������LED������������������
	CCUFduAnnouncement(ccu_state.current_line_no,ccu_state.ccu_region_info[ccu_state.current_line_no - 3].p_regin_info->over_station_no,
			ccu_state.ccu_region_info[ccu_state.current_line_no - 3].p_regin_info->whether_the_cycle_bus,
			ccu_state.ccu_region_info[ccu_state.current_line_no - 3].p_regin_info->running_direction,
			                0);///<modify on 2013/07/23
	return 0;
}

/*Chaneg thread's priority*/
int PthreadPriorityChangeForSchedRr(pthread_t  target_thread_id, int  new_prior )
{
	  struct sched_param param_temp;

	  param_temp.sched_priority = new_prior;
	  pthread_setschedparam(target_thread_id, SCHED_RR, &param_temp);
	  sched_yield();
	  return 0;
}

void CCUStateInit()
{// CCU���������������
	struct dev_type_and_no temp_dev_type_and_no;
	char dev_name[] = "CCU";
	temp_dev_type_and_no = ReadFromIO();
	diag_printf("no:%d\n",temp_dev_type_and_no.dev_number);
	diag_printf("type:%d\n",temp_dev_type_and_no.dev_type);
	diag_printf("ReadType:%d\n",ReadType());

	ccu_state.ccu_tms_car_running_info.current_time.year = 13;
	ccu_state.ccu_tms_car_running_info.current_time.month = 10;
	ccu_state.ccu_tms_car_running_info.current_time.date = 15;
	ccu_state.ccu_tms_car_running_info.current_time.hour = 00;
	ccu_state.ccu_tms_car_running_info.current_time.minute = 00;
	ccu_state.ccu_tms_car_running_info.current_time.second = 00;
	ccu_state.current_line_no = 3;
	ccu_state.key_info = 0;
	ccu_state.ccu_cmu_state = 0;

	ccu_state.pp_skip_station_info = station_information;
	ccu_state.pp_station_info = station_information;
	ccu_state.led_language = 0;

	ccu_state.ccu_info.devices_no = temp_dev_type_and_no.dev_number;
	if(ccu_state.ccu_info.devices_no == 1)
	{
		ccu_state.other_ccu_no = 2;
	}
	else if(ccu_state.ccu_info.devices_no == 2)
	{
		ccu_state.other_ccu_no = 1;
	}
//	heart_beat_other_ccu_no = ccu_state.other_ccu_no;

	strcpy(ccu_state.ccu_info.devices_name,"CCU");
	ccu_state.tms_effective_flag = 1;
	ccu_state.p_ccu_emerg_info = &ccu_emerg_info;
	memset((void *)&ccu_state.pcu_state_to_tcms[0],0x00,4);
	memset((void *)&ccu_state.pcu_state_to_monitor[0],0x00,7);
	memset((void *)&ccu_state.pa_failure_info[0],0x00,32);
	memset((void *)&ccu_state.pa_running_info_to_tms[0],0x00,32);
	ccu_state.pa_running_info_to_tms[24] |= 0x01 ;

	///<���������������������
	ccu_state.pa_running_info_to_tms[6] = 1;
	ccu_state.pa_running_info_to_tms[7] = 0;
	ccu_state.pa_running_info_to_tms[8] = 8;

	///<���������������������
	ccu_state.pa_running_info_to_tms[9] = 1;
	ccu_state.pa_running_info_to_tms[10] = 0;
	ccu_state.pa_running_info_to_tms[11] = 16;

	///<���������������������
	ccu_state.pa_running_info_to_tms[12] = 1;
	ccu_state.pa_running_info_to_tms[13] = 0;
	ccu_state.pa_running_info_to_tms[14] = 1;

	ccu_state.pa_running_info_to_tms[21] = ccu_state.current_pa_volume;//DEFAULT_CAR_VOLUME;
//	ccu_state.current_pa_volume = DEFAULT_CAR_VOLUME;
}
void InitCCUDataBaseInfo()
{//���������CCU���������������
#ifndef WITH_FDU

	 ///������������������������������
	 GetSimulateVariableInfo();

	/*Get default line no from the database*/
	GetLineInfo();

	/*Get all stations's name from the database*/
	GetAllStationsName();

	/*Get emerg info from the database*/
	GetRegionInfo();
	ShowAllRegionInfo(3);
//	ShowAllRegionInfo(4);

	GetDefaultRegion();

	UpdateSkipStationInformation(3);
	UpdateSkipStationInformation(4);

//	 CreateCCUEmergSpace(EMERG_DEBUG_FORTS_TOTAL_ITEMS);
//	 InitCCUEmergInfo();
	 GetEmergInfo();

#else
		CreateCCUEmergSpace(EMERG_DEBUG_FORTS_TOTAL_ITEMS);
			InitCCUEmergInfo();

			CreateCCURegionSpace(3,3);
			CreateCCURegionSpace(4,7);
			InitCCURegionInfo();
			ccu_state.pp_station_info = station_information;

			ccu_state.current_line_no = 3;
			ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no = 1;

			GetCurrentregionPosition(ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no);
			UpdateRegionStationInfo(ccu_state.current_line_no,ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_position);

#endif

}
void GetCurrentregionPosition(unsigned int param_region_no)
{//���������������������������������������������������
	int i = 0;
	for(i = 0;i < ccu_state.ccu_region_info[ccu_state.current_line_no - 3].total_regions_number;i ++)
	{
		if(param_region_no == (ccu_state.ccu_region_info[ccu_state.current_line_no - 3].p_regin_info + i)->region_no)
		{
			ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_position = i + 1;
			break;
		}
	}
}
void UpdateRegionStationInfo(unsigned char param_line_no,unsigned char param_region_position)
{//������������������������
	ccu_state.ccu_region_info[param_line_no - 3].current_region_no =
			(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info  + param_region_position -1)->region_no;

	ccu_state.ccu_region_info[param_line_no - 3].current_region_info.region_no =
			(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info  + param_region_position -1)->region_no;

	ccu_state.ccu_region_info[param_line_no - 3].current_region_info.line_no = param_line_no;

	ccu_state.ccu_region_info[param_line_no - 3].current_region_info.start_station_no  =
			(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info  + param_region_position -1)->start_station_no;

	ccu_state.ccu_region_info[param_line_no - 3].current_region_info.over_station_no  =
			(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info  + param_region_position -1)->over_station_no;

	ccu_state.ccu_region_info[param_line_no - 3].current_region_info.running_direction  =
			(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info  + param_region_position -1)->running_direction;

	ccu_state.ccu_region_info[param_line_no - 3].current_region_info.whether_the_cycle_bus  =
			(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info  + param_region_position -1)->whether_the_cycle_bus;

	ccu_state.pa_running_info_to_tms[15] = ccu_state.ccu_region_info[param_line_no - 3].current_region_info.start_station_no;
	ccu_state.pa_running_info_to_tms[16] = ccu_state.ccu_region_info[param_line_no - 3].current_region_info.over_station_no;
	ccu_state.pa_running_info_to_tms[17] = ccu_state.ccu_region_info[param_line_no - 3].current_region_info.start_station_no;
	ccu_state.pa_running_info_to_tms[18] = ccu_state.ccu_region_info[param_line_no - 3].current_region_info.start_station_no;
}
void InitPCUDeviceState()
{//���������PCU���������������
	int i = 0;
	for(i = 0;i < 16;i++)
	{
		ccu_state.door_emerg_unlock_state_and_smoke_info[i] = 0;
	}
	for(i = 1;i <= 12;i++)
	{
		pcu_state_on_ccu[i].pcu_connenting_state = 0;
		pcu_state_on_ccu[i].pcu_no = i;
		pcu_state_on_ccu[i].pcu_request_state = 0;
	}
}

int GetSimulateVariableInfo()
{///������������������������������
	int row = 0;
	int column = 0;
	void *p_simulate_variable_table = NULL;
	simulate_variable_info_t *p;
	int ret = 0;
	int k = 0 ;

	SimulateVariableOfEntry(&row, &column, &p_simulate_variable_table);
	p  = (simulate_variable_info_t *)p_simulate_variable_table;
	for(k = 0;k < row;k++)
	{
		if(strcmp(p->type,"DDU-SPEED-INFO") == 0)
		{
			ccu_state.db_setting_speed = (unsigned char)atoi(p->value);
			diag_printf("$$-%s,%s,%s\n",p->type,p->value,p->explanation);
		}
		if(strcmp(p->type,"TS_PASSWORD_INFO") == 0)
		{
			strncpy(ccu_state.ts_password,p->value,6);
			diag_printf("$$-%s,%s,%s\n",p->type,p->value,p->explanation);
		}
		p ++;
	}
	diag_printf("ccu_state.db_setting_speed = %d\n",ccu_state.db_setting_speed);
	diag_printf("ccu_state.ts_password = %s\n",ccu_state.ts_password);
	if(ccu_state.db_setting_speed == 0)
	{
		ccu_state.db_setting_speed = 80;
	}
	SqlTableBufferFree(&p_simulate_variable_table);
	return ret;
}

int GetLineInfo()
{//������������������������������������
	int row = 0;
	int column = 0;
	void *p_line_table = NULL;
	line_table_t *p;
	int current_line_no;
	int ret = 0;
	int k = 0 ;

	SqlOfTableTline(&row, &column, &p_line_table);
	p  = (line_table_t *)p_line_table;

	if ((current_line_no = JudgeDefaultLine(p_line_table, row)) < 0)
	{
		ret = -1;
	}
	else
	{
		ccu_state.current_line_no = current_line_no;
	}
	SqlTableBufferFree(&p_line_table);
	return ret;
}
void GetAllStationsName()
{//������������������������������������������
	int j = 0;
	int k = 0;
	int row = 0;
	int column = 0;
	void *p_station_code = NULL;
	station_code_of_line_t *p;

	for(k = 3;k <= 3;k ++)
	{
		int ret  =  0;
		ret = StationCodeOfEntry(k, &row, &column, &p_station_code);
		if(ret != 0)
		{
		//	while(1)
			{
				diag_printf("StationCodeOfEntry ret = %d\n",ret);
			}
		}
		ccu_state.ccu_region_info[k - 3].total_stations_number = row;
		p = (station_code_of_line_t *)p_station_code;
		for(j = 0;j < row && j < MAX_STATION_NUMBER;j++)
		{
			if((p + j)->station_no < MAX_STATION_NUMBER)
			{
				ccu_state.pp_station_info[k - 3][(p + j)->station_no].line_number = 3;
				ccu_state.pp_station_info[k - 3][(p + j)->station_no].station_no = (p + j)->station_no;

				if((p + j)->fix_across_station == 1)
				{
					ccu_state.pp_station_info[k - 3][(p + j)->station_no].effective = 2;
				}
				else
				{
					ccu_state.pp_station_info[k - 3][(p + j)->station_no].effective = 1;
				}

				diag_printf("zhw ***** station name = %d-%d-%d\n",(p + j)->station_display[0],(p + j)->station_display[1],(p + j)->station_display[2]);
				diag_printf("zhw ***** station name = %d\n",(p + j)->station_no);

				strncpy(ccu_state.pp_station_info[k - 3][(p + j)->station_no].station_chinese_name,
						(p + j)->station_display,
						sizeof(ccu_state.pp_station_info[k - 3][(p + j)->station_no].station_chinese_name));

				strncpy(ccu_state.pp_station_info[k - 3][(p + j)->station_no].station_english_name,
						(p + j)->station_display_eng,
						sizeof(ccu_state.pp_station_info[k - 3][(p + j)->station_no].station_english_name));
			}
		}
	}
	SqlTableBufferFree(&p_station_code);
}

/*Update skip station information*/
void UpdateSkipStationInformation(send_infomation_t *param_cmd_info)
{
	if(param_cmd_info->event_type_ann == SKIP_STATION &&
	   param_cmd_info->event_type_intercom == INTERCOM_IDLE)
	{
		if(param_cmd_info->event_info_ann.station_information.line_number != ccu_state.current_line_no)
		{
			debug_print(("Line number is error\n"));
			param_cmd_info->event_type_ann  = ANN_IDLE;
			param_cmd_info->event_type_intercom = INTERCOM_IDLE;
			param_cmd_info->event_info_ann.line_info.line_no = 0;
			return ;
		}
		station_information[param_cmd_info->event_info_ann.station_information.line_number - 3][param_cmd_info->event_info_ann.station_information.station_no].effective = param_cmd_info->event_info_ann.station_information.effective;

		param_cmd_info->event_type_ann  = ANN_IDLE;
		param_cmd_info->event_type_intercom = INTERCOM_IDLE;
		param_cmd_info->event_info_ann.line_info.line_no = 0;

		ShowStationInformation(ccu_state.current_line_no);
	}
}
/*Show the station information*/
void ShowStationInformation(int line_number_of_skip_station)
{
	int i = 0;
	debug_print(("@@@@@@@@@@@\n"));
	for(i = 0; i < MAX_STATION_NUMBER; i++)
	{
		if(station_information[line_number_of_skip_station - 3][i].effective != 1)
		{
			debug_print(("line:%d_%d:%d\n",line_number_of_skip_station,i,station_information[line_number_of_skip_station - 3][i].effective));//
		}
	}
	debug_print(("@@@@@@@@@@@\n"));
}

void GetDefaultRegion()
{//������������������
	ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no =
			ccu_state.ccu_region_info[ccu_state.current_line_no - 3].default_region_no;
	UpdateRegionInfo(ccu_state.current_line_no,ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no);
}

void GetRegionInfo()
{//������������������������������
	int row = 0;
	int column = 0;
	void *p_line_section = NULL;
	line_section_table_t *p;
	int k = 0;
	int j = 0;
	int section = 0;
	for(k = 3;k <= 3;k++)
	{
		LineSectionOfEntry(k, &row, &column, &p_line_section);
		p = (line_section_table_t *)p_line_section;
		ccu_state.ccu_region_info[k - 3].total_regions_number = row;
		CreateCCURegionSpace(k,row);
		if ((section = JudgeDefaultSecion(p_line_section, row)) < 0)
		{
			perror("error on JudgeDefaultSecion\n");
			return ;
		}
		else
		{
			ccu_state.ccu_region_info[k - 3].default_region_no =  section;
		}
		for(j = 0;j < row;j++)
		{
			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->line_no = k;
			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->region_no = (p + j)->section_no;
			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->start_station_no = (p + j)->start_station;
			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->over_station_no = (p + j)->end_station;

			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->current_region_flag = j + 1;
			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->running_direction = (p + j)->line_direction + 1;
			debug_print(("Line %d,%d\n",k,(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->running_direction));
			if((ccu_state.ccu_region_info[k - 3].p_regin_info + j)->running_direction == 1)
			{
				(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->running_direction = 2;
			}
			else
			{
				(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->running_direction = 1;
			}
# if 0
			if((ccu_state.ccu_region_info[k - 3].p_regin_info + j)->start_station_no == (ccu_state.ccu_region_info[k - 3].p_regin_info + j)->over_station_no)
			{
				(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->whether_the_cycle_bus = 1;
			}
			else
			{
				(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->whether_the_cycle_bus = 0;
			}
#else
			(ccu_state.ccu_region_info[k - 3].p_regin_info + j)->whether_the_cycle_bus = (p + j)->spec_cycle;
#endif


			strncpy((ccu_state.ccu_region_info[k - 3].p_regin_info + j)->start_time,(p + j)->start_time,
					sizeof((ccu_state.ccu_region_info[k - 3].p_regin_info + j)->start_time));
			strncpy((ccu_state.ccu_region_info[k - 3].p_regin_info + j)->end_time,(p + j)->end_time,
					sizeof((ccu_state.ccu_region_info[k - 3].p_regin_info + j)->end_time));
		}
	}
	SqlTableBufferFree(&p_line_section);
}

void UpdateRegionInfo(int param_line_no,int param_region_no)
{//������������������������
	GetCurrentregionPosition(param_region_no);
	UpdateRegionStationInfo( param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_position);
	ShowRegionInfo(param_line_no);
}
void ShowRegionInfo(int param_line_no)
{
	return ;
//	int param_line_no = ccu_state.current_line_no;
	diag_printf("line%d:region_no = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.region_no);
	diag_printf("line%d:start_station_no = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.start_station_no);
	diag_printf("line%d:over_station_no = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.over_station_no);
	diag_printf("line%d:running_direction = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.running_direction);
	diag_printf("line%d:whether_the_cycle_bus = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.whether_the_cycle_bus);
}
void ShowRegionInfo_diag_printf(int param_line_no)
{
	//return ;
	diag_printf("line%d:region_no = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.region_no);
	diag_printf("line%d:start_station_no = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.start_station_no);
	diag_printf("line%d:over_station_no = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.over_station_no);
	diag_printf("line%d:running_direction = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.running_direction);
	diag_printf("line%d:whether_the_cycle_bus = %d\n",param_line_no,ccu_state.ccu_region_info[param_line_no - 3].current_region_info.whether_the_cycle_bus);
}
void ShowAllRegionInfo(int param_line_no)
{
//	return ;
	int k = 0;
	diag_printf("AA-%d\n",ccu_state.ccu_region_info[param_line_no - 3].total_regions_number);
	for(k = 0;k < ccu_state.ccu_region_info[param_line_no - 3].total_regions_number;k++)
	{
		diag_printf("line%d:region_no = %d\n",
				param_line_no,(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info + k)->region_no);
		diag_printf("line%d:start_station_no = %d\n",
						param_line_no,(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info + k)->start_station_no);
		diag_printf("line%d:over_station_no = %d\n",
						param_line_no,(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info + k)->over_station_no);
		diag_printf("line%d:running_direction = %d\n",
						param_line_no,(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info + k)->running_direction);
		diag_printf("line%d:whether_the_cycle_bus = %d\n",
						param_line_no,(ccu_state.ccu_region_info[param_line_no - 3].p_regin_info + k)->whether_the_cycle_bus);
	}
	diag_printf("@@@@@@@@@@@@@@@@\n");
}

void GetEmergInfo()
{//���������������������������������������
#if 1
	int temp_total_row = 0;
	int temp_total_col = 0;
	void *p_emerg_info = NULL;
	emerg_table_item_t *p;
//	debug_print(("111111111\n"));
	EmergencyOfEntry(0, &temp_total_row,&temp_total_col,&p_emerg_info);
//	debug_print(("222\n"));
	p = (emerg_table_item_t *)p_emerg_info;
	ccu_state.p_ccu_emerg_info->emerg_total_items = temp_total_row;
	ccu_state.p_ccu_emerg_info->emerg_table_items = malloc(sizeof(emerg_table_items_t)  * temp_total_row);
	int k = 0;
	for(k = 0; k < ccu_state.p_ccu_emerg_info->emerg_total_items;k++)
	{
//		debug_print(("k = %d,%d,%d,%d,%s\n",k, (p + k)->emerg_ann_id,(p + k)->line_no,(p + k)->repeat_times,(p + k)->audio_path));
		(ccu_state.p_ccu_emerg_info->emerg_table_items + k)->emerg_ann_id = 0;
		(ccu_state.p_ccu_emerg_info->emerg_table_items + k)->emerg_ann_id = (p + k)->emerg_ann_id;
		(ccu_state.p_ccu_emerg_info->emerg_table_items + k)->line_no = (p + k)->line_no;
		(ccu_state.p_ccu_emerg_info->emerg_table_items + k)->repeat_times = (p + k)->repeat_times;
		strncpy((ccu_state.p_ccu_emerg_info->emerg_table_items+k)->audio_path,(p + k)->audio_path,sizeof((ccu_state.p_ccu_emerg_info->emerg_table_items+k)->audio_path));
//		debug_print(("display_message - %s\n%s\n",(ccu_state.p_ccu_emerg_info->emerg_table_items+k)->display_message,
//				(p + k)->display_message	));
		strncpy((ccu_state.p_ccu_emerg_info->emerg_table_items+k)->emerg_ann_name,(p + k)->emerg_ann_name,sizeof((ccu_state.p_ccu_emerg_info->emerg_table_items+k)->emerg_ann_name));
		strncpy((ccu_state.p_ccu_emerg_info->emerg_table_items+k)->display_message,(p + k)->display_message,sizeof((ccu_state.p_ccu_emerg_info->emerg_table_items+k)->display_message));
	}

	SqlTableBufferFree(&p_emerg_info);
#endif
}

int GetHardwareKeyStatus()
{// ������������������������
	int ret = 0;
	if(ccu_read_tc_key_in_status() == 0)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}
//	diag_printf("GetHardwareKeyStatus = %d\n",ret);
//	ret = 1;
	return ret;
}

int GetDriverKeyStatus()
{//���������������������������������������������������������������������������������������������
//	return GetHardwareKeyStatus();
	int ret = 0;
	if(GetHardwareKeyStatus() == 1)
	{
		cyg_thread_delay(10);
		if(GetHardwareKeyStatus() == 1)
		{
			cyg_thread_delay(10);
			if(GetHardwareKeyStatus() == 1)
			{
				ret = 1;
			}
		}
	}
	return ret ;
}

void GetKeyInfoNew(int flag)
{//���������������������������������������������������
	if(flag == 1)///<key info
	{
		if(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] == 0)
		{
			ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] = 1;
			SendKeyInfo(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1]);
			debug_print(("this ccu get key single\n"));
			diag_printf("this ccu get key single\n");
			UpdateGlobalkeyInfo();
		}
	}
	else
	{
//		diag_printf("B2016: %d,%d,%d\n", ccu_state.key_status[ccu_state.ccu_info.devices_no - 1],
//			ccu_state.ccu_info.devices_no,
//			ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] );
		if(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] == 1)
		{
			ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] = 0;
			SendKeyInfo(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1]);
			debug_print(("this ccu lost key single\n"));
			diag_printf("this ccu lost key single\n");
			UpdateGlobalkeyInfo();
		}
	}
	//diag_printf("ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] = %d\n",ccu_state.key_status[ccu_state.ccu_info.devices_no - 1]);
}

void SendKeyInfo(unsigned char param_key_status)
{//������������������
	network_send_package_t network_send_package;
	int ret = 0;
	strcpy(network_send_package.dst_devices_name,"BCU");
	network_send_package.dst_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(network_send_package.send_information.src_devices_name,"CCU");
	network_send_package.send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	network_send_package.send_information.event_type_ann = PA_KEY_INFORMATION;
	network_send_package.send_information.event_type_intercom = INTERCOM_IDLE;
	network_send_package.send_information.event_info_ann.pa_key_info.get_or_set_key_info = 0;
	network_send_package.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.ccu_info.devices_no-1] = param_key_status;
	network_send_package.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.other_ccu_no-1] = ccu_state.key_status[ccu_state.other_ccu_no - 1];
	network_send_package.send_information.event_info_ann.pa_key_info.line_no = ccu_state.current_line_no;
	network_send_package.send_information.event_info_ann.pa_key_info.region_no = ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no;
	do
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &network_send_package, sizeof(network_send_package));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0);

//	strcpy(network_send_package.dst_devices_name,"EAMP");
//	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &network_send_package, sizeof(network_send_package));

	strcpy(network_send_package.dst_devices_name,"CCU");
	network_send_package.dst_devices_no = ccu_state.other_ccu_no;
	do
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &network_send_package, sizeof(network_send_package));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
		}while(ret <= 0);
}

void SendTMSKeyInfo()
{//������TCMS������������
	network_send_package_t network_send_package;
	strcpy(network_send_package.dst_devices_name,"BCU");
	network_send_package.dst_devices_no = MUL_DST_NO;
	network_send_package.send_information.event_type_ann = PA_KEY_INFORMATION;
	network_send_package.send_information.event_type_intercom = INTERCOM_IDLE;
	network_send_package.send_information.event_info_ann.pa_key_info.get_or_set_key_info = 0;
	network_send_package.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.ccu_info.devices_no-1] = ccu_state.key_status[ccu_state.ccu_info.devices_no - 1];
	network_send_package.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.other_ccu_no-1] = ccu_state.key_status[ccu_state.other_ccu_no - 1];
	network_send_package.send_information.event_info_ann.pa_key_info.line_no = ccu_state.current_line_no;
	network_send_package.send_information.event_info_ann.pa_key_info.region_no = ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no;
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &network_send_package, sizeof(network_send_package));

	strcpy(network_send_package.dst_devices_name,"EAMP");
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &network_send_package, sizeof(network_send_package));

//	strcpy(network_send_package.dst_devices_name,"CCU");
//	network_send_package.dst_devices_no = ccu_state.other_ccu_no;
//	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &network_send_package, sizeof(network_send_package));
}

void UpdateGlobalkeyInfo()
{//���������������������������������
	if(ccu_state.key_status[0] == 1)
	{
		ccu_state.key_info = 1;//TC1������
	}
	else if(ccu_state.key_status[1] == 1)
	{
		ccu_state.key_info = 2;//TC2������
	}
	else
	{
		ccu_state.key_info = 0;//���������������
	}
	diag_printf("ccu_state.key_info = %d\n",ccu_state.key_info);
}

void CCUControlBufferInit()
{//CCU������������������
	ccu_state.tms_cmd_recv_buffer_id = BlockBufferOpen("ccu-tms-cmd-Rx");
	ccu_state.tms_cmd_send_buffer_id = BlockBufferOpen("ccu-tms-cmd-Tx");
	ccu_state.common_big_send_buffer_id = BlockBufferOpen("network-udp-common-big-send");
	ccu_state.ccu_recv_audio_data_buffer_id = CharBufferOpen("ccu-network-audio-Rx");
	ccu_state.ccu_send_audio_data_buffer_id = CharBufferOpen("ccu-network-audio-Tx");

	ccu_state.ccu_recv_cmd_buffer_id = BlockBufferOpen("ccu-network-cmd-Rx");
	ccu_state.ccu_send_cmd_buffer_id = BlockBufferOpen("ccu-network-cmd-Tx");

	ccu_state.tms_true_cmd_recv_buffer_id = BlockBufferOpen("ccu-true-tms-cmd-Rx");
	ccu_state.tms_true_cmd_send_buffer_id = BlockBufferOpen("ccu-true-tms-cmd-Tx");


	ccu_state.ccu_recv_common_cmd_buffer_id = BlockBufferOpen( "network-udp-common-recv");
	ccu_state.ccu_send_common_cmd_buffer_id = BlockBufferOpen( "network-udp-common-send");

	ccu_state.ccu_tcms_log_info = BlockBufferOpen( "ccu-tcms-log-info");

	ccu_state.ccu_intercomm_audio_data_record_recv_buffer_id = BlockBufferOpen("intercomm-audio_data-record-recv");
	ccu_state.software_version_buffer_id = BlockBufferOpen("network-udp-common-big-recv-version-check");

	ccu_state.ccu_cmu_state_buffer_id = BlockBufferOpen("network-udp-common-TCMS-cmu-state");

	ccu_state.shell_recv_buffer_id = BlockBufferOpen("network-udp-shell-recv");
	ccu_state.shell_send_buffer_id = BlockBufferOpen("network-udp-shell-send");
	
	ZhwBufferRegister(1,COMMON_PACKAGE_TYPE_TCMS_CMU_STATE,1,ccu_state.ccu_cmu_state_buffer_id);

	ZhwBufferRegister(2,TMS_PACKAGE_ID,1,ccu_state.tms_cmd_recv_buffer_id);
	ZhwBufferRegister(1,COMMON_PACKAGE_TYPE_INTERCOMM_LOG,1,ccu_state.ccu_intercomm_audio_data_record_recv_buffer_id);
	ZhwBufferRegister(2,COMMON_PACKAGE_TYPE_SOFTWARE_VERSION,1,ccu_state.software_version_buffer_id);
}

void AlarmPeriDeviceThatLineOrRegionHasChanged()
{//���������������������������������������
	ClearSkipStationInfo(ccu_state.current_line_no);
	ccu_state.led_language = 0;
	whether_show_line4_cycle_terminal_station = 1;
	whether_send_serial_fdu_data = 1;
	whether_send_fdu_can_data = 1;
	RestartFduShowTimer();
	sem_post(&sem_wakeup_eamp_led);
}

void SetSkipStationResponseToTCMS(unsigned char line_no,unsigned char station_no,unsigned char skip_station_flag)
{
	if(line_no == 3 && skip_station_flag <= 1)
	{
		if(station_no >= 1 && station_no <= 3)
		{
			ccu_state.pa_failure_info[15] &= ((0xDF << (station_no-1)) | (0xDF >> (9-station_no)));
			ccu_state.pa_failure_info[15] |= ((1-skip_station_flag) << (station_no + 4));
		}
		else if(station_no >= 4 && station_no <= 11)
		{
			ccu_state.pa_failure_info[16] &= ((0xFE << (station_no-4)) | (0xFE >> (12 - station_no)));
			ccu_state.pa_failure_info[16] |= ((1-skip_station_flag) << (station_no - 4));
		}
		else if(station_no >= 12 && station_no <= 19)
		{
			ccu_state.pa_failure_info[17] &= ((0xFE << (station_no-12)) | (0xFE >> (20 - station_no)));
			ccu_state.pa_failure_info[17] |= ((1-skip_station_flag) << (station_no -12));
		}
		else if(station_no >= 20 && station_no <= 27)
		{
			ccu_state.pa_failure_info[18] &= ((0xFE << (station_no-20)) | (0xFE >> (28- station_no)));
			ccu_state.pa_failure_info[18] |= ((1-skip_station_flag) << (station_no -20));
		}
		else if(station_no >= 28 && station_no <= 29)
		{
			ccu_state.pa_failure_info[19] &= ((0xFE << (station_no-28)) | (0xFE >> (36 - station_no)));
			ccu_state.pa_failure_info[19] |= ((1-skip_station_flag) << (station_no - 28));
		}
	}
	else if(skip_station_flag <= 1)
	{
		if(station_no >= 1 && station_no <= 6)
		{
			ccu_state.pa_failure_info[21] &= ((0xFE << (station_no+1)) | (0xDF >> (7-station_no)));
			ccu_state.pa_failure_info[21] |= ((1-skip_station_flag) << (station_no + 1));
		}
		else if(station_no >= 7 && station_no <= 10)
		{
			ccu_state.pa_failure_info[22] &= ((0xFE << (station_no-7)) | (0xFE >> (15 - station_no)));
			ccu_state.pa_failure_info[22] |= ((1-skip_station_flag) << (station_no - 7));
		}
		else if(station_no >= 11 && station_no <= 16)
		{
			ccu_state.pa_failure_info[19] &= ((0xFE << (station_no-9)) | (0xFE >> (17 - station_no)));
			ccu_state.pa_failure_info[19] |= ((1-skip_station_flag) << (station_no - 9));
		}
		else if(station_no >= 17 && station_no <= 24)
		{
			ccu_state.pa_failure_info[20] &= ((0xFE << (station_no-17)) | (0xFE >> (25 - station_no)));
			ccu_state.pa_failure_info[20] |= ((1-skip_station_flag) << (station_no - 17));
		}
		else if(station_no >= 25 && station_no <= 26)
		{
			ccu_state.pa_failure_info[21] &= ((0xFE << (station_no-25)) | (0xFE >> (33 - station_no)));
			ccu_state.pa_failure_info[21] |= ((1-skip_station_flag) << (station_no - 25));
		}
	}
//	skip_station_setting_is_effective = 8;
}

void UpdateIntercommAudioDataRecordingFile(common_package_t common_package_recv_from_bcu)
{///<���������������������������������������������
	if(common_package_recv_from_bcu.pkg_type == COMMON_PACKAGE_TYPE_INTERCOMM_LOG)
	{
		debug_print(("UpdateIntercommAudioDataRecordingFile:%d-%d-%d-%d\n",common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.intercomm_type,
				common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.src_no,common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.dst_no,
				common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.begin_or_over));
		if(common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.intercomm_type == 0)
		{
			if(common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.begin_or_over ==1)
			{
				SetRecordingFileName(1,common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.src_no,
						common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.dst_no);
				OpenNewFile(&fp_intercomm_audio_info,recording_file_path,"wb");
			}
			else
			{
				if(fp_intercomm_audio_info != NULL)
				{
					fclose(fp_intercomm_audio_info);
					fp_intercomm_audio_info = NULL;
				}
			}
		}
		else if(common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.intercomm_type == 1)
		{
			if(common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.begin_or_over ==1)
			{
				SetRecordingFileName(2,common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.src_no,
										common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.dst_no);
				OpenNewFile(&fp_intercomm_audio_info,recording_file_path,"wb");
			}
			else
			{
				if(fp_intercomm_audio_info != NULL)
				{
					fclose(fp_intercomm_audio_info);
					fp_intercomm_audio_info = NULL;
				}
			}
		}
		else if(common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.intercomm_type == 2)
		{
			if(common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.begin_or_over ==1)
			{
				SetRecordingFileName(1,common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.src_no,
										common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.dst_no);
				OpenNewFile(&fp_intercomm_audio_info,recording_file_path,"wb");
			}
			else
			{
				if(fp_intercomm_audio_info != NULL)
				{
					fclose(fp_intercomm_audio_info);
					fp_intercomm_audio_info = NULL;
				}
			}
		}
		ccu_state.enable_audio_data_to_file = common_package_recv_from_bcu.common_data_u.intercomm_audio_data_log.begin_or_over;
	}
}

void UpdatePAVolumeInfo(common_package_t common_package_recv)
{///<������PA������������������������������������������������
	int ret = 0;
//	diag_printf("common_package_recv.pkg_type = %d\n",common_package_recv.pkg_type);
	if(common_package_recv.pkg_type == COMMON_PACKAGE_TYPE_DEVICE_INFO)
	{
		if(strcmp(common_package_recv.common_data_u.ts_dev_volume_info.device_name,"EAMP") == 0)
		{
			ret = 1;
			ccu_state.pa_running_info_to_tms[21] = common_package_recv.common_data_u.ts_dev_volume_info.device_volume;
			ccu_state.current_pa_volume = common_package_recv.common_data_u.ts_dev_volume_info.device_volume;
		}
		else if(strcmp(common_package_recv.common_data_u.ts_dev_volume_info.device_name,"PCU") == 0)
		{
			ccu_state.current_d2p_volume = common_package_recv.common_data_u.ts_dev_volume_info.device_volume;
			ret = 1;
		}
		else if(strcmp(common_package_recv.common_data_u.ts_dev_volume_info.device_name,"BCU") == 0)
		{
			ccu_state.current_d2d_volume = common_package_recv.common_data_u.ts_dev_volume_info.device_volume;
			ret = 1;
		}
	}
	else if(common_package_recv.pkg_type == COMMON_PACKAGE_TYPE_MONITOR_SPEAKER_CONTROL)
	{
		if(common_package_recv.common_data_u.monitor_speaker_control.monitor_speaker_state == 2)
		{
			ret = 1;
			ccu_state.current_monitor_volume = common_package_recv.common_data_u.monitor_speaker_control.monitor_speaker_volume;
		}
	}
	if(ret == 1)
	{///<������������������������������
		if(ccu_state.current_d2d_volume > 4)
		{
			ccu_state.current_d2d_volume = 3;
		}

		if(ccu_state.current_d2p_volume > 4)
		{
			ccu_state.current_d2p_volume = 4;
		}

		if(ccu_state.current_pa_volume > 100)
		{
			ccu_state.current_pa_volume = 85;
		}

		if(ccu_state.current_monitor_volume > 4)
		{
			ccu_state.current_monitor_volume = 2;
		}
		WritePISPADevVolumeInfoToFile();
	}
}

void GetPerDevFailureInfo(unsigned char failure_info_buffer[],int buffer_size)
{
	if(ReadType() == 5)
	{
		if(buffer_size >= sizeof(ccu_state.ccu_per_dev_failure_info))
		{//������������������������������������������������PIS������������
			memcpy((void *)&failure_info_buffer[0],(void *)&ccu_state.ccu_per_dev_failure_info,sizeof(ccu_state.ccu_per_dev_failure_info));
		}
	}
}
int ReadNumWilson(void)
{
	return ccu_state.ccu_info.devices_no;
}
void alarm_func_eamp_network_heart_beta(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	send_eamp_network_state = 1;
	check_dev_state ++;
}

void CreateEAMPNetworkHeartBetaTimer()
{///<������EAMP���������������������
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_eamp_network_heart_beta);
	cyg_alarm_create(counter_handle_eamp_network_heart_beta,alarm_func_eamp_network_heart_beta,0,&alarm_handle_eamp_network_heart_beta,&alarm_object_eamp_network_heart_beta);
	cyg_alarm_initialize(alarm_handle_eamp_network_heart_beta,cyg_current_time(),300);
	cyg_alarm_enable(alarm_handle_eamp_network_heart_beta);
}
void SendHeartBetaSignalToEAMP()
{///<������������������EAMP
	int ret = 0;
	common_package_t heart_beta_info;
	heart_beta_info.dst_dev_number =  MUL_SYNC_DST_NO;
	strcpy((char *)heart_beta_info.dst_dev_name,"EAMP");
	heart_beta_info.src_dev_number =  ccu_state.ccu_info.devices_no;
	strcpy((char *)heart_beta_info.src_dev_name,ccu_state.ccu_info.devices_name);
	heart_beta_info.pkg_type = COMMON_PACKAGE_TYPE_EAMP_NET_STATE;
	heart_beta_info.common_data_u.eamp_heart_beta.monitor_volume = ccu_state.current_monitor_volume;
	heart_beta_info.common_data_u.eamp_heart_beta.pa_volume = ccu_state.current_pa_volume;
	heart_beta_info.common_data_u.eamp_heart_beta.tcms_flag = ccu_state.tms_effective_flag;
	strncpy(&(heart_beta_info.common_data_u.eamp_heart_beta.tcms_time),&(ccu_state.ccu_tms_car_running_info.current_time),6);
//	diag_printf("ccu_state.current_monitor_volume = %d\n",ccu_state.current_monitor_volume);
//	diag_printf("ccu_state.current_pa_volume = %d\n",ccu_state.current_pa_volume);
	do//������������������������������
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id,&heart_beta_info,sizeof(heart_beta_info));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0);
}

void CreateSYNCTimer()
{///<���������������������
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_sync);
	cyg_alarm_create(counter_handle_sync,alarm_func_handle_sync,0,&alarm_handle_sync,&alarm_object_sync);
	cyg_alarm_initialize(alarm_handle_sync,cyg_current_time()+400,400);
	cyg_alarm_disable(alarm_handle_sync);
}
void RestartSYNCTimer()
{///<���������������������
	cyg_alarm_initialize(alarm_handle_sync,cyg_current_time()+400,400);
	cyg_alarm_enable(alarm_handle_sync);
	diag_printf("RestartSYNCTimer\n");
}
void CloseSYNCTimer()
{
	cyg_alarm_disable(alarm_handle_sync);
//	diag_printf("CloseSYNCTimer\n");
}
void alarm_func_handle_sync(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	wherther_send_sync_signal = 1;
}
void SendSYNCPackageToEamp()
{///<������������������EAMP
	diag_printf("SendSYNCPackageToEamp\n");
	int ret = 0;
	network_send_package_t sync_info = {"\0",0};
	sync_info.dst_devices_no =  MUL_DST_NO;
	strcpy((char *)sync_info.dst_devices_name,"EAMP");
	sync_info.send_information.src_devices_no =  ccu_state.ccu_info.devices_no;
	strcpy((char *)sync_info.send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	sync_info.send_information.event_type_ann = AUTO_EAMP_SYNC_SIGNAL;
	sync_info.send_information.event_type_intercom = INTERCOM_IDLE;
	do
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&sync_info,sizeof(sync_info));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0);
}


void CCUUpdateLineInfo(network_send_package_t package_from_network)
{///<CCU���������������������������
	unsigned char update_flag = 0;
	if (package_from_network.send_information.event_info_ann.line_info.line_no != 0)
	{//���������������������
		if(package_from_network.send_information.event_info_ann.line_info.line_no != ccu_state.current_line_no)
		{
			ccu_state.current_line_no = package_from_network.send_information.event_info_ann.line_info.line_no;
			if(package_from_network.send_information.event_info_ann.line_info.region_no == 0)
			{//TCMS���������������
				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_no = 0;

				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.region_no  = 0;

				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.start_station_no =
						package_from_network.send_information.event_info_ann.line_info.start_station_no;

				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.over_station_no =
							package_from_network.send_information.event_info_ann.line_info.terminal_station_no;

				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction =
							package_from_network.send_information.event_info_ann.line_info.up_or_down;

				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus =
							package_from_network.send_information.event_info_ann.line_info.whether_the_cycle;
				update_flag = 1;
			}
			else
			{//BCU������������������
				ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no =
												package_from_network.send_information.event_info_ann.line_info.region_no;
				UpdateRegionInfo(ccu_state.current_line_no,ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no);
				update_flag = 2;
			}

		}
		else if(package_from_network.send_information.event_info_ann.line_info.region_no == 0)
		{//���������������������������������������
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_no = 0;

			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.region_no  = 0;

			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.start_station_no =
					package_from_network.send_information.event_info_ann.line_info.start_station_no;

			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.over_station_no =
						package_from_network.send_information.event_info_ann.line_info.terminal_station_no;

			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction =
						package_from_network.send_information.event_info_ann.line_info.up_or_down;

			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus =
						package_from_network.send_information.event_info_ann.line_info.whether_the_cycle;

			update_flag = 3;
		}
		else if(package_from_network.send_information.event_info_ann.line_info.region_no !=
				ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.region_no)
		{//���������������������������������������
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_no =
					package_from_network.send_information.event_info_ann.line_info.region_no;

			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.region_no  =
					package_from_network.send_information.event_info_ann.line_info.region_no;

			UpdateRegionInfo(ccu_state.current_line_no,ccu_state.ccu_region_info[ccu_state.current_line_no - 3].current_region_no);

			update_flag = 4;
		}
	}
	if(update_flag != 0)
	{
		AlarmPeriDeviceThatLineOrRegionHasChanged();
	}
}

void SetSimulatedduInfo(common_package_t common_package_recv_simulateddu)
{
	if(common_package_recv_simulateddu.pkg_type == COMMON_PACKAGE_TYPE_DDU_SIMULATE)
	{
		diag_printf("simulateddu speed == %d\n simulateddu door_status == %d\n whether start simddu == %d\n",
				common_package_recv_simulateddu.common_data_u.simulate_ddu.speed,
				common_package_recv_simulateddu.common_data_u.simulate_ddu.door_status,
				common_package_recv_simulateddu.common_data_u.simulate_ddu.whether_start_simulateddu_flag);
		if(common_package_recv_simulateddu.common_data_u.simulate_ddu.whether_start_simulateddu_flag == 1)
		{
			ccu_state.simulate_speed = common_package_recv_simulateddu.common_data_u.simulate_ddu.speed;
			ccu_state.simulate_door_status = common_package_recv_simulateddu.common_data_u.simulate_ddu.door_status;
			ccu_state.simulate_flag = common_package_recv_simulateddu.common_data_u.simulate_ddu.whether_start_simulateddu_flag;
		}
		else
		{
			ccu_state.simulate_flag = 0;
			ccu_state.simulate_speed = 0;
			ccu_state.simulate_door_status = 0;
		}
	}
}

void GetCCUDeviceVersionInfo()
{
	int ret_of_read_cmd_buffer = 0;
	common_big_package_t common_big_package_temp;
	if((ret_of_read_cmd_buffer = BlockBufferRead(ccu_state.software_version_buffer_id,&common_big_package_temp,sizeof(common_big_package_temp))) > 0)
	{
		if(strcmp(common_big_package_temp.src_dev_name,"PTU") == 0 &&
				common_big_package_temp.pkg_type == COMMON_PACKAGE_TYPE_SOFTWARE_VERSION &&
				common_big_package_temp.common_big_data_u.pa_software_version.get_or_set_version_info == 1)
		{
			strcpy(common_big_package_temp.dst_dev_name,"PTU");
			common_big_package_temp.dst_dev_number = 1;
			strcpy(common_big_package_temp.src_dev_name,ccu_state.ccu_info.devices_name);
			common_big_package_temp.src_dev_number = ccu_state.ccu_info.devices_no;
			common_big_package_temp.pkg_type = COMMON_PACKAGE_TYPE_SOFTWARE_VERSION;
			common_big_package_temp.common_big_data_u.pa_software_version.get_or_set_version_info = 0;
			strncpy(common_big_package_temp.common_big_data_u.pa_software_version.db_version_audio,ccu_state.pa_software_version.db_version_audio,30);
			strncpy(common_big_package_temp.common_big_data_u.pa_software_version.db_version_config,ccu_state.pa_software_version.db_version_config,30);
			strncpy(common_big_package_temp.common_big_data_u.pa_software_version.software_date,ccu_state.pa_software_version.software_date,30);
			strncpy(common_big_package_temp.common_big_data_u.pa_software_version.software_version,ccu_state.pa_software_version.software_version,30);
			BlockBufferWrite(ccu_state.common_big_send_buffer_id,&common_big_package_temp,sizeof(common_big_package_temp));
		}
	}
}

void SendTCMSCMUStateResponsePackage(common_package_t param_common_package)
{
	common_package_t ccu_response_tcms_cmu_state_package;
	int temp_ret,a;
	strcpy(ccu_response_tcms_cmu_state_package.dst_dev_name,param_common_package.src_dev_name);
	ccu_response_tcms_cmu_state_package.dst_dev_number = param_common_package.src_dev_number;
	strcpy(ccu_response_tcms_cmu_state_package.src_dev_name,ccu_state.ccu_info.devices_name);
	ccu_response_tcms_cmu_state_package.src_dev_number = ccu_state.ccu_info.devices_no;
	ccu_response_tcms_cmu_state_package.pkg_type = param_common_package.pkg_type;
	ccu_response_tcms_cmu_state_package.common_data_u.tcms_cmu_state.get_or_set = 0;
	ccu_response_tcms_cmu_state_package.common_data_u.tcms_cmu_state.tcms_current_cmu_state = ccu_state.ccu_cmu_state;
	diag_printf("!!!-%s%d\n",ccu_response_tcms_cmu_state_package.dst_dev_name,ccu_response_tcms_cmu_state_package.dst_dev_number);
	a = 0;
	do
	{
		temp_ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_response_tcms_cmu_state_package, sizeof(ccu_response_tcms_cmu_state_package));
		a++;
			if(temp_ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(temp_ret <= 0 && a <= 2);
}
