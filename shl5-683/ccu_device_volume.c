/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_device_volume.c
 * 文件标识：
 * 摘    要： PIS音量信息文件操作
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-08-01
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#include "ccu_device_volume.h"

typedef struct
{
	unsigned char d2d_volume;//司机对讲音量
	unsigned char d2p_volume;//紧急对讲音量
	unsigned char monitor_volume;//监听音量
	unsigned char pa_volume;//广播音量
}device_volume_info_t;//设备音量信息

FILE *fp_pis_pa_volume = NULL;

void ReadLocalDeviceVolumeInfo()
{
	if(FileExist(pis_pa_volume_file_name) == 0)
	{///<文件不存在
		//diag_printf("File Not Exist\n");
		WriteDefaultDeviceVolumeToFile();
	}
	UpdatePISPADevVolume();
}

//打开本地音量信息文件
void OpenPISPACDevVolumeFile()
{
	fp_pis_pa_volume = fopen(pis_pa_volume_file_name,"w+");
}

//将PIS音量信息写入到文件中去
void WritePISPADevVolumeInfoToFile()
{
	int ret = 0;
	fp_pis_pa_volume = fopen(pis_pa_volume_file_name,"w+");
	device_volume_info_t device_volume_info;
	device_volume_info.d2d_volume = ccu_state.current_d2d_volume;
	device_volume_info.d2p_volume = ccu_state.current_d2p_volume;
	device_volume_info.monitor_volume = ccu_state.current_monitor_volume;
	device_volume_info.pa_volume = ccu_state.current_pa_volume;
	if(fp_pis_pa_volume != NULL)
	{
		ret = fwrite(&device_volume_info,sizeof(device_volume_info),1,fp_pis_pa_volume);
//		if(ret > 0)//<2014-09-11
		{
			fclose(fp_pis_pa_volume);//写完立马刷新，防止文件关闭之前掉电
		}
	}
}
void UpdatePISPADevVolume()
{
	//从文件中读取音量大小信息
	FILE *fp;
	fp = fopen(pis_pa_volume_file_name,"r");
	device_volume_info_t device_volume_info;
	if(fp != NULL)
	{
		fread(&device_volume_info,sizeof(device_volume_info),1,fp);
		fclose(fp);
		ccu_state.current_d2d_volume = device_volume_info.d2d_volume;
		ccu_state.current_d2p_volume = device_volume_info.d2p_volume;
		ccu_state.current_pa_volume = device_volume_info.pa_volume;
		ccu_state.current_monitor_volume = device_volume_info.monitor_volume;
		ccu_state.pa_running_info_to_tms[21] = ccu_state.current_pa_volume;
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
		diag_printf("ccu_state.current_pa_volume = %d\n",ccu_state.current_pa_volume);
	}
	else
	{
//		while(1)
//		{
			diag_printf("%s open is error\n",pis_pa_volume_file_name);
//		}
	}
}
void UpdatePISPADevVolume2()
{
	//从文件中读取音量大小信息
	FILE *fp;
	fp = fopen(pis_pa_volume_file_name,"r");
	device_volume_info_t device_volume_info;
	if(fp != NULL)
	{
		fread(&device_volume_info,sizeof(device_volume_info),1,fp);
		diag_printf("device_volume_info.pa_volume = %d\n",device_volume_info.pa_volume);
		return ;
		ccu_state.current_d2d_volume = device_volume_info.d2d_volume;
		ccu_state.current_d2p_volume = device_volume_info.d2p_volume;
		ccu_state.current_pa_volume = device_volume_info.pa_volume;
		ccu_state.current_monitor_volume = device_volume_info.monitor_volume;
		ccu_state.pa_running_info_to_tms[21] = ccu_state.current_pa_volume;
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
		diag_printf("ccu_state.current_pa_volume = %d\n",ccu_state.current_pa_volume);
	}
	else
	{
//		while(1)
//		{
//			diag_printf("%s open is error\n",pis_pa_volume_file_name);
//		}
	}
}

void WriteDefaultDeviceVolumeToFile()
{
	//设备首次运行时，设置默认的线路号信息
	FILE *fp;
	device_volume_info_t device_volume_info;
	device_volume_info.d2d_volume = 3;
	device_volume_info.d2p_volume = 3;
	device_volume_info.monitor_volume = 2;
	device_volume_info.pa_volume = 92;
	fp = fopen(pis_pa_volume_file_name,"w+");
	if(fp != NULL)
	{
		int ret = fwrite(&device_volume_info,sizeof(device_volume_info),1,fp);
		//diag_printf("ret = %d\n",ret);
		fclose(fp);
	}
}


