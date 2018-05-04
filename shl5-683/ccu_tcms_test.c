/*
 * Copyright (c) 2012,MIT
 * All right reserved.
 *
 * ????: ccu_tcms_test.c
 * ????:
 * ?    ?: TCMS????????
 *
 * ????: V1.0.0
 * ?    ?: wilson
 * ????:2014-04-16
 *
 * ????:
 * ???  :
 * ????:
 */
#include "ccu_tcms_test.h"

unsigned char map_emerg[10] = {101,103,105,107};
void TestTCMSEmergOk(unsigned char emerg_sequence,unsigned char emerg_times)
{
		network_send_package_t *p_emerg_cmd;
		network_send_package_t tms_emerg_cmd;
		p_emerg_cmd = &tms_emerg_cmd;
		p_emerg_cmd->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
		strcpy(p_emerg_cmd->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
		p_emerg_cmd->dst_devices_no =  6;//MUL_DST_NO;
		strcpy(p_emerg_cmd->dst_devices_name,"EAMP");
		p_emerg_cmd->send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
		p_emerg_cmd->send_information.event_type_intercom = INTERCOM_IDLE;
		p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_type_flag = 4;
		p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
		p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
		tms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = map_emerg[emerg_sequence];
		tms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = emerg_times;
		BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_emerg_cmd, sizeof(tms_emerg_cmd));
}

void TestTCMSEmerg()
{
	network_send_package_t *p_emerg_cmd;
	network_send_package_t tms_emerg_cmd;
	p_emerg_cmd = &tms_emerg_cmd;
	p_emerg_cmd->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_emerg_cmd->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_emerg_cmd->dst_devices_no = MUL_DST_NO;
	strcpy(p_emerg_cmd->dst_devices_name,"EAMP");
	p_emerg_cmd->send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
	p_emerg_cmd->send_information.event_type_intercom = INTERCOM_IDLE;
	p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_type_flag = 4;
	p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
	p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
	tms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 1;
	tms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = 1;
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_emerg_cmd, sizeof(tms_emerg_cmd));
}

void TestDDU()
{
	network_send_package_t *p_ddu_cmd;
	network_send_package_t tms_ddu_cmd;
	p_ddu_cmd = &tms_ddu_cmd;
	p_ddu_cmd->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_ddu_cmd->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_ddu_cmd->dst_devices_no =  MUL_DST_NO;
	strcpy(p_ddu_cmd->dst_devices_name,"EAMP");
	p_ddu_cmd->send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
	p_ddu_cmd->send_information.event_type_intercom = INTERCOM_IDLE;
	p_ddu_cmd->send_information.event_info_ann.tms_auto_announce.line_number = 3;
	p_ddu_cmd->send_information.event_info_ann.tms_auto_announce.tms_type_flag = 2;
	p_ddu_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_pre_read = 0;
	p_ddu_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
	p_ddu_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 101;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = 4;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.current_station_code  = 10;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.door_side = 0;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.end_station_code = 29;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.next_station_code = 11;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.up_down = 1;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.start_station_code = 1;
	tms_ddu_cmd.send_information.event_info_ann.tms_auto_announce.stop_or_leave = 0;
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_ddu_cmd, sizeof(tms_ddu_cmd));
	return ;
}

void TestLineInfo()
{
	ccu_state.current_line_no = 3;

	ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.start_station_no = 1;

	ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.over_station_no = 10;

	ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction = 1;

	ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus = 0;

	SendTCMSLineAndRegionInfo();
}
void ShowTCMSInfo(unsigned char recv_tcms_info[])
{
	int i = 0;
	int port = 0;
	for(i = 0;i < 64;i++)
	{
		if(recv_tcms_info[i] != 0)
		{
			if(i <= 15)
			{
				if(i == 4)
				{
					diag_printf("port1:   location:%d : %d,%d\n",i,recv_tcms_info[i]*36,recv_tcms_info[i]);
				}
				else
				{
					diag_printf("port1:   location:%d : %d\n",i,recv_tcms_info[i]);
				}

			}
			else if(i <= 31)
			{
				port = 1;
				if(recv_tcms_info[i] != 0)
				{
					diag_printf("port%d:   location:%d,%x  \n",port,i,recv_tcms_info[i]);
				}
				if((recv_tcms_info[i] & 0x80) == 0x80)
				{
					diag_printf("port%d:   location:%d   bit:%d\n",port,i,7);
				}
				if((recv_tcms_info[i] & 0x01) == 0x01)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,0);
				}
				if((recv_tcms_info[i] & 0x02) == 0x02)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,1);
				}
				if((recv_tcms_info[i] & 0x04) == 0x04)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,2);
				}
				if((recv_tcms_info[i] & 0x08) == 0x08)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,3);
				}
				if((recv_tcms_info[i] & 0x10) == 0x10)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,4);
				}
				if((recv_tcms_info[i] & 0x20) == 0x20)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,5);
				}
				if((recv_tcms_info[i] & 0x40) == 0x40)
				{
					diag_printf("port%d:   location:%d   bit:%d\n",port,i,6);
				}
			}
			else if(i <= 37)
			{
				port = 2;
//				diag_printf("111\n");
					//diag_printf("port:0x919:   location:%d,%d\n",i,recv_tcms_info[i]);
				diag_printf("port%d:   location:%d,%x  \n",port,i,recv_tcms_info[i]);
			}
			else if(i <= 45)
			{
				port = 2;

				if((recv_tcms_info[i] & 0x80) == 0x80)
				{
					diag_printf("port%d:   location:%d   bit:%d\n",port,i,7);
				}
				if((recv_tcms_info[i] & 0x01) == 0x01)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,0);
				}
				if((recv_tcms_info[i] & 0x02) == 0x02)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,1);
				}
				if((recv_tcms_info[i] & 0x04) == 0x04)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,2);
				}
				if((recv_tcms_info[i] & 0x08) == 0x08)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,3);
				}
				if((recv_tcms_info[i] & 0x10) == 0x10)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,4);
				}
				if((recv_tcms_info[i] & 0x20) == 0x20)
				{
				diag_printf("port%d:   location:%d   bit:%d\n",port,i,5);
				}
				if((recv_tcms_info[i] & 0x40) == 0x40)
				{
					diag_printf("port%d:   location:%d   bit:%d\n",port,i,6);
				}
			}
		}
	}
}


void SetPISInfoToTCMS(char pc_info[])
{
	unsigned char pis_to_tcms_one[32];
	unsigned char pis_to_tcms_two[32];
	int i = 0,j = 0;
	memset((void *)pis_to_tcms_one,0x00,sizeof(pis_to_tcms_one));
	memset((void *)pis_to_tcms_two,0x00,sizeof(pis_to_tcms_two));
	if(pc_info[4] == 0)
	{//port 1
		for(i = 1;i <= 3;i++)
		{
			if(pc_info[3] == i)
			{
				pis_to_tcms_one[2 * (i - 1)] = pc_info[0];
				pis_to_tcms_one[2 * (i - 1) + 1] = pc_info[1];
			}
		}
		for(i = 4;i <= 21;i++)
		{
			if(pc_info[3] == i)
			{
				pis_to_tcms_one[i + 2] = pc_info[0];
			}
		}
		for(i = 22;i <= 50;i++)
		{
			if(pc_info[3] == i)
			{
				pis_to_tcms_one[24 + (i - 22)/8] = 0;
				for(j = 0 ;j < 8; j++)
				{
					if(pc_info[1] == j)
					{
						pis_to_tcms_one[24 + (i - 22)/8] |= (pc_info[0] << j);
					}
				}
			}
		}
	}
	else
	{//port2
//		diag_printf("port2:%d\n",pc_info[3]);
		for(i = 1;i <= 126;i++)
		{
			if(pc_info[3] == i)
			{
				pis_to_tcms_two[(i - 1)/8] = 0;
				for(j = 0 ;j < 8; j++)
				{
					if(pc_info[1] == j)
					{
						pis_to_tcms_two[(i - 1)/8] |= (pc_info[0] << j);
					}
				}
			}
		}
	}
	int kk =0;
	for(kk = 0;kk < 32;kk++)
	{
		if(pc_info[4] == 0)
		{
			if(pis_to_tcms_one[kk] != 0)
			diag_printf("port1:%d:%x\n",kk,pis_to_tcms_one[kk]);
		}
		else
		{
			if(pis_to_tcms_two[kk] != 0)
			diag_printf("port2:%d:%x\n",kk,pis_to_tcms_two[kk]);
		}
	}
	pis_to_tcms_one[24] |= 0x01;
	memcpy((void *)&ccu_state.pa_running_info_to_tms1[2],(void *)&pis_to_tcms_one[2],30);

	memcpy((void *)&ccu_state.pa_failure_info1[0],(void *)&pis_to_tcms_two[0],32);
}
