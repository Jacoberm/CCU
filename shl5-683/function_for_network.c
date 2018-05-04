/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： function_for_network.c
 * 文件标识：
 * 摘    要： 与网络线程共用函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-5-21
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "function_for_network.h"
unsigned char second = 0;
unsigned char fdu_failure_flag = 0;
unsigned char failure_time_falg = 0;
char fdu_heart_beta_package[10];
unsigned char check_dev_state = 0;
current_time_t GetCurrentTCMSTime()
{///<获取TCMS时间
	second ++;
	if(second == 60)
	{
		second = 0;
	}
//	ccu_state.ccu_tms_car_running_info.current_time.year = 14;
//	ccu_state.ccu_tms_car_running_info.current_time.month = 5;
//	ccu_state.ccu_tms_car_running_info.current_time.date = 21;
//	ccu_state.ccu_tms_car_running_info.current_time.hour = 12;
//	ccu_state.ccu_tms_car_running_info.current_time.minute = 12;
//	ccu_state.ccu_tms_car_running_info.current_time.second = 20;
	return ccu_state.ccu_tms_car_running_info.current_time;
}

void CCUGetAllPerDevFailureInfo1()
{
	int i = 0 ;
	for(i = 1;i <= 2;i++)
	{
		if(g_dev_fault_info.bcu_info[i-1].online == 0)
		{
			diag_printf("BCU %d is offline\n",i);
		}
		else
		{
			diag_printf("BCU %d is online\n",i);
		}
	}
	for(i = 1;i <= 2;i++)
		{
			if(g_dev_fault_info.ccu_info[i-1].online == 0)
			{
				diag_printf("CCU %d is offline\n",i);
			}
			else
			{
				diag_printf("CCU %d is online\n",i);
			}
			if(g_dev_fault_info.ccu_info[i-1].ccu_per_dev_failure_info.fdu_failure_info == 1)
			{
				diag_printf("CCU FDU %d is offline\n",i);
			}
			else
			{
				diag_printf("CCU FDU %d is online\n",i);
			}
		}
	for(i = 1;i <= 6;i++)
		{
			if(g_dev_fault_info.eamp_info[i-1].online == 0)
			{
				diag_printf("EAMP %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.dm_failure_info == 1)
			{
				diag_printf("EAMP DM %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP DM %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.outer_sider_led1_failure_info == 1)
			{
				diag_printf("EAMP idu1 %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP idu1 %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.outer_sider_led2_failure_info == 1)
			{
				diag_printf("EAMP idu2 %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP idu2 %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.outer_sider_led1_failure_info == 1)
			{
				diag_printf("EAMP Sdu1 %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP sdu1 %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.outer_sider_led2_failure_info == 1)
			{
				diag_printf("EAMP sdu2 %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP sdu2 %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.nd1_failure_info == 1)
			{
				diag_printf("EAMP ndu1 %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP ndu1 %d is online\n",i);
			}
			if(g_dev_fault_info.eamp_info[i-1].eamp_per_dev_failure_info.nd2_failure_info == 1)
			{
				diag_printf("EAMP ndu2 %d is offline\n",i);
			}
			else
			{
				diag_printf("EAMP ndu2 %d is online\n",i);
			}
		}
	for(i = 1;i <= 12;i++)
		{
			if(g_dev_fault_info.pcu_info[i-1].online == 0)
			{
				diag_printf("PCU %d is offline\n",i);
			}
			else
			{
				diag_printf("PCU %d is online\n",i);
			}
		}
}

void CCUGetAllPerDevFailureInfo()
{///<获取PA系统所有设备故障信息

//	return ;
	ccu_state.nd_failure_info[0] = 0 |
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.nd1_failure_info) << 7);
	ccu_state.nd_failure_info[1] = 0 |
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.nd2_failure_info) << 0)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.nd1_failure_info) << 1)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.nd2_failure_info) << 2)|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.nd1_failure_info) << 3)|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.nd2_failure_info) << 4)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.nd1_failure_info) << 5)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.nd2_failure_info) << 6)|
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.nd1_failure_info) << 7);
	ccu_state.nd_failure_info[2] = 0 |
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.nd2_failure_info) << 0)|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.nd1_failure_info) << 1)|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.nd2_failure_info) << 2);


	ccu_state.dm_failure_info[0] = 0 |
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.dm_failure_info) << 3)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.dm_failure_info) << 4)|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.dm_failure_info) << 5)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.dm_failure_info) << 6)|
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.dm_failure_info) << 7);
	ccu_state.dm_failure_info[1] = 0|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.dm_failure_info) << 0);

	ccu_state.led_failure_info[0] = 0 |
			((g_dev_fault_info.ccu_info[0].ccu_per_dev_failure_info.fdu_failure_info) << 5)|
			((g_dev_fault_info.ccu_info[1].ccu_per_dev_failure_info.fdu_failure_info) << 6)|
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.outer_sider_led1_failure_info) << 7);

	ccu_state.led_failure_info[1] = 0|
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.outer_sider_led2_failure_info) << 0)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.outer_sider_led1_failure_info) << 1)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.outer_sider_led2_failure_info) << 2)|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.outer_sider_led1_failure_info) << 3)|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.outer_sider_led2_failure_info) << 4)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.outer_sider_led1_failure_info) << 5)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.outer_sider_led2_failure_info) << 6)|
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.outer_sider_led1_failure_info) << 7);
	ccu_state.led_failure_info[2] = 0|
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.outer_sider_led2_failure_info) << 0)|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.outer_sider_led1_failure_info) << 1)|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.outer_sider_led2_failure_info) << 2)|
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.in_sider_led1_failure_info) << 3)|
			((g_dev_fault_info.eamp_info[0].eamp_per_dev_failure_info.in_sider_led2_failure_info) << 4)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.in_sider_led1_failure_info) << 5)|
			((g_dev_fault_info.eamp_info[1].eamp_per_dev_failure_info.in_sider_led2_failure_info) << 6)|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.in_sider_led1_failure_info) << 7);
	ccu_state.led_failure_info[3] = 0|
			((g_dev_fault_info.eamp_info[2].eamp_per_dev_failure_info.in_sider_led2_failure_info) << 0)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.in_sider_led1_failure_info) << 1)|
			((g_dev_fault_info.eamp_info[3].eamp_per_dev_failure_info.in_sider_led2_failure_info) << 2)|
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.in_sider_led1_failure_info) << 3)|
			((g_dev_fault_info.eamp_info[4].eamp_per_dev_failure_info.in_sider_led2_failure_info) << 4)|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.in_sider_led1_failure_info) << 5)|
			((g_dev_fault_info.eamp_info[5].eamp_per_dev_failure_info.in_sider_led2_failure_info) << 6);

	ccu_state.pcu_failure_info[0] = 0 |
			((1 - g_dev_fault_info.pcu_info[0].online) << 1)|
			((1 - g_dev_fault_info.pcu_info[1].online) << 2)|
			((1 - g_dev_fault_info.pcu_info[2].online) << 3)|
			((1 - g_dev_fault_info.pcu_info[3].online) << 4)|
			((1 - g_dev_fault_info.pcu_info[4].online) << 5)|
			((1 - g_dev_fault_info.pcu_info[5].online) << 6)|
			((1 - g_dev_fault_info.pcu_info[6].online) << 7);
	ccu_state.pcu_failure_info[1] = 0 |
			((1 - g_dev_fault_info.pcu_info[7].online) << 0)|
			((1 - g_dev_fault_info.pcu_info[8].online) << 1)|
			((1 - g_dev_fault_info.pcu_info[9].online) << 2)|
			((1 - g_dev_fault_info.pcu_info[10].online) << 3)|
			((1 - g_dev_fault_info.pcu_info[11].online) << 4);

	ccu_state.eamp_failure_info[0] = 0 |
			((1 - g_dev_fault_info.eamp_info[0].online) << 3)|
			((1 - g_dev_fault_info.eamp_info[1].online) << 4)|
			((1 - g_dev_fault_info.eamp_info[2].online) << 5)|
			((1 - g_dev_fault_info.eamp_info[3].online) << 6)|
			((1 - g_dev_fault_info.eamp_info[4].online) << 7);
	ccu_state.eamp_failure_info[1] = 0 |
			((1 - g_dev_fault_info.eamp_info[5].online) << 0)|
			((1 - g_dev_fault_info.eamp_info[0].online) << 1)|
			((1 - g_dev_fault_info.eamp_info[1].online) << 2)|
			((1 - g_dev_fault_info.eamp_info[2].online) << 3)|
			((1 - g_dev_fault_info.eamp_info[3].online) << 4)|
			((1 - g_dev_fault_info.eamp_info[4].online) << 5)|
			((1 - g_dev_fault_info.eamp_info[5].online) << 6);

//	CCUGetAllPerDevFailureInfo1();

	ccu_state.ccu_failure_info = 0 | ((1 - g_dev_fault_info.ccu_info[0].online) << 3) |((1 - g_dev_fault_info.ccu_info[1].online) << 4);
	ccu_state.bcu_failure_info = 0 | ((1 - g_dev_fault_info.bcu_info[0].online) << 3) |((1- g_dev_fault_info.bcu_info[1].online) << 4);

	UpdateDMFailureToTMS();

	UpdateNDFailureToTMS();

	UpdateLEDFailureToTMS();

//	UpdatePCUFailureToTMS();

	UpdateEAMPFailureToTMS();

	UpdateCCUFailureToTMS();

	UpdateBCUFailureToTMS();
}


void InitializeHeartBeatPackage()
{///<初始化心跳包
	int i = 0;
	unsigned char check_sum = 0xff;
	fdu_heart_beta_package[0] = 0x02;
	fdu_heart_beta_package[1] = 0x21;
	fdu_heart_beta_package[2] = 0x37;
	fdu_heart_beta_package[3] = 0x31;
	fdu_heart_beta_package[4] = 0x03;

	for (i = 0; i < 5; i++)
	{
		check_sum ^= fdu_heart_beta_package[i];
	}
	fdu_heart_beta_package[5] = ((check_sum & 0xf0) >> 4) | 0x30;
	fdu_heart_beta_package[6] = (check_sum & 0x0f) | 0x30;
}


void SetFailureTimeFlag()///<3 * heart time
{
	failure_time_falg ++;
}

void SendHeartBetaToPeripheral()
{
	if(failure_time_falg >= 2)
	{
		failure_time_falg = 0;
		SendHeartBetaToFDU();
	}
}

void SendHeartBetaToFDU()
{///<发送心跳包给FDU
	fdu_failure_flag ++;
	whether_send_fdu_can_heart_beta = 1;
	sem_post(&sem_wakeup_eamp_led);
	if(fdu_failure_flag >= 2)
	{
		ccu_state.ccu_per_dev_failure_info.fdu_failure_info = 1;
	}
	else
	{
		ccu_state.ccu_per_dev_failure_info.fdu_failure_info = 0;
	}
	(dev_status_info.ccu_dev_info_head+ccu_state.ccu_info.devices_no-1)->ccu_info.ccu_per_dev_failure_info.fdu_failure_info = ccu_state.ccu_per_dev_failure_info.fdu_failure_info;
}
