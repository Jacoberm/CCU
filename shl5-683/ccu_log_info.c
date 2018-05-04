/*
 * Copyright (c) 2012,MIT
 * All right reserved.
 *
 * ????: ccu_log_info.c
 * ????:
 * ?    ?: CCUlog????
 *
 * ????: V1.0.0
 * ?    ?: wilson
 * ????:2014-04-04
 *
 * ????:
 * ???  :
 * ????:
 */
#include "ccu_log_info.h"
char recording_file_path[30];

void *CCULogInfoEntry(void *arg)
{
	common_big_package_t bcu_log_info;
	OpenNewFile(&fp_bcu1_log_info,"/nand/log/bcu1_log.log","wb");
	OpenNewFile(&fp_bcu2_log_info,"/nand/log/bcu2_log.log","wb");
//	OpenNewFile(&fp_intercomm_audio_info,"/nand/log/aa_log.log","wb");
	unsigned char tmp_buff[8*1024];
	char tcms_buffer[100];
	int ret = 0;
	for(;;)
	{
		if(BlockBufferRead(ccu_state.ccu_get_bcu_log_info_buffer_id, &bcu_log_info, sizeof(bcu_log_info)) > 0 )
		{
			if(bcu_log_info.src_dev_number == 1)
			{
				debug_print(("111\n"));
//				WriteLogInfoToFile(fp_bcu1_log_info,bcu_log_info.common_big_data_u.bcu_log_info.bcu_log_info,strlen(bcu_log_info.common_big_data_u.bcu_log_info.bcu_log_info));
			}
			else if(bcu_log_info.src_dev_number == 2)
			{
				debug_print(("222\n"));
//				WriteLogInfoToFile(fp_bcu2_log_info,bcu_log_info.common_big_data_u.bcu_log_info.bcu_log_info,strlen(bcu_log_info.common_big_data_u.bcu_log_info.bcu_log_info));
			}
		}
		if((ret = BlockBufferRead(ccu_state.ccu_tcms_log_info, &tcms_buffer, 67)) > 0 )
		{
//			WriteTCMSLogToFile(fp_tcms_log, tcms_buffer,ret);
		}
		if(ccu_state.enable_audio_data_to_file == 1)
		{
			ret = CharBufferRead(ccu_state.ccu_recv_audio_data_buffer_id, tmp_buff, sizeof(tmp_buff));
			debug_print(("CCULogInfoEntry ret = %d\n",ret));
			if (ret > 0)
			{
//				WriteAudioDataToFile(fp_intercomm_audio_info, tmp_buff,ret);
			}
		}
		sched_yield();
	}
	return NULL;
}

void SetRecordingFileName(unsigned char file_type,unsigned char src_no,unsigned char dst_no)
{
	unsigned char dst_name[30];
	memset((void *)recording_file_path,0x00,sizeof(recording_file_path));

	if(file_type == 1)///d2d
	{
		strncpy(dst_name,"BCU",3);
	}
	else if(file_type == 2)///d2p
	{
		strncpy(dst_name,"PCU",3);
	}
	sprintf(recording_file_path,"/nand/record/BCU%d_%s%d_%d_%d_%d_%d_%d_%d_%d",src_no,dst_name,dst_no,
													ccu_state.ccu_tms_car_running_info.current_time.year,
													ccu_state.ccu_tms_car_running_info.current_time.month,
													ccu_state.ccu_tms_car_running_info.current_time.date,
													ccu_state.ccu_tms_car_running_info.current_time.hour,
													ccu_state.ccu_tms_car_running_info.current_time.minute,
													ccu_state.ccu_tms_car_running_info.current_time.second);
//	OpenNewFile(&fp_intercomm_audio_info,recording_file_path,"wb");
}
