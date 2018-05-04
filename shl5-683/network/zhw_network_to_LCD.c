/*
 * ZhwNetworkToLCD.c
 *
 *  Created on: 2015-11-29
 *      Author: benty
 */
#include "zhw_network_to_LCD.h"

eamp_to_LCD_t eamp_to_LCD_info;
int dmu_recv_buffer;
int dmu_send_buffer;

void To_LCD_info_init() {
	dmu_recv_buffer = BlockBufferOpen("new-eamp-recv-dynamic-map-data");
	dmu_send_buffer = BlockBufferOpen("new-eamp-send-dynamic-map-data");
	eamp_to_LCD_info.frame_head = 0x7e;
	eamp_to_LCD_info.message_head.first = 0x01;
	eamp_to_LCD_info.message_head.second = 0x01;
	eamp_to_LCD_info.message_head.target_device_type = 0x07;
	eamp_to_LCD_info.message_head.target_device_number = 0xff;
	eamp_to_LCD_info.message_head.persist_1 = 0;
	eamp_to_LCD_info.message_head.inspect_ctl = 0;
	eamp_to_LCD_info.message_head.inspect_device_info = 0x1d;
	memset(eamp_to_LCD_info.message_head.persist_2,0,sizeof(eamp_to_LCD_info.message_head.persist_2));
	eamp_to_LCD_info.ctl_code[0] = 0x01;
	eamp_to_LCD_info.ctl_code[1] = 0x00;
	eamp_to_LCD_info.data_length[0] = 0x13;
	eamp_to_LCD_info.data_length[1] = 0x00;
	eamp_to_LCD_info.data.main_flag = 0xff;
	eamp_to_LCD_info.data.direct_and_dooropen.upstream = 1;
	eamp_to_LCD_info.data.direct_and_dooropen.downstream = 0;
	eamp_to_LCD_info.data.direct_and_dooropen.left_door_open = 1;
	eamp_to_LCD_info.data.direct_and_dooropen.right_door_open = 0;
	eamp_to_LCD_info.data.direct_and_dooropen.left_door_close = 0;
	eamp_to_LCD_info.data.direct_and_dooropen.right_door_close = 1;
	eamp_to_LCD_info.data.arrive_or_leave.car_arrive = 0;
	eamp_to_LCD_info.data.arrive_or_leave.car_leave = 1;
	eamp_to_LCD_info.data.persist_1 = 0;
	eamp_to_LCD_info.data.start_station = 1;
	eamp_to_LCD_info.data.end_station = 16;
	eamp_to_LCD_info.data.current_station = 1;
	eamp_to_LCD_info.data.next_station = 2;
	eamp_to_LCD_info.data.over_station_ctl = 0;
	eamp_to_LCD_info.data.over_station_info.state_1 = 0;
	eamp_to_LCD_info.data.over_station_info.state_2 = 0;
	eamp_to_LCD_info.data.over_station_info.state_3 = 0;
	eamp_to_LCD_info.data.over_station_info.state_4 = 0;
	eamp_to_LCD_info.data.over_station_info.state_5 = 0;
	eamp_to_LCD_info.data.over_station_info.state_6 = 0;
	eamp_to_LCD_info.data.over_station_info.state_7 = 0;
	eamp_to_LCD_info.data.over_station_info.state_8 = 0;
	eamp_to_LCD_info.data.over_station_info.state_9 = 0;
	eamp_to_LCD_info.data.over_station_info.state_10 = 0;
	eamp_to_LCD_info.data.over_station_info.state_11 = 0;
	eamp_to_LCD_info.data.over_station_info.state_12 = 0;
	eamp_to_LCD_info.data.over_station_info.state_13 = 0;
	eamp_to_LCD_info.data.over_station_info.state_14 = 0;
	eamp_to_LCD_info.data.over_station_info.state_15 = 0;
	eamp_to_LCD_info.data.over_station_info.state_16 = 0;
	eamp_to_LCD_info.data.over_station_info.state_17 = 0;
	eamp_to_LCD_info.data.over_station_info.state_18 = 0;
	eamp_to_LCD_info.data.over_station_info.state_19 = 0;
	eamp_to_LCD_info.data.over_station_info.state_20 = 0;
	eamp_to_LCD_info.data.over_station_info.state_21 = 0;
	eamp_to_LCD_info.data.over_station_info.state_22 = 0;
	eamp_to_LCD_info.data.over_station_info.state_23 = 0;
	eamp_to_LCD_info.data.over_station_info.state_24 = 0;
	eamp_to_LCD_info.data.over_station_info.state_25 = 0;
	eamp_to_LCD_info.data.over_station_info.state_26 = 0;
	eamp_to_LCD_info.data.over_station_info.state_27 = 0;
	eamp_to_LCD_info.data.over_station_info.state_28 = 0;
	eamp_to_LCD_info.data.over_station_info.state_29 = 0;
	eamp_to_LCD_info.data.over_station_info.state_30 = 0;
	eamp_to_LCD_info.data.over_station_info.state_31 = 0;
	eamp_to_LCD_info.data.over_station_info.state_32 = 0;
	eamp_to_LCD_info.data.over_station_info.state_33 = 0;
	eamp_to_LCD_info.data.over_station_info.state_34 = 0;
	eamp_to_LCD_info.data.over_station_info.state_35 = 0;
	eamp_to_LCD_info.data.over_station_info.state_36 = 0;
	eamp_to_LCD_info.data.over_station_info.state_37 = 0;
	eamp_to_LCD_info.data.over_station_info.state_38 = 0;
	eamp_to_LCD_info.data.over_station_info.state_39 = 0;
	eamp_to_LCD_info.data.over_station_info.state_40 = 0;
	eamp_to_LCD_info.data.over_station_info.state_41 = 0;
	eamp_to_LCD_info.data.over_station_info.state_42 = 0;
	eamp_to_LCD_info.data.over_station_info.state_43 = 0;
	eamp_to_LCD_info.data.over_station_info.state_44 = 0;
	eamp_to_LCD_info.data.over_station_info.state_45 = 0;
	eamp_to_LCD_info.data.over_station_info.state_46 = 0;
	eamp_to_LCD_info.data.over_station_info.state_47 = 0;
	eamp_to_LCD_info.data.over_station_info.state_48 = 0;
	memset(eamp_to_LCD_info.data.persist_2,0,sizeof(eamp_to_LCD_info.data.persist_2));
//	eamp_to_LCD_info.check_sum = 0;

	return ;
}

//int checksystem() {
//	union check {
//		int i;
//		char ch;
//	}c;
//	c.i = 1;
//	return (c.ch == 1);
//}
void ZhwFirstSendLineInfoToDmu()
{
	diag_printf("############# run into ZhwSendCmdToDynamicMap function #########\n");
	int write_ret = 0;
	eamp_to_LCD_t eamp_to_lcd_buffer;
	memcpy(&eamp_to_lcd_buffer,&eamp_to_LCD_info,sizeof(eamp_to_LCD_info));
	char check_sum = 0xff;
	unsigned char temp_line_no;
	unsigned char temp_current_station_no;
		diag_printf("$$$$$ now send MANUAL_ANN_EVENT command to LCD $$$$$$$$$\n");
		if(param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.stop_or_leave == 1)
		{
			//car status code
			eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive = 0x01;
			eamp_to_lcd_buffer.data.arrive_or_leave.car_leave = 0x00;
		}
		else
		{
			//car status code
			eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive = 0x00;
			eamp_to_lcd_buffer.data.arrive_or_leave.car_leave = 0x01;
		}

		//running direction
		if(0 == param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.up_down) {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0x00;
		}
		else {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0x01;
		}

		//station information
		eamp_to_lcd_buffer.data.start_station = param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.start_station_code;
		eamp_to_lcd_buffer.data.current_station = param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.current_station_code;
		eamp_to_lcd_buffer.data.next_station = CalculateNextStationNo(eamp_state.current_line_no,param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.current_station_code);
		eamp_to_lcd_buffer.data.end_station = GetTerminalStationNo(eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.over_station_no);

		//open door sider
		if(1 == param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.door_side)
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0x01;
		}
		else if(0 == param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.door_side)
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0x01;
		}
		else
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0x00;
		}
	*p++ = 0xff;
	//function code
	*p++ = 0x06;
	//line no
	*p++ = eamp_state.current_line_no;
	//running direction
	*p++ = eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.running_direction-1;
	//station information
	*p++ = eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.start_station_no;
	*p++  = eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.start_station_no;
	*p++  = CalculateNextStationNo(eamp_state.current_line_no,
											eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.start_station_no);
	*p++ = GetTerminalStationNo(eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.over_station_no);
	//open door sider
	*p++ = 0;
	//cycle line
	*p ++ = eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.whether_the_cycle_bus;
	//set skip station
	*p++ = 0;
	//recovery skip station
	*p++ = 1;
	// key sider info
	*p++ = 0;

}

void ZhwSendCmdToDynamicMap(send_infomation_t param_dynamic_map_send_cmd_info) {///<发送报站信息给动态地图
	diag_printf("############# run into ZhwSendCmdToDynamicMap function #########\n");
	int write_ret = 0;
	eamp_to_LCD_t eamp_to_lcd_buffer;
	memcpy(&eamp_to_lcd_buffer,&eamp_to_LCD_info,sizeof(eamp_to_LCD_info));
	char check_sum = 0xff;
	unsigned char temp_line_no;
	unsigned char temp_current_station_no;
	diag_printf("now event_type_ann = %d !!!\n",param_dynamic_map_send_cmd_info.event_type_ann);
	if(param_dynamic_map_send_cmd_info.event_type_ann == MANUAL_ANN_EVENT)
	{
		diag_printf("$$$$$ now send MANUAL_ANN_EVENT command to LCD $$$$$$$$$\n");
		if(param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.stop_or_leave == 1)
		{
			//car status code
			eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive = 0x01;
			eamp_to_lcd_buffer.data.arrive_or_leave.car_leave = 0x00;
		}
		else
		{
			//car status code
			eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive = 0x00;
			eamp_to_lcd_buffer.data.arrive_or_leave.car_leave = 0x01;
		}

		//running direction
		if(0 == param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.up_down) {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0x00;
		}
		else {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0x01;
		}

		//station information
		eamp_to_lcd_buffer.data.start_station = param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.start_station_code;
		eamp_to_lcd_buffer.data.current_station = param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.current_station_code;
		eamp_to_lcd_buffer.data.next_station = CalculateNextStationNo(eamp_state.current_line_no,param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.current_station_code);
		eamp_to_lcd_buffer.data.end_station = GetTerminalStationNo(eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.over_station_no);

		//open door sider
		if(1 == param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.door_side)
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0x01;
		}
		else if(0 == param_dynamic_map_send_cmd_info.event_info_ann.manual_annnounce.door_side)
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0x01;
		}
		else
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0x01;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0x00;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0x00;
		}
	}
	else if( param_dynamic_map_send_cmd_info.event_type_ann == TMS_AUTO_ANN_EVENT )
	{
		diag_printf("%%%%%%% now send TMS_AUTO_ANN_EVENT command to LCD %%%%%%%%%-- \n");
		if(param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.stop_or_leave == 1)
		{
			//car status code
			eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive = 1;
			eamp_to_lcd_buffer.data.arrive_or_leave.car_leave = 0;
		}
		else
		{
			//car status code
			eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive = 0;
			eamp_to_lcd_buffer.data.arrive_or_leave.car_leave = 1;
		}

		//running direction
		if(0 == param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.up_down) {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0;
		}
		else {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 1;
		}

		//station information
		eamp_to_lcd_buffer.data.start_station = eamp_state.eamp_region_info[eamp_state.current_line_no-3].current_region_info.start_station_no;
		eamp_to_lcd_buffer.data.current_station = param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.current_station_code;
		eamp_to_lcd_buffer.data.next_station = CalculateNextStationNo(eamp_state.current_line_no,param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.current_station_code);
		eamp_to_lcd_buffer.data.end_station = GetTerminalStationNo(param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.end_station_code);

		//open door sider
		if(1 == param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.door_side)
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 1;
		}
		else if(0 == param_dynamic_map_send_cmd_info.event_info_ann.tms_auto_announce.door_side)
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 1;
		}
		else
		{
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close = 0;
		}
	}
	else if(param_dynamic_map_send_cmd_info.event_type_ann == SKIP_STATION)//set skip station
	{
		diag_printf("^^^^^^^^^^^^^ now send SKIP_STATION command to LCD ^^^^^^^^^\n");
		temp_line_no = param_dynamic_map_send_cmd_info.event_info_ann.station_information.line_number;
		temp_current_station_no = param_dynamic_map_send_cmd_info.event_info_ann.station_information.station_no;

		//running direction
		if(1 == eamp_state.eamp_region_info[temp_line_no - 3].current_region_info.running_direction) {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0;
		}
		else {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 1;
		}

		//station information
		eamp_to_lcd_buffer.data.start_station = eamp_state.eamp_region_info[temp_line_no - 3].current_region_info.start_station_no;
//		eamp_to_lcd_buffer.data.current_station = temp_current_station_no;
		eamp_to_lcd_buffer.data.next_station = CalculateNextStationNo(temp_line_no,temp_current_station_no);
		eamp_to_lcd_buffer.data.end_station = GetTerminalStationNo(eamp_state.eamp_region_info[eamp_state.current_line_no - 3].current_region_info.over_station_no);

		//set skip station
		if(param_dynamic_map_send_cmd_info.event_info_ann.station_information.effective == 1)
		{
			eamp_to_lcd_buffer.data.over_station_ctl = 0;
		}
		else
		{
			eamp_to_lcd_buffer.data.over_station_ctl = 1;
			switch(temp_current_station_no) {
			case 1:
				eamp_to_lcd_buffer.data.over_station_info.state_1 = 1;
				break;
			case 2:
				eamp_to_lcd_buffer.data.over_station_info.state_2 = 1;
				break;
			case 3:
				eamp_to_lcd_buffer.data.over_station_info.state_3 = 1;
				break;
			case 4:
				eamp_to_lcd_buffer.data.over_station_info.state_4 = 1;
				break;
			case 5:
				eamp_to_lcd_buffer.data.over_station_info.state_5 = 1;
				break;
			case 6:
				eamp_to_lcd_buffer.data.over_station_info.state_6 = 1;
				break;
			case 7:
				eamp_to_lcd_buffer.data.over_station_info.state_7 = 1;
				break;
			case 8:
				eamp_to_lcd_buffer.data.over_station_info.state_8 = 1;
				break;
			case 9:
				eamp_to_lcd_buffer.data.over_station_info.state_9 = 1;
				break;
			case 10:
				eamp_to_lcd_buffer.data.over_station_info.state_10 = 1;
				break;
			case 11:
				eamp_to_lcd_buffer.data.over_station_info.state_11 = 1;
				break;
			case 12:
				eamp_to_lcd_buffer.data.over_station_info.state_12 = 1;
				break;
			case 13:
				eamp_to_lcd_buffer.data.over_station_info.state_13 = 1;
				break;
			case 14:
				eamp_to_lcd_buffer.data.over_station_info.state_14 = 1;
				break;
			case 15:
				eamp_to_lcd_buffer.data.over_station_info.state_15 = 1;
				break;
			case 16:
				eamp_to_lcd_buffer.data.over_station_info.state_16 = 1;
				break;
			case 17:
				eamp_to_lcd_buffer.data.over_station_info.state_17 = 1;
				break;
			case 18:
				eamp_to_lcd_buffer.data.over_station_info.state_18 = 1;
				break;
			case 19:
				eamp_to_lcd_buffer.data.over_station_info.state_19 = 1;
				break;
			case 20:
				eamp_to_lcd_buffer.data.over_station_info.state_20 = 1;
				break;
			case 21:
				eamp_to_lcd_buffer.data.over_station_info.state_21 = 1;
				break;
			case 22:
				eamp_to_lcd_buffer.data.over_station_info.state_22 = 1;
				break;
			case 23:
				eamp_to_lcd_buffer.data.over_station_info.state_23 = 1;
				break;
			case 24:
				eamp_to_lcd_buffer.data.over_station_info.state_24 = 1;
				break;
			case 25:
				eamp_to_lcd_buffer.data.over_station_info.state_25 = 1;
				break;
			case 26:
				eamp_to_lcd_buffer.data.over_station_info.state_26 = 1;
				break;
			case 27:
				eamp_to_lcd_buffer.data.over_station_info.state_27 = 1;
				break;
			case 28:
				eamp_to_lcd_buffer.data.over_station_info.state_28 = 1;
				break;
			case 29:
				eamp_to_lcd_buffer.data.over_station_info.state_29 = 1;
				break;
			case 30:
				eamp_to_lcd_buffer.data.over_station_info.state_30 = 1;
				break;
			case 31:
				eamp_to_lcd_buffer.data.over_station_info.state_31 = 1;
				break;
			case 32:
				eamp_to_lcd_buffer.data.over_station_info.state_32 = 1;
				break;
			case 33:
				eamp_to_lcd_buffer.data.over_station_info.state_33 = 1;
				break;
			case 34:
				eamp_to_lcd_buffer.data.over_station_info.state_34 = 1;
				break;
			case 35:
				eamp_to_lcd_buffer.data.over_station_info.state_35 = 1;
				break;
			case 36:
				eamp_to_lcd_buffer.data.over_station_info.state_36 = 1;
				break;
			case 37:
				eamp_to_lcd_buffer.data.over_station_info.state_37 = 1;
				break;
			case 38:
				eamp_to_lcd_buffer.data.over_station_info.state_38 = 1;
				break;
			case 39:
				eamp_to_lcd_buffer.data.over_station_info.state_39 = 1;
				break;
			case 40:
				eamp_to_lcd_buffer.data.over_station_info.state_40 = 1;
				break;
			case 41:
				eamp_to_lcd_buffer.data.over_station_info.state_41 = 1;
				break;
			case 42:
				eamp_to_lcd_buffer.data.over_station_info.state_42 = 1;
				break;
			default:
				diag_printf("the station of over is error!!!\n");
				break;
			}
		}
	}
	else if(param_dynamic_map_send_cmd_info.event_type_ann == CLEAR_ALL_SKIP_STATION)//clear all skip station
	{
		diag_printf("&&&&&&&&&&&& now send CLEAR_ALL_SKIP_STATION to LCD &&&&&&&&&\n");
		temp_line_no = param_dynamic_map_send_cmd_info.event_info_ann.clear_all_skip_station.line_no;
		temp_line_no = param_dynamic_map_send_cmd_info.event_info_ann.station_information.line_number;
		temp_current_station_no = param_dynamic_map_send_cmd_info.event_info_ann.station_information.station_no;

		//running direction
		if(1 == eamp_state.eamp_region_info[temp_line_no - 3].current_region_info.running_direction) {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 1;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 0;
		}
		else {
			eamp_to_lcd_buffer.data.direct_and_dooropen.upstream = 0;
			eamp_to_lcd_buffer.data.direct_and_dooropen.downstream = 1;
		}

		eamp_to_lcd_buffer.data.over_station_ctl = 2;
	}

	diag_printf("the send message is frame_head = 0x%x,message_head = %d-%d-0x%x-0x%x-0-0x%x-0x%x-000000000, ctl_code = 0x%x-0x%x,-:"
			,eamp_to_lcd_buffer.frame_head
			,eamp_to_lcd_buffer.message_head.first
			,eamp_to_lcd_buffer.message_head.second
			,eamp_to_lcd_buffer.message_head.target_device_number
			,eamp_to_lcd_buffer.message_head.target_device_type
			,eamp_to_lcd_buffer.message_head.inspect_ctl
			,eamp_to_lcd_buffer.message_head.inspect_device_info
			,eamp_to_lcd_buffer.ctl_code[0]
			,eamp_to_lcd_buffer.ctl_code[1]);
	diag_printf("data_length = 0x%x-0x%x,main_flag = 0x%x,direct_and_door = %d%d%d%d%d%d00,arrive_or_leave = %d%d000000-0,start-end-current-next = 0x%x-0x%x-0x%x-0x%x,00000000000----end\n"
			,eamp_to_lcd_buffer.data_length[0]
	       ,eamp_to_lcd_buffer.data_length[1]
	       ,eamp_to_lcd_buffer.data.main_flag
			,eamp_to_lcd_buffer.data.direct_and_dooropen.upstream
			,eamp_to_lcd_buffer.data.direct_and_dooropen.downstream
			,eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_open
			,eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_open
			,eamp_to_lcd_buffer.data.direct_and_dooropen.left_door_close
			,eamp_to_lcd_buffer.data.direct_and_dooropen.right_door_close
			,eamp_to_lcd_buffer.data.arrive_or_leave.car_leave
			,eamp_to_lcd_buffer.data.arrive_or_leave.car_arrive
			,eamp_to_lcd_buffer.data.start_station
			,eamp_to_lcd_buffer.data.end_station
			,eamp_to_lcd_buffer.data.current_station
			,eamp_to_lcd_buffer.data.next_station);

	write_ret = BlockBufferWrite(dmu_send_buffer, (void *)&eamp_to_lcd_buffer, sizeof(eamp_to_lcd_buffer));
	if ( write_ret < 0 ) {
		net_debug_printf(("the cmd send to LCD is write error!!!\n"));
	}
	else {
		diag_printf("********** send command to LCD is OK **********\n");
	}
	return ;
}
