/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� ccu_tcms.c
 * ���������������
 * ���    ������ TCMS������������������������
 *
 * ��������������� V1.0.0
 * ���    ������ wilson
 * ���������������2014-04-09
 *
 * ���������������
 * ���������  ���
 * ���������������
 */
#include "ccu_tms.h"
#include "media_and_monitor.h"
#include "simulate_station_code.h"

#define tcms_debug_print(s)     //diag_printf s
#define ddu_simulate_print(s) //diag_printf s
#define ddu_debug_print(s)    //diag_printf s
#define ato_debug_print(s)	//diag_printf s
#define ato_door_state_debug_print(s)	//diag_printf s

#define MAX_SECONDS_ 2114380800//1970.1.1-2037.1.1
#define ENABLE_MAX_SECONDS_ 4070818800//2098-12-31 0:0:00
#define INITIAL_YEAR_ 1900
//#define MENG_DEBUG
#define IO_DEBUG


unsigned short tms_life_signal = 0;///<TCMS������������
network_send_package_t tms_effective_cmd;///<TCMS������������������PA������������������������
common_package_t tcms_volume_info_to_bcu,tcms_volume_info_to_eamp,tcms_volume_info_to_other_ccu;
common_package_t tcms_cmu_state_to_bcu_and_cmu;//TCMS ������������������BCU���CMU
network_send_package_t tcms_emerg_cmd,tcms_skip_station_info,dingdong_cmd;
network_send_package_t tcms_half_auto_ann,tcms_ato_ann,tcms_ddu_ann,last_tcms_ddu_manual_ann;

char tcms_log[1024] = {'\0'};

int station_id_mapping_table[100];

//PIS-TCMS������������������
unsigned char tcms_should_running_times = 0;
//������TCMS���������������������
unsigned long tcms_life_signal = 0;
//������TCMS���������������������
unsigned long tcms_last_life_signal = 0;
//TCMS-PIS������������������������
unsigned char tcms_last_life_signal_same_as_current_times = 0;
//������������������������������������
unsigned char last_emerg_ann_start_state = 0;
//CCU���������������������������������PA������
unsigned char whether_ccu_should_send_emerg_info = 0;
//������������������������������������
unsigned char last_emerg_ann_over_state = 0;
//������������������������
unsigned char emerg_cycyle_times = 1;
//������������������
unsigned char current_pa_mode = 3;//1:auto 2:half auto 3:manual

unsigned char skip_station_flag = 0; //0:不做处理　1:skip station 2:cancel skiping station

unsigned char ddu_skip_first_station_id = 0;

unsigned char ddu_skip_last_station_id = 0;
//���������
unsigned char current_mission_number = 1;
//当前任务号: １:莘庄->南桥新城　　２南桥新城->莘庄
unsigned char ddu_start_stataion_id = 1;
//���������
unsigned char ddu_end_stataion_id = 15;
//���������
unsigned char ddu_current_stataion_id = 1;
//���������
unsigned char ddu_next_stataion_id = 1;
//CCU���������������������������������PA������
unsigned char whether_ccu_should_send_line_info = 0;
//CCU���������������������������������PA������
unsigned char whether_ccu_should_send_manual_ann_info = 0;


unsigned char whether_send_to_pis_data_to_tcms=0;
unsigned char send_pis_data_to_tcms_buffer[100];
unsigned char check_sum=0;
unsigned int life_signal_=0;
int desire_send_to_tms_data_len = 0;
// add by mengliwei
unsigned char button_F1 = 0x00, button_F2 = 0x00, button_F3 = 0x00; button_F4 = 0x00;
unsigned char count_F1 = 0, count_F2 = 0, count_F3 = 0, count_F4 = 0;
unsigned char public_simulate_speed = 0;//列车速度
unsigned int public_distance_expand_times = 5000;//距离放大倍数
unsigned long public_pre_station_distance = 0;//预到站距离
unsigned long public_next_station_distance = 0;//到站距离
unsigned long running_distance = 0;//行走距离（相对于当前站而言）
unsigned char public_open_door_side = 0;//开门侧
unsigned char train_on_backway = 0;//列车是否
unsigned char halfAuto_depart_flag = 0;//半自动模式第一站发车命令
unsigned char count_distance_flag = 0;
unsigned char car_on_station = 0;//０：车没有停靠在站台　１：车停靠在站台
unsigned char broadcast_flag[2] = {0};//预到站，到站广播
unsigned char open_door_status = 0;
unsigned long public_station_distance[32] = {0};
static unsigned long distanceToNext = 0;
unsigned char simulate_mode = 0;
unsigned char simulate_open_door = 0;


cyg_handle_t counter_handle_mvb_recv;
cyg_handle_t alarm_handle_mvb_recv;
cyg_alarm alarm_object_mvb_recv;

void SendPISInfoToTCMS(void)
{
	unsigned char start_no, current_no, next_no, end_no;
	ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id, ddu_end_stataion_id);
	start_no = GetIndexFromStation(ddu_start_stataion_id);
	current_no = GetIndexFromStation(ddu_current_stataion_id);
	next_no = GetIndexFromStation(ddu_next_stataion_id);
	end_no = GetIndexFromStation(ddu_end_stataion_id);

	life_signal_++;
	memset(send_pis_data_to_tcms_buffer,0x00,sizeof(send_pis_data_to_tcms_buffer));
	send_pis_data_to_tcms_buffer[0]=0xfd;
	send_pis_data_to_tcms_buffer[1]=0x50;
	send_pis_data_to_tcms_buffer[2]=0x30;
	send_pis_data_to_tcms_buffer[3]=(life_signal_ >> 8);
	send_pis_data_to_tcms_buffer[4]=(life_signal_ & 0xff);
	send_pis_data_to_tcms_buffer[5]|= 0x80;

	//������������
	send_pis_data_to_tcms_buffer[7] = start_no;
	send_pis_data_to_tcms_buffer[8] = current_no;
	send_pis_data_to_tcms_buffer[9] = next_no;
	send_pis_data_to_tcms_buffer[10] = end_no;

	//������������������
	memcpy(&send_pis_data_to_tcms_buffer[11],&ccu_state.pis_fault_tcms_simulate.data[0],40);

	//���������������
	send_pis_data_to_tcms_buffer[13] = 0x01;
	send_pis_data_to_tcms_buffer[14] = 0x00;
	send_pis_data_to_tcms_buffer[15] = 0x0a;


	//���������������������������������CCTV���������������������������������������������������
	memcpy((void *)&send_pis_data_to_tcms_buffer[43],(void *)&ccu_state.pcu_state_to_tcms[0],4);//pcu state

//	PIS������������
	if(ccu_state.is_this_server == 1)
//	if(1)
	{
		send_pis_data_to_tcms_buffer[50] |= 0x20;
	}
	else
	{
		send_pis_data_to_tcms_buffer[50]&= 0xDF;
	}

	check_sum=0;
	int i= 0;
	for(i = 3;i < 51;i++)
	{
		check_sum+=send_pis_data_to_tcms_buffer[i];
	}
	send_pis_data_to_tcms_buffer[51]=check_sum;
	send_pis_data_to_tcms_buffer[52]=0xff;
	desire_send_to_tms_data_len=53;
	cyg_io_write(ser_led, &send_pis_data_to_tcms_buffer[0], &desire_send_to_tms_data_len);

}

void InitStationIdMappingTable()
{
	station_id_mapping_table[1] = 1;
	station_id_mapping_table[2] = 1;

	station_id_mapping_table[3] = 2;
	station_id_mapping_table[4] = 2;

	station_id_mapping_table[5] = 3;
	station_id_mapping_table[6] = 3;

	station_id_mapping_table[9] = 4;
	station_id_mapping_table[10] = 4;

	station_id_mapping_table[13] = 5;
	station_id_mapping_table[14] = 5;

	station_id_mapping_table[15] = 6;
	station_id_mapping_table[16] = 6;

	station_id_mapping_table[47] = 7;
	station_id_mapping_table[48] = 7;

	station_id_mapping_table[27] = 8;
	station_id_mapping_table[28] = 8;

	station_id_mapping_table[29] = 9;
	station_id_mapping_table[30] = 9;

	station_id_mapping_table[33] = 10;
	station_id_mapping_table[34] = 10;

	station_id_mapping_table[37] = 11;
	station_id_mapping_table[38] = 11;

	station_id_mapping_table[39] = 12;
	station_id_mapping_table[40] = 12;

	station_id_mapping_table[41] = 13;
	station_id_mapping_table[42] = 13;

	station_id_mapping_table[43] = 14;
	station_id_mapping_table[44] = 14;

	station_id_mapping_table[45] = 15;
	station_id_mapping_table[46] = 15;

}

unsigned char TCMS_data[256] = {0};
void *CcuTmsHandleThreadEntry(void *arg)
{
	diag_printf("CcuTmsHandleThreadEntry !!!!!!\n");
	//wilson ���������������MVB������������������������1���������������������������������������������0XFE���������������������������������������������������������������������������,������������152���������
	unsigned int desire_tms_data_len = 1;

	//wilson MVB���������������������������������1024���������������������152������
	unsigned char msg_recv_from_tcms[1024];

	int msglen = 0; //记录TCMS数据的长度转发给地面

	log_type log_info[300];
	int log_cursor=0;
	MvbData_t TempDD;
	MvbData_t *DataTmp=NULL;

	//���������RS485������������
	hal_delay_us(100000); ///< 100ms
	RS485_DIS;
	hal_delay_us(100000); ///< 100ms
	SerialFlushInit((cyg_io_handle_t)&ser_led);

	desire_tms_data_len=sizeof(MvbData_t);

	memset(&TempDD,0,desire_tms_data_len);

	DataTmp=&TempDD;
	unsigned char mpu_1_state = 255,mpu_2_state = 255;
	unsigned char mpu_mast_has_change=0;
	unsigned char log_record_counts=0;
	int i = 0;
	for(i = 0;i < 100;i++)
	{
		log_info[i].flag=0;
		log_info[i].mpu1_state=0;
		log_info[i].mpu2_state=0;
		memset(&log_info[i].time[0],0x00,4);
	}

	CreateMvbRecvTimer();
	StartMvbRecvTimer();
	//wilson ������������TCMS������������������������������,������������������������������������������������������������������������������������������������������������������������������
	InitTCMSPackage();
	TMSATOAnnPackageInit(&tcms_ddu_ann);
	TMSSkipStationPackageInit( &tcms_skip_station_info);
//	SetManualInfo(15,7,3,0);
//	SendTCMSManualInfo();
	InitStationIdMappingTable();
	InitStationDistance();
	ccu_state.ccu_videolink_cmd_send_buff_id = BlockBufferOpen("ccu-videolink-cmd-tx");
	for(;;)
	{
		memset((void *)msg_recv_from_tcms, 0x00, sizeof(msg_recv_from_tcms));
		sem_wait(&sem_mvb_new_cmd);
		desire_tms_data_len = 1;
		mpu_mast_has_change = 0;
#ifdef IO_DEBUG
		do{
			cyg_io_read(ser_led, &msg_recv_from_tcms[0], &desire_tms_data_len);
			if(msg_recv_from_tcms[0] != 0xFE)
			{
				diag_printf("tcms recv is error,%02x\n",msg_recv_from_tcms[0]);
				LOG("收到一个错误的MVB数据包\n");
			}
		}while(msg_recv_from_tcms[0] != 0xFE);
#endif
		desire_tms_data_len = 83;
#ifdef IO_DEBUG
		cyg_io_read(ser_led, &msg_recv_from_tcms[1], &desire_tms_data_len);
#endif
		desire_tms_data_len += 1;
		log_record_counts++;

		RS485_EN;
		hal_delay_us(20000);
		SendPISInfoToTCMS();//PIS���������������������������TCMS
		hal_delay_us(20000);
		RS485_DIS;
		if(mpu_1_state != ((msg_recv_from_tcms[6] & 0x02) >> 1))//MPU_CMPU1Master
		{
			mpu_1_state = ((msg_recv_from_tcms[6] & 0x02) >> 1);
			mpu_mast_has_change =1;

		}
		if(mpu_2_state != (msg_recv_from_tcms[6] & 0x01))//MPU_CMPU21Master
		{
			mpu_2_state = (msg_recv_from_tcms[6] & 0x01);
			mpu_mast_has_change=1;
			log_info[(log_cursor++)%100].mpu2_state=mpu_2_state;
		}
#ifdef IO_DEBUG
		if(mpu_mast_has_change==1 || log_record_counts % 40 == 0)
		{
			log_info[(log_cursor)%100].flag = log_cursor;
			log_info[(log_cursor)%100].mpu1_state=mpu_1_state;
			log_info[(log_cursor)%100].mpu2_state=mpu_2_state;
			memcpy(&log_info[(log_cursor)%100].time[0],&msg_recv_from_tcms[19],4);
			diag_printf("%d,%02x,%02x,%02x,%02x,%d,%d\n",log_info[(log_cursor)%100].flag,log_info[(log_cursor)%100].time[0],
					log_info[(log_cursor)%100].time[1],log_info[(log_cursor)%100].time[2],log_info[(log_cursor)%100].time[3],
					log_info[(log_cursor)%100].mpu1_state,log_info[(log_cursor)%100].mpu2_state);
			log_cursor++;

		}
#endif

		sprintf(ccu_state.tcms_log_head,"[%d-%d-%d %d:%d:%d]:\n\t设备名:CCU%d,版本:%s,主备:%d,模式:%d,本端TCMS:%d,对端TCMS:%d,钥匙:%d,主CCU网络:%d\n\t广播:%d,乘客:%d,司机:%d,监听:%d,媒体:%d,TC1联动生命:%d,TC2联动生命:%d",
				ccu_state.ccu_tms_car_running_info.current_time.year,ccu_state.ccu_tms_car_running_info.current_time.month,ccu_state.ccu_tms_car_running_info.current_time.date,
				ccu_state.ccu_tms_car_running_info.current_time.hour,ccu_state.ccu_tms_car_running_info.current_time.minute,ccu_state.ccu_tms_car_running_info.current_time.second,
						ccu_state.ccu_info.devices_no,ccu_state.pa_software_version.software_date,
						ccu_state.is_this_server,current_pa_mode,ccu_state.tms_effective_flag,OtherCCUTCMSState(),
						ccu_state.key_info,
						ZhwGetPrimaryServerNS(),
						ccu_state.current_pa_volume,
						ccu_state.current_d2p_volume,
						ccu_state.current_d2d_volume,
						ccu_state.current_monitor_volume,ccu_state.current_media_volume,mpu_1_state,mpu_2_state);

		//TCMS������������������
		TCMSToPISInfoHandle(msg_recv_from_tcms);

		if(1&&((log_record_counts++)%10 == 1) && ccu_state.is_this_server == 1)
		{///<仅主CCU记录TCMS数据
			msglen = 6+desire_tms_data_len;
			TCMS_data[0]='T';TCMS_data[1]='C';TCMS_data[2]='M';TCMS_data[3]='S';
			TCMS_data[4]=msglen&0xff;TCMS_data[5]=(msglen>>8)&0xFF;
			memcpy((void *)&TCMS_data[6],(void *)msg_recv_from_tcms,84);
			if(BlockBufferWrite(ccu_state.ccu_videolink_cmd_send_buff_id, (void *)&TCMS_data[0], msglen) < 0 )
			{
				diag_printf("Net : send monitor package to network error!!! \n");
			}
			memset(TCMS_data,0x00,sizeof(TCMS_data));
		}

	}
	return NULL;
}

int tcms_handle_counts = 0;
int last_trigger_leave_station = 0;
int last_trigger_arrive_station = 0;
int last_emerg_state[10];
void TCMSToPISInfoHandle(unsigned char tcms[])
{
	//������������������
	tcms_life_signal = tcms[3]*256+tcms[4];
	if(tcms_life_signal == tcms_last_life_signal)
	{
		if(tcms_last_life_signal_same_as_current_times < 100)
		{
			tcms_last_life_signal_same_as_current_times++;
		}
		if(tcms_last_life_signal_same_as_current_times > 10)
		{
			if(ccu_state.tms_effective_flag == 1)
			{
				diag_printf("the communication with tcms is offline\n");
				LOG("TCMS失效\n");
			}
			ccu_state.tms_effective_flag = 0;
#ifdef IO_DEBUG
			return;
#endif
		}
	}
	else
	{
		tcms_last_life_signal_same_as_current_times = 0;
		if(ccu_state.tms_effective_flag == 0)
		{
			diag_printf("the communication with tcms is online\n");
			LOG("TCMS可用\n");
		}
		tcms_should_running_times  = 0;
		ccu_state.tms_effective_flag = 1;
		tcms_last_life_signal = tcms_life_signal;
	}

	//������������
	ccu_state.ccu_tms_car_running_info.tms_train_no = tcms[23]*256+tcms[24];


	//���������������������������
	 GetDoorState(tcms);

	//������������TCMS������������
	if(tcms_handle_counts++ % 50 == 0 )
	{
		CalculateCurrentDateInfo(tcms[19]*256*256*256+tcms[20]*256*256+tcms[21]*256+tcms[22]);
		ShowDoorStateAndStrategy(tcms[35],tcms[36]);
//		diag_printf("simulate speed:%d kph,mode=%02x\n",tcms[52],tcms[59]);
		diag_printf("running_distance:%ld,public_simulate_speed:%d\n",running_distance,public_simulate_speed);

		public_simulate_speed = tcms[52];

	    //������������������
	    if(tcms[50] != 0 && tcms[51]!=0)
	    {
	    	diag_printf("begin skip station:%d\n",tcms[50]);//only current_pa_mode =  2 or 3
	    	diag_printf("over skip station:%d\n",tcms[51]);//only current_pa_mode =  2 or 3
	    }
		if((tcms[59] & 0x10)==0x10)
		{
			diag_printf("clear all skip station\n");//only current_pa_mode =  2 or 3
		}
	    //������������
	    if((tcms[42] & 0x80)== 0x80)
	    {
	    	diag_printf("car a1 is active\n");
	    }
	    if((tcms[42] & 0x40)== 0x40)
	    {
	    	diag_printf("car a2 is active\n");
	    }
	}
#if 0
	if(open_door_status == 1)
	{
		tcms[36] = 0x40;
		open_door_status = 2;
	}
	else if(tcms_handle_counts % 50 == 0 && open_door_status == 2)
	{
		tcms[36] = 0x80;
		open_door_status = 0;
	}else
	{
		tcms[36] = 0x00;
	}
#endif
	//������������������������������
	if((tcms[39] & 0x08)==0x08)
	{
		emerg_cycyle_times = 255;
	}
	else
	{
		emerg_cycyle_times = tcms[40];
	}


	if(tcms[36] & 0x40 == 0x40)
	{
		tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.door_side = 1;
	}
	if(tcms[36] & 0x20 == 0x20)
	{
		tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.door_side = 2;
	}

	//������������������������
	if((tcms[36] & 0x01)==0x01)
	{//emerg 1
		diag_printf("tcms emerg 1 begin\n");
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = emerg_cycyle_times;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 1;
		diag_printf("tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
	}
	if((tcms[36] & 0x01) == 0 && (last_emerg_state[1] & 0x01)==0x01)
	{
		diag_printf("AA1:tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
		whether_ccu_should_send_emerg_info = 1;
	}
	last_emerg_state[1] = (tcms[36] & 0x01);
	if((tcms[37] & 0x80)==0x80)
	{//emerg 2
		diag_printf("tcms emerg 2 begin\n");
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = emerg_cycyle_times;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 2;
		diag_printf("tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
	}
	if((tcms[37] & 0x80) == 0 && (last_emerg_state[2] & 0x80)==0x80)
	{
		diag_printf("AA2:tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
		whether_ccu_should_send_emerg_info = 1;
	}
	last_emerg_state[2] = (tcms[37] & 0x80);
	if((tcms[37] & 0x40)==0x40)
	{//emerg 3
		diag_printf("tcms emerg 3 begin\n");
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = emerg_cycyle_times;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 3;
		diag_printf("tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
	}
	if((tcms[37] & 0x40) == 0 && (last_emerg_state[3] & 0x40)==0x40)
	{
		diag_printf("AA3:tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
		whether_ccu_should_send_emerg_info = 1;
	}
	last_emerg_state[3] = (tcms[37] & 0x40);
	if((tcms[37] & 0x20)==0x20)
	{//emerg 4
		diag_printf("tcms emerg 4 begin\n");
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = emerg_cycyle_times;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 4;
		diag_printf("tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
	}
	if((tcms[37] & 0x20) == 0 && (last_emerg_state[4] & 0x20)==0x20)
	{
		diag_printf("AA4:tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
		whether_ccu_should_send_emerg_info = 1;
	}
	last_emerg_state[4] = (tcms[37] & 0x20);


	//������������������������
	if((tcms[39] & 0x08)==0x08 && last_emerg_ann_start_state == 0)
	{
		diag_printf("AA:tcms emerg begin-times:%d,delay:%d\n",emerg_cycyle_times,tcms[41]);
		whether_ccu_should_send_emerg_info = 1;
	}
	last_emerg_ann_start_state = (tcms[39] & 0x08);

    //������������������
    if(last_emerg_ann_over_state == 0 && (tcms[39] & 0x10) == 0x10)
    {
    	diag_printf("tcms emerg over\n");
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_active = 0;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 0;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_times = 0;
		tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_emerg_sequence = 0;
    	whether_ccu_should_send_emerg_info = 1;
    }
    last_emerg_ann_over_state = (tcms[39] & 0x10);


    //������������
    if((tcms[39] & 0x04)== 0x04)
    {
    	diag_printf("car a1 is coupled\n");
		LOG("A1联挂\n");
    }
    if((tcms[39] & 0x02)== 0x02)
    {
    	diag_printf("car a2 is coupled\n");
		LOG("A2联挂\n");
    }

    if(tcms[45] != 0 && current_mission_number != tcms[45])
    {
    	current_mission_number = tcms[45];
    }
    //������������������
    if((tcms[59] & 0x80)== 0x80 && current_pa_mode != 1)
    {//auto
    	diag_printf("current mode is auto\n");
    	current_pa_mode = 1;
		LOG("PIS进入全自动报站模式\n");
    }
    if((tcms[59] & 0x40)== 0x40 && current_pa_mode != 2)
    {//half
    	diag_printf("current mode is semi-auto\n");
    	current_pa_mode = 2;
		LOG("PIS进入半自动报站模式\n");
    }
    if((tcms[59] & 0x20)== 0x20 && current_pa_mode != 3)
    {//manual
    	diag_printf("current mode is manual\n");
    	current_pa_mode = 3;
		LOG("PIS进入手动报站模式\n");
    }
    if((tcms[60]&0x40) == 0x40 && simulate_mode == 0)
    {
    	diag_printf("now let's begin to simulate test\n");
    	simulate_mode = 1;
    }else if((tcms[60]&0x40) == 0x40 && simulate_mode == 1)
    {
    	diag_printf("now over simulate test\n");
    	simulate_mode = 0;
    }
    if((tcms[60]&0x20) == 0x20 && simulate_open_door == 0)
    {
    	diag_printf("now simulate door is opened\n");
    	simulate_open_door = 1;
    }else if((tcms[60]&0x20) == 0x00 && simulate_open_door == 1)
    {
    	diag_printf("now simulate door is closed\n");
    	simulate_open_door = 0;
    }


    //计算按钮按下的次序
    CountButtonClick(tcms);

    //������������������(������������������������)
    if( current_pa_mode )
    {
    	if(current_pa_mode == 2 || current_pa_mode == 3)
    	{
    		if(station_id_mapping_table[tcms[46]] != 0 && ddu_start_stataion_id != station_id_mapping_table[tcms[46]])
    		{
    			ddu_start_stataion_id = station_id_mapping_table[tcms[46]];
    			ddu_current_stataion_id = station_id_mapping_table[tcms[46]];
    			diag_printf(":b:%d-c:%d\n",ddu_start_stataion_id,ddu_current_stataion_id);
    		}
    	}
    	if(ddu_end_stataion_id != station_id_mapping_table[tcms[49]] && station_id_mapping_table[tcms[49]] != 0)
    	{
    		ddu_end_stataion_id = station_id_mapping_table[tcms[49]];
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		if(ddu_end_stataion_id > 0 && ddu_end_stataion_id <=15 && ddu_start_stataion_id > 0 && ddu_start_stataion_id <=15)
    		{
    			SetLineInfo(ddu_start_stataion_id,ddu_end_stataion_id,current_mission_number);
    		}
    		diag_printf("line region info:b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
    	}
    	if(current_pa_mode == 2 || current_pa_mode == 3)
    	{
    		if(station_id_mapping_table[tcms[50]] != 0 && ddu_skip_first_station_id != station_id_mapping_table[tcms[50]])
    		{
    			ddu_skip_first_station_id = station_id_mapping_table[tcms[50]];
    			diag_printf("ddu set skip staion from: %d\n",ddu_skip_first_station_id);
    		}
    		if(station_id_mapping_table[tcms[51]] != ddu_skip_last_station_id && station_id_mapping_table[tcms[51]] != 0)
    		{
    			ddu_skip_last_station_id = station_id_mapping_table[tcms[51]];
    			diag_printf("ddu set skip staion to: %d\n",ddu_skip_last_station_id);
    			if(ddu_skip_first_station_id <= ddu_start_stataion_id && ddu_skip_last_station_id <= ddu_end_stataion_id)
    			{
    				skip_station_flag = 1;
    			}
    		}
    		if(skip_station_flag != (tcms[59]&0x10) && (tcms[59]&0x10 != 0x10))
    		{
    			//取消越站就是重发路线信息，标志位暂时不用
    			skip_station_flag = 2;
    			SetLineInfo(ddu_start_stataion_id,ddu_end_stataion_id,current_mission_number);
    		}
    	}
    }
    //���������������������
    if(current_pa_mode == 1)
    {
    	ddu_current_stataion_id = station_id_mapping_table[tcms[47]];
    	ddu_next_stataion_id = station_id_mapping_table[tcms[48]];
		if((tcms[59] & 0x04)== 0 && last_trigger_leave_station == 0x04)
		{
			ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
			SetManualInfo(ddu_start_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id, current_mission_number, 0);
			whether_ccu_should_send_manual_ann_info = 1;
			ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
			ddu_current_stataion_id = ddu_next_stataion_id;
			ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
			distanceToNext = CountDistanceToNext(ddu_current_stataion_id, ddu_next_stataion_id);
			diag_printf("ATC:we will leave current station-c:%d,n:%d\n",ddu_current_stataion_id,ddu_next_stataion_id);
		}
    	last_trigger_leave_station = (tcms[59] & 0x04);
		if((tcms[60] & 0x80)== 0 && last_trigger_arrive_station == 0x80)
		{
			ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
			SetManualInfo(ddu_start_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id, current_mission_number,1);
			whether_ccu_should_send_manual_ann_info = 1;
			diag_printf("ATC:we will arrive next station-c:%d,n:%d\n",ddu_current_stataion_id,ddu_next_stataion_id);
		}
    	last_trigger_arrive_station = (tcms[60] & 0x80);
    }
#ifdef MENG_DEBUG
    public_simulate_speed = 20;
    count_distance_flag = 1;
    current_pa_mode = 2;
#endif
    if(current_pa_mode == 2)    	//半自动模式下的报站处理
    {
    	//通过开关门信号判断列车是否在站台，如果在站台上则当前站累加
    	if(((tcms[36]&0x40) == 0x40 || (tcms[36]&0x20) == 0x20) && car_on_station != 1)
    	{
    		car_on_station = 1;
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		ddu_current_stataion_id = ddu_next_stataion_id;
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		distanceToNext = CountDistanceToNext(ddu_current_stataion_id, ddu_next_stataion_id);
    		if(current_mission_number == 1)
    		{
    			if(ddu_current_stataion_id >= ddu_end_stataion_id)
    				ddu_current_stataion_id = ddu_end_stataion_id;
    			diag_printf("mission=%d:b:%d-c:%d,n:%d,t:%d\n",current_mission_number,ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
    		}
    		else if(current_mission_number == 2)
    		{
    			if(ddu_current_stataion_id <= ddu_end_stataion_id)
    				ddu_current_stataion_id = ddu_end_stataion_id;
    			diag_printf("mission=%d:b:%d-c:%d,n:%d,t:%d\n",current_mission_number,ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
    		}
    		public_pre_station_distance = ccu_state.ccu_tms_station_diatance[0][ddu_next_stataion_id].pre_triger_distance[current_mission_number - 1]*public_distance_expand_times;
    		public_next_station_distance = ccu_state.ccu_tms_station_diatance[0][ddu_next_stataion_id].triger_distance[current_mission_number - 1]*public_distance_expand_times;
    	}else if((tcms[36]&0x80) == 0x80 && car_on_station != 0)
    	{
    		car_on_station = 0;
    	}

    	//列车行进时的距离处理
    	if(car_on_station  == 0 && count_distance_flag == 1)
    	{
    		running_distance = running_distance + public_simulate_speed*public_distance_expand_times*1000/3600*200/1000;
//    		diag_printf("running_distance:%ld,public_simulate_speed:%d\n",running_distance,public_simulate_speed);
    	}else if(car_on_station == 1)
    	{
    		running_distance = 0;
    	}
    	//距离触发报站
    	if(running_distance >= public_pre_station_distance && broadcast_flag[0] == 0)
    	{
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		broadcast_flag[0] = 1;
    		if(ccu_state.is_this_server == 1)
    		{
    			TcmsSendHalfAutoCmd(ddu_start_stataion_id, ddu_end_stataion_id, ddu_current_stataion_id, ddu_next_stataion_id, current_mission_number, 0);
    		}
    	}
    	if((distanceToNext - running_distance) <= public_next_station_distance && broadcast_flag[0] == 1 && broadcast_flag[1] == 0)
    	{
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		broadcast_flag[1] = 1;
    		if(ccu_state.is_this_server == 1)
    		{
    			TcmsSendHalfAutoCmd(ddu_start_stataion_id, ddu_end_stataion_id, ddu_current_stataion_id, ddu_next_stataion_id, current_mission_number, 1);
    		}
    	}
    	if(broadcast_flag[0] == 1 && broadcast_flag[1] == 1 )
    	{
    		open_door_status = 1;
    		broadcast_flag[0] = 0;
    		broadcast_flag[1] = 0;
        	if(ddu_current_stataion_id == ddu_end_stataion_id)
        	{
        		running_distance = 0;
        		count_distance_flag = 0;
				LOG("单向运营结束\n");
        	}
    	}
    }

    //������������������������������������(������������)
    if(current_pa_mode == 3 )
    {
    	if((tcms[59] & 0x02) == 0 && last_trigger_leave_station== 0x02)
    	{
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		SetManualInfo(ddu_start_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id,current_mission_number,0);
    		whether_ccu_should_send_manual_ann_info = 1;
    		diag_printf("Manual:we will leave current station-b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
    	}
    	last_trigger_leave_station = (tcms[59] & 0x02);
    	if((tcms[59] & 0x01) == 0 && last_trigger_arrive_station == 0x01)
    	{
    		ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
    		SetManualInfo(ddu_start_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id,current_mission_number,1);
    		whether_ccu_should_send_manual_ann_info = 1;
    		diag_printf("Manual:we will arrive next station-b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
    	}
    	last_trigger_arrive_station = (tcms[59] & 0x01);
    }
}

//���������������������
unsigned char GetTCMSNextStation(unsigned char cur_station_no,unsigned char ter_station_no)
{
	unsigned char ret;
	if(cur_station_no <= ter_station_no )
	{
		ret = cur_station_no + 1;
		if(ret > ter_station_no)
		{
			ret = ter_station_no;
		}
		while(ccu_state.pp_skip_station_info[0][ret].effective == 0)
		{
			ret = cur_station_no +1;
			if(ret > ter_station_no)
			{
				ret = ter_station_no;
				break;
			}
		}
	}
	else
	{
		ret = cur_station_no - 1;
		if(ret < ter_station_no)
		{
			ret = ter_station_no;
		}
		while(ccu_state.pp_skip_station_info[0][ret].effective == 0)
		{
			ret = cur_station_no -1;
			if(ret < ter_station_no)
			{
				ret = ter_station_no;
				break;
			}
		}
	}
	return ret;
}


//������������������������������������������
void SetLineInfo(unsigned char tmp_start_station_no,unsigned char tmp_terminal_station_no,unsigned char temp_current_mission_number)
{
	int i;
	for(i = tmp_start_station_no; i <= tmp_terminal_station_no; i++)
	{
		ccu_state.pp_skip_station_info[0][i].effective = 1;
	}
	ccu_state.current_line_no = 3;
	ccu_state.ccu_region_info[0].current_region_no = 0;
	ccu_state.ccu_region_info[0].current_region_info.region_no = 0;
	ccu_state.ccu_region_info[0].current_region_info.line_no = 3;
	ccu_state.ccu_region_info[0].current_region_info.running_direction = tmp_start_station_no>tmp_terminal_station_no?2:1;
	ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus = 0;
	ccu_state.ccu_region_info[0].current_station_no = tmp_start_station_no;
	ccu_state.ccu_region_info[0].current_region_info.start_station_no = tmp_start_station_no;
	ccu_state.ccu_region_info[0].current_region_info.over_station_no = tmp_terminal_station_no;
	if(ccu_state.is_this_server == 1)
	{
		diag_printf("SetHalfAtoModeLineInfo-cur=%d,ter=%d\n",tmp_start_station_no,tmp_terminal_station_no);
		whether_ccu_should_send_line_info = 1;
	}
	ShowRegionInfo_diag_printf(ccu_state.current_line_no);
	AlarmPeriDeviceThatLineOrRegionHasChanged();
}
void CalculateCurrentDateInfo(unsigned int  seconds)
{
    if(seconds > ENABLE_MAX_SECONDS_)
    {
        diag_printf("date is more than 2088\n");
        return ;
    }
    int begin_year=INITIAL_YEAR_;
    unsigned int dia = seconds/MAX_SECONDS_;
    unsigned long temp_seconds = MAX_SECONDS_*dia;
    if(seconds > temp_seconds)
    {
        seconds = seconds - temp_seconds;
        begin_year = begin_year + (2037-1970)*dia;
    }

    char current_time_seconds_string[1000];
//    itoa(seconds,current_time_seconds_string,10);
    sprintf(current_time_seconds_string,"%d",seconds);
    diag_printf("current_time_seconds_string = %s\n",current_time_seconds_string);
    time_t ttime = atoi(current_time_seconds_string);
    struct tm* tt;
    tt=localtime(&ttime);
    diag_printf("%04d/%02d/%02d %02d:%02d:%02d\n",tt->tm_year+begin_year, tt->tm_mon+1, tt->tm_mday,tt->tm_hour , tt->tm_min,tt->tm_sec);
    ccu_state.ccu_tms_car_running_info.current_time.year = tt->tm_year+begin_year-2000;
    ccu_state.ccu_tms_car_running_info.current_time.month = tt->tm_mon+1;
    ccu_state.ccu_tms_car_running_info.current_time.date = tt->tm_mday;
    ccu_state.ccu_tms_car_running_info.current_time.hour = tt->tm_hour;
    ccu_state.ccu_tms_car_running_info.current_time.minute = tt->tm_min;
    ccu_state.ccu_tms_car_running_info.current_time.second = tt->tm_sec;
}

void ShowDoorStateAndStrategy(unsigned char tcms_door_1,unsigned char tcms_door_2)
{
	if((tcms_door_1 & 0x01) ==  0x01)
	{
		diag_printf("door close command\n");
	}
	if((tcms_door_2 & 0x80) ==  0x80)
	{
		diag_printf("all doors are closed\n");
	}
	if((tcms_door_2 & 0x40) ==  0x40)
	{
		diag_printf("open a side door\n");
	}
	if((tcms_door_2 & 0x20) ==  0x20)
	{
		diag_printf("open b side door\n");
	}

	if((tcms_door_2 & 0x10) ==  0x10)
	{
		diag_printf("will open any door\n");
	}
	if((tcms_door_2 & 0x08) ==  0x08)
	{
		diag_printf("will open a door\n");
	}
	if((tcms_door_2 & 0x04) ==  0x04)
	{
		diag_printf("will open b door\n");
	}
	if((tcms_door_2 & 0x02) ==  0x02)
	{
		diag_printf("will open both door\n");
	}
}
void TCMSCMUStateInit(common_package_t *p_tcms_cmu_state)
{
	p_tcms_cmu_state->dst_dev_number = MUL_SYNC_DST_NO;
	strcpy(p_tcms_cmu_state->dst_dev_name,"BCU");
	p_tcms_cmu_state->pkg_type = COMMON_PACKAGE_TYPE_TCMS_CMU_STATE;
	strcpy(p_tcms_cmu_state->src_dev_name,ccu_state.ccu_info.devices_name);
	p_tcms_cmu_state->src_dev_number = ccu_state.ccu_info.devices_no;
	p_tcms_cmu_state->common_data_u.tcms_cmu_state.get_or_set = 0;
}

void TCMSDeviceVolumeInit(common_package_t *p_to_bcu,common_package_t *p_to_eamp,common_package_t *p_to_other_ccu)
{
	p_to_bcu->dst_dev_number = MUL_DST_NO;/////2015-5-6 wilson
	strcpy(p_to_bcu->dst_dev_name,"BCU");
	p_to_bcu->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_bcu->src_dev_name,ccu_state.ccu_info.devices_name);
	p_to_bcu->src_dev_number = ccu_state.ccu_info.devices_no;
	strcpy(p_to_bcu->common_data_u.ts_dev_volume_info.device_name,"EAMP");

	p_to_eamp->dst_dev_number = MUL_DST_NO;
	strcpy(p_to_eamp->dst_dev_name,"EAMP");
	p_to_eamp->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_eamp->src_dev_name,ccu_state.ccu_info.devices_name);
	p_to_eamp->src_dev_number = ccu_state.ccu_info.devices_no;
	strcpy(p_to_eamp->common_data_u.ts_dev_volume_info.device_name,"EAMP");

	p_to_other_ccu->dst_dev_number = ccu_state.other_ccu_no;
	strcpy(p_to_other_ccu->dst_dev_name,"CCU");
	p_to_other_ccu->pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
	strcpy(p_to_other_ccu->src_dev_name,ccu_state.ccu_info.devices_name);
	p_to_other_ccu->src_dev_number = ccu_state.ccu_info.devices_no;
	strcpy(p_to_other_ccu->common_data_u.ts_dev_volume_info.device_name,"EAMP");
}

void TMSATOAnnPackageInit(network_send_package_t *p_ato_ann)
{
	p_ato_ann->dst_devices_no = MUL_DST_NO;
	strcpy(p_ato_ann->dst_devices_name,"EAMP");
	p_ato_ann->send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
	p_ato_ann->send_information.event_type_intercom = INTERCOM_IDLE;
	p_ato_ann->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_ato_ann->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.line_number = ccu_state.current_line_no;
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.tms_type_flag  = 1;
//	if(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] == 1)
	{
		p_ato_ann->send_information.event_info_ann.tms_auto_announce.key_side  = ccu_state.ccu_info.devices_no -1;
	}

}

void TMSSkipStationPackageInit(network_send_package_t *p_skip_station)
{
	p_skip_station->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_skip_station->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_skip_station->dst_devices_no =  MUL_DST_NO;
	strcpy(p_skip_station->dst_devices_name,"EAMP");
	p_skip_station->send_information.event_type_ann = SKIP_STATION;
	p_skip_station->send_information.event_type_intercom = INTERCOM_IDLE;
}

void UpdateSkipStationInfo(unsigned char start_id, unsigned char end_id)
{
	int i,ret;
	for(i = start_id; i <= end_id; i++)
	{
		ccu_state.pp_skip_station_info[0][i].effective = 0;
		tcms_skip_station_info.send_information.event_info_ann.station_information.line_number = ccu_state.current_line_no;
		tcms_skip_station_info.send_information.event_info_ann.station_information.station_no = i;
		tcms_skip_station_info.send_information.event_info_ann.station_information.effective = 0;
		do
		{
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id, &tcms_skip_station_info, sizeof(tcms_skip_station_info));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);
		cyg_thread_delay(20);
		diag_printf("send skip station info at the same time -----------skip_station:%d\n",i);
	}
}
void TMSEmergCMDInfoInit(network_send_package_t *p_emerg_cmd)
{
	p_emerg_cmd->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_emerg_cmd->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_emerg_cmd->dst_devices_no =  MUL_DST_NO;
	strcpy(p_emerg_cmd->dst_devices_name,"EAMP");
	p_emerg_cmd->send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
	p_emerg_cmd->send_information.event_type_intercom = INTERCOM_IDLE;
	p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_type_flag = 4;
	p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
	p_emerg_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
}

void TMSEffectiveCMDInfoInit(network_send_package_t *p_effective_cmd)
{
	p_effective_cmd->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_effective_cmd->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_effective_cmd->dst_devices_no =  MUL_DST_NO;
	strcpy(p_effective_cmd->dst_devices_name,"BCU");
	p_effective_cmd->send_information.event_type_ann = ANN_IDLE;
	p_effective_cmd->send_information.event_type_intercom = INTERCOM_IDLE;
	p_effective_cmd->send_information.update_tcms_effective_flag = 1;
//	p_effective_cmd->send_information.tcms_effective_flag_info
}

void UpdatePASoftwareVersionInfoToTMS(int flag)
{
	switch(flag)
	{
		case 0:
			memcpy((void *)&ccu_state.pa_running_info_to_tms[6],(void *)&ccu_state.bcu_software_version,3);
			break;
		case 1:
			memcpy((void *)&ccu_state.pa_running_info_to_tms[12],(void *)&ccu_state.media_software_version,3);
			break;
		case 2:
			memcpy((void *)&ccu_state.pa_running_info_to_tms[9],(void *)&ccu_state.monitor_software_version,3);
			break;
		default:
			break;
	}
}

void UpdatePISDeviceVolumeInfoToTMS(int flag)
{
	switch(flag)
	{
		case 1:
//			ccu_state.current_media_volume[0] = ccu_state.tcms_media_volume[0];
			ccu_state.pa_running_info_to_tms[20] = ccu_state.current_media_volume[0];
			break;
		case 2:
			ccu_state.pa_running_info_to_tms[21] = ccu_state.current_pa_volume;
			break;
		default:
			break;
	}
}

void UpdateBroadcastModeToTMS(int flag)
{
	if(ccu_state.simulate_flag == 1)
	{
		ccu_state.pa_running_info_to_tms[19] = 4;
	}
	else
	{
		ccu_state.pa_running_info_to_tms[19] = flag;
	}
}

void UpdatePARunningStationInfoToTMS(unsigned char begin_station_code,unsigned char terminal_station_code,
										unsigned char current_station_code,unsigned char next_station_code)
{
	ccu_state.pa_running_info_to_tms[15] = begin_station_code;
	ccu_state.pa_running_info_to_tms[16] = terminal_station_code;
	ccu_state.pa_running_info_to_tms[17] = current_station_code;
	ccu_state.pa_running_info_to_tms[18] = next_station_code;
}

void UpdateKeyInfoToTMS(int flag)
{
	ccu_state.pa_running_info_to_tms[24] &= 0xFE;
	ccu_state.pa_running_info_to_tms[24] |= (flag < 0);
}

void UpdateMediaFailureToTMS()
{
	ccu_state.pa_failure_info[0] &= 0x7F;
	ccu_state.pa_failure_info[0] |= (ccu_state.media_failure_info[0] & 0x80);

	ccu_state.pa_failure_info[1] &= 0x1E;
	ccu_state.pa_failure_info[1] |= (ccu_state.media_failure_info[1] & 0xE1);

	ccu_state.pa_failure_info[2] &= 0xF8;
	ccu_state.pa_failure_info[2] |= (ccu_state.media_failure_info[2] & 0x07);
}

void UpdateMonitorFailureToTMS()
{
	ccu_state.pa_failure_info[0] &= 0x9F;
	ccu_state.pa_failure_info[0] |= (ccu_state.monitor_failure_info[0] & 0x60);

	ccu_state.pa_failure_info[1] &= 0xE1;
	ccu_state.pa_failure_info[1] |= (ccu_state.monitor_failure_info[0] & 0x1E);

	ccu_state.pa_failure_info[3] &= 0x7E;
	ccu_state.pa_failure_info[3] |= (ccu_state.monitor_failure_info[1] & 0x80);

	ccu_state.pa_failure_info[4] = 0;
	ccu_state.pa_failure_info[4] |= ccu_state.monitor_failure_info[2];

	ccu_state.pa_failure_info[5] = 0;
	ccu_state.pa_failure_info[5] |= ccu_state.monitor_failure_info[3];

	ccu_state.pa_failure_info[6] = 0;
	ccu_state.pa_failure_info[6] |= ccu_state.monitor_failure_info[4];

	ccu_state.pa_failure_info[7] = 0;
	ccu_state.pa_failure_info[7] |= ccu_state.monitor_failure_info[5];

	ccu_state.pa_failure_info[8] &= 0xF8;
	ccu_state.pa_failure_info[8] |= (ccu_state.monitor_failure_info[6] & 0x07);
}

void UpdateNDFailureToTMS()
{
	ccu_state.pa_failure_info[11] &= 0x7F;
	ccu_state.pa_failure_info[11] |= (ccu_state.nd_failure_info[0] & 0x80);

	ccu_state.pa_failure_info[12] = 0;
	ccu_state.pa_failure_info[12] |= ccu_state.nd_failure_info[1];

	ccu_state.pa_failure_info[13] &= 0xF8;
	ccu_state.pa_failure_info[13] |= (ccu_state.nd_failure_info[2] & 0x07);
}

void UpdateDMFailureToTMS()
{
	ccu_state.pa_failure_info[13] &= 0x07;
	ccu_state.pa_failure_info[13] |= (ccu_state.dm_failure_info[0] & 0xF8);

	ccu_state.pa_failure_info[14] &= 0xFE;
	ccu_state.pa_failure_info[14] |= (ccu_state.dm_failure_info[1] & 0x01);
}

void UpdateEAMPFailureToTMS()
{
	ccu_state.pa_failure_info[2] &= 0x07;
	ccu_state.pa_failure_info[2] |= (ccu_state.eamp_failure_info[0] & 0xF8);

	ccu_state.pa_failure_info[3] &= 0x80;
	ccu_state.pa_failure_info[3] |= (ccu_state.eamp_failure_info[1] & 0x7F);
}

void UpdateCCUFailureToTMS()
{
	ccu_state.pa_failure_info[0] &= 0xE7;
	ccu_state.pa_failure_info[0] |= (ccu_state.ccu_failure_info & 0x18);
}

void UpdateBCUFailureToTMS()
{
	ccu_state.pa_failure_info[8] &= 0xE7;
	ccu_state.pa_failure_info[8] |= (ccu_state.bcu_failure_info & 0x18);
}

void UpdateLEDFailureToTMS()
{
	ccu_state.pa_failure_info[8] &=  0x1F;
	ccu_state.pa_failure_info[8] |= (ccu_state.led_failure_info[0] & 0xE0);

	ccu_state.pa_failure_info[9] =  0;
	ccu_state.pa_failure_info[9] |= ccu_state.led_failure_info[1];

	ccu_state.pa_failure_info[10] =  0;
	ccu_state.pa_failure_info[10] |= ccu_state.led_failure_info[2];

	ccu_state.pa_failure_info[11] &=  0x80;
	ccu_state.pa_failure_info[11] |= (ccu_state.led_failure_info[3]&0x7F);
}

void SendTCMSEffectivateFlagToBCU(network_send_package_t tms_effective_cmd,unsigned char tms_effective_flag)
{
	tms_effective_cmd.send_information.tcms_effective_flag_info.get_or_set_tcms_effective_flag = 0;
	tms_effective_cmd.send_information.tcms_effective_flag_info.tms_effective_flag = tms_effective_flag;
	strcpy(tms_effective_cmd.dst_devices_name,"BCU");
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_effective_cmd, sizeof(tms_effective_cmd));
//	strcpy(tms_effective_cmd.dst_devices_name,"EAMP");
//	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_effective_cmd, sizeof(tms_effective_cmd));
}

void SendTCMSLineAndRegionInfo()
{
	network_send_package_t tms_line_and_region_cmd;
	tms_line_and_region_cmd.dst_devices_no = MUL_DST_NO;
	strcpy(tms_line_and_region_cmd.send_information.src_devices_name,"CCU");
	tms_line_and_region_cmd.send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	tms_line_and_region_cmd.send_information.event_type_ann = LINE_NUMBER_CHANGE_EVENT;
	tms_line_and_region_cmd.send_information.event_type_intercom = INTERCOM_IDLE;
	tms_line_and_region_cmd.send_information.event_info_ann.line_info.line_no = ccu_state.current_line_no;
	tms_line_and_region_cmd.send_information.event_info_ann.line_info.region_no = 0;
	tms_line_and_region_cmd.send_information.event_info_ann.line_info.region_update_type = 1;

	tms_line_and_region_cmd.send_information.event_info_ann.line_info.start_station_no = ddu_start_stataion_id;
	tms_line_and_region_cmd.send_information.event_info_ann.line_info.current_station_no = ddu_start_stataion_id;
	tms_line_and_region_cmd.send_information.event_info_ann.line_info.terminal_station_no = ddu_end_stataion_id;

	tms_line_and_region_cmd.send_information.event_info_ann.line_info.up_or_down =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction;

	tms_line_and_region_cmd.send_information.event_info_ann.line_info.whether_the_cycle =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus;

	tms_line_and_region_cmd.send_information.event_info_ann.line_info.get_or_set = 0;
	int ret = 0;
	strcpy(tms_line_and_region_cmd.dst_devices_name,"EAMP");
	do
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_line_and_region_cmd, sizeof(tms_line_and_region_cmd));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0);


	strcpy(tms_line_and_region_cmd.dst_devices_name,"BCU");
	do
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_line_and_region_cmd, sizeof(tms_line_and_region_cmd));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0);


	tms_line_and_region_cmd.dst_devices_no = ccu_state.other_ccu_no;
	strcpy(tms_line_and_region_cmd.dst_devices_name,"CCU");
	do
	{
		ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tms_line_and_region_cmd, sizeof(tms_line_and_region_cmd));
		if(ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(ret <= 0);

}

void SendCurrentCCULineInfo()
{
	diag_printf("SendCurrentCCULineInfo-01\n");
	network_send_package_t line_and_region_cmd;
	line_and_region_cmd.dst_devices_no = MUL_DST_NO;
	strcpy(line_and_region_cmd.send_information.src_devices_name,"CCU");
	line_and_region_cmd.send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	line_and_region_cmd.send_information.event_type_ann = LINE_NUMBER_CHANGE_EVENT;
	line_and_region_cmd.send_information.event_type_intercom = INTERCOM_IDLE;
	line_and_region_cmd.send_information.event_info_ann.line_info.line_no = ccu_state.current_line_no;
	line_and_region_cmd.send_information.event_info_ann.line_info.region_no =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_no;

	if(line_and_region_cmd.send_information.event_info_ann.line_info.region_no != 0)
	{
		line_and_region_cmd.send_information.event_info_ann.line_info.region_update_type = 0;
	}
	else
	{
		line_and_region_cmd.send_information.event_info_ann.line_info.region_update_type = 1;
	}

	line_and_region_cmd.send_information.event_info_ann.line_info.start_station_no =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.start_station_no;

	line_and_region_cmd.send_information.event_info_ann.line_info.terminal_station_no =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.over_station_no;

	line_and_region_cmd.send_information.event_info_ann.line_info.up_or_down =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction;

	line_and_region_cmd.send_information.event_info_ann.line_info.whether_the_cycle =
			ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus;

	line_and_region_cmd.send_information.event_info_ann.line_info.get_or_set = 0;
	diag_printf("line-%d\n",line_and_region_cmd.send_information.event_info_ann.line_info.line_no);
	//diag_printf("region-%d\n",line_and_region_cmd.send_information.event_info_ann.line_info.region_no);
	strcpy(line_and_region_cmd.dst_devices_name,"EAMP");
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&line_and_region_cmd, sizeof(line_and_region_cmd));
	strcpy(line_and_region_cmd.dst_devices_name,"BCU");
	BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&line_and_region_cmd, sizeof(line_and_region_cmd));

}

void SendHardSignalManual(unsigned char open_door_side,unsigned char stop_or_leave)
{}

void InitTCMSPackage()
{
	///<���������TCMS������������PIS������������������������������-������������������������������-BCU-EAMP-CCU
	TCMSDeviceVolumeInit(&tcms_volume_info_to_bcu,&tcms_volume_info_to_eamp,&tcms_volume_info_to_other_ccu);
	TCMSCMUStateInit(&tcms_cmu_state_to_bcu_and_cmu);
	///<���������������TCMS���������������������EAMP
	TMSEmergCMDInfoInit(&tcms_emerg_cmd);
	InitHalfAutoPackage(&tcms_half_auto_ann);
}


void SendTCMSVolumeToAllDevice()
{///<������TCMS���������������������������
	int temp_ret = 0;
	do
	{
		temp_ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &tcms_volume_info_to_bcu, sizeof(tcms_volume_info_to_bcu));
		if(temp_ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(temp_ret <= 0);

	do
	{
		temp_ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &tcms_volume_info_to_eamp, sizeof(tcms_volume_info_to_eamp));
		if(temp_ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(temp_ret <= 0);

	do
	{
		temp_ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &tcms_volume_info_to_other_ccu, sizeof(tcms_volume_info_to_other_ccu));
		if(temp_ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(temp_ret <= 0);
}

void ShowTCMSSkipStationInfo()
{///<������TCMS������������������
	int i = 0;
	for(i = 1;i <= 29;i++)
	{
		if(station_information[0][i].effective == 0)
		{
			diag_printf("line3:%d,%d\n",i,station_information[0][i].effective);
		}
	}
	for(i = 1;i <= 26;i++)
	{
		if(station_information[1][i].effective == 0)
		{
			diag_printf("line4:%d,%d\n",i,station_information[1][i].effective);
		}
	}
}

void SendATCCMD()
{///<������ATC������
	if(ccu_state.is_this_server == 1)
	{
		diag_printf("\nAUTO ccu_state.is_this_server == 1\n");
		if(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] == 0)
		{
			tcms_ato_ann.send_information.event_info_ann.tms_auto_announce.key_side = 1;
		}
		else
		{
			tcms_ato_ann.send_information.event_info_ann.tms_auto_announce.key_side  = ccu_state.ccu_info.devices_no -1;
		}
		int ret = 0;
		do
		{
			diag_printf("\n AUTO write buffer to network\n");
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tcms_ato_ann, sizeof(tcms_ato_ann));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);
	}
	RestartSYNCTimer();
}

void SendDDUCMD()
{///<������DDU ������
	int ret = 0;
	if(tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.line_number == 3)
	{
		diag_printf("3333-%d\n",ccu_state.ccu_region_info[0].total_stations_number);
		if(tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code <= 0 ||
				tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code > ccu_state.ccu_region_info[0].total_stations_number)
		{
			diag_printf("error station no-line3-%d\n",tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code);
			return;
		}
	}
	else if(tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.line_number == 4)
	{
		diag_printf("4444-%d\n",ccu_state.ccu_region_info[1].total_stations_number);
		if(tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code <= 0 ||
				tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code >
			ccu_state.ccu_region_info[1].total_stations_number)
		{
			diag_printf("error station no-line4-%d\n",tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code);
			return;
		}
	}
	if(ccu_state.is_this_server == 1)
	{
		if(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] == 0)
		{
			tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.key_side = 1;
		}
		else
		{
			tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.key_side  = ccu_state.ccu_info.devices_no -1;
		}
		do
		{
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tcms_ddu_ann, sizeof(tcms_ddu_ann));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);
		cyg_thread_delay(100);
	}
	RestartSYNCTimer();
	cyg_thread_delay(100);
}

void SendDDUCMDAgain()
{///<������������DDU ������
	int ret = 0;
	if(ccu_state.is_this_server == 1)
	{

		if(last_tcms_ddu_manual_ann.send_information.event_info_ann.tms_auto_announce.line_number !=
				ccu_state.current_line_no ||
				last_tcms_ddu_manual_ann.send_information.event_info_ann.tms_auto_announce.start_station_code !=
						ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.start_station_no ||
				last_tcms_ddu_manual_ann.send_information.event_info_ann.tms_auto_announce.end_station_code !=
						ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.over_station_no)
		{//2015-5-25
			diag_printf("adad\n");
			return;
		}
		do
		{
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&last_tcms_ddu_manual_ann, sizeof(last_tcms_ddu_manual_ann));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);
	}
	RestartSYNCTimer();
}

void SendTCMSEmergInfo()
{
	int ret = 0;
//	TCMSNoticeMediaEmergInfo(&(tcms_emerg_cmd.send_information));
	if(ccu_state.is_this_server == 1)
	{
		diag_printf("SendTCMSEmergInfo\n");
		do
		{
			tcms_emerg_cmd.send_information.event_info_ann.tms_auto_announce.tms_type_flag = 4;
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tcms_emerg_cmd, sizeof(tcms_emerg_cmd));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);
	}
	RestartSYNCTimer();
}


void SendCMUStateToBCUAndCMU(unsigned char current_cmu_state)
{//tcms_cmu_state_to_bcu_and_cmu
	int temp_ret = 0,a= 0;
	if(ccu_state.is_this_server == 0)
	{
		//return;
	}
	strcpy(tcms_cmu_state_to_bcu_and_cmu.dst_dev_name,"BCU");
	tcms_cmu_state_to_bcu_and_cmu.common_data_u.tcms_cmu_state.tcms_current_cmu_state = current_cmu_state;
	a = 0;
	do
	{
		temp_ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &tcms_cmu_state_to_bcu_and_cmu, sizeof(tcms_cmu_state_to_bcu_and_cmu));
		a++;
		if(temp_ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(temp_ret <= 0 && a <= 2);

	strcpy(tcms_cmu_state_to_bcu_and_cmu.dst_dev_name,"CMU");
	a = 0;
	do
	{
		temp_ret = BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &tcms_cmu_state_to_bcu_and_cmu, sizeof(tcms_cmu_state_to_bcu_and_cmu));
		a++;
		if(temp_ret <= 0)
		{
			cyg_thread_delay(10);
		}
	}while(temp_ret <= 0 && a <= 2);
}

void SendTCMSNetworkToPA()
{
	if(whether_ccu_should_send_emerg_info == 1)
	{
		SendTCMSEmergInfo();
		whether_ccu_should_send_emerg_info = 0;
	}

	if(whether_ccu_should_send_line_info == 1)
	{
		whether_ccu_should_send_line_info = 0;
		SendTCMSLineAndRegionInfo();
	}
	if(whether_ccu_should_send_manual_ann_info == 1)
	{
		whether_ccu_should_send_manual_ann_info = 0;
		SendTCMSManualInfo();
	}
	if(skip_station_flag == 1)
	{
		skip_station_flag = 0;
		UpdateSkipStationInfo(ddu_skip_first_station_id, ddu_skip_last_station_id);
	}
}
void SendTCMSManualInfo()
{
	int ret = 0;
	diag_printf("!!!!!!!!!!!!!!!-is server=%d\n",ccu_state.is_this_server);
	if(ccu_state.is_this_server == 1)
	{
		do
		{
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tcms_ddu_ann, sizeof(tcms_ddu_ann));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);

	}
	RestartSYNCTimer();
}

void SetManualInfo(unsigned char s_no,unsigned char c_no,unsigned char t_no, unsigned char up_down, unsigned char stop_or_leave)
{
	tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.line_number = 3;
	tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.current_station_code = c_no;
	tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.door_side = 1;
	tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.end_station_code = t_no;
	tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.stop_or_leave = stop_or_leave;
	if(up_down == 1)
		tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.up_down = 1;
	else if(up_down == 2)
		tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.up_down = 2;
	tcms_ddu_ann.send_information.event_info_ann.tms_auto_announce.start_station_code = s_no;
}
void SendSyncSignal()
{
	if(wherther_send_sync_signal == 1)
	{
		wherther_send_sync_signal = 0;
		if(ccu_state.is_this_server == 1)
		{
			SendSYNCPackageToEamp();
		}
		CloseSYNCTimer();
	}
}
//���������������������������

void GetDoorState(unsigned char ccu_recv_tms_info[])
{
	memset(&ccu_state.door_emerg_unlock_state_and_smoke_info[0],0x00,sizeof(ccu_state.door_emerg_unlock_state_and_smoke_info));
	int i = 0;
	if((ccu_recv_tms_info[61] & 0x40) == 0x40)
	{//door A1-1
		ato_door_state_debug_print(("door A1-1\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[0] |= 0x80;
	}
	if((ccu_recv_tms_info[61] & 0x20) == 0x20)
	{//door A1-2
		ato_door_state_debug_print(("door A1-2\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x01;
	}
	if((ccu_recv_tms_info[61] & 0x10) == 0x10)
	{//door A1-3
		ato_door_state_debug_print(("door A1-3\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x02;
	}
	if((ccu_recv_tms_info[61] & 0x08) == 0x08)
	{//door A1-4
		ato_door_state_debug_print(("door A1-4\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x04;
	}
	if((ccu_recv_tms_info[61] & 0x04) == 0x04)
	{//door A1-5
		ato_door_state_debug_print(("door A1-5\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x08;
	}
	if((ccu_recv_tms_info[61] & 0x02) == 0x02)
	{//door A1-6
		ato_door_state_debug_print(("door A1-6\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x10;
	}
	if((ccu_recv_tms_info[61] & 0x01) == 0x01)
	{//door A1-7
		ato_door_state_debug_print(("door A1-7\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x20;
	}
	if((ccu_recv_tms_info[62] & 0x80) == 0x80)
	{//door A1-8
		ato_door_state_debug_print(("door A1-8\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[1] |= 0x40;
	}


	if((ccu_recv_tms_info[62] & 0x10) == 0x10)
	{//door B1-1
		ato_door_state_debug_print(("door B1-1\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[2] |= 0x02;
	}
	if((ccu_recv_tms_info[62] & 0x08) == 0x08)
	{//door B1-2
		ato_door_state_debug_print(("door B1-2\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[2] |= 0x04;
	}
	if((ccu_recv_tms_info[62] & 0x04) == 0x04)
	{//door B1-3
		ato_door_state_debug_print(("door B1-3\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[2] |= 0x08;
	}
	if((ccu_recv_tms_info[62] & 0x02) == 0x02)
	{//door B1-4
		ato_door_state_debug_print(("door B1-4\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[2] |= 0x10;
	}
	if((ccu_recv_tms_info[62] & 0x01) == 0x01)
	{//door B1-5
		ato_door_state_debug_print(("door B1-5\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[2] |= 0x20;
	}
	if((ccu_recv_tms_info[63] & 0x80) == 0x80)
	{//door B1-6
		ato_door_state_debug_print(("door B1-6\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x01;
	}
	if((ccu_recv_tms_info[63] & 0x40) == 0x40)
	{//door B1-7
		ato_door_state_debug_print(("door B1-7\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x02;
	}
	if((ccu_recv_tms_info[63] & 0x20) == 0x20)
	{//door B1-8
		ato_door_state_debug_print(("door B1-8\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x04;
	}

	if((ccu_recv_tms_info[63] & 0x04) == 0x04)
	{//door C1-1
		ato_door_state_debug_print(("door C1-1\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x08;
	}
	if((ccu_recv_tms_info[63] & 0x02) == 0x02)
	{//door C1-2
		ato_door_state_debug_print(("door C1-2\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x10;
	}
	if((ccu_recv_tms_info[63] & 0x01) == 0x01)
	{//door C1-3
		ato_door_state_debug_print(("door C1-3\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x20;
	}
	if((ccu_recv_tms_info[64] & 0x80) == 0x80)
	{//door C1-4
		ato_door_state_debug_print(("door C1-4\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x40;
	}
	if((ccu_recv_tms_info[64] & 0x40) == 0x40)
	{//door C1-5
		ato_door_state_debug_print(("door C1-5\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[3] |= 0x80;
	}
	if((ccu_recv_tms_info[64] & 0x20) == 0x20)
	{//door C1-6
		ato_door_state_debug_print(("door C1-6\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[4] |= 0x01;
	}
	if((ccu_recv_tms_info[64] & 0x10) == 0x10)
	{//door C1-7
		ato_door_state_debug_print(("door C1-7\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[4] |= 0x02;
	}
	if((ccu_recv_tms_info[64] & 0x08) == 0x08)
	{//door C1-8
		ato_door_state_debug_print(("door C1-8\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[4] |= 0x04;
	}

	if((ccu_recv_tms_info[67] & 0x10) == 0x10)
	{//door C2-1
		ato_door_state_debug_print(("door C2-1\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[4] |= 0x20;
	}
	if((ccu_recv_tms_info[67] & 0x08) == 0x08)
	{//door C2-2
		ato_door_state_debug_print(("door C2-2\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[4] |= 0x40;
	}
	if((ccu_recv_tms_info[67] & 0x04) == 0x04)
	{//door C2-3
		ato_door_state_debug_print(("door C2-3\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[4] |= 0x80;
	}
	if((ccu_recv_tms_info[67] & 0x02) == 0x02)
	{//door C2-4
		ato_door_state_debug_print(("door C2-4\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[5] |= 0x01;
	}
	if((ccu_recv_tms_info[67] & 0x01) == 0x01)
	{//door C2-5
		ato_door_state_debug_print(("door C2-5\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[5] |= 0x02;
	}
	if((ccu_recv_tms_info[68] & 0x80) == 0x80)
	{//door C2-6
		ato_door_state_debug_print(("door C2-6\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[5] |= 0x04;
	}
	if((ccu_recv_tms_info[68] & 0x40) == 0x40)
	{//door C2-7
		ato_door_state_debug_print(("door C2-7\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[5] |= 0x08;
	}
	if((ccu_recv_tms_info[68] & 0x20) == 0x20)
	{//door C2-8
		ato_door_state_debug_print(("door C2-8\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[5] |= 0x10;
	}

	if((ccu_recv_tms_info[66] & 0x40) == 0x40)
	{//door B2-1
		ato_door_state_debug_print(("door B2-1\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[5] |= 0x08;
	}
	if((ccu_recv_tms_info[66] & 0x20) == 0x20)
	{//door B2-2
		ato_door_state_debug_print(("door B2-2\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x01;
	}
	if((ccu_recv_tms_info[66] & 0x10) == 0x10)
	{//door B2-3
		ato_door_state_debug_print(("door B2-3\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x02;
	}
	if((ccu_recv_tms_info[66] & 0x08) == 0x08)
	{//door B2-4
		ato_door_state_debug_print(("door B2-4\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x04;
	}
	if((ccu_recv_tms_info[66] & 0x04) == 0x04)
	{//door B2-5
		ato_door_state_debug_print(("door B2-5\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x08;
	}
	if((ccu_recv_tms_info[66] & 0x02) == 0x02)
	{//door B2-6
		ato_door_state_debug_print(("door B2-6\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x10;
	}
	if((ccu_recv_tms_info[66] & 0x01) == 0x01)
	{//door B2-7
		ato_door_state_debug_print(("door B2-7\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x20;
	}
	if((ccu_recv_tms_info[67] & 0x80) == 0x80)
	{//door B2-8
		ato_door_state_debug_print(("door B2-8\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[6] |= 0x40;
	}

	if((ccu_recv_tms_info[64] & 0x01) == 0x01)
	{//door A2-1
		ato_door_state_debug_print(("door A2-1\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x02;
	}
	if((ccu_recv_tms_info[65] & 0x80) == 0x80)
	{//door A2-2
		ato_door_state_debug_print(("door A2-2\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x04;
	}
	if((ccu_recv_tms_info[65] & 0x40) == 0x40)
	{//door A2-3
		ato_door_state_debug_print(("door A2-3\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x08;
	}
	if((ccu_recv_tms_info[65] & 0x20) == 0x20)
	{//door A2-4
		ato_door_state_debug_print(("door A2-4\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x10;
	}
	if((ccu_recv_tms_info[65] & 0x10) == 0x10)
	{//door A2-5
		ato_door_state_debug_print(("door A2-5\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x20;
	}
	if((ccu_recv_tms_info[65] & 0x08) == 0x08)
	{//door A2-6
		ato_door_state_debug_print(("door A2-6\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x40;
	}
	if((ccu_recv_tms_info[65] & 0x04) == 0x04)
	{//door A2-7
		ato_door_state_debug_print(("door A2-7\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[7] |= 0x80;
	}
	if((ccu_recv_tms_info[65] & 0x02) == 0x02)
	{//door A2-8
		ato_door_state_debug_print(("door A2-8\n"));
		ccu_state.door_emerg_unlock_state_and_smoke_info[8] |= 0x01;
	}

}

void CountButtonClick(unsigned char array[])
{
	if(current_pa_mode == 2 && (array[59]&0x40 == 0x40)&& halfAuto_depart_flag != 1)
	{
		halfAuto_depart_flag = 1;
		diag_printf("HalfAuto car begin to depart\n");
	}else if(current_pa_mode == 2 && (array[59]&0x40 == 0x00) && halfAuto_depart_flag != 0)
	{
		halfAuto_depart_flag = 0;
		diag_printf("HalfAuto car begin to depart\n");
	}
	if(halfAuto_depart_flag == 1)
	{
		count_distance_flag = 1;
	}
	if((array[59] & 0x04) == 0 && (button_F1 & 0x04) == 0x04)
	{
		count_F1 = 1;
		diag_printf("count_F1 clicked:b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);

	}
	button_F1 = array[59]&0x04;
	if((array[59] & 0x02) == 0 && (button_F2 & 0x02) == 0x02)
	{
		count_F2 = 1;
		diag_printf("count_F2 clicked:b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
	}
	button_F2 = array[59]&0x02;
	if((array[59] & 0x01) == 0 && (button_F3 & 0x01) == 0x01)
	{
		count_F3 = 1;
		diag_printf("count_F3 clicked:b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
	}
	button_F3 = array[59]&0x01;
	if((array[60] & 0x80) == 0 && (button_F4 & 0x80) == 0x80)
	{
		count_F4 = 1;
		diag_printf("count_F4 clicked:b:%d-c:%d,n:%d,t:%d\n",ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
	}
	button_F4 = array[60]&0x80;

	if( count_F2 == 1 && count_F3 == 1 && count_F1 == 1 && count_F4 == 1)
	{
		count_F1 = 0;
		count_F2 = 0;
		count_F3 = 0;
		count_F4 = 0;
		diag_printf("count_F1 && count_F2 && F3 && F4 clicked\n");
		if(current_mission_number == 1)
		{
			ddu_current_stataion_id = GetTCMSNextStation(ddu_current_stataion_id, ddu_end_stataion_id);
			ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id, ddu_end_stataion_id);
			if(ddu_current_stataion_id >= ddu_end_stataion_id)
				ddu_current_stataion_id = ddu_end_stataion_id;
    		diag_printf("mission=%d:b:%d-c:%d,n:%d,t:%d\n",current_mission_number,ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
		}
		else if(current_mission_number == 2)
		{
			ddu_current_stataion_id = GetTCMSNextStation(ddu_current_stataion_id, ddu_end_stataion_id);
			ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id, ddu_end_stataion_id);
			if(ddu_current_stataion_id <= ddu_end_stataion_id)
				ddu_current_stataion_id = ddu_end_stataion_id;
    		diag_printf("mission=%d:b:%d-c:%d,n:%d,t:%d\n",current_mission_number,ddu_start_stataion_id,ddu_current_stataion_id,ddu_next_stataion_id,ddu_end_stataion_id);
		}
	}
}

void TcmsSendHalfAutoCmd(unsigned char s_n, unsigned char t_n, unsigned char c_n,  unsigned char n_n, unsigned char up_down, unsigned char stop_leave)
{
	int ret=0;
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.line_number = ccu_state.current_line_no;
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.start_station_code = s_n;
	///<CCU下一站填写给EAMP的当前站
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.current_station_code = n_n;
	///<CCU下下一站填写给EAMP的下一站
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.next_station_code = GetTCMSNextStation(n_n, t_n);
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.end_station_code = t_n;

	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.door_side = ccu_state.pp_station_info[0][n_n].open_door_side[up_down - 1];
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.stop_or_leave = stop_leave;
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.up_down = up_down;
	tcms_half_auto_ann.send_information.event_info_ann.tms_auto_announce.key_side = ccu_state.key_info;
	diag_printf("HALF:stop_or_leave:%d next station:%d terminal station:%d,up_or down:%d door_side:%d,cycle = %d\n",\
				stop_leave,n_n,t_n,up_down,\
				ccu_state.pp_station_info[LOOP_A][n_n].open_door_side[up_down - 1],\
				ccu_state.ccu_region_info[LOOP_A].current_region_info.whether_the_cycle_bus);
//	BlockBufferWrite(ccu_state.ccu_recv_cmd_buffer_id, (void *)&tcms_half_auto_ann, sizeof(tcms_half_auto_ann));
#if 1
	strcpy(tcms_half_auto_ann.dst_devices_name,"BCU");
	if(1 == ccu_state.is_this_server)
		{
			do
			{
				ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tcms_half_auto_ann, sizeof(tcms_half_auto_ann));
				if(ret <= 0)
				{
					cyg_thread_delay(10);
				}
			}while(ret <= 0);
		}
#endif

	strcpy(tcms_half_auto_ann.dst_devices_name,"EAMP");
	if(1 == ccu_state.is_this_server)
	{
		do
		{
			ret = BlockBufferWrite(ccu_state.ccu_send_cmd_buffer_id,&tcms_half_auto_ann, sizeof(tcms_half_auto_ann));
			if(ret <= 0)
			{
				cyg_thread_delay(10);
			}
		}while(ret <= 0);
	}
	RestartSYNCTimer();
}

void InitHalfAutoPackage(network_send_package_t *p_ato_ann)
{
	p_ato_ann->dst_devices_no = MUL_DST_NO;
	strcpy(p_ato_ann->dst_devices_name,"BCU");
	p_ato_ann->send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
	p_ato_ann->send_information.event_type_intercom = INTERCOM_IDLE;
	p_ato_ann->send_information.src_devices_no = ccu_state.ccu_info.devices_no;
	strcpy(p_ato_ann->send_information.src_devices_name,ccu_state.ccu_info.devices_name);
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.line_number = ccu_state.current_line_no;
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.tms_auto_active = 1;
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over = 1;
	p_ato_ann->send_information.event_info_ann.tms_auto_announce.tms_type_flag  = 2;
}

void alarm_func_handle_mvb_recv(cyg_handle_t counter_handle, cyg_addrword_t data)
{
	sem_post(&sem_mvb_new_cmd);
}

/*Create the mvb receive timer*/
int CreateMvbRecvTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_mvb_recv);
	cyg_alarm_create(counter_handle_mvb_recv,alarm_func_handle_mvb_recv,0,&alarm_handle_mvb_recv,&alarm_object_mvb_recv);
	cyg_alarm_initialize(alarm_handle_mvb_recv,cyg_current_time()+25,25);
	return 0;
}

/*Delete the mvb receive timer*/
int DeleteMvbRecvTimer()
{
	cyg_alarm_delete(alarm_handle_mvb_recv);
	return 0;
}

/*Set the mvb receive timer time interval*/
int SetMvbRecvTimeInterval(int time_interval)
{
	//if(bcu_state.this_bcu_request == 0)
	if(1)
	{
		cyg_alarm_initialize(alarm_handle_mvb_recv,cyg_current_time()+time_interval,time_interval);
	}
	else
	{
		cyg_alarm_initialize(alarm_handle_mvb_recv,cyg_current_time()+(20),time_interval);
	}
	return 0;
}

/*Open the mvb reveive timer*/
int OpenMvbRecvTimer()
{
	cyg_alarm_enable(alarm_handle_mvb_recv);
	return 0;
}

/*Close the mvb receive timer*/
int CloseMvbRecvTimer()
{
	diag_printf("CloseMvbRecvTimer\n");
	cyg_alarm_disable(alarm_handle_mvb_recv);
	return 0;
}

/*Start the audio sample timer*/
void StartMvbRecvTimer()
{
	SetMvbRecvTimeInterval(20);///<设置定时时间per 10ms
	OpenMvbRecvTimer();
}

void ResetMvbRecvTimer()
{///<
	cyg_alarm_initialize(alarm_handle_mvb_recv,cyg_current_time()+22,22);
	OpenMvbRecvTimer();
}

void InitStationDistance()
{
	int i;
	for(i = 1; i <= 15; i++)
	{
//		if(i == 1)
//		{
//			public_station_distance[i] = 1300 * public_distance_expand_times;
//		}
//		else
		public_station_distance[i-1] = (ccu_state.ccu_tms_station_diatance[0][i].distance-ccu_state.ccu_tms_station_diatance[0][i-1].distance)*public_distance_expand_times;
		diag_printf("station[%d]:distance:%ld\n",i,public_station_distance[i-1]);
	}
	ddu_next_stataion_id = GetTCMSNextStation(ddu_current_stataion_id,ddu_end_stataion_id);
	public_pre_station_distance = ccu_state.ccu_tms_station_diatance[0][ddu_next_stataion_id].pre_triger_distance[current_mission_number - 1]*public_distance_expand_times;
	public_next_station_distance = ccu_state.ccu_tms_station_diatance[0][ddu_next_stataion_id].triger_distance[current_mission_number - 1]*public_distance_expand_times;
	distanceToNext = CountDistanceToNext(ddu_current_stataion_id, ddu_next_stataion_id);
	diag_printf("distanceToNext:%ld\n",distanceToNext);

}

unsigned long CountDistanceToNext(unsigned char current_no, unsigned char next_no)
{
	int i;
	unsigned long ret = 0;
	if(current_no < next_no)
	{
		for(i = current_no; i < next_no; i++)
		{
			ret += public_station_distance[i+1];
		}
	}else if(current_no > next_no)
	{
		for(i = current_no; i > next_no; i--)
		{
			ret += public_station_distance[i-1];
		}
	}
	return ret;
}
unsigned char GetIndexFromStation(unsigned char station_no)
{
	int i;
	unsigned char ret = -1;
	for(i = 1; i <= 50; i++)
	{
		if(station_no == station_id_mapping_table[i])
		{
			ret = i;
			break;
		}
	}
	return ret;
}

