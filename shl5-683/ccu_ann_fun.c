/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_ann_fun.c
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
#include "ccu_ann_fun.h"

static unsigned int current_event = ANN_IDLE;

static FILE *fp_emerg = NULL;
static FILE *fp_stop_station = NULL;
static FILE *fp_next_station = NULL;
static FILE *fp_station_play = NULL;
static unsigned int total_bytes_read_emerg = 0;
static unsigned int total_bytes_write_to_buffer_emerg = 0;
static local_audio_buffer_t *p_buffer_free = NULL;
static local_audio_buffer_t *p_buffer_busy = NULL;
static unsigned char manual_pre_read_flag = 0; ///< debug only

int CcuStartCmdJudge(send_infomation_t  *precv_send_info)
{
       int ret = 0;

      if(NULL==precv_send_info)
      {
           return -1;
      }

     switch(precv_send_info->event_type_ann)
     {
             case LIVE_ANN_EVENT:
                 if( precv_send_info->event_info_ann.live_announce.live_active==1)
                 {
                       ret = 1;
                 }
             break;
			 
             case OCC_EVENT:
                 if( precv_send_info->event_info_ann.occ_announce.occ_active==1)
                 {
                     ret = 1;
                 }
             break;
			 
             case EMERG_ANN_EVENT:
               if( precv_send_info->event_info_ann.emerg_announce.emerg_active==1)
               {
                     ret = 1;
               }
             break;
			 
             case TMS_AUTO_ANN_EVENT:
                if( precv_send_info->event_info_ann.tms_auto_announce.tms_auto_active==1)
                {
                      ret = 1;	       
                }
             break;
			 
             case TMS_MANUAL_ANN_EVENT:
                if( precv_send_info->event_info_ann.tms_manual_announce.tms_manual_active==1)
                {
                      ret = 1;
                }
             break;
			 
             case MANUAL_ANN_EVENT:
               if( precv_send_info->event_info_ann.manual_annnounce.manual_active==1)
               {
                     ret = 1;
               }
             break;
    		 
             default:
	        debug_print(("SendResponseCmd: unknown event\n"));
	     break;
     }
	 
      return ret;       
}
int CcuEndCmdJudge(send_infomation_t  *precv_send_info)
{
       int ret = 0;

      if(NULL==precv_send_info)
      {
           return -1;
      }

     switch(precv_send_info->event_type_ann)
     {
             case LIVE_ANN_EVENT:
                 if( precv_send_info->event_info_ann.live_announce.live_active==0)
                 {
                       ret = 1;
                 }
             break;
			 
             case OCC_EVENT:
                 if( precv_send_info->event_info_ann.occ_announce.occ_active==0)
                 {
                     ret = 1;
                 }
             break;
			 
             case EMERG_ANN_EVENT:
               if( precv_send_info->event_info_ann.emerg_announce.emerg_active==0)
               {
                     ret = 1;
               }
             break;
			 
             case TMS_AUTO_ANN_EVENT:
                if( precv_send_info->event_info_ann.tms_auto_announce.tms_auto_active==0)
                {
                      ret = 1;	       
                }
             break;
			 
             case TMS_MANUAL_ANN_EVENT:
                if( precv_send_info->event_info_ann.tms_manual_announce.tms_manual_active==0)
                {
                      ret = 1;
                }
             break;
			 
             case MANUAL_ANN_EVENT:
               if( precv_send_info->event_info_ann.manual_annnounce.manual_active==0)
               {
                     ret = 1;
               }
             break;
    		 
             default:
	        debug_print(("SendResponseCmd: unknown event\n"));
	     break;
     }
	 
      return ret;       
}
int  SendResponseCmd(ann_state_param_t *pann_state_fun_param, char *dst_device_name, int dst_device_no)
{
         send_infomation_t *psend_cmd_info = NULL;
         network_send_package_t network_send_information;		 

         if(NULL == pann_state_fun_param)
         {
	       debug_print(("SendResponseCmd:  null ptr\n"));         
               return -1;
	 }
         psend_cmd_info = (send_infomation_t *)&(pann_state_fun_param->precv_net_cmd->send_information);
         if(0==psend_cmd_info->event_type_ann)
         {
              return -1;
         }
         switch(psend_cmd_info->event_type_ann)
         {
                 case LIVE_ANN_EVENT:
		 if( psend_cmd_info->event_info_ann.live_announce.live_active==1 &&
                      psend_cmd_info->event_info_ann.live_announce.live_response==0)
		 {
                       psend_cmd_info->event_info_ann.live_announce.live_response = 1;		       
		 }
		 else
		 {
                        return -1;
		 }
                 break;
				 
                 case OCC_EVENT:
		 if( psend_cmd_info->event_info_ann.occ_announce.occ_active==1 &&
                      psend_cmd_info->event_info_ann.occ_announce.occ_response==0)
		 {
                      psend_cmd_info->event_info_ann.occ_announce.occ_response = 1;		       
		 }
		 else
		 {
                        return -1;
		 }
                 break;
				 
                 case EMERG_ANN_EVENT:
		 if( psend_cmd_info->event_info_ann.emerg_announce.emerg_active==1 &&
                      psend_cmd_info->event_info_ann.emerg_announce.emerg_response==0)
		 {
                       psend_cmd_info->event_info_ann.emerg_announce.emerg_response = 1;		       
		 }
		 else
		 {
                        return -1;
		 }
                 break;
				 
                 case TMS_AUTO_ANN_EVENT:
		 if( psend_cmd_info->event_info_ann.tms_auto_announce.tms_auto_active==1 &&
                      psend_cmd_info->event_info_ann.tms_auto_announce.tms_auto_response==0)
		 {
                      psend_cmd_info->event_info_ann.tms_auto_announce.tms_auto_response = 1;	       
		 }
		 else
		 {
                        return -1;
		 }
                 break;
				 
                 case TMS_MANUAL_ANN_EVENT:
		 if( psend_cmd_info->event_info_ann.tms_manual_announce.tms_manual_active==1 &&
                      psend_cmd_info->event_info_ann.tms_manual_announce.tms_manual_response==0)
		 {
                       psend_cmd_info->event_info_ann.tms_manual_announce.tms_manual_response = 1;	       
		 }
		 else
		 {
                        return -1;
		 }
                 break;
				 
                 case MANUAL_ANN_EVENT:
		 if( psend_cmd_info->event_info_ann.manual_annnounce.manual_active==1 &&
                      psend_cmd_info->event_info_ann.manual_annnounce.manual_response==0)
		 {
                      psend_cmd_info->event_info_ann.manual_annnounce.manual_response = 1;	       
		 }
		 else
		 {
                        return -1;
		 }
                 break;
        		 
                 default:
		 debug_print(("SendResponseCmd: unknown event\n"));
		 return -1;      
         }
         
         memcpy((void *)&network_send_information.send_information, (void *)psend_cmd_info, sizeof(*psend_cmd_info));
         strcpy(network_send_information.send_information.src_devices_name, pann_state_fun_param->src_dev_name);
         network_send_information.send_information.src_devices_no = pann_state_fun_param->src_dev_no;
         strcpy(network_send_information.dst_devices_name, dst_device_name);
         network_send_information.dst_devices_no = dst_device_no;
         
         BlockBufferWrite(pann_state_fun_param->send_net_cmd_buffer_id, (void *)&network_send_information, sizeof(network_send_information));
         debug_print(("ccu response: src=%s-%d\n", pann_state_fun_param->src_dev_name, pann_state_fun_param->src_dev_no));		 
         debug_print(("ccu response: dst=%s-%d\n", network_send_information.dst_devices_name, network_send_information.dst_devices_no));
         
         return ;
}
int CcuSetAudioPlayCmd(ann_state_param_t *pann_state_fun_param, unsigned char flag)
{
       int ret = 0;
       audio_play_control_t  tmp_control_cmd;
       send_infomation_t *precv_cmd_info = NULL;	   
	   
       if(NULL==pann_state_fun_param)
       {
              return -1;
       }       
       precv_cmd_info = (send_infomation_t *)&(pann_state_fun_param->precv_net_cmd->send_information);
	   
       tmp_control_cmd.audio_play_buffer_id = pann_state_fun_param->effective_local_audio_data_buffer_id;
       tmp_control_cmd.current_event = precv_cmd_info->event_type_ann;
       tmp_control_cmd.enable_play = flag; ///< flag=1: play;  flag=0:  stop
       tmp_control_cmd.enable_sample = 0;
       if(tmp_control_cmd.current_event == EMERG_ANN_EVENT)
       {
            tmp_control_cmd.play_times = precv_cmd_info->event_info_ann.emerg_announce.emerg_times;
       }
      else
       {
            tmp_control_cmd.play_times = 1;
       }
       if(tmp_control_cmd.current_event==OCC_EVENT)	
       {
              ///< for OCC, sample --> EAMP via Net and play --> analog bus
              tmp_control_cmd.audio_play_buffer_id = pann_state_fun_param->local_audio_play_buffer_id;
              tmp_control_cmd.send_audio_buffer_id = pann_state_fun_param->net_audio_send_buffer_id;
              tmp_control_cmd.pending_audio_buffer_id = pann_state_fun_param->audio_pending_buffer_id;
              tmp_control_cmd.enable_play = 0;			  
              tmp_control_cmd.enable_sample = flag;			  
       }

       if( BlockBufferWrite(pann_state_fun_param->audio_play_control_buffer_id, &tmp_control_cmd, sizeof(tmp_control_cmd)) < 0 )
       {
              debug_print(("CcuSetAudioPlayCmd:  cmd not written into buffer\n"));
	      ret = -1;
       }

       return ret;
}
ccu_skip_station_info_t  *CcuGetCurrentSkipStationInfoTable(void)
{
      return NULL;
}
void CloseAllPreReadFilePointer_manual(void)
{
	if(fp_next_station != NULL)
	{
		fclose(fp_next_station);
		fp_next_station = NULL;
	}
	if(fp_stop_station != NULL)
	{
		fclose(fp_stop_station);
		fp_stop_station = NULL;
	}
	
	return ;
}
void ClearAllAudioDataBuffer(ann_state_param_t *pann_state_fun_param)
{
        if(NULL==pann_state_fun_param)		 
        {
               return -1;
	}
		
       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id);
       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_stop_audio_data_buffer_id);
       CharBufferClear(pann_state_fun_param->local_audio_buffer_two.local_leave_audio_data_buffer_id);
       CharBufferClear(pann_state_fun_param->local_audio_buffer_two.local_stop_audio_data_buffer_id);
       pann_state_fun_param->local_audio_buffer_one.buffer_effective = 0;
       pann_state_fun_param->local_audio_buffer_one.leave_station_file_name[0] = '\0';
       pann_state_fun_param->local_audio_buffer_one.stop_station_file_name[0] = '\0';
       pann_state_fun_param->local_audio_buffer_two.buffer_effective = 0;			   
       pann_state_fun_param->local_audio_buffer_two.leave_station_file_name[0] = '\0';
       pann_state_fun_param->local_audio_buffer_two.stop_station_file_name[0] = '\0';
       pann_state_fun_param->effective_local_audio_data_buffer_id = 0;	

	return ;
}
void CloseAllReadFilePointer_manual(void)
{
	if(fp_station_play != NULL)
	{
		fclose(fp_station_play);
		fp_station_play = NULL;
	}
	
	return ;
}
void CloseAllFilePointer_manual(void)
{
	CloseAllPreReadFilePointer_manual();
	CloseAllReadFilePointer_manual();
	return ;
}


void NoticeMediaEmergInfo(ann_state_param_t  *pann_state_fun_param)
{}

void TCMSNoticeMediaEmergInfo(send_infomation_t  *p_send_infomation_param)
{}


int ReadEmergeFile(ann_state_param_t  *pann_state_fun_param)
{
	int ret_of_write_to_buffer = 0;
	char *file_path = NULL;
	send_infomation_t *pemerg_info = NULL;

	if(NULL == pann_state_fun_param )
	{
		debug_print(("ReadEmergeFile:  null ptr\n"));
		return -1;
	}

	pemerg_info = (send_infomation_t *)&(pann_state_fun_param->precv_net_cmd->send_information);
	if(pemerg_info->event_info_ann.emerg_announce.emerg_active == 1 &&
	   pemerg_info->event_info_ann.emerg_announce.emerg_begin_or_over == 1)
	{
		//file_path = GetEmergAudioDataFilePath(pemerg_info->event_info_ann.emerg_announce.emerg_sequence);
		file_path = (char *)("/emerg-01.mp3");
		if(file_path == NULL)
		{
			debug_print(("ReadEmergeFile: have not find the file\n"));
			return -1;
		}
		debug_print(("ReadEmergeFile: file name=%s\n", file_path));

		if(fp_emerg != NULL)
		{
			fclose(fp_emerg);
			fp_emerg = NULL;
		}

		if((fp_emerg = fopen(file_path,"r")) == NULL)
		{
			debug_print(("ReadEmergeFile:  open file fail\n"));
			return 0;
		}

		CharBufferClear(pann_state_fun_param->effective_local_audio_data_buffer_id);
		CloseSndCard(&ccu_analog_audio_handle, "playback", "finish");
	}

	if(fp_emerg == NULL)
	{
		return ;
	}

	if(!feof(fp_emerg))
	{
		char temp_buffer[LOCAL_AUDIO_DATA_BUFFER_SIZE];
		int number_of_read_local_file = 0;
		int bytes_write_into_buffer = 0;

		debug_print(("ReadEmergeFile: begin to read file\n"));

		if((number_of_read_local_file = fread(temp_buffer, sizeof(char), sizeof(temp_buffer), fp_emerg))<0)
		{
			debug_print(("ReadEmergeFile:  read file error,%d",number_of_read_local_file));
			fclose(fp_emerg);
			fp_emerg = NULL;
			return ;
		}
		total_bytes_read_emerg  += number_of_read_local_file;
		bytes_write_into_buffer = CharBufferWrite(pann_state_fun_param->effective_local_audio_data_buffer_id, (const void *)temp_buffer, number_of_read_local_file);
		if(bytes_write_into_buffer > 0)
		{
			total_bytes_write_to_buffer_emerg += bytes_write_into_buffer;
		}
		if(total_bytes_read_emerg >= (LOCAL_AUDIO_DATA_BUFFER_SIZE*2))
		{
			if( CcuSetAudioPlayCmd(pann_state_fun_param, 1) < 0 )
			{
				debug_print(("ReadEmergeFile:  play cmd into buffer, fail\n"));
			}
		}
	}
	else
	{
		debug_print(("ReadEmergeFile, file end:  total bytes: %d-%d\n", total_bytes_read_emerg, total_bytes_write_to_buffer_emerg));
		total_bytes_read_emerg = 0;
		total_bytes_write_to_buffer_emerg = 0;
		fclose(fp_emerg);
		fp_emerg = NULL;
	}

        return 0;
}
int PreReadLocalFile_manual(ann_state_param_t *pann_state_fun_param)
{
	int ret_of_write_to_buffer = 0;
        send_infomation_t  *param_manual_send_info = NULL;
        char temp_buffer[LOCAL_AUDIO_DATA_BUFFER_SIZE];
        int number_of_read_local_file;
	
        if(NULL==pann_state_fun_param)
        {
               return -1;
	}
        param_manual_send_info = (send_infomation_t *)&(pann_state_fun_param->precv_net_cmd->send_information);		
		
	if(param_manual_send_info->event_info_ann.manual_annnounce.manual_active == 1 &&
	    param_manual_send_info->event_info_ann.manual_annnounce.manual_pre_read == 1 &&
	    param_manual_send_info->event_info_ann.manual_annnounce.manual_begin_or_over == 0)
	{
		char stop_file_path[AUDIO_FILE_NAME_LENGTH], leave_file_path[AUDIO_FILE_NAME_LENGTH];
                #if 0
		sprintf(leave_file_path,"%d_%d.mp3",param_manual_send_info->event_info_ann.manual_annnounce.line_number,
				param_manual_send_info->event_info_ann.manual_annnounce.current_station_code,1);

		sprintf(stop_file_path,"%d_%d.mp3",param_manual_send_info->event_info_ann.manual_annnounce.line_number,
				param_manual_send_info->event_info_ann.manual_annnounce.current_station_code,2);
                #endif
                #if 1
		sprintf(leave_file_path,"%d_%d.mp3", param_manual_send_info->event_info_ann.manual_annnounce.current_station_code,1);

		sprintf(stop_file_path,"%d_%d.mp3", param_manual_send_info->event_info_ann.manual_annnounce.current_station_code,2);
                #endif				
	        #if 0
		  EampGetAudioPath( leave_file_path, (int)sizeof(leave_file_path),
		                                      (int)param_manual_send_info.event_info_ann.manual_annnounce.current_station_code, 0 );
		  EampGetAudioPath( stop_file_path, sizeof(stop_file_path), 
		                                      param_manual_send_info.event_info_ann.manual_annnounce.current_station_code, 1 );
	        #endif

		if( strcmp(pann_state_fun_param->local_audio_buffer_one.leave_station_file_name,leave_file_path) == 0 &&
	             strcmp(pann_state_fun_param->local_audio_buffer_one.stop_station_file_name,stop_file_path) == 0 )
		{
			debug_print(("PreReadLocalFile_manual:  Those files have been on 1st buffer,so there is no need to pre-read to buffer\n"));
			return -1;
		}
		else if( strcmp(pann_state_fun_param->local_audio_buffer_two.leave_station_file_name,leave_file_path) == 0 &&
		             strcmp(pann_state_fun_param->local_audio_buffer_two.stop_station_file_name,stop_file_path) == 0 )		     
		{
			debug_print(("PreReadLocalFile_manual: Those files have been on the 2nd buffer,so there is no need to pre-read to buffer\n"));
			return -1;
		}
               else
		{
			if(pann_state_fun_param->local_audio_buffer_one.buffer_effective == 0)
			{
                               debug_print(("PreReadLocalFile_manual:  go here, 011\n"));			
				p_buffer_free = (local_audio_buffer_t *)&(pann_state_fun_param->local_audio_buffer_one);
			        strcpy((char *)(p_buffer_free->leave_station_file_name), (char *)leave_file_path);
			        strcpy((char *)(p_buffer_free->stop_station_file_name), (char *)stop_file_path);				
			}
			else if(pann_state_fun_param->local_audio_buffer_two.buffer_effective == 0)
			{
                               debug_print(("PreReadLocalFile_manual:  go here, 012\n"));				
				p_buffer_free = (local_audio_buffer_t *)&(pann_state_fun_param->local_audio_buffer_two);
			        strcpy((char *)(p_buffer_free->leave_station_file_name), (char *)leave_file_path);
			        strcpy((char *)(p_buffer_free->stop_station_file_name), (char *)stop_file_path);				
			}                      		
		}

		CloseAllPreReadFilePointer_manual();

		if((fp_next_station = fopen(leave_file_path,"r")) == NULL)
		{
			debug_print(("PreReadLocalFile_Manual: open %s, fail!\n", leave_file_path));
		}
		if((fp_stop_station = fopen(stop_file_path,"r")) == NULL)
		{
			debug_print(("PreReadLocalFile_Manual: open %s, fail!\n", stop_file_path));
		}

		CharBufferClear(p_buffer_free->local_leave_audio_data_buffer_id);
		CharBufferClear(p_buffer_free->local_stop_audio_data_buffer_id);
                manual_pre_read_flag = 1;		

                //EampLoadBlankAudio(eamp_state.pre_local_leave_audio_data_buffer_id,(3 * 1024));
                //EampLoadBlankAudio(eamp_state.pre_local_stop_audio_data_buffer_id,(3 * 1024));
	}

	if(fp_next_station != NULL)
	{
		if(!feof(fp_next_station))
		{
			debug_print(("PreReadLocalFile_manual:  begin to read next station audio\n"));

			if((number_of_read_local_file = fread(temp_buffer, sizeof(char), sizeof(temp_buffer), fp_next_station))<0)
			{
				debug_print(("PreReadLocalFile_manual:  read away from station file error,%d",number_of_read_local_file));
				fclose(fp_next_station);
				fp_next_station = NULL;
				return -1;
			}
			//totoal_bytes_manual_pre_leave_read_file_size = totoal_bytes_manual_pre_leave_read_file_size + number_of_read_local_file;

			ret_of_write_to_buffer = CharBufferWrite(p_buffer_free->local_leave_audio_data_buffer_id, (const void *)temp_buffer, number_of_read_local_file);
			if(ret_of_write_to_buffer < 0)
			{
				debug_print(("PreReadLocalFile_AUTO:write audio data to buffer is error\n"));
			}
			else
			{
				//totoal_bytes_manual_pre_leave_write_to_buffer_size += ret_of_write_to_buffer;
			}
		}
		else
		{
			//totoal_bytes_manual_pre_leave_read_file_size = 0;
			//totoal_bytes_manual_pre_leave_write_to_buffer_size = 0;
			fclose(fp_next_station);
			fp_next_station = NULL;
		}
	}

	if(fp_stop_station != NULL)
	{
		if(!feof(fp_stop_station))
		{
			debug_print(("PreReadLocalFile_manual:  begin to read  stop station audio\n"));

			if((number_of_read_local_file = fread(temp_buffer, sizeof(char), sizeof(temp_buffer), fp_stop_station))<0)
			{
				debug_print(("PreReadLocalFile_manual: read arrive station file error,%d",number_of_read_local_file));
				fclose(fp_stop_station);
				fp_stop_station = NULL;
				return -1;
			}
			//totoal_bytes_manual_pre_stop_read_file_size += number_of_read_local_file;

			ret_of_write_to_buffer = CharBufferWrite(p_buffer_free->local_stop_audio_data_buffer_id, (const void *)temp_buffer, number_of_read_local_file);
			if(ret_of_write_to_buffer < 0)
			{
				debug_print(("PreReadLocalFile_Manual: write audio data to buffer is error\n"));
			}
			else
			{
				//totoal_bytes_manual_pre_stop_write_to_buffer_size += ret_of_write_to_buffer;
			}
		}
		else
		{
			//totoal_bytes_manual_pre_stop_read_file_size = 0;
			//totoal_bytes_manual_pre_stop_write_to_buffer_size = 0;
			fclose(fp_stop_station);
			fp_stop_station = NULL;
		}
	}

        #if 1 ///< debug only
        {
               if(manual_pre_read_flag)
               {                    
	             ReadSimulateAudioData(p_buffer_free->local_stop_audio_data_buffer_id, p_buffer_free->local_leave_audio_data_buffer_id, temp_buffer, sizeof(temp_buffer));
	       }
        }
        #endif

	return 0;
}
int ReadLocalFile_manual(ann_state_param_t *pann_state_fun_param) ///< play or stop
{
	int ret_of_write_to_buffer = 0;
        send_infomation_t  *param_manual_send_info = NULL;
	
        if(NULL==pann_state_fun_param)		 
        {
               return -1;
	}
        param_manual_send_info = (send_infomation_t *)&(pann_state_fun_param->precv_net_cmd->send_information);
		
	if(param_manual_send_info->event_info_ann.manual_annnounce.manual_active == 1 &&
	   param_manual_send_info->event_info_ann.manual_annnounce.manual_pre_read == 0)
	{
		if(param_manual_send_info->event_info_ann.manual_annnounce.manual_begin_or_over == 0)
		{
			CloseAllFilePointer_manual();
			ClearAllAudioDataBuffer(pann_state_fun_param);
		       CcuSetAudioPlayCmd(pann_state_fun_param, 0);
			return -1;
		}

		if( param_manual_send_info->event_info_ann.manual_annnounce.line_number != 3 && 
		    param_manual_send_info->event_info_ann.manual_annnounce.line_number != 4)
		{
			debug_print(("ReadLocalFile_manual:  Line mumber is error,this line is %d\n",
				param_manual_send_info->event_info_ann.manual_annnounce.line_number));
			return -1;
		}
		else
		{
		        #if 0
		        ccu_skip_station_info_t  *pskip_station_info = NULL;
		        unsigned char current_station_seq = 0;
		        pskip_station_info = CcuGetCurrentSkipStationInfoTable();
                        current_station_seq = param_manual_send_info->event_info_ann.manual_annnounce.current_station_code;
                        if(NULL == pskip_station_info)						
                        {
                               return -1;
			}
			if( pskip_station_info[current_station_seq].effective == 0 )
			{
				return -1;
			}
                        #endif			
		}

		char file_path[AUDIO_FILE_NAME_LENGTH] = "\0";
                #if 0
		sprintf(file_path,"%d_%d.mp3",param_manual_send_info->event_info_ann.manual_annnounce.line_number,
				param_manual_send_info->event_info_ann.manual_annnounce.current_station_code,
				param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave); ///< file name being played
               #else
	        ///< file name being played
                debug_print(("ReadLocalFile_manual, go here 33:  stop_or_leave,%d\n", param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave));	        
		if( param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave==0 )
		{
		      sprintf(file_path,"%d_%d.mp3", param_manual_send_info->event_info_ann.manual_annnounce.current_station_code, 1);
		}
		else if( param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave==1 )
		{
		      sprintf(file_path,"%d_%d.mp3", param_manual_send_info->event_info_ann.manual_annnounce.current_station_code, 2);
		}
               else
               {
                    debug_print(("ReadLocalFile_manual, go here 35:  wrong stop_or_leave,%d\n", param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave));
               }
              #endif				

		if(strcmp((char *)pann_state_fun_param->local_audio_buffer_one.leave_station_file_name, (char *)file_path) == 0)
		{
			debug_print(("ReadLocalFile_manual, buffer one:  next station audio has been in the buffer, so there is no need to read agian\n"));
			pann_state_fun_param->effective_local_audio_data_buffer_id = pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id;
			pann_state_fun_param->local_audio_buffer_one.buffer_effective = 1;
			pann_state_fun_param->local_audio_buffer_two.buffer_effective = 0;              
                        
			CharBufferBrush(pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id);
                        manual_pre_read_flag = 0; ///< debug only
		       CcuSetAudioPlayCmd(pann_state_fun_param, 1);
			return 0;
		}
		if(strcmp((char *)pann_state_fun_param->local_audio_buffer_one.stop_station_file_name, (char *)file_path) == 0)
		{
			debug_print(("ReadLocalFile_manual, buffer one:  stop station audio has been the buffer,so there is no need to read agian\n"));

			pann_state_fun_param->effective_local_audio_data_buffer_id = pann_state_fun_param->local_audio_buffer_one.local_stop_audio_data_buffer_id;
			pann_state_fun_param->local_audio_buffer_one.buffer_effective = 1;
			pann_state_fun_param->local_audio_buffer_two.buffer_effective = 0;

			CharBufferBrush(pann_state_fun_param->local_audio_buffer_one.local_stop_audio_data_buffer_id);
                        manual_pre_read_flag = 0; ///< debug only	
                        CcuSetAudioPlayCmd(pann_state_fun_param, 1);
			return 0;
		}

		if(strcmp((char *)pann_state_fun_param->local_audio_buffer_two.leave_station_file_name, (char *)file_path) == 0)
		{
			debug_print(("ReadLocalFile_manual, buffer two:  next station audio has been in the buffer, so there is no need to read agian\n"));
			
			pann_state_fun_param->effective_local_audio_data_buffer_id = pann_state_fun_param->local_audio_buffer_two.local_leave_audio_data_buffer_id;
			pann_state_fun_param->local_audio_buffer_two.buffer_effective = 1;
			pann_state_fun_param->local_audio_buffer_one.buffer_effective = 0;

			CharBufferBrush(pann_state_fun_param->local_audio_buffer_two.local_leave_audio_data_buffer_id);
                        manual_pre_read_flag = 0; ///< debug only	
		       CcuSetAudioPlayCmd(pann_state_fun_param, 1);                        
			return 0;
		}

		if(strcmp((char *)pann_state_fun_param->local_audio_buffer_two.stop_station_file_name, (char *)file_path) == 0)
		{
			debug_print(("ReadLocalFile_manual, buffer two:  stop station audio has been in the buffer, so there is no need to read agian\n"));

			pann_state_fun_param->effective_local_audio_data_buffer_id = pann_state_fun_param->local_audio_buffer_two.local_stop_audio_data_buffer_id;
			pann_state_fun_param->local_audio_buffer_two.buffer_effective = 1;
			pann_state_fun_param->local_audio_buffer_one.buffer_effective = 0;

			CharBufferBrush(pann_state_fun_param->local_audio_buffer_two.local_stop_audio_data_buffer_id);
                        manual_pre_read_flag = 0; ///< debug only	
		       CcuSetAudioPlayCmd(pann_state_fun_param, 1);                        
			return -1;
		}

		ClearAllAudioDataBuffer(pann_state_fun_param);
		CloseAllReadFilePointer_manual();

		if((fp_station_play = fopen(file_path,"r")) == NULL)
		{
			debug_print(("ReadLocalFile_manual, all buffer free:  must read file but fail\n"));
		}
                if(param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave==0 ||
                   param_manual_send_info->event_info_ann.manual_annnounce.stop_or_leave==1)
                {
                        strcpy((char *)pann_state_fun_param->local_audio_buffer_one.leave_station_file_name,(char *)file_path);
                        pann_state_fun_param->effective_local_audio_data_buffer_id = 
							pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id;
                        pann_state_fun_param->local_audio_buffer_one.buffer_effective = 1;
                        pann_state_fun_param->local_audio_buffer_two.buffer_effective = 0;
                        p_buffer_busy = &(pann_state_fun_param->local_audio_buffer_one);
		}
                else
                {
			debug_print(("ReadLocalFile_manual:  param is wrong\n"));
		}
	}

	if(fp_station_play== NULL)
	{
		return -1;
	}

	if(!feof(fp_station_play))
	{
		char temp_buffer[LOCAL_AUDIO_DATA_BUFFER_SIZE];
                int number_of_read_local_file;

		debug_print(("ReadLocalFile_manual:  Begin to read file\n"));

		if((number_of_read_local_file = fread(temp_buffer,sizeof(char),sizeof(temp_buffer), fp_station_play))<0)
		{
			  CloseAllReadFilePointer_manual();
                          return -1;
		}
		//totoal_bytes_manual_read_file_size += number_of_read_local_file;

		ret_of_write_to_buffer = CharBufferWrite(p_buffer_busy->local_leave_audio_data_buffer_id, (const void *)temp_buffer, number_of_read_local_file);
		if(ret_of_write_to_buffer < 0)
		{
			debug_print(("ReadLocalFile_manual:  write buffer wrong\n"));
		}
		else
		{
		       CcuSetAudioPlayCmd(pann_state_fun_param, 1);
			//begin_to_broadcast_audio_data = 1;
			//totoal_bytes_manual_write_to_buffer_size += ret_of_write_to_buffer;
		}
		CloseAllPreReadFilePointer_manual();
	}
	else
	{
		debug_print(("finish read local file \n"));
		//totoal_bytes_manual_write_to_buffer_size = 0;
		//totoal_bytes_manual_read_file_size = 0;
		CloseAllReadFilePointer_manual();
	}

	return 0;
}


///< now, these are state functions
int CcuStartOCC(ann_state_param_t *pann_state_fun_param)
{
	debug_print(("CcuStartOCC: enter\n"));

	if( NULL != pann_state_fun_param )
	{
	        CharBufferClear(pann_state_fun_param->net_audio_send_buffer_id);
                CharBufferClear(pann_state_fun_param->local_audio_play_buffer_id);
	        CharBufferClear(pann_state_fun_param->audio_pending_buffer_id);
			
	        PthreadPriorityChangeForSchedRr(pthread_self(), CCU_APP_THREAD_MAX_PRIOR); ///< up
   	        OpenSndCard(&ccu_analog_audio_handle, "codec");
   	        PthreadPriorityChangeForSchedRr(pthread_self(), CCU_PRIORIT); ///< down

	        //MicAdPcmWavheader(pann_state_fun_param->net_audio_send_buffer_id);
	        MicAdPcmWavheader(pann_state_fun_param->local_audio_play_buffer_id);
                SetDSTDeviceNameAndNumber("EAMP", AUDIO_DST_NO);

	}

	return 0;
}
int CcuProcessOCC(ann_state_param_t *pann_state_fun_param)
{ 
        network_send_package_t  *precv_net_cmd = NULL;
	 
        if( NULL != pann_state_fun_param )
        {      
               precv_net_cmd = pann_state_fun_param->precv_net_cmd;	       
               SendResponseCmd(pann_state_fun_param, "BCU", MUL_DST_NO);

//	        if( pann_state_fun_param->is_db_server)
               if(1)
	        {
                       if( NULL != precv_net_cmd && precv_net_cmd->send_information.event_type_ann==OCC_EVENT)
                       {
		              CcuSetAudioPlayCmd(pann_state_fun_param, 1); ///< 
		              CCU_OCC_EN;
		       }
		}	        
	}

        return 0;
}
int CcuStopOCC(ann_state_param_t *pann_state_fun_param)
{
	network_send_package_t  *precv_net_cmd = NULL;

	debug_print(("CcuStopOCC: enter\n"));		

	if( NULL != pann_state_fun_param )
	{
		precv_net_cmd = pann_state_fun_param->precv_net_cmd;
		//	        if( pann_state_fun_param->is_db_server)
		if(1)
		{
			if( NULL != precv_net_cmd && precv_net_cmd->send_information.event_type_ann==OCC_EVENT)
			{
				CcuSetAudioPlayCmd(pann_state_fun_param, 0); ///<
				CCU_OCC_DIS;
			}
		}	        
	}

	return 0;
}

int CcuStartAnnEmerg(ann_state_param_t *pann_state_fun_param)
{
	debug_print(("CcuStartAnnEmerg: enter\n"));
	if( NULL != pann_state_fun_param )
	{
		CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id);
		pann_state_fun_param->effective_local_audio_data_buffer_id =
		pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id;
		SendResponseCmd(pann_state_fun_param, "BCU", MUL_DST_NO);
	}

        return 0;
}
int CcuProcessAnnEmerg(ann_state_param_t *pann_state_fun_param)
{
	if( NULL != pann_state_fun_param )
	{
		NoticeMediaEmergInfo(pann_state_fun_param);
		SendResponseCmd(pann_state_fun_param, "BCU", MUL_DST_NO);
	}

        return 0;
}
int CcuStopAnnEmerg(ann_state_param_t *pann_state_fun_param)
{
	debug_print(("CcuStopAnnEmerg: enter\n"));
//	diag_printf("CcuStopAnnEmerg: exit\n");
//	diag_printf("CcuStopAnnEmerg: enter\n");
        if( NULL != pann_state_fun_param )
        {
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id);
	       pann_state_fun_param->effective_local_audio_data_buffer_id = 0;
//               SendResponseCmd(pann_state_fun_param, "BCU", MUL_DST_NO);
             //  CloseSndCard(&ccu_analog_audio_handle, "playback", "finish");
	       CcuSetAudioPlayCmd(pann_state_fun_param, 0);
	}
        ClearEmergInfo();
        cyg_thread_delay(100);
        ClearEmergInfo();
	return 0;
}

int CcuStartAnnManual(ann_state_param_t *pann_state_fun_param)
{
	debug_print(("CcuStartAnnManual:  enter\n"));

        if( NULL != pann_state_fun_param )
        {
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id);
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_stop_audio_data_buffer_id);
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_two.local_leave_audio_data_buffer_id);
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_two.local_stop_audio_data_buffer_id);
               pann_state_fun_param->local_audio_buffer_one.buffer_effective = 0;
	       pann_state_fun_param->local_audio_buffer_one.leave_station_file_name[0] = '\0';
	       pann_state_fun_param->local_audio_buffer_one.stop_station_file_name[0] = '\0';
               pann_state_fun_param->local_audio_buffer_two.buffer_effective = 0;			   
	       pann_state_fun_param->local_audio_buffer_two.leave_station_file_name[0] = '\0';
	       pann_state_fun_param->local_audio_buffer_two.stop_station_file_name[0] = '\0';
	       pann_state_fun_param->effective_local_audio_data_buffer_id = 0;
//               pecu_amp_6d5w_ctrl(3);
	}

        return 0;
}
int CcuProcessAnnManual(ann_state_param_t *pann_state_fun_param)
{
        if( NULL != pann_state_fun_param )
        {
               SendResponseCmd(pann_state_fun_param, "BCU", MUL_DST_NO);
//               PreReadLocalFile_manual(pann_state_fun_param);
//               ReadLocalFile_manual(pann_state_fun_param);

	}

	return 0;
}
int CcuStopAnnManual(ann_state_param_t *pann_state_fun_param)
{
      debug_print(("CcuStopAnnManual:  enter\n"));

        if( NULL != pann_state_fun_param )
        {
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_leave_audio_data_buffer_id);
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_one.local_stop_audio_data_buffer_id);
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_two.local_leave_audio_data_buffer_id);
	       CharBufferClear(pann_state_fun_param->local_audio_buffer_two.local_stop_audio_data_buffer_id);
               pann_state_fun_param->local_audio_buffer_one.buffer_effective = 0;
	       pann_state_fun_param->local_audio_buffer_one.leave_station_file_name[0] = '\0';
	       pann_state_fun_param->local_audio_buffer_one.stop_station_file_name[0] = '\0';
               pann_state_fun_param->local_audio_buffer_two.buffer_effective = 0;			   
	       pann_state_fun_param->local_audio_buffer_two.leave_station_file_name[0] = '\0';
	       pann_state_fun_param->local_audio_buffer_two.stop_station_file_name[0] = '\0';
	       pann_state_fun_param->effective_local_audio_data_buffer_id = 0;
	       CcuSetAudioPlayCmd(pann_state_fun_param, 0);
	}	  

      return 0;
}

int CcuStartAnnTms(ann_state_param_t *pann_state_fun_param)
{
      debug_print(("CcuStartAnnTms:  enter\n"));

      return 0;
}
int CcuProcessAnnTms(ann_state_param_t *pann_state_fun_param)
{

}
int CcuStopAnnTms(ann_state_param_t *pann_state_fun_param)
{
      debug_print(("CcuStopAnnTms:  enter\n"));

      return 0;
}

int CcuStartAnnIdle(ann_state_param_t *pann_state_fun_param)
{
      debug_print(("CcuStartAnnIdle:  enter\n"));

      return 0;
}
int CcuProcessAnnIdle(ann_state_param_t *pann_state_fun_param)
{

}
int CcuStopAnnIdle(ann_state_param_t *pann_state_fun_param)
{
      debug_print(("CcuStopAnnIdle:  enter\n"));

      return 0;
}

///< main state functions
int CcuGetEventPriorValue(int event)
{
       int prior_v = 0;
       state_priority_list_t  *tmp_node = NULL;	

      tmp_node = CcuAnnStatePriorityListSearch( (unsigned int)event );
      if( NULL==tmp_node )
      {
           return -1;
      }

       return (int)(tmp_node->priority);
}
int 	CcuDisplayEventName(int event)
{
        int ret = 0;
		
        switch(event)
        {
                case OCC_EVENT:
		debug_print(("curr event name:  OCC\n"));
                break;
				
                case EMERG_ANN_EVENT:	
		debug_print(("curr event name:  Emerg Ann\n"));				
                break;
				
                case MANUAL_ANN_EVENT:
		debug_print(("curr event name:  Manual Ann\n"));					
                break;
				
                case TMS_MANUAL_ANN_EVENT:
                case TMS_AUTO_ANN_EVENT:
		debug_print(("curr event name:  Tms Auto/Manual\n"));					
                break;

                case ANN_IDLE:
		debug_print(("curr event name:  Ann Idle\n"));					
                break;				

                default: 
		ret = -1;
		debug_print(("curr event name:  unsupported event %d\n", event));					
                break;
        }

        return ret;
}
int CcuAnnCmdParse(send_infomation_t  *precv_send_info)
{
       int ret = -1;
       int input_event = 0;
       int curr_evt_prior = 0;
       int input_evt_prior = 0;	   

       if(NULL==precv_send_info)	   
       {
            return -1;
       }

       input_event = precv_send_info->event_type_ann;
       debug_print(("CcuAnnCmdParse-1:  curr_evt=%d, new_evt=%d\n", current_event, input_event));
       curr_evt_prior = CcuGetEventPriorValue(current_event);
       input_evt_prior = CcuGetEventPriorValue(input_event);
       debug_print(("CcuAnnCmdParse-2:  curr_prior=%d, new_prior=%d\n", curr_evt_prior, input_evt_prior));
       if( curr_evt_prior>0 && input_evt_prior>0 )
       {
               if(curr_evt_prior  > input_evt_prior )
               {
                     if( 1==CcuStartCmdJudge(precv_send_info))
                     {
                          ret = 0; ///< switch
                     }
               }
               else if(curr_evt_prior  == input_evt_prior )
               {
                    if( 1==CcuEndCmdJudge(precv_send_info))
                    {
                         ret = 0;
                    }
               }
       }

       return ret;
}

int CcuStartNewEvent(ann_state_param_t *pann_state_fun_param)
{		
        switch(current_event)
        {
                case OCC_EVENT:
		CcuStartOCC(pann_state_fun_param);
                break;
				
                case EMERG_ANN_EVENT:	
		CcuStartAnnEmerg(pann_state_fun_param);					
                break;
				
                case MANUAL_ANN_EVENT:
		CcuStartAnnManual(pann_state_fun_param);						
                break;
				
                case TMS_MANUAL_ANN_EVENT:
                case TMS_AUTO_ANN_EVENT:
		CcuStartAnnTms(pann_state_fun_param);					
                break;

                case ANN_IDLE:
		CcuStartAnnIdle(pann_state_fun_param);					
                break;				

                default: 
                break;
        }

        return 0;
}

int CcuProcessCurrentEvent(ann_state_param_t *pann_state_fun_param)
{		
        switch(current_event)
        {
                case OCC_EVENT:
		CcuProcessOCC(pann_state_fun_param);
                break;
				
                case EMERG_ANN_EVENT:	
		CcuProcessAnnEmerg(pann_state_fun_param);
                break;
				
                case MANUAL_ANN_EVENT:
		CcuProcessAnnManual(pann_state_fun_param);
                break;
				
                case TMS_MANUAL_ANN_EVENT:
                case TMS_AUTO_ANN_EVENT:
		CcuProcessAnnTms(pann_state_fun_param);
                break;

                case ANN_IDLE:
		CcuProcessAnnIdle(pann_state_fun_param);
                break;

                default: 
//		debug_print(("CcuProcessCurrentEvent: unknown event=%d\n", current_event));
                break;				
        }

        return 0;
}
int CcuStopCurrentEvent(ann_state_param_t *pann_state_fun_param)
{
        send_infomation_t  *precv_send_info = NULL;
		
        if(NULL==pann_state_fun_param)
        {
              return -1;
	}
		
        switch(current_event)
        {
                case OCC_EVENT:
		CcuStopOCC(pann_state_fun_param);
                break;
				
                case EMERG_ANN_EVENT:	
		CcuStopAnnEmerg(pann_state_fun_param);					
                break;
				
                case MANUAL_ANN_EVENT:
		CcuStopAnnManual(pann_state_fun_param);						
                break;
				
                case TMS_MANUAL_ANN_EVENT:
                case TMS_AUTO_ANN_EVENT:
		CcuStopAnnTms(pann_state_fun_param);					
                break;

                case ANN_IDLE:
		CcuStopAnnIdle(pann_state_fun_param);
                break;				

                default: 
                break;
        }

        CcuDisplayEventName(current_event);
        precv_send_info = (send_infomation_t *)&(pann_state_fun_param->precv_net_cmd->send_information);		
        if(1==CcuEndCmdJudge(precv_send_info) )
        {
        	current_event = ANN_IDLE; ///< defautl
        }
        else
        {
        	current_event = pann_state_fun_param->precv_net_cmd->send_information.event_type_ann;
        }
        debug_print(("event changed, "));
        CcuDisplayEventName(current_event);
	
        return 0;
}

void ClearEmergInfo()
{
	notice_ccu_send_to_media_t notice_ccu_send_to_media,temp;
	notice_ccu_send_to_media.emerg_state = 0;
	notice_ccu_send_to_media.emerg_sequence = 1;
	int ret = 0;
	do{
		ret = BlockBufferWrite(ccu_state.ccu_send_to_media_cmd_buffer_id, (void *)&notice_ccu_send_to_media, sizeof(notice_ccu_send_to_media));
		if(ret <= 0)
		{
			BlockBufferRead(ccu_state.ccu_send_to_media_cmd_buffer_id, (void *)&temp, sizeof(temp));
			cyg_thread_delay(10);
		}
	}while(ret <= 0);
}


