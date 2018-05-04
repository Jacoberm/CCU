/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： tms_auto_simulate.c
 * 文件标识：
 * 摘    要：The source file of current bcu to simulate auto broadcast
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-09
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

 #include "tms_auto_simulate.h"

/*This timer is used to send heart beat information to keep all decive connect*/
cyg_handle_t  counter_handle_auto_simulate;//计数器句柄
cyg_handle_t alarm_handle_auto_simulate;//告警器句柄
cyg_alarm alarm_object_auto_simulate;//告警器

int temp_start_stop_station[5][2] = {{0,0},
										 {0,0},
										 {0,0},
										 {1,29},
										 {1,26}};

int line_no_tms = 4;
int n1_tms = 1;
int n2_tms = 2;

void alarm_func_handle_auto_simulate(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	return ;
}

int CreateAutoSimulateTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_auto_simulate);
	cyg_alarm_create(counter_handle_auto_simulate,alarm_func_handle_auto_simulate,0,&alarm_handle_auto_simulate,&alarm_object_auto_simulate);
	return 0;
}

int DeleteAutoSimulateTimer()
{
   cyg_alarm_delete(alarm_handle_auto_simulate);
	return 0;
}

int SetAutoSimulateTimeInterval(int time_interval)
{
	cyg_alarm_initialize(alarm_handle_auto_simulate,cyg_current_time()+time_interval,time_interval);
	return 0;
}


int OpenAutoSimulateTimer()
{
	cyg_alarm_enable(alarm_handle_auto_simulate);
	return 0;
}

int CloseAutoSimulateTimer()
{
	cyg_alarm_disable(alarm_handle_auto_simulate);
	return 0;
}

/*Start the heart beta timer*/
void StartAutoSimulateTimer()
{
	SetAutoSimulateTimeInterval(HEART_BETA);
	OpenHeartBetaTimer();
}

/*Auto change line no*/
void BcuUpdateAutoAnnouncement()
{
	if(bcu_state.bcu_active_ann_state->state_id == TMS_AUTO_ANN_EVENT)
	{
		if(line_no_tms == 3)
		{
			n1_tms = LINE_3_BEGIJN_STATION_NO;
			n2_tms = 0;//Line 3
		}
		else if(line_no_tms == 4)
		{
			n1_tms = LINE_4_BEGIJN_STATION_NO;
			n2_tms = 0;//Line 4
		}
	}
}
