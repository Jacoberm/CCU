/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� media_and_monitor.c
 * ���������������
 * ���    ������ ���������
 *
 * ��������������� V1.0.0
 * ���    ������ wilson
 * ���������������2013-09-05
 *
 * ���������������
 * ���������  ���
 * ���������������
 */

#include "media_and_monitor.h"

#define ANALOG_MODE 0
#define DIGITAL_MODE 1

static char global_up_down= 0;
static unsigned char whether_send_manual = 0;
int whether_send_serial_fdu_data = 0;
unsigned char whether_send_fdu_can_data = 0;
unsigned char whether_send_fdu_can_heart_beta = 0;
cyg_handle_t counter_handle_head_led_show;/*the handle of counter*/
cyg_handle_t alarm_handle_head_led_show;/*the handle of alarm*/
cyg_alarm alarm_object_head_led_show;/*the alarm*/

cyg_handle_t counter_handle_test;/*the handle of counter*/
cyg_handle_t alarm_handle_test;/*the handle of alarm*/
cyg_alarm alarm_object_test;/*the alarm*/

cyg_handle_t  manual_counter_handle;
cyg_handle_t alarm_handle_manual;
cyg_alarm alarm_object_manual;

int CreateManualPackageTimer(unsigned int msecond)
{
	cyg_clock_to_counter(cyg_real_time_clock(),&manual_counter_handle);
	cyg_alarm_create(manual_counter_handle,JudgeManualPackage,0,&alarm_handle_manual,&alarm_object_manual);
	cyg_alarm_initialize(alarm_handle_manual,cyg_current_time()+(100),msecond);
	cyg_alarm_disable(alarm_handle_manual);
	return 0;
}

void restartManualPackageTimer()
{
	cyg_alarm_initialize(alarm_handle_manual,cyg_current_time()+(100),200);
	cyg_alarm_enable(alarm_handle_manual);
}

void CloseManualPackageTimer()
{
	whether_send_manual = 0;
	cyg_alarm_disable(alarm_handle_manual);
}
void JudgeManualPackage()
{
	if(whether_send_manual == 1)
	{
		SetTSManualCMDPackage(1,1);
	}
	else if(whether_send_manual == 2)
	{
		SetTSManualCMDPackage(0,1);
	}
}

//���������������������������������������������������������������������������������
char re_send_cmd_package_media[512];
unsigned short re_send_cmd_package_media_length = 0;

//���������������������������������������������������������������������������������
char re_send_cmd_package_monitor[512];
unsigned short re_send_cmd_package_monitor_length = 0;

//������4������������������������
unsigned char whether_show_line4_cycle_terminal_station = 0;

unsigned char media_failure_flag = 0;//���������������������
unsigned char monitor_failure_flag = 0;//���������������������

void *FDUCanSendThreadEntry(void *arg)
{
//	return 0;
	CreateFduShowTimer();
	for(;;)
	{
		sem_wait(&sem_wakeup_eamp_led);
//		printf("wakeup the can send thread!!!,whether_send_fdu_can_data = %d\n",whether_send_fdu_can_data);
		if(whether_send_fdu_can_data == 1)
		{
			//������FDU������������������
			SendFDUInfo(&whether_send_serial_fdu_data);
			whether_send_fdu_can_data = 0;
		}
		if(whether_send_fdu_can_heart_beta == 1)
		{
			//������FDU���������������
			whether_send_fdu_can_heart_beta = 0;
//			SendFDUHeartBetaCan();
		}
		//printf("--------------------------\n");

	}
	return NULL;
}
char media_and_monitor_test_buffer[100];
void *MediaAndMonitorSerialCMDParseThreadEntry(void *arg)///<just for test
{///<������������������������������������������
	return NULL;
	int loop_count = 0;
	network_send_package_t tms_to_eamp_package = {"\0",0};

	unsigned char desire_tms_data_len = 1;
	char test_msg[100];
	char test_send_to_tcms_msg[100];
	int kk = 0;
	unsigned char check_sum = 0;
	test_send_to_tcms_msg[0] = 0xFE;
	notice_ccu_send_to_media_t notice_ccu_send_to_media;
	unsigned char times = 0;
	CreateTestTimer();
	unsigned int a = 0;
	unsigned int temp_no = 0;
	for(;;)
	{
		sem_wait(&sem_wakeup_test);
		memcpy((void *)&test_msg[3],(void *)&media_and_monitor_test_buffer[3],67);
		times ++;
		memcpy((void *)&test_send_to_tcms_msg[3],(void *)&ccu_state.pa_running_info_to_tms[2],30);
		memcpy((void *)&test_send_to_tcms_msg[33],(void *)&ccu_state.pa_failure_info[0],32);
		temp_no ++;
		test_msg[1] = a >> 8;
		test_msg[2] = (a & 0xff);
		a = a + 1;
		check_sum = 0;
		for(kk = 1;kk<= 64;kk++)
		{
			check_sum = check_sum + test_msg[kk];
		}
		test_msg[65] = check_sum;
		memcpy((void *)&ccu_state.door_emerg_unlock_state_and_smoke_info[0],(void *)&test_msg[17],16);
		if(whether_send_to_emerg > 0)
		{
			if(BlockBufferRead(ccu_state.ccu_send_to_media_cmd_buffer_id, (void *)&notice_ccu_send_to_media, sizeof(notice_ccu_send_to_media)) > 0)
			{
				times = 3;
				whether_send_to_emerg = whether_send_to_emerg - 1;
				TransmitBoradcastMessageToMedia(notice_ccu_send_to_media.emerg_state,notice_ccu_send_to_media.emerg_sequence);///<������������ (0:close 1:open); ���������������0:over 1:begin) ���������������
			}
		}
		else
		{
			TCMSToPISInfoHandle(test_msg);
			if(times == 2)
			{
				TransmitDevVolumeToMedia();
			}
			else if(times >= 4)
			{
				TransmitBoradcastMessageToMonitor();
				times = 0;
			}
		}
		if(temp_no >= 4 * 10)
		{
			temp_no = 0;
			int i = 0;
			for(i = 3;i < 100;i++)
			{
				if(test_msg[i] != 0)
				{
					diag_printf("%d:%d\n",i,test_msg[i]);
				}
			}
		}
	 }
	 return NULL;

}

void *MediaAndMonitorSerialRecvThreadEntry(void *arg)
{
	unsigned char msg_recv_from_232[100];
	int desire_tms_data_len = 0;
	for(;;)
	{
		desire_tms_data_len = 1;
		cyg_io_read(ser0_handle, &msg_recv_from_232[0], &desire_tms_data_len);
		if(msg_recv_from_232[0] == 'R')
		{
			CYGACC_CALL_IF_RESET();
		}

	}

		 if(msg_recv_from_232[0] == 'M')
		{
			ccu_state.pis_fault_tcms_simulate.pis_fault_tcms_simulate_state = msg_recv_from_232[40];
			diag_printf("pis_fault_tcms_simulate_state = %d\n",ccu_state.pis_fault_tcms_simulate.pis_fault_tcms_simulate_state);
		}
		else if(msg_recv_from_232[0] == 'D')
		{
			memcpy(&ccu_state.pis_fault_tcms_simulate.data[0],&msg_recv_from_232[1],40);
			int i;
			for(i = 0;i < 41;i++)
			{
				if(ccu_state.pis_fault_tcms_simulate.data[i] != 0)
				{
					diag_printf("RECV:%d=%02x\n",i,ccu_state.pis_fault_tcms_simulate.data[i]);
				}
			}
		}
	return NULL;
}

void alarm_func_front_led_show(cyg_handle_t alarm_handle, cyg_addrword_t data)
{
	whether_send_serial_fdu_data = 1;
	whether_send_fdu_can_data = 1;
	sem_post(&sem_wakeup_eamp_led);
}
void alarm_func_test(cyg_handle_t alarm_handle, cyg_addrword_t data)
{
	sem_post(&sem_wakeup_test);
}
void CreateFduShowTimer()
{///<������FDU���������������
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_head_led_show);
	cyg_alarm_create(counter_handle_head_led_show,alarm_func_front_led_show,0,&alarm_handle_head_led_show,&alarm_object_head_led_show);
	cyg_alarm_initialize(alarm_handle_head_led_show,cyg_current_time()+1000,1000);
	cyg_alarm_enable(alarm_handle_head_led_show);
}
void RestartFduShowTimer()
{///<������FDU���������������
	cyg_alarm_initialize(alarm_handle_head_led_show,cyg_current_time()+1200,1200);
	cyg_alarm_enable(alarm_handle_head_led_show);
}
void CreateTestTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_test);
	cyg_alarm_create(counter_handle_test,alarm_func_test,0,&alarm_handle_test,&alarm_object_test);
	cyg_alarm_initialize(alarm_handle_test,cyg_current_time()+26,26);
	cyg_alarm_enable(alarm_handle_test);
}

void TransmitBoradcastMessageToMedia(char emerg_state,char emerg_no)
{///<���������������������������������
	diag_printf("&&&&&&-zhw run into TransmitBoradcastMessageToMedia() function !!!\n");
	char test_msg[512];
	unsigned int msglen;
	char *pchTest;
	char checksum;
	char checksumB1;
	char checksumB2;
	int i;
	unsigned short usLen;
	unsigned short tem_len = 0;
	unsigned short emerg_len = 0;

	memset(test_msg, '\0', sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0xfe;
	*pchTest++ = 0x31;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	if(ccu_state.key_info_tcms == 0)
	{
		*pchTest++ = 0x32;///<key information
	}
	else
	{
		*pchTest++ = 0x31;///<key information
	}
	tem_len = 7;

	///<���������������������6������������
	*pchTest = 0x0;
//	pchTest = strcat((test_msg+tem_len),(char *)&(ccu_state.ccu_tms_car_running_info.current_time));
	pchTest = test_msg+tem_len;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.year;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.month;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.date;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.hour;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.minute;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.second;
	tem_len = 13;
	pchTest = test_msg;
	pchTest += tem_len;

	*pchTest++ = ccu_state.current_media_volume[0];///<���������������������
	*pchTest++ = ccu_state.current_media_volume[1];///<������������������������������������������

	if(emerg_state == 0)
	{
		*pchTest++ = 0x32;///<emerg state flag
	}
	else
	{
		*pchTest++ = 0x31;///<emerg state flag
	}
	tem_len = tem_len + 3;
	if(emerg_state == 1)///<broadcaset emerg info
	{
		*pchTest = 0x0;
		pchTest = strcat((test_msg+tem_len),GetEmergDisplayMessage(emerg_no,&emerg_len));
		tem_len = tem_len + emerg_len;
	}
	else
	{
		*pchTest = 0x0;
	}

	//calculate length
//	usLen = strlen(test_msg) - 6;///<
	usLen = tem_len - 6;///<

	pchTest = test_msg;

	pchTest += 2;///< begin at the key info

	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

//	usLen = strlen(test_msg);
	usLen = tem_len;

	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0xff;///<end flag
	*pchTest = 0x0;

	tem_len = tem_len + 1;
	checksum = 0xff;
//	usLen = strlen(test_msg);
	usLen = tem_len;

	pchTest = test_msg;
	for (i = 0; i < usLen; i++)
	{
		checksum ^= *pchTest++;
	}
	checksumB1 = ((checksum & 0xf0) >> 4) | 0x30;
	checksumB2 = (checksum & 0x0f) | 0x30;
	*pchTest++ = checksumB1;
	*pchTest++ = checksumB2;
	//end of add
	*pchTest = 0x0;

//	msglen = strlen(test_msg);
	msglen = tem_len + 2;

//	strncpy(re_send_cmd_package_media,test_msg,msglen);
//	*(pchTest - 2) =  0xff;

	cyg_io_write(ser0_handle, &test_msg[0], &msglen);
	re_send_cmd_package_media_length = msglen;
	strncpy(re_send_cmd_package_media,test_msg,msglen);
	return ;
	cyg_thread_delay(500);
	diag_printf("data begin\n");
	for(i = 0;i < msglen;i ++)
	{
		diag_printf("%x ",test_msg[i]);
	}
	diag_printf("\ndata over\n");

	return ;
}

void TransmitDevVolumeToMedia()
{///<���������������������
	char test_msg[512];
	unsigned int msglen;
	char *pchTest;
	char checksum;
	char checksumB1;
	char checksumB2;
	int i;
	unsigned short usLen;
	unsigned short tem_len = 0;
	unsigned short emerg_len = 0;

	memset(test_msg, '\0', sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0xfe;
	*pchTest++ = 0x31;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	if(ccu_state.key_info_tcms == 0)
	{
		*pchTest++ = 0x32;///<key information
	}
	else
	{
		*pchTest++ = 0x31;///<key information
	}
	tem_len = 7;

	///<���������������������6������������
	*pchTest = 0x0;
//	pchTest = strcat((test_msg+tem_len),(char *)&(ccu_state.ccu_tms_car_running_info.current_time));
	pchTest = test_msg+tem_len;
//	diag_printf(":%d:%d:%d:%d:%d:%d\n",ccu_state.ccu_tms_car_running_info.current_time.year,
//			ccu_state.ccu_tms_car_running_info.current_time.month,
//			ccu_state.ccu_tms_car_running_info.current_time.date,
//			ccu_state.ccu_tms_car_running_info.current_time.hour,
//			ccu_state.ccu_tms_car_running_info.current_time.minute,
//			ccu_state.ccu_tms_car_running_info.current_time.second);
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.year;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.month;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.date;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.hour;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.minute;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.second;
	tem_len = 13;
	pchTest = test_msg;
	pchTest += tem_len;

	int tcms_media_volume = ccu_state.tcms_media_volume[0];
	*pchTest++ = tcms_media_volume;// * 25 / 10;///<���������������������
	*pchTest++ = ccu_state.tcms_media_volume[1];///<������������������������������������������

	*pchTest++ = 0x30;///<emerg state flag
	tem_len = tem_len + 3;
	*pchTest = 0x0;

	usLen = tem_len - 6;///<

	pchTest = test_msg;

	pchTest += 2;///< begin at the key info

	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = tem_len;

	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0xff;///<end flag
	*pchTest = 0x0;

	tem_len = tem_len + 1;
	checksum = 0xff;
	usLen = tem_len;

	pchTest = test_msg;
	for (i = 0; i < usLen; i++)
	{
		checksum ^= *pchTest++;
	}
	checksumB1 = ((checksum & 0xf0) >> 4) | 0x30;
	checksumB2 = (checksum & 0x0f) | 0x30;
	*pchTest++ = checksumB1;
	*pchTest++ = checksumB2;

	//end of add
	*pchTest = 0x0;

	msglen = tem_len + 2;

	cyg_io_write(ser0_handle, &test_msg[0], &msglen);
	re_send_cmd_package_media_length = msglen;
	strncpy(re_send_cmd_package_media,test_msg,msglen);
	return ;
}

//add by zhw 2015-12-23
//message_to_monitor_t message_to_monitor;
//void ZhwSendMessageInit() {
//	message_to_monitor.message_head = 0xfe;
//}
//end of add

void TransmitBoradcastMessageToMonitor()
{///PIS到监控的报文格式
	int send_ret = 0;
	int send_buffer;

	unsigned char test_msg[40]; //changed by zhw 2015-12-23
	unsigned int msglen;
	char *pchTest;
	unsigned char checksum;
	unsigned char checksumB1;
	unsigned char checksumB2;
	int i;
	unsigned short usLen;

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0xfe;
	*pchTest++ = 0x32;///<monitor

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	/*������������6������������������*/
	*pchTest = 0;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.year;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.month;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.date;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.hour;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.minute;
	*pchTest++ = ccu_state.ccu_tms_car_running_info.current_time.second;
	pchTest = test_msg;
	pchTest += 12;
	*pchTest++ = (ccu_state.ccu_tms_car_running_info.tms_train_no  >> 8);
	*pchTest++ = (ccu_state.ccu_tms_car_running_info.tms_train_no  & 0xFF);

	/*������ ������4���������������12������������������*/
	*pchTest = 0;
	memcpy((void *)&test_msg[14],(char *)&ccu_state.pcu_state_to_monitor[0],4);///<PCU������������
	pchTest = test_msg;
	pchTest += 18;

	/*���������������16������������������������������*/
	*pchTest = 0x0;
	memcpy((void *)&test_msg[18],(char *)&ccu_state.door_emerg_unlock_state_and_smoke_info[0],13);///<������������������
	memcpy((void *)&test_msg[31],(char *)&ccu_state.pcu_state_to_monitor[4],3);///<PCU������������

	usLen = 28;
	pchTest = test_msg;
	pchTest += 2;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = 34;
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0xff;///<end flag
	*pchTest = 0x0;

	test_msg[14]= test_msg[14]&0xFE;///<������������������

	///<������������������������������������������(������������������������������)������������������������������
	unsigned char temp[20];
	static unsigned char record_msg[40]={'\0'};

	memset(temp,0x0,sizeof(temp));
	static unsigned char finish_package_count=0;
	if (1 == ccu_state.is_this_server)
	{
		if(0 != (test_msg[18]&(1<<7))||(0!=bcmp(&test_msg[14],temp,4))||(0!=bcmp(&test_msg[19],temp,15)))//<������������������������0������������������������������CCTV
		{
			finish_package_count=1;
			if ((0 != bcmp(&record_msg[14],&test_msg[14],20)))
			{
				diag_printf("----------------------------TransmitBoradcastMessageToMonitor!!!\n");
				checksum = 0xff;
				usLen = 35;
				pchTest = test_msg;
				for (i = 0; i < usLen; i++)
				{
					checksum ^= *pchTest++;
				}
				checksumB1 = (checksum >> 4) | 0x30;
				checksumB2 = (checksum & 0x0f) | 0x30;
				*pchTest++ = checksumB1;
				*pchTest++ = checksumB2;
				*pchTest = 0x0;
				msglen = 37;
				send_buffer = BlockBufferOpen("ccu-monitor-command-send");
				send_ret = BlockBufferWrite(send_buffer, (void *)&test_msg[0], msglen);
				if ( send_ret < 0 )
				{
					net_debug_printf(("Net : send monitor package to network error!!! \n"));
				}
				else
				{
				#if 1///< debug
					//for(i = 0;i < msglen;i ++)
					for(i = 14;i < 34;i ++)
					{
						diag_printf("%d:%02X|",(i+1),test_msg[i]);
					}
					diag_printf("\n");
				#endif
				}
				memcpy(&record_msg[14],&test_msg[14],20);
			}
		}
		else if(finish_package_count > 0)
		{///<������������������CCTV���������������
			//diag_printf("----------------------------emergency OVER!!!\n");
			checksum = 0xff;
			usLen = 35;
			pchTest = test_msg;
			for (i = 0; i < usLen; i++)
			{
				checksum ^= *pchTest++;
			}
			checksumB1 = (checksum >> 4) | 0x30;
			checksumB2 = (checksum & 0x0f) | 0x30;
			*pchTest++ = checksumB1;
			*pchTest++ = checksumB2;
			*pchTest = 0x0;
			msglen = 37;
			send_buffer = BlockBufferOpen("ccu-monitor-command-send");
			send_ret = BlockBufferWrite(send_buffer, (void *)&test_msg[0], msglen);
			if ( send_ret < 0 )
			{
				net_debug_printf(("Net : send monitor package to network error!!! \n"));
			}
			finish_package_count--;
			memset(record_msg,0x0,sizeof(record_msg));
		}
	}
	return ;
}

void ReTransmitBoradcastMessageToMedia()
{///<���������������������������
	unsigned int msglen;
	msglen = re_send_cmd_package_media_length;
	cyg_io_write(ser0_handle, &re_send_cmd_package_media[0], &msglen);
}

void ReTransmitBoradcastMessageToMonitor()
{///<���������������������������
	unsigned int msglen;
	msglen = re_send_cmd_package_monitor_length;
	cyg_io_write(ser0_handle, &re_send_cmd_package_monitor[0], &msglen);
}


void ReceiveControlMessageFromMonitor()
{
	int recv_ret = 0;
	int recv_buffer;

	unsigned char test_msg[40];
	unsigned int msglen;
	char *pchTest;
	unsigned char checksum;
	unsigned char checksumB1;
	unsigned char checksumB2;
	int i;
	unsigned short usLen;

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	msglen = 32;
	recv_buffer = BlockBufferOpen("ccu-monitor-command-recv");
	recv_ret = BlockBufferRead(recv_buffer,(void *)&test_msg[0],msglen);
#if 0
	if(recv_ret < 0){
		diag_printf("read BlockBufferRead :recv_ret = %d\n",recv_ret);
		return;
	}
#endif
	if(recv_ret > 0)
	{
		if(test_msg[0] == 0xfe && test_msg[29] == 0xff && test_msg[1] == 0x32)
		{
			checksum = 0xff;
			for(i = 0; i < msglen-2; ++i)
			{
				checksum ^= test_msg[i];
			}
			checksumB1 = (checksum >> 4) | 0x30;
			checksumB2 = (checksum & 0x0f) | 0x30;

			if(!(checksumB1 == test_msg[30] && checksumB2 == test_msg[31]))
			{
				//net_debug_printf2(("Net : receive monitor package checksum error!!! \n"));
				//return;
			}

			if( test_msg[10] == 0x1 )//������������
			{
				common_package_t ccu_send_monitor_control_info_to_bcu;

				ccu_send_monitor_control_info_to_bcu.pkg_type = COMMON_PACKAGE_RUNNING_MODE_INFO;
				strcpy(ccu_send_monitor_control_info_to_bcu.dst_dev_name,"BCU");
				ccu_send_monitor_control_info_to_bcu.dst_dev_number = MUL_SYNC_DST_NO;

				strcpy(ccu_send_monitor_control_info_to_bcu.src_dev_name,ccu_state.ccu_info.devices_name);
				ccu_send_monitor_control_info_to_bcu.src_dev_number = ccu_state.ccu_info.devices_no;

				if(test_msg[11] & 0x01)//������������
				{
					unsigned char temp = test_msg[11] & 0x40;
					ccu_state.current_pa_running_mode = (temp == 0x40) ? ANALOG_MODE:DIGITAL_MODE;
					net_debug_printf2(("Net : receive monitor package to network ok to change current_pa_running_mode!!! \n"));
				}
				if(test_msg[11] & 0x02)//������������
				{
					unsigned char temp = test_msg[11] & 0x80;
					ccu_state.current_d2d_running_mode = (temp == 0x80) ? ANALOG_MODE:DIGITAL_MODE;
					net_debug_printf2(("Net : receive monitor package to network ok to change current_d2d_running_mode!!! \n"));
				}

				ccu_send_monitor_control_info_to_bcu.common_data_u.monitor_control_info.pa_running_mode = ccu_state.current_pa_running_mode;
				ccu_send_monitor_control_info_to_bcu.common_data_u.monitor_control_info.d2d_running_mode = ccu_state.current_d2d_running_mode;

			    BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_control_info_to_bcu, sizeof(ccu_send_monitor_control_info_to_bcu));
			}
			else if(test_msg[10] == 0x3)//������������
			{
				if(ccu_state.current_pa_running_mode == DIGITAL_MODE)
				{
					if(test_msg[12] & 0x1)
					{//������������

						ccu_state.current_pa_volume = test_msg[13];
						common_package_t ccu_send_monitor_volume_info_to_eamp;
						ccu_send_monitor_volume_info_to_eamp.pkg_type = COMMON_PACKAGE_TYPE_DEVICE_INFO;
						strcpy(ccu_send_monitor_volume_info_to_eamp.dst_dev_name,"EAMP");
						ccu_send_monitor_volume_info_to_eamp.dst_dev_number = MUL_SYNC_DST_NO;
						strcpy(ccu_send_monitor_volume_info_to_eamp.src_dev_name,ccu_state.ccu_info.devices_name);
						ccu_send_monitor_volume_info_to_eamp.src_dev_number = ccu_state.ccu_info.devices_no;
						ccu_send_monitor_volume_info_to_eamp.common_data_u.ts_dev_volume_info.device_volume= ccu_state.current_pa_volume;
						BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_volume_info_to_eamp, sizeof(ccu_send_monitor_volume_info_to_eamp));
						net_debug_printf2(("DIGITAL_MODE : receive package to eamp for adjust pa_volume:%d\n",test_msg[13]));
					}
					else if(test_msg[12] & 0x2)
					{//������������������
						ccu_state.current_d2d_volume = test_msg[13];
						common_package_t ccu_send_monitor_control_info_to_bcu;
						ccu_send_monitor_control_info_to_bcu.pkg_type = COMMON_PACKAGE_TYPE_VOLUME_ADJ_INFO;
						strcpy(ccu_send_monitor_control_info_to_bcu.dst_dev_name,"BCU");
						ccu_send_monitor_control_info_to_bcu.dst_dev_number = MUL_SYNC_DST_NO;
						strcpy(ccu_send_monitor_control_info_to_bcu.src_dev_name,ccu_state.ccu_info.devices_name);
						ccu_send_monitor_control_info_to_bcu.src_dev_number = ccu_state.ccu_info.devices_no;
						ccu_send_monitor_control_info_to_bcu.common_data_u.monitor_control_info.d2d_volume = ccu_state.current_d2d_volume;
						BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_control_info_to_bcu, sizeof(ccu_send_monitor_control_info_to_bcu));
						net_debug_printf2(("DIGITAL_MODE : receive package to bcu for adjust d2d_volume\n"));
					}
					else if(test_msg[12] & 0x4)
					{//������������
						ccu_state.current_monitor_state = test_msg[13];
						common_package_t ccu_send_monitor_volume_info_to_eamp;
						ccu_send_monitor_volume_info_to_eamp.pkg_type = COMMON_PACKAGE_TYPE_MONITOR_SPEAKER_CONTROL;
						strcpy(ccu_send_monitor_volume_info_to_eamp.dst_dev_name,"EAMP");
						ccu_send_monitor_volume_info_to_eamp.dst_dev_number = MUL_SYNC_DST_NO;
						strcpy(ccu_send_monitor_volume_info_to_eamp.src_dev_name,ccu_state.ccu_info.devices_name);
						ccu_send_monitor_volume_info_to_eamp.src_dev_number = ccu_state.ccu_info.devices_no;
						ccu_send_monitor_volume_info_to_eamp.common_data_u.monitor_speaker_control.monitor_speaker_state= ccu_state.current_monitor_state;
						net_debug_printf2(("Net : receive package to bcu for adjust volume monitor_speaker_state:%d\n",ccu_state.current_monitor_state));
						BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_volume_info_to_eamp, sizeof(ccu_send_monitor_volume_info_to_eamp));

					}
					else if(test_msg[12] & 0x8)
					{//������������

					}
				}
				if(ccu_state.current_pa_running_mode == ANALOG_MODE)
				{
					if(test_msg[12] & 0x1)//������������������
					{
						if(test_msg[13] >= 0 && test_msg[13] <= 50)
							ccu_state.current_pa_volume = 1;
						else if(test_msg[13] > 50 && test_msg[13] <= 100)
							ccu_state.current_pa_volume = 2;
						common_package_t ccu_send_monitor_volume_info_to_eamp;
						ccu_send_monitor_volume_info_to_eamp.pkg_type = COMMON_PACKAGE_TYPE_ANN_SPEAKER_ANA_CONTROL;
						strcpy(ccu_send_monitor_volume_info_to_eamp.dst_dev_name,"EAMP");
						ccu_send_monitor_volume_info_to_eamp.dst_dev_number = MUL_SYNC_DST_NO;
						strcpy(ccu_send_monitor_volume_info_to_eamp.src_dev_name,ccu_state.ccu_info.devices_name);
						ccu_send_monitor_volume_info_to_eamp.src_dev_number = ccu_state.ccu_info.devices_no;
						ccu_send_monitor_volume_info_to_eamp.common_data_u.monitor_control_info.pa_volume= ccu_state.current_pa_volume;
						if(test_msg[11]&0x80)
							ccu_send_monitor_volume_info_to_eamp.common_data_u.monitor_control_info.pa_running_mode = ANALOG_MODE;
						else
							ccu_send_monitor_volume_info_to_eamp.common_data_u.monitor_control_info.pa_running_mode = DIGITAL_MODE;
						BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_volume_info_to_eamp, sizeof(ccu_send_monitor_volume_info_to_eamp));
						net_debug_printf2(("ANALOG_MODE: receive package to eamp for adjust pa_volume:%d\n",test_msg[13]));
					}
					else if(test_msg[12] & 0x2)//������������������������
					{

						common_package_t ccu_send_monitor_control_info_to_bcu;
						ccu_send_monitor_control_info_to_bcu.pkg_type = COMMON_PACKAGE_TYPE_VOLUME_ADJ_INFO;
						strcpy(ccu_send_monitor_control_info_to_bcu.dst_dev_name,"BCU");
						ccu_send_monitor_control_info_to_bcu.dst_dev_number = MUL_SYNC_DST_NO;
						strcpy(ccu_send_monitor_control_info_to_bcu.src_dev_name,ccu_state.ccu_info.devices_name);
						ccu_send_monitor_control_info_to_bcu.src_dev_number = ccu_state.ccu_info.devices_no;
						ccu_state.current_d2d_volume = test_msg[13];
						ccu_send_monitor_control_info_to_bcu.common_data_u.monitor_control_info.d2d_volume = ccu_state.current_d2d_volume;
						if(test_msg[11]&0x80)
							ccu_send_monitor_control_info_to_bcu.common_data_u.monitor_control_info.d2d_running_mode = ANALOG_MODE;
						else
							ccu_send_monitor_control_info_to_bcu.common_data_u.monitor_control_info.d2d_running_mode = DIGITAL_MODE;
						BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_control_info_to_bcu, sizeof(ccu_send_monitor_control_info_to_bcu));
						net_debug_printf2(("ANALOG_MODE: receive package to bcu for adjust d2d_volume:%d\n",ccu_state.current_d2d_volume));
					}
					else if(test_msg[12] & 0x4)
					{//������������
						ccu_state.current_monitor_state = test_msg[13];
						common_package_t ccu_send_monitor_volume_info_to_eamp;
						ccu_send_monitor_volume_info_to_eamp.pkg_type = COMMON_PACKAGE_TYPE_MONITOR_SPEAKER_CONTROL;
						strcpy(ccu_send_monitor_volume_info_to_eamp.dst_dev_name,"EAMP");
						ccu_send_monitor_volume_info_to_eamp.dst_dev_number = MUL_SYNC_DST_NO;
						strcpy(ccu_send_monitor_volume_info_to_eamp.src_dev_name,ccu_state.ccu_info.devices_name);
						ccu_send_monitor_volume_info_to_eamp.src_dev_number = ccu_state.ccu_info.devices_no;
						ccu_send_monitor_volume_info_to_eamp.common_data_u.monitor_speaker_control.monitor_speaker_state= ccu_state.current_monitor_state;
						BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &ccu_send_monitor_volume_info_to_eamp, sizeof(ccu_send_monitor_volume_info_to_eamp));
						net_debug_printf2(("ANALOG_MODE : receive package to bcu for adjust volume monitor_speaker_state:%d\n",ccu_state.current_monitor_state));
					}
					else if(test_msg[12] & 0x8)
					{//������������

					}
				}
			}
			else if(test_msg[10] == 0x5)
			{
				net_debug_printf2(("Net : receive monitor package to network ok to manual speak station!!! \n"));
				//上下行区间选择以及到站和预到站
				if(test_msg[14]&0x01)//下行
				{
					if(global_up_down == 2 || global_up_down == 0)
					{
						global_up_down = 1;
						//SetUpOrDownStream(2);
					}
				}else//上行
				{
					if(global_up_down == 1 || global_up_down == 0)
					{
						global_up_down = 2;
						//SetUpOrDownStream(1);
					}
				}

				if(test_msg[15] & 0X80)
				{
					ccu_state.ccu_region_info[0].current_station_no = test_msg[15]&0x7F;
					whether_send_manual = 1;
					restartManualPackageTimer();
				}
				else
				{
					ccu_state.ccu_region_info[0].current_station_no = test_msg[15]&0x7F;
					whether_send_manual = 2;
					restartManualPackageTimer();
				}
			}
			else if(test_msg[10] == 0x7)
			{
				net_debug_printf2(("MODE: send package to bcu for adjust pcu refuse or recept\n"));
				common_package_t pcu_control;
				pcu_control.pkg_type = COMMON_PACKAGE_TYPE_PCU_CONTROL;
				strcpy(pcu_control.dst_dev_name,"BCU");
//				pcu_control.dst_dev_number = MUL_SYNC_DST_NO;
				pcu_control.dst_dev_number = ccu_state.key_info;//只往钥匙端发送接通信号
				strcpy(pcu_control.src_dev_name,ccu_state.ccu_info.devices_name);
				pcu_control.src_dev_number = ccu_state.ccu_info.devices_no;
				//判断ＰＣＵ接通设备号
				int i;
				int j = 0;
				for(i = 0; i < 3; i++)
				{
					net_debug_printf2(("test_msg[%d] = %.2x\n", 16+i, test_msg[16+i]));
					if(test_msg[16+i] & 0x01)
					{
						pcu_control.common_data_u.monitor_control_info.pcu_recept_no = j+1;
					}
					if(test_msg[16+i] & 0x02)
					{
						pcu_control.common_data_u.monitor_control_info.pcu_recept_no = j+2;
					}
					if(test_msg[16+i] & 0x10)
					{
						pcu_control.common_data_u.monitor_control_info.pcu_recept_no = j+3;
					}
					if(test_msg[16+i] & 0x20)
					{
						pcu_control.common_data_u.monitor_control_info.pcu_recept_no = j+4;
					}
						pcu_control.common_data_u.monitor_control_info.pcu_refuse_no = 0xff;
					j = j+4;
				}
				if(test_msg[16+i] & 0x01)
				{
					pcu_control.common_data_u.monitor_control_info.pcu_refuse_no = 0;
					pcu_control.common_data_u.monitor_control_info.pcu_recept_no = 0xff;
				}
				net_debug_printf2(("1111:  pcu_recept_no:%d\n",pcu_control.common_data_u.monitor_control_info.pcu_recept_no));
				BlockBufferWrite(ccu_state.ccu_send_common_cmd_buffer_id, &pcu_control, sizeof(pcu_control));
			}
		}
	}
}

void SendFDUInfo(int *p_param_whether_send_serial_fdu_data)
{///<������FDU���������������������

//	if(*p_param_whether_send_serial_fdu_data == 1)
	{
		unsigned char temp_current_line_no = 0;
		temp_current_line_no = ccu_state.current_line_no;
//		diag_printf("SendFDUInfo,now temp_current_line_no = %d\n",temp_current_line_no);
//		*p_param_whether_send_serial_fdu_data = 0;
		if(temp_current_line_no == 3 ||
				(temp_current_line_no == 4 && ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus == 0))
		{
			CCUFduAnnouncementOnlyUp16(ccu_state.current_line_no,
											ccu_state.ccu_region_info[temp_current_line_no-3].current_region_info.over_station_no,
											ccu_state.ccu_region_info[temp_current_line_no-3].current_region_info.whether_the_cycle_bus,
											ccu_state.ccu_region_info[temp_current_line_no-3].current_region_info.running_direction,
											0,
											ccu_state.led_language);///<modify on 2013/07
			ccu_state.led_language = abs(ccu_state.led_language - 1);
		}

		 if(temp_current_line_no == 4 && ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus == 1)
		 {
			 if(ccu_state.led_language == 0)
			   {
				   ccu_state.led_language = 1;
			   }
			   else
			   {
				   ccu_state.led_language = 0;
			   }
			   CCUFduAnnouncementOnlyUp16(temp_current_line_no,
												ccu_state.ccu_region_info[1].current_region_info.over_station_no,
												ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus,
												ccu_state.ccu_region_info[1].current_region_info.running_direction,
												0,
												ccu_state.led_language);///<modify on 2013/07/23
		 }
	}
}


void UpdatePCUGlobalInfoToTCMSAndMonitor(unsigned char src_dev_no,unsigned char pcu_state)
{///<CCU������PCU���������������TCMS���������
	diag_printf("src_dev_no = %d,pcu_state = %d\n",src_dev_no,pcu_state);
	switch(src_dev_no)
	{
		case 1:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 1));
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 0));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 7));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 7));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 1);
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 0));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 7);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 7));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 1));
				ccu_state.pcu_state_to_monitor[4] |= ((1 << 0));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 7));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 7));
			}
			break;

		case 2:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 2));
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 1));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 6));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 6));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 2);
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 1));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 6);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 6));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 2));
				ccu_state.pcu_state_to_monitor[4] |= ((1 << 1));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 6));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 6));
			}
			break;
		case 3:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 3));
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 4));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 5));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 5));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 3);
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 4));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 5);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 5));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 3));
				ccu_state.pcu_state_to_monitor[4] |= ((1 << 4));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 5));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 5));
			}
			break;
		case 4:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 4));
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 5));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 4));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 4));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 4);
				ccu_state.pcu_state_to_monitor[4] &= (~(1 << 5));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 4);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 4));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 4));
				ccu_state.pcu_state_to_monitor[4] |= ((1 << 5));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 4));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 4));
			}
			break;
		case 5:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 5));
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 0));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 3));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 3));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 5);
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 0));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 3);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 3));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 5));
				ccu_state.pcu_state_to_monitor[5] |= ((1 << 0));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 3));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 3));
			}
			break;
		case 6:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 6));
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 1));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 2));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 2));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 6);
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 1));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 2);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 2));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 6));
				ccu_state.pcu_state_to_monitor[5] |= ((1 << 1));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 2));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 2));
			}
			break;
		case 7:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 7));
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 4));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 1));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 1));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[0] |= (1 << 7);
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 4));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 1);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 1));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[0] &= (~(1 << 7));
				ccu_state.pcu_state_to_monitor[5] |= ((1 << 4));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 1));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 1));
			}
			break;
		case 8:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 0));
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 5));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 0));
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 0));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[1] |= (1 << 0);
				ccu_state.pcu_state_to_monitor[5] &= (~(1 << 5));

				ccu_state.pcu_state_to_tcms[0] |= (1 << 0);
				ccu_state.pcu_state_to_tcms[2] &= (~(1 << 0));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 0));
				ccu_state.pcu_state_to_monitor[5] |= ((1 << 5));

				ccu_state.pcu_state_to_tcms[0] &= (~(1 << 0));
				ccu_state.pcu_state_to_tcms[2] |= ((1 << 0));
			}
			break;
		case 9:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 1));
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 0));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 7));
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 7));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[1] |= (1 << 1);
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 0));

				ccu_state.pcu_state_to_tcms[1] |= (1 << 7);
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 7));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 1));
				ccu_state.pcu_state_to_monitor[6] |= ((1 << 0));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 7));
				ccu_state.pcu_state_to_tcms[3] |= ((1 << 7));
			}
			break;
		case 10:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 2));
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 1));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 6));
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 6));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[1] |= (1 << 2);
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 1));

				ccu_state.pcu_state_to_tcms[1] |= (1 << 6);
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 6));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 2));
				ccu_state.pcu_state_to_monitor[6] |= ((1 << 1));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 6));
				ccu_state.pcu_state_to_tcms[3] |= ((1 << 6));
			}
			break;
		case 11:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 3));
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 4));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 5));
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 5));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[1] |= (1 << 3);
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 4));

				ccu_state.pcu_state_to_tcms[1] |= (1 << 5);
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 5));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 3));
				ccu_state.pcu_state_to_monitor[6] |= ((1 << 4));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 5));
				ccu_state.pcu_state_to_tcms[3] |= ((1 << 5));
			}
			break;
		case 12:
			if(pcu_state == 0)///<���������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 4));
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 5));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 4));
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 4));
			}
			else if(pcu_state == 1)///<������
			{
				ccu_state.pcu_state_to_monitor[1] |= (1 << 4);
				ccu_state.pcu_state_to_monitor[6] &= (~(1 << 5));

				ccu_state.pcu_state_to_tcms[1] |= (1 << 4);
				ccu_state.pcu_state_to_tcms[3] &= (~(1 << 4));
			}
			else///<pcu_state == 2 ������
			{
				ccu_state.pcu_state_to_monitor[1] &= (~(1 << 4));
				ccu_state.pcu_state_to_monitor[6] |= ((1 << 5));

				ccu_state.pcu_state_to_tcms[1] &= (~(1 << 4));
				ccu_state.pcu_state_to_tcms[3] |= ((1 << 4));
			}
			break;
		default:break;
	}
}

void UpdatePCURequestLinkage(network_send_package_t recv_network_info_from_network)
{
	int i;
	if(recv_network_info_from_network.send_information.event_type_ann == ANN_IDLE &&
		recv_network_info_from_network.send_information.event_type_intercom == EVENT_NO_PCU_REQUEST)
	{
		for(i = 0;i < 12;i++)
		{
			//������������PCU���������������������
			pcu_state_on_ccu[i+1].pcu_request_state = 0;
			pcu_state_on_ccu[i+1].pcu_connenting_state = 0;
			UpdatePCUGlobalInfoToTCMSAndMonitor(i+1,0);
		}
	}
	if(recv_network_info_from_network.send_information.event_type_ann == ANN_IDLE &&
		recv_network_info_from_network.send_information.event_type_intercom == D2P_INTERCOMM_EVENT)
	{//���������������������������������PCU������
		if(recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1 &&
				recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over == 1 &&
				recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_response == 0  &&
				recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting == 0)
		{///<PCU������
//					diag_printf("111--%d\n",recv_network_info_from_network.send_information.src_devices_no);
			pcu_state_on_ccu[recv_network_info_from_network.send_information.src_devices_no].pcu_request_state = 1;
			pcu_state_on_ccu[recv_network_info_from_network.send_information.src_devices_no].pcu_connenting_state = 0;
			UpdatePCUGlobalInfoToTCMSAndMonitor(recv_network_info_from_network.send_information.src_devices_no,1);
		}
		else if(recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1 &&
				recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_whether_is_connecting == 1)
		{///<PCU������
//					diag_printf("222--%d\n",recv_network_info_from_network.send_information.src_devices_no);
			pcu_state_on_ccu[recv_network_info_from_network.send_information.src_devices_no].pcu_request_state = 0;
			pcu_state_on_ccu[recv_network_info_from_network.send_information.src_devices_no].pcu_connenting_state = 1;
			UpdatePCUGlobalInfoToTCMSAndMonitor(recv_network_info_from_network.send_information.src_devices_no,2);
		}
		else if(recv_network_info_from_network.send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0/* &&
				strcmp(recv_network_info_from_network.send_information.src_devices_name, "BCU") != 0*/)
		{///<PCU ������
//					diag_printf("333--%d\n",recv_network_info_from_network.send_information.src_devices_no);
			pcu_state_on_ccu[recv_network_info_from_network.send_information.src_devices_no].pcu_request_state = 0;
			pcu_state_on_ccu[recv_network_info_from_network.send_information.src_devices_no].pcu_connenting_state = 0;
			UpdatePCUGlobalInfoToTCMSAndMonitor(recv_network_info_from_network.send_information.src_devices_no,0);
		}
		TransmitBoradcastMessageToMonitor();
	}
}
