/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_ann_fun.h
 * File Identification: 
 * Abstract:  
 *
 * Current Version:	V2013_0415_0.0.1
 * Author:	 H.product Department
 * Completion Date:	2013-04-16
 *
 * Last Version:
 * Last Author:
 * Finishing Date:
 */

 #ifndef CCU_ANN_FUN_H
 #define CCU_ANN_FUN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "data_struct.h"
#include "sound_driver.h"
#include "ccu_state_machine.h"
#include "ccu_function.h"
#include "thread_config.h"


typedef struct
{
	char leave_station_file_name[AUDIO_FILE_NAME_LENGTH];
	char stop_station_file_name[AUDIO_FILE_NAME_LENGTH];
	int local_stop_audio_data_buffer_id;
	int local_leave_audio_data_buffer_id;
	unsigned char buffer_effective;
}local_audio_buffer_t;

typedef struct
{ 
     network_send_package_t  *precv_net_cmd;	 
     unsigned int send_net_cmd_buffer_id;
     int audio_play_control_buffer_id;
     int src_dev_no;
     char src_dev_name[DEVICE_NAME_LENGTH];
     local_audio_buffer_t local_audio_buffer_one;///<add chenyinhua 2013-06-13
     local_audio_buffer_t local_audio_buffer_two;///<add chenyinhua 2013-06-13
     int effective_local_audio_data_buffer_id;///<add chenyinhua 2013-06-13
     int net_audio_send_buffer_id;
     int audio_pending_buffer_id;
     int local_audio_play_buffer_id;
     unsigned char ccu_eamp_net_bad;     
     unsigned char play_cmd_set;
     unsigned char is_db_server;
     unsigned char occ_cmd_flag;
}ann_state_param_t;

typedef struct
{
      unsigned char line_number;
      unsigned char station_no;
      unsigned char effective; ///< <0>: ignore this station 
}ccu_skip_station_info_t;

/** ClearEmergInfo function description
 *  ????????????
 *
 *  @param   null
 *  @return  null
 */
void ClearEmergInfo();

void TCMSNoticeMediaEmergInfo(send_infomation_t  *p_send_infomation_param);
 #endif /* CCU_ANN_FUN_H, end */
 
