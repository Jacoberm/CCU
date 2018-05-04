#include "manual_ann.h"
///<����������������������������������������������������������������������

send_infomation_t send_ts_cmd_to_control;

cyg_handle_t  counter_handle;
cyg_handle_t alarm_handle_monitor;
cyg_alarm alarm_object_monitor;

int CreateEAMPToSyncTimer(unsigned int msecond)
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle);
	cyg_alarm_create(counter_handle,SendManualSYNCPackageToEamp,0,&alarm_handle_monitor,&alarm_object_monitor);
	cyg_alarm_initialize(alarm_handle_monitor,cyg_current_time()+(100),msecond);
	cyg_alarm_disable(alarm_handle_monitor);
	return 0;
}

void SendManualSYNCPackageToEamp()
{///<������������������EAMP
	diag_printf("SendManualSYNCPackageToEamp\n");
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
	cyg_alarm_disable(alarm_handle_monitor);
}

void SetTSManualCMDPackage(int param_stop_or_leave,int param_open_door_flag)
{///<���������������������������������������������������������

   diag_printf("zhw **** run into SetManualCmd function\n");

    diag_printf(">>>>> MANUAL_ANN_EVENT = %d\n",MANUAL_ANN_EVENT);
	network_send_package_t manual_ann_info;
	manual_ann_info.dst_devices_no = MUL_DST_NO;
	strcpy(manual_ann_info.dst_devices_name,"EAMP");
	manual_ann_info.send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
	manual_ann_info.send_information.event_type_intercom = INTERCOM_IDLE;
	manual_ann_info.send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(manual_ann_info.send_information.src_devices_name,ccu_state.ccu_info.devices_name);

	manual_ann_info.send_information.event_info_ann.tms_auto_announce.line_number = ccu_state.current_line_no;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.tms_type_flag  = 1;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.key_side  = ccu_state.ccu_info.devices_no -1;

	manual_ann_info.send_information.event_info_ann.tms_auto_announce.start_station_code = ccu_state.ccu_region_info[0].current_region_info.start_station_no;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.end_station_code = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.current_station_code = ccu_state.ccu_region_info[0].current_station_no;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.stop_or_leave = param_stop_or_leave;
	manual_ann_info.send_information.event_info_ann.tms_auto_announce.door_side  = param_open_door_flag;

    if(ccu_state.ccu_region_info[0].current_region_info.start_station_no < ccu_state.ccu_region_info[0].current_region_info.over_station_no)
    	manual_ann_info.send_information.event_info_ann.tms_auto_announce.up_down = 1;
    else
    	manual_ann_info.send_information.event_info_ann.tms_auto_announce.up_down = 2;
    send_ts_cmd_to_control.event_info_ann.manual_annnounce.door_side = 1;
	
    int ret = 0;
    if(ccu_state.is_this_server == 1)
    {
    	do
    	{
    		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&manual_ann_info, sizeof(manual_ann_info));
    		if(ret <= 0)
    		{
    			cyg_thread_delay(10);
    		}
    	}while(ret <= 0);

    }
    CloseManualPackageTimer();
    RestartSYNCTimer();
}

void SetUpOrDownStream(int up_down_flag)
{
	network_send_package_t send_package_information;
	if(up_down_flag == 1)
	{
		send_package_information.send_information.event_info_ann.line_info.start_station_no = ccu_state.ccu_region_info[0].current_region_info.start_station_no;
		send_package_information.send_information.event_info_ann.line_info.terminal_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
	}
	else if(up_down_flag == 2)
	{
		send_package_information.send_information.event_info_ann.line_info.start_station_no = ccu_state.ccu_region_info[0].current_region_info.start_station_no;
		send_package_information.send_information.event_info_ann.line_info.terminal_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
	}

	strcpy(send_package_information.send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	send_package_information.send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	send_package_information.send_information.event_type_ann = LINE_NUMBER_CHANGE_EVENT;
	send_package_information.send_information.event_type_intercom = INTERCOM_IDLE;
	send_package_information.send_information.event_info_ann.line_info.line_no = ccu_state.current_line_no;
	send_package_information.send_information.event_info_ann.line_info.region_no = ccu_state.region_station_info.region_no;
	send_package_information.send_information.event_info_ann.line_info.region_update_type = 0;
	send_package_information.send_information.event_info_ann.line_info.get_or_set = 0;
	send_package_information.send_information.event_info_ann.line_info.up_or_down = up_down_flag;
	ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction = up_down_flag;
	send_package_information.send_information.event_info_ann.line_info.whether_the_cycle = ccu_state.region_station_info.os_cycle_line;
	strcpy(send_package_information.dst_devices_name,	"EAMP");
	send_package_information.dst_devices_no = MUL_DST_NO;
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, (void *)&send_package_information, sizeof(send_package_information));
	diag_printf("SetUpOrDownStream:ccu_state.region_station_info.os_running_up_or_down = %d\n",ccu_state.region_station_info.os_running_up_or_down);
	diag_printf("SetUpOrDownStream:ccu_state.current_line_no = %d\n",ccu_state.current_line_no);
	diag_printf("SetUpOrDownStream:ccu_state.region_station_info.region_no = %d\n",ccu_state.region_station_info.region_no);
	diag_printf("SetUpOrDownStream:ccu_state.region_station_info.os_cycle_line = %d\n",ccu_state.region_station_info.os_cycle_line);

}
