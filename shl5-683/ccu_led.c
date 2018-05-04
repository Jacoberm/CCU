/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� eamp_led.c
 * ���������������
 * ���    ������ CCU LED���������
 *
 * ��������������� V1.0.0
 * ���    ������ wilson
 * ���������������2013-07-23
 *
 * ���������������
 * ���������  ���
 * ���������������
 */

#include "ccu_led.h"
#include "ccu_function.h"
#define Debug(s) diag_printf s
cyg_io_handle_t ser0_handle;
cyg_io_handle_t ser1_handle;
cyg_io_handle_t ser_led;

//������can������
cyg_io_handle_t    hCAN_Tbl[2];
cyg_io_handle_t    hLoopCAN_Tbl[2];

cyg_handle_t counter_handle_can;/*the handle of counter*/
cyg_handle_t alarm_handle_can;/*the handle of alarm*/
cyg_alarm alarm_object_can;/*the alarm*/

tip_info_t cycle_line_msg[2] = {
									{"���������","ShangHai Line 5"},
									{"���������������","ShangHai Line 5"}
								};

void CCUSerialInit()
{
	SerialInit(&ser0_handle,"/dev/ser0",38400);//������������ (551 has no this port)
	SerialInit(&ser_led,"/dev/ser1",38400);///<mvb is 38400
    if (ENOERR != cyg_io_lookup("/dev/can0", &hCAN_Tbl[0]))
    {
        CYG_TEST_FAIL_FINISH("Error opening CAN channel 1");
        diag_printf("Error opening CAN channel 1");
    }
}

void SerialFlushInit(cyg_io_handle_t *param_io) ///< flush serail before 1-read
{
	Cyg_ErrNo err;
	cyg_serial_info_t ser_info;
	unsigned int len;
	unsigned char buffer[1024];

	if (NULL != param_io)
	{
		debug_print2(("Flush serial-2016\n"));
		len = sizeof(buffer);

		cyg_io_get_config(*param_io, CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH, &buffer[0], &len);
		cyg_io_get_config(*param_io, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH, &buffer[0], &len);		
	}
	else
	{
		debug_print2(("null serial-2016\n"));
	}

	return ;
}

void SerialInit(cyg_io_handle_t *param_io,char param_dev_string[],int param_baud)
{//���������������
	Cyg_ErrNo err;
	cyg_serial_info_t ser_info;
	unsigned int len;
	unsigned char buffer[16];

	err = cyg_io_lookup(param_dev_string, param_io);
	if (ENOERR == err)
	{
		debug_print2(("%s found\n",param_dev_string));
		len = sizeof(buffer);

		cyg_io_get_config(*param_io, CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH, &buffer[0], &len);
		cyg_io_get_config(*param_io, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH, &buffer[0], &len);

		// cyg_io_set_config
		if(param_baud == 115200)
		{
			ser_info.baud = CYGNUM_SERIAL_BAUD_115200;
		}
		else if(param_baud == 38400)
		{
			ser_info.baud = CYGNUM_SERIAL_BAUD_38400;
		}
		else if(param_baud == 9600)
		{
			ser_info.baud = CYGNUM_SERIAL_BAUD_9600;
		}
		else
		{
				ser_info.baud = CYGNUM_SERIAL_BAUD_115200;
		}
		ser_info.word_length = CYGNUM_SERIAL_WORD_LENGTH_8;
		ser_info.stop = CYGNUM_SERIAL_STOP_1;
		ser_info.parity = CYGNUM_SERIAL_PARITY_NONE;
		ser_info.flags = CYGNUM_SERIAL_FLOW_NONE;

		len = sizeof(ser_info);
		err = cyg_io_set_config(*param_io, CYG_IO_SET_CONFIG_SERIAL_INFO,
							&ser_info, &len);
        if (err != ENOERR)
            diag_printf("<2016-0322> set %s config: err=%d\n",
            		param_dev_string, err);
      cyg_uint32 cfg_data;
      cfg_data = 0;
      len = sizeof(cfg_data);
	  err = cyg_io_set_config(*param_io, CYG_IO_SET_CONFIG_SERIAL_WRITE_BLOCKING,
			&cfg_data, &len); ///< non-block
      if (err != ENOERR)
        diag_printf("<2016-0322> set %s wr non-block: err=%d\n",
        		param_dev_string, err);
      cfg_data = 1;
	  err = cyg_io_set_config(*param_io, CYG_IO_SET_CONFIG_SERIAL_READ_BLOCKING,
			&cfg_data, &len); ///< red block
      if (err != ENOERR)
            diag_printf("<2016-0322> set %s  rd non-block: err=%d\n",
            		param_dev_string, err);
	}
	else
	{
		debug_print2(("%s not found\n",param_dev_string));
		diag_printf("%s not found\n",param_dev_string);
	}
}

void TransmitShowCommand_wilson(char address)
{//������RS232������������������FDU������������can������������������
	char test_msg[10];
		unsigned int msglen;
		unsigned short usLen;
		char *pchTest;
		unsigned short i;
		char checksum;
		char checksumB1;
		char checksumB2;

		memset(test_msg, 0x0, sizeof(test_msg));
		pchTest = test_msg;

		*pchTest++ = 0x2;
		*pchTest++ = address;
		*pchTest++ = 0x31;
		*pchTest++ = 0x30;
		*pchTest++ = 0x38;
		*pchTest++ = 0x40;
		*pchTest++ = 0x3;
		//add for checksum
		//*pchTest++ = 0x30;
		//*pchTest++ = 0x30;
		*pchTest = 0x0;
		checksum = 0xff;
		usLen = strlen(test_msg);
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

		msglen = strlen(test_msg);
		cyg_io_write(ser_led, &test_msg[0], &msglen);
	//	diag_printf("show cmd begin\n");
	//	for(i = 0;i < msglen;i ++)
	//	{
	//		diag_printf("%x \n",test_msg[i]);
	//	}
	//	diag_printf("show cmd over\n");

}

void TransmitShowCommand_wilson_can(char address,cyg_uint8 can_num ,  cyg_uint8 device_add)
{
//	diag_printf("run into TransmitShowCommand_wilson_can() funciton!!!\n");
	char test_msg[10];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;

	memset(test_msg, 0x0, sizeof(test_msg));
	pchTest = test_msg;

	*pchTest++ = 0x2;
	*pchTest++ = address;
	*pchTest++ = 0x31;
	*pchTest++ = 0x30;
	*pchTest++ = 0x38;
	*pchTest++ = 0x40;
	*pchTest++ = 0x3;

	//add for checksum
	//*pchTest++ = 0x30;
	//*pchTest++ = 0x30;
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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

	msglen = strlen(test_msg);
	cyg_io_write(ser_led, &test_msg[0], &msglen);
	sendCanData(&test_msg[0], msglen, device_add , can_num);
//	diag_printf("show cmd begin\n");
//	for(i = 0;i < msglen;i ++)
//	{
//		diag_printf("%x \n",test_msg[i]);
//	}
//	diag_printf("show cmd over\n");

}


void CCUFduAnnouncement(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, int param_color)
{
	TransmitFduData_wilson(0x21, param_line_no, param_terminal_no, param_whether_the_cycle_line,up_or_down,param_color);
	cyg_thread_delay(50); //add,2012-11-23
	TransmitShowCommand_wilson(0x21);
	cyg_thread_delay(50);
}

void CCUFduAnnouncementOnlyUp16(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, int param_color,unsigned char param_language)
{
//	diag_printf("run into CCUFduAnnouncementOnlyUp16() function!!!!\n");
//	TransmitFduDataOnlyUp16(0x21, param_line_no, param_terminal_no, param_whether_the_cycle_line,up_or_down,param_color,param_language);
	TransmitFduDataOnlyUp16_can(0x21, param_line_no, param_terminal_no, param_whether_the_cycle_line,up_or_down,param_color,param_language,1,0x98);
//	TransmitFduDataOnlyUp16_can(0x21, param_line_no, param_terminal_no, param_whether_the_cycle_line,up_or_down,param_color,param_language,2,0x98);
//	cyg_thread_delay(50); //add,2012-11-23
//	TransmitShowCommand_wilson(0x21);
	TransmitShowCommand_wilson_can(0x21,1,0x98);
//	TransmitShowCommand_wilson_can(0x21,2,0x98);
//	cyg_thread_delay(50);
//	diag_printf("###\n");
//	diag_printf("222-%d,%d\n",param_line_no,param_terminal_no);
}
void FduClear()
{
//	cyg_thread_delay(50);
//	TransmitClearCommand_wilson
	TransmitClearCommand_wilson_can(0x21,1,0x98);
//	cyg_thread_delay(50);
}
void CCUFduAnnouncement_line4_terminal_station(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, char param_language)
{
	FduClear();
	cyg_thread_delay(5); //add,2012-11-23
	TransmitFduData_line4_terminal_station(0x21, param_line_no, param_terminal_no, param_whether_the_cycle_line,up_or_down,param_language);
	cyg_thread_delay(5);
	TransmitShowCommand_wilson(0x21);
	cyg_thread_delay(50);
}

void CCUFduAnnouncementLine4TerminalStationUp16(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, char param_language)
{
	FduClear();
	cyg_thread_delay(5); //add,2012-11-23
//	TransmitFduDataLine4TerminalStationUp16(0x21, param_line_no, param_terminal_no, param_whether_the_cycle_line,up_or_down,param_language);
//	cyg_thread_delay(5);
//	TransmitShowCommand_wilson(0x21);
//	cyg_thread_delay(50);
	TransmitShowCommand_wilson_can(0x21,1,0x98);
//	TransmitShowCommand_wilson_can(0x21,2,0x98);
}


void TransmitFduData_line4_terminal_station(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, char chinese)
{
	char test_msg[512];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;
	int up_or_down = (int)param_up_or_down;
	if(up_or_down >= 1)
	{
		up_or_down --;
	}
	line_no = line_no - 3;///<0���3������   1:4������

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0x2;
	*pchTest++ = address;
	*pchTest++ = 0x34;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;//filed
	*pchTest++ = 0x31;


	*pchTest++ = 0x05;
	*pchTest++ = 0x58;
	if(up_or_down == 0)
	{
		*pchTest++ = 0x30;///<up
	}
	else
	{
		*pchTest++ = 0x31;///<down
	}


	*pchTest++ = 0x05;// color
	*pchTest++ = 0x5a;
	if(line_no == 0)
	{
		*pchTest++ = 0x33;///<���������  ������
	}
	else
	{
		*pchTest++ = 0x32;///<���������  ������
	}

	*pchTest = 0x0;

	pchTest = strcat(test_msg, ccu_state.pp_station_info[1][terminal_no].station_chinese_name);

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x31;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;
	*pchTest++ = 0x44;
	*pchTest++ = 0x32;
	*pchTest = 0x0;
	pchTest = strcat(test_msg, "Line 4");

	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;


	//add for checksum
	//*pchTest++ = 0x30;
	//*pchTest++ = 0x30;
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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

	msglen = strlen(test_msg);
	cyg_io_write(ser_led, &test_msg[0], &msglen);
//	cyg_io_write(ser0_handle, &test_msg[0], &msglen);
}

void TransmitFduDataLine4TerminalStationUp16(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, char chinese)
{
	char test_msg[512];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;
	int up_or_down = (int)param_up_or_down;
	if(up_or_down >= 1)
	{
		up_or_down --;
	}
	line_no = line_no - 3;///<0���3������   1:4������

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0x2;
	*pchTest++ = address;
	*pchTest++ = 0x34;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;// color
	*pchTest++ = 0x5a;
	if(line_no == 0)
	{
		*pchTest++ = 0x33;///<���������  ������
	}
	else
	{
		*pchTest++ = 0x32;///<���������  ������
	}

	*pchTest = 0x0;

	pchTest = strcat(test_msg, ccu_state.pp_station_info[1][terminal_no].station_chinese_name);

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;

	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;

	//add for checksum
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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

	msglen = strlen(test_msg);
	cyg_io_write(ser_led, &test_msg[0], &msglen);
}


//LED 24���
void TransmitFduData_wilson(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, int param_color)
{
	char test_msg[512];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;
	int up_or_down = (int)param_up_or_down;
	if(up_or_down >= 1)
	{
		up_or_down --;
	}
	if(line_no >= 3)
	{
		line_no = line_no - 3;///<0���3������   1:4������
	}

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0x2;
//	*pchTest++ = address;
	*pchTest++ = 0x21;
	*pchTest++ = 0x34;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;		//color
	*pchTest++ = 0x5a;
	if(line_no == 0)
	{
		*pchTest++ = 0x33;///<���������  ������
	}
	else
	{
		*pchTest++ = 0x32;///<���������  ������
	}
	*pchTest = 0x0;


	if(line_no == 1 && whether_cycle_line == 1)
	{
		pchTest = strcat(test_msg, cycle_line_msg[up_or_down].chinese_info);
	}
	else
	{
			pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_chinese_name);
//		pchTest = strcat(test_msg, "������������");
	}

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x31;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;
	*pchTest++ = 0x44;
	*pchTest++ = 0x32;
	*pchTest = 0x0;

	if(line_no == 1 && whether_cycle_line == 1)
	{
		pchTest = strcat(test_msg, cycle_line_msg[up_or_down].english_info);
//		pchTest = strcat(test_msg, "Line 4");
	}
	else
	{
			pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_english_name);
//		pchTest = strcat(test_msg, "North JiangYang Road");
	}

	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;


	//add for checksum
	//*pchTest++ = 0x30;
	//*pchTest++ = 0x30;
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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

	msglen = strlen(test_msg);
	cyg_io_write(ser_led, &test_msg[0], &msglen);
//	cyg_io_write(ser0_handle, &test_msg[0], &msglen);
	return ;
	diag_printf("data begin\n");
	for(i = 0;i < msglen;i ++)
	{
		diag_printf("%2X \n",test_msg[i]);
	}
	diag_printf("data over\n");
}
#if 0
void TransmitFduDataOnlyUp16_can(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, int param_color,unsigned char param_language,cyg_uint8 can_num ,  cyg_uint8 device_add)
{
 diag_printf("run into TransmitFduDataOnlyUp16_can() function !!!\n");
	char test_msg[512];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;
	int up_or_down = (int)param_up_or_down;
	if(up_or_down >= 1)
	{
		up_or_down --;
	}
	if(line_no >= 3)
	{
		line_no = line_no - 3;///<0���3������   1:4������
	}

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0x2;
	//*pchTest++ = address;
	*pchTest++ = 0x21;
	*pchTest++ = 0x34;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;		//scroll 1 time
	*pchTest++ = 0x58;
	*pchTest++ = 0x31;	//LED ������������

	//*pchTest++ = 0x05;		//color
	//pchTest++ = 0x5a;
	//pchTest++ = 0x33;///<���������  ������
	*pchTest++=0x81;
	*pchTest=0x0;
	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;

	//add for checksum
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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
	
	msglen = strlen(test_msg);
	diag_printf("*******data1111 begin********\n");
	for(i = 0;i < 19;i ++)
	{
		diag_printf("%2X\n",test_msg[i]);
	}
	diag_printf("*******data 111111over*********\n");
	cyg_io_write(ser_led, &test_msg[0], &msglen);
	return ;
	
	diag_printf("*******data1111 begin********\n");
	for(i = 0;i < 23;i ++)
	{
		diag_printf("%2X\n",test_msg[i]);
	}
	diag_printf("*******data 111111over*********\n");
	
   	//Debug(("Start send can data\n"));//wy-add-201609018-9:01
	
	//sendCanData(&test_msg[0], 19, device_add , can_num);
	//Debug(("End send can data\n"));//wy-add-201609018-9:01
#if 0
#if 0

	if(line_no == 0)
	{
		*pchTest++ = 0x33;///<���������  ������
		*pchTest++=0x81;
		*pchTest++=0x0;
	
		if(param_language == 0)///<chinese name
		{
			pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_chinese_name);
//			pchTest = strcat(test_msg,"���������������������");
		}
		else
		{
			pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_english_name);
//			pchTest = strcat(test_msg,"���������������������");
		}
	}
	else
	{
		*pchTest++ = 0x32;///<���������  ������
		*pchTest = 0x0;
		if(param_language == 0)///<chinese name
		{
			if(whether_cycle_line == 0)
			{
				pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_chinese_name);
			}
			else
			{
				pchTest = strcat(test_msg, cycle_line_msg[up_or_down].chinese_info);
			}

		}
		else
		{
			if(whether_cycle_line == 0)
			{
				pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_english_name);
			}
			else
			{
				pchTest = strcat(test_msg, cycle_line_msg[up_or_down].english_info);
			}
		}
	}
#endif
	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;

	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;

	//add for checksum
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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
	msglen = strlen(test_msg);
//	cyg_io_write(ser_led, &test_msg[0], &msglen);
	diag_printf("*******data begin********\n");
	for(i = 0;i < msglen;i ++)
	{
		diag_printf("%2X\n",test_msg[i]);
	}
	diag_printf("*******data over*********\n");
	test_msg[0]=0x02;
	test_msg[1]=0x21;
test_msg[2]=0x34;
test_msg[3]=0x30;
test_msg[4]=0x30;
test_msg[5]=0x33;
test_msg[6]=0x32;
test_msg[7]=0x1b;
test_msg[8]=0x30;
test_msg[9]=0x32;
test_msg[10]=0x31;
test_msg[11]=0x05;
test_msg[12]=0x5a;
test_msg[13]=0x32;
test_msg[14]=0x05;
test_msg[15]=0x5b;
if(up_or_down==0)
{
	test_msg[16]=0x33;
	test_msg[17]=0x33;
}
else
{
test_msg[16]=0x31;
	test_msg[17]=0x34;
}
test_msg[18]=0xe5;
test_msg[19]=0x8f;
test_msg[20]=0xb7;
test_msg[21]=0xe7;
test_msg[22]=0xba;
test_msg[23]=0xbf;
test_msg[24]=0x1b;
test_msg[25]=0x30;
test_msg[26]=0x31;
test_msg[27]=0x31;
test_msg[28]=0x05;
test_msg[29]=0x58;
test_msg[30]=0x31;
test_msg[31]=0x05;
test_msg[32]=0x44;
test_msg[33]=0x32;
test_msg[34]=0x4c;
test_msg[35]=0x69;
test_msg[36]=0x6e;
test_msg[37]=0x65;
if(up_or_down==0)
test_msg[38]=0x33;
else
test_msg[38]=0x34;
test_msg[39]=0x03;
test_msg[40]=0x3e;
if(up_or_down==0)
//test_msg[41]=0x37;
test_msg[41]=0x81;

else
//test_msg[41]=0x35;
test_msg[41]=0x8d;

msglen=42;
Debug(("Start send can data\n"));//wy-add-201609018-9:01
	sendCanData(&test_msg[0], msglen, device_add , can_num);
Debug(("End send can data\n"));//wy-add-201609018-9:01
#endif
	return ;

}
#endif
void TransmitFduDataOnlyUp16_can(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, int param_color,unsigned char param_language,cyg_uint8 can_num ,  cyg_uint8 device_add)
{
	char test_msg[512];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;
	int up_or_down = (int)param_up_or_down;
	if(up_or_down >= 1)
	{
		up_or_down --;
	}
	if(line_no >= 3)
	{
		line_no = line_no - 3;///<0???3??��?o?   1:4??��?o?
	}

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0x2;
//	*pchTest++ = address;
	*pchTest++ = 0x21;
	*pchTest++ = 0x34;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;		//scroll 1 time
	*pchTest++ = 0x58;
	*pchTest++ = 0x39;	//LED ????��o?????��

	*pchTest++ = 0x05;		//color
	*pchTest++ = 0x5a;
	*pchTest++ = 0x34; ///< 0x33: yellow for qd13; 0x34: white


//	if(ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction ==2)
//	{
//		//*pchTest = 0x8d;
//		pchTest = strcat(test_msg, "莘庄");
//	}
//	else if(ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.running_direction==1)
//	{
//		//*pchTest = 0x81;
//		pchTest = strcat(test_msg, "南桥新城");
//	}
	if(param_language == 0)
		pchTest = strcat(test_msg, ccu_state.pp_station_info[0][terminal_no].station_chinese_name);
	else
		pchTest = strcat(test_msg, ccu_state.pp_station_info[0][terminal_no].station_english_name);
	
	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;

	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;

	//add for checksum
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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
	msglen = strlen(test_msg);
	sendCanData(&test_msg[0], msglen, device_add , can_num);

	return ;

}

///led 16
void TransmitFduDataOnlyUp16(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, int param_color,unsigned char param_language)
{
	char test_msg[512];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;
	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;
	int up_or_down = (int)param_up_or_down;
	if(up_or_down >= 1)
	{
		up_or_down --;
	}
	if(line_no >= 3)
	{
		line_no = line_no - 3;///<0���3������   1:4������
	}

	memset(test_msg, 0x0, sizeof(test_msg));

	pchTest = test_msg;

	*pchTest++ = 0x2;
//	*pchTest++ = address;
	*pchTest++ = 0x21;
	*pchTest++ = 0x34;

	*pchTest++ = 0x30;				//len
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;

	*pchTest++ = 0x1b;
	*pchTest++ = 0x30;
	*pchTest++ = 0x30;//filed
	*pchTest++ = 0x31;

	*pchTest++ = 0x05;		//color
	*pchTest++ = 0x5a;

	if(line_no == 0)
	{
		*pchTest++ = 0x33;///<���������  ������
		*pchTest = 0x0;
		if(param_language == 0)///<chinese name
		{
			pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_chinese_name);
		}
		else
		{
			pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_english_name);
		}
	}
	else
	{
		*pchTest++ = 0x32;///<���������  ������
		*pchTest = 0x0;
		if(param_language == 0)///<chinese name
		{
			if(whether_cycle_line == 0)
			{
				pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_chinese_name);
			}
			else
			{
				pchTest = strcat(test_msg, cycle_line_msg[up_or_down].chinese_info);
			}

		}
		else
		{
			if(whether_cycle_line == 0)
			{
				pchTest = strcat(test_msg, ccu_state.pp_station_info[line_no][terminal_no].station_english_name);
			}
			else
			{
				pchTest = strcat(test_msg, cycle_line_msg[up_or_down].english_info);
			}
		}
	}

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;

	//calculate length
	usLen = strlen(test_msg) - 7;
	pchTest = test_msg;
	pchTest += 3;
	*pchTest++ = (usLen /1000) | 0x30;
	usLen = usLen % 1000;
	*pchTest++ = (usLen /100) | 0x30;
	usLen = usLen % 100;
	*pchTest++ = (usLen /10) | 0x30;
	usLen = usLen % 10;
	*pchTest++ = usLen | 0x30;

	usLen = strlen(test_msg);
	pchTest = test_msg;
	pchTest += usLen;
	*pchTest++ = 0x3;

	//add for checksum
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
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
	msglen = strlen(test_msg);
	cyg_io_write(ser_led, &test_msg[0], &msglen);
	return ;
}



void TransmitClearCommand_wilson(char address)
{
	char test_msg[10];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;

	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;

	memset(test_msg, 0x0, sizeof(test_msg));
	pchTest = test_msg;

	*pchTest++ = 0x2;
	*pchTest++ = address;
	*pchTest++ = 0x32;
	*pchTest++ = 0x31;
	*pchTest++ = 0x3;
	//add for checksum
	//*pchTest++ = 0x30;
	//*pchTest++ = 0x30;
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
	pchTest = test_msg;
	for (i = 0; i < usLen; i++)
	{
		checksum ^= *pchTest++;
	}
	checksumB1 = ((checksum & 0xf0) >> 4) | 0x30;
	checksumB2 = (checksum & 0x0f) | 0x30;
	*pchTest++ = checksumB1;
	*pchTest++ = checksumB2;
	*pchTest = 0x0;

	msglen = strlen(test_msg);
	cyg_io_write(ser_led, &test_msg[0], &msglen);
}

void SendFDUHeartBetaCan()
{
	char test_msg[10];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;

	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;

	memset(test_msg, 0x0, sizeof(test_msg));
	pchTest = test_msg;

	*pchTest++ = 0x2;
	*pchTest++ = 0x21;
	*pchTest++ = 0x37;
	*pchTest++ = 0x31;
	*pchTest++ = 0x3;
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
	pchTest = test_msg;
	for (i = 0; i < usLen; i++)
	{
		checksum ^= *pchTest++;
	}
	checksumB1 = ((checksum & 0xf0) >> 4) | 0x30;
	checksumB2 = (checksum & 0x0f) | 0x30;
	*pchTest++ = checksumB1;
	*pchTest++ = checksumB2;
	*pchTest = 0x0;

	msglen = strlen(test_msg);
	sendCanData(&test_msg[0], msglen, 0x98 , 1);
}

void TransmitClearCommand_wilson_can(char address,cyg_uint8 can_num ,  cyg_uint8 device_add)
{
	char test_msg[10];
	unsigned int msglen;
	unsigned short usLen;
	char *pchTest;

	unsigned short i;
	char checksum;
	char checksumB1;
	char checksumB2;

	memset(test_msg, 0x0, sizeof(test_msg));
	pchTest = test_msg;

	*pchTest++ = 0x2;
	*pchTest++ = address;
	*pchTest++ = 0x32;
	*pchTest++ = 0x31;
	*pchTest++ = 0x3;
	//add for checksum
	//*pchTest++ = 0x30;
	//*pchTest++ = 0x30;
	*pchTest = 0x0;
	checksum = 0xff;
	usLen = strlen(test_msg);
	pchTest = test_msg;
	for (i = 0; i < usLen; i++)
	{
		checksum ^= *pchTest++;
	}
	checksumB1 = ((checksum & 0xf0) >> 4) | 0x30;
	checksumB2 = (checksum & 0x0f) | 0x30;
	*pchTest++ = checksumB1;
	*pchTest++ = checksumB2;
	*pchTest = 0x0;

	msglen = strlen(test_msg);
//	cyg_io_write(ser_led, &test_msg[0], &msglen);
	sendCanData(&test_msg[0], msglen, device_add , can_num);
}


void IduClear()
{
	cyg_thread_delay(50);
	TransmitClearCommand_wilson_can(0x22,1,0x98);
	cyg_thread_delay(50);
}

void sendCanData(cyg_uint8 *program_data, cyg_uint32 count, cyg_uint16	std_id , cyg_uint8 can_num)
{

	//add bu zhw 2016-1-1
#if 1

	cyg_uint8	i;
	cyg_uint32	len;
    cyg_can_message tx_msg;
	cyg_uint8 	*temp_data = program_data;

//	std_id = 0xb8;//IDU���������
//	std_id = 0xa8;//IDU���������
//	diag_printf("std_id = %d\n",std_id);
//	diag_printf("can_num = %d\n",can_num);
//	diag_printf("count = %d\n",count);

#if 0
	while(*temp_data != 0)
	{
		diag_printf("%X ", *temp_data);
		temp_data++;
	}
#endif
	//printf("\ncount = %d\n", count);
	while(count != 0)
	{
//		diag_printf("before send count = %d\n",count);
		if (count > 8)
		{
//			      		     ���������  ���������  ���������������   ������������������	���������
			CYG_CAN_MSG_SET_PARAM(tx_msg, std_id, CYGNUM_CAN_ID_STD, 8, CYGNUM_CAN_FRAME_DATA);
			for(i = 0; i < 8; i++)
			{
				tx_msg.data.bytes[i] = (*program_data);
				program_data++;
			}
			count -= 8;
		}
		else
		{
			CYG_CAN_MSG_SET_PARAM(tx_msg, std_id, CYGNUM_CAN_ID_STD, count, CYGNUM_CAN_FRAME_DATA);
			for(i = 0; i < count; i++)
			{
				tx_msg.data.bytes[i] = (*program_data);
				program_data++;
			}
			count = 0;
		}
		len = sizeof(tx_msg);
		hal_delay_us(500);
		cyg_io_write(hCAN_Tbl[can_num-1], &tx_msg, &len);
		hal_delay_us(500);
	}

	//add by zhw 2016-1-1
#endif
}
void alarm_func_can(cyg_handle_t alarm_handle, cyg_addrword_t data)
{
	sem_post(&sem_can_recv);
}

void CreatCANTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_can);
	cyg_alarm_create(counter_handle_can,alarm_func_can,0,&alarm_handle_can,&alarm_object_can);
	cyg_alarm_initialize(alarm_handle_can,cyg_current_time()+2000,2000);
	cyg_alarm_enable(alarm_handle_can);
}
