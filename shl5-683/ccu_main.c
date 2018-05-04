/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_main.c
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
#include "ccu_main.h"

#define debug_printf_qc(s)  diag_printf s

extern int db_err_status;
extern unsigned char fdu_failure_flag;

char thread_stack_of_network[PTHREAD_STACK_MIN*8];
char thread_stack_of_control[PTHREAD_STACK_MIN*8];
char thread_stack_of_ccu_sync[PTHREAD_STACK_MIN*4];
char thread_stack_of_tftp_queue[PTHREAD_STACK_MIN*4];
char thread_stack_of_mvb_recv[PTHREAD_STACK_MIN*2];
char thread_stack_of_tms_cmd[PTHREAD_STACK_MIN*4];
char thread_stack_of_send_tms_cmd[PTHREAD_STACK_MIN*4];
char thread_stack_of_analog_sample[PTHREAD_STACK_MIN*8];
char thread_stack_of_serial_send[PTHREAD_STACK_MIN*8];
char thread_stack_of_media_and_monitor_serial_recv[PTHREAD_STACK_MIN*8];
char thread_stack_of_media_and_monitor_serial_cmd_parse[PTHREAD_STACK_MIN*8];
char thread_stack_of_failure_statics[PTHREAD_STACK_MIN*4];
char thread_stack_of_key_info[PTHREAD_STACK_MIN*2];
char thread_stack_of_log_info[PTHREAD_STACK_MIN*16];
pthread_t thread_of_network;
pthread_t thread_of_control;
pthread_t thread_of_ccu_sync;
pthread_t thread_of_tftp_queue;
pthread_t thread_of_mvb_recv;
pthread_t thread_of_tms_cmd;
pthread_t thread_of_tms_send_cmd;
pthread_t thread_of_analog_sample;
pthread_t thread_of_serial_send;
pthread_t thread_of_media_and_monitor_serial_recv;
pthread_t thread_of_media_and_monitor_serial_cmd_parse;
pthread_t thread_of_failure_statics;
pthread_t thread_of_key_info;
pthread_t thread_of_log_info;
//serial 0
cyg_io_handle_t ser0_handle;
static int  local_ccu_dev_no = 0;

cyg_io_handle_t  *GetSndCardHandle(void)
{
	return (cyg_io_handle_t *)&ccu_analog_audio_handle;
}
int CcuReadLocalMp3( FILE **audio_file_fp, int audio_buffer_id )
{
	int ret = -1;
	int rw_bytes = 0;
	unsigned char temp_buffer[1024*4];

	if( NULL != audio_file_fp && NULL != (*audio_file_fp))
	{
		if( !feof(*audio_file_fp) )
		{
			rw_bytes = fread(temp_buffer, sizeof(char), sizeof(temp_buffer), (*audio_file_fp));
			if( rw_bytes <0 )
			{
				debug_print(("read file error, %d", rw_bytes));
				fclose( *audio_file_fp );
				(*audio_file_fp) = NULL;
				return -1;
			}
			//pre_file_bytes += rw_bytes;
			rw_bytes = CharBufferWrite( audio_buffer_id, (const void *)temp_buffer, rw_bytes);
			if(rw_bytes < 0)
			{
				debug_print(("write audio data to buffer is error\n"));
			}
		}
		else
		{
			//debug_print(("read file end:  bytes=%d\n", totoal_file_bytes));
			//pre_file_bytes = 0; //reset
			fclose( *audio_file_fp );
			(*audio_file_fp) = NULL;
		}
	}

	return ret;
}
int CcuPlayLocalMp3( cyg_io_handle_t audio_handle, int audio_buffer_id )
{
	unsigned int current_total_bytes = 0;
	unsigned int current_right_bytes = 0;
	unsigned char *start_address = NULL;
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0, left_len = 0;
	unsigned char buffer_clear_flag = 0;

	buffer_clear_flag = CharBufferCheckClearFlag(audio_buffer_id);
	start_address = (unsigned char *)CharBufferCurrentReadPointer( audio_buffer_id, &current_total_bytes,&current_right_bytes);
	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_buffer_id) )
	{
		start_address = (unsigned char *)CharBufferCurrentReadPointer( audio_buffer_id, &current_total_bytes,&current_right_bytes);
		debug_print(("ccu: before bwrite, buffer cleared: %d,so return -1\n", current_total_bytes));
		return -1;
	}
	if( start_address != NULL && current_total_bytes != 0  )
	{
		len = (cyg_uint32)current_total_bytes;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM ;
		ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)start_address, &len, snd_set_local);
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_buffer_id) )
		{
			debug_print(("ccu: after bwrite, buffer cleared,so return -1\n"));
			return -1;
		}
		debug_print(("ccu: bw=%d, left=%d,%d\n", ret_bwrite, len, len_backup));
		if( len > len_backup )
		{
			left_len = 0;
		}
		else if ( len >= 0 )
		{
			left_len = len;
		}

		CharBufferUpdateCurrentReadPointer(audio_buffer_id, current_total_bytes, left_len );
	}
	else if( start_address != NULL && current_total_bytes == 0  )
	{
		return -2;
	}

	return 0;
}

///< add total bytes from IC
unsigned int sample_total_bytes = 0;
extern FILE *audio_file; ///< debug
cyg_uint8  audio_rx[ 1024 * 8 ]; ///< debug
void CcuAnalogAudioSample(cyg_io_handle_t audio_handle, int dest_buffer_id, int cp_dest_buffer_id)
{
	int wr_ret = 0;
	//cyg_uint8  audio_rx[ 1024 * 8 ];
	cyg_int32 len = 100, len_origin = 0;
	cyg_uint32 snd_set ;
	Cyg_ErrNo stat_rw;

	snd_set = (1<<8) |CYG_SND_AUDIO_STREAM;
	debug_print(("AnalogAudioSample:  enter \n", len));
	stat_rw = cyg_io_bread(audio_handle, (void *)audio_rx, &len, snd_set);
	debug_print(("AnalogAudioSample:  len = %d\n", len));
	if(stat_rw == -EIO ||stat_rw < 0 )
	{
		len = 0 ;
	}

	if( len > 0 )
	{
		sample_total_bytes += len;
#if 1
		wr_ret = CharBufferWrite(cp_dest_buffer_id, audio_rx, len); ///< copy it
		wr_ret = CharBufferWrite(dest_buffer_id, audio_rx, len);
		debug_print(("AnalogAudioSample:  wr_ret=%d, len=%d\n", wr_ret, len));
#else
		//ReadSimulateAudioData_OCC(audio_rx, len);
		//wr_ret = CharBufferWrite(cp_dest_buffer_id, audio_rx, len);
		//wr_ret = CharBufferWrite(dest_buffer_id, audio_rx, len);
		if( sample_total_bytes >= 1024*127 )
		{
			if( NULL != audio_file )
			{
				audio_play_control_t  tmp_control_cmd;
				int con_buffer_id = 0;

				SaveSampleAudioData_CloseFile();
				audio_file = NULL;


				tmp_control_cmd.enable_sample = 0;
				tmp_control_cmd.enable_play = 0;
				con_buffer_id = BlockBufferOpen("ccu-audio-play-cmd-Rx");
				BlockBufferWrite(con_buffer_id, &tmp_control_cmd, sizeof(tmp_control_cmd))	;
				debug_print(("AnalogAudioSample:  audio_file > 127KiB, close it \n"));
			}
		}
		else
		{
			SaveSampleAudioData_OCC(audio_rx, len);
			debug_print(("AnalogAudioSample:  wr_ret=%d, len=%d\n", wr_ret, len));
		}
#endif
	}

	return ;
}
int CcuAnalogAudioPlay(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id)
{
	unsigned int current_total_bytes = 0;
	unsigned int main_current_total_bytes = 0;
	unsigned int pending_current_total_bytes = 0;
	unsigned int right_bytes = 0;	
	cyg_uint32 snd_set_local;
	int ret_bwrite = 0;
	cyg_uint32 len = 0, len_backup = 0, left_len = 0;
	int ret = 0;
	char tmp_buff[3*1024];
	int tmp_read_len = 0;
	int index = 0;
	int total_read_len = 0;
	unsigned char  buffer_clear_flag  = CharBufferCheckClearFlag(audio_source_buffer_id);
	unsigned char if_read_main_buffer = 0;
	int border_of_audia_data_size = 64;

	CharBufferCurrentReadPointer(pending_audio_buffer_id, &pending_current_total_bytes, &right_bytes);
	CharBufferCurrentReadPointer(audio_source_buffer_id, &main_current_total_bytes, &right_bytes);
	current_total_bytes = main_current_total_bytes + pending_current_total_bytes;
	//diag_printf("&&&&&&& total = %d &&&&&&\n", current_total_bytes);
	if( current_total_bytes < 1024*32 )
	{
		//return -100;
	}
	if( current_total_bytes <=  border_of_audia_data_size )
	{
		//debug_print(("CcuAnalogAudioPlay:  2 buffer < 2K\n",current_total_bytes));	
		return -5;
	}

	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )
	{
		debug_print(("CcuAnalogAudioPlay: before bwrite, buffer cleared: %d,so return -1\n",current_total_bytes));
		return -1;
	}

	if( current_total_bytes > border_of_audia_data_size )
	{
		ret = 0;
		ret = CharBufferRead(pending_audio_buffer_id, tmp_buff, sizeof(tmp_buff) );
		if( ret >0 )
		{
			tmp_read_len = sizeof(tmp_buff) - ret;
			index = ret;
			total_read_len += ret;
			if( total_read_len < sizeof(tmp_buff) )
			{
				if_read_main_buffer = 1;
			}
		}
		else if( -2 == ret ) ///< pending is empty
		{
			tmp_read_len = sizeof(tmp_buff);
			index = 0;
			total_read_len = 0;
			if_read_main_buffer = 1;
		}
		else
		{
			debug_print(("CcuAnalogAudioPlay:  read pending=%d, This is unlikely\n", ret));
			return -6; ///< error
		}

		if( if_read_main_buffer==1)
		{
			ret = 0;
			ret = CharBufferRead(audio_source_buffer_id, &tmp_buff[index], tmp_read_len );
			if( ret > 0 )
			{
				total_read_len += ret;
			}
			else
			{
				//				debug_print(("CcuAnalogAudioPlay:  read main=%d, This is unlikely\n", ret));
				//				return -8;
			}
		}

		if( total_read_len < border_of_audia_data_size)
		{
			debug_print(("CcuAnalogAudioPlay:  total(%d) < 2KiB, This is unlikely\n", total_read_len));
			return -6;
		}
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )///<2013-11-11
		{
			debug_print(("CcuAnalogAudioPlay: before bwrite, buffer cleared-02\n"));
			return -1;
		}

		//debug_print(("eamp, playTwice:  play total=%d \n", total_read_len));
		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;

		//		if(begin_to_broadcast_audio_data == 1)
		{
			ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
		}


		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) ) //add, 2013
		{
			debug_print(("CcuAnalogAudioPlay: after bwrite, buffer cleared,so return -1\n"));
			return -1;
		}

		//debug_print(("CcuAnalogAudioPlay: bw=%d, left=%d,%d,dec=%d\n", ret_bwrite, len, len_backup,len_backup - len));
		debug_print(("CcuAnalogAudioPlay: bw=%d, total=%d, dec=%d, left=%d \n", ret_bwrite, current_total_bytes, len_backup - len, current_total_bytes-(len_backup - len)));

		if( len > len_backup )
		{
			left_len = 0;
		}
		else if ( len >= 0 )
		{
			left_len = len;
		}
		if( left_len > 0 ) ///< write into pending buffer
		{
			CharBufferWrite(pending_audio_buffer_id, &tmp_buff[total_read_len-left_len], left_len );
		}
		ret = 0; ///< write 2KiB,ok
	}
	else
	{
		ret = -2;
	}

	return ret;
}
int CcuGetFileNameFromDb( char *pre_file_name, char *stop_file_name, int current_station_code)
{
	return 0;
}
int CcuPreProcessTmsCmd(send_infomation_t  *psend_info, FILE **pre_file_fp, FILE **stop_file_fp, int *valid_evt )
{
	int ret = -1;
	FILE *tmp_pre_file_fp = NULL;
	FILE *tmp_stop_file_fp = NULL;
	char pre_file_name[FILE_PATH_LENGTH];
	char stop_file_name[FILE_PATH_LENGTH];

	if( NULL != psend_info &&
			(psend_info->event_type_ann == TMS_AUTO_ANN_EVENT ||psend_info->event_type_ann == TMS_MANUAL_ANN_EVENT) )
	{
		if( psend_info->event_info_ann.tms_auto_announce.tms_auto_active == 1 &&
				psend_info->event_info_ann.tms_auto_announce.tms_auto_pre_read == 1 &&
				psend_info->event_info_ann.tms_auto_announce.tms_auto_begin_or_over == 0 )
		{
			CcuGetFileNameFromDb( pre_file_name, stop_file_name, psend_info->event_info_ann.tms_auto_announce.current_station_code);
			tmp_pre_file_fp = fopen(pre_file_name, "r");
			if(  NULL==tmp_pre_file_fp)
			{
				debug_print(("open %s fail\n", pre_file_name));
			}
			(*pre_file_fp) = tmp_pre_file_fp;

			tmp_stop_file_fp = fopen(stop_file_name, "r");
			if(  NULL==tmp_stop_file_fp)
			{
				debug_print(("open %s fail\n", stop_file_name));
			}
			(*stop_file_fp) = tmp_stop_file_fp;
		}
		else if( psend_info->event_info_ann.tms_auto_announce.tms_auto_active == 1 &&
				psend_info->event_info_ann.tms_auto_announce.stop_or_leave == 0 ) //play pre-file
		{
			(*valid_evt) = 0;
		}
		else if( psend_info->event_info_ann.tms_auto_announce.tms_auto_active == 1 &&
				psend_info->event_info_ann.tms_auto_announce.stop_or_leave == 1 ) //play stop-file
		{
			(*valid_evt) = 1;
		}

		//clear cmd
		psend_info->event_type_ann = ANN_IDLE;
		ret = 0; //valid cmd
	}

	return ret;
}

int CcuSetLocalDevNumberFromDriver(void)
{
	int  device_type = -1;
	int  device_no = -1;

	device_type = ReadType();
	device_no = ReadNumWilson();

	local_ccu_dev_no = device_no;

	return 0;
}
int CcuGetLocalDevNumber(void)
{
	return local_ccu_dev_no;
}

int CcuProcessTmsControlCmd( tms_to_ccu_t  *ptms_cmd_recv_package, int net_cmd_send_buffer, int local_cmd_recv_buffer )
{
	int ret = -1;
	ccu_to_tms_t  tmp_tms_res_package = {0};
	network_send_package_t   tms_to_eamp_package = {"\0",0};

	if( NULL != ptms_cmd_recv_package )
	{
		ret = 0;
		if( ptms_cmd_recv_package->frame_begin_flag != TMS_SIG_PACK_HEAD ||
				ptms_cmd_recv_package->frame_over_flag   != TMS_SIG_PACK_HEAD )
		{
			debug_print(("CcuProcessTmsControlCmd: wrong head/tail for tms package\n"));
			return -1;
		}

		if ( ptms_cmd_recv_package->frame_type == TMS_SIG_PACK_TYPE_PING ) //just like ping ip,so ping back
		{
			tmp_tms_res_package.frame_begin_flag = TMS_SIG_PACK_HEAD;
			tmp_tms_res_package.communication_state = TMS_SIG_PACK_TYPE_PING;
		}
		else if ( ptms_cmd_recv_package->frame_type == TMS_SIG_PACK_TYPE_SETTING )
		{

		}
		else if ( ptms_cmd_recv_package->frame_type == TMS_SIG_PACK_TYPE_DATA )
		{
			strcpy(tms_to_eamp_package.dst_devices_name, "EAMP");
			tms_to_eamp_package.dst_devices_no = 255;
			strcpy(tms_to_eamp_package.send_information.src_devices_name, "CCU");
			tms_to_eamp_package.send_information.src_devices_no = CcuGetLocalDevNumber();
			tms_to_eamp_package.send_information.event_type_ann = TMS_AUTO_ANN_EVENT;
			tms_to_eamp_package.send_information.event_info_ann.tms_auto_announce.current_station_code = ptms_cmd_recv_package->next_station_no;
			tms_to_eamp_package.send_information.event_info_ann.tms_auto_announce.line_number = 3;
			BlockBufferWrite(net_cmd_send_buffer, &tms_to_eamp_package, sizeof(tms_to_eamp_package));
			strcpy(tms_to_eamp_package.dst_devices_name, "BCU");
			tms_to_eamp_package.dst_devices_no = 255;
			BlockBufferWrite(net_cmd_send_buffer, &tms_to_eamp_package, sizeof(tms_to_eamp_package));
			BlockBufferWrite(local_cmd_recv_buffer, &tms_to_eamp_package, sizeof(tms_to_eamp_package));
		}
		else
		{

		}
	}

	return ret;
}
int DisplayTmsControlCmd(tms_to_ccu_t  *ptms_cmd_recv_package)
{
	int ret = -1;

	if( NULL != ptms_cmd_recv_package )
	{
		ret = 0;
	}

	return ret;
}

int CcuSerialZeroPortInit(void)
{
	int ret = -1;
	Cyg_ErrNo err;
	cyg_serial_info_t ser_info;
	unsigned int len;
	unsigned char buffer[16];

	err = cyg_io_lookup("/dev/ser0", &ser0_handle);

	if (ENOERR == err)
	{
		ret = 0;
		debug_print(("CcuSerPortInit: serial 0 found\n"));

		// cyg_io_get_config
		len = sizeof(buffer);

		cyg_io_get_config(ser0_handle, CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH, &buffer[0], &len);
		cyg_io_get_config(ser0_handle, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_FLUSH, &buffer[0], &len);

		// cyg_io_set_config
		ser_info.baud = CYGNUM_SERIAL_BAUD_9600; //CYGNUM_SERIAL_BAUD_38400
		ser_info.word_length = CYGNUM_SERIAL_WORD_LENGTH_8;
		ser_info.stop = CYGNUM_SERIAL_STOP_1;
		ser_info.parity = CYGNUM_SERIAL_PARITY_NONE;
		ser_info.flags = CYGNUM_SERIAL_FLOW_NONE;
		len = sizeof(ser_info);
		cyg_io_set_config(ser0_handle, CYG_IO_SET_CONFIG_SERIAL_INFO, &ser_info, &len);

		len = 1;
		cyg_io_set_config(ser0_handle, CYG_IO_SET_CONFIG_SERIAL_WRITE_BLOCKING, 0, &len);
		cyg_io_set_config(ser0_handle, CYG_IO_SET_CONFIG_SERIAL_READ_BLOCKING, 0, &len);
		//cyg_io_set_config(ser0_handle, CYGOPT_IO_SERIAL_SUPPORT_NONBLOCKING, 0, &len);
	}
	else
	{
		debug_print(("CcuSerPortInit:  serial 0 not found\n"));
	}

	return ret;
}

int CcuCheckDriverKey(void)
{
	int ret = -1;

	if(1)
	{
		ret = 1;
	}
	else
	{
		ret = 0;
	}

	return ret;
}


int CcuGetControlNetCmdTxBufferId(void)
{
	return 0;
}

int CcuDisplayRecvNetPackage(network_send_package_t  *precv_net_cmd)
{
	if( NULL==precv_net_cmd )
	{
		debug_print(("CcuDisplayRecvNetPackage: null ptr\n"));
		return -1;
	}

	debug_print(("dst: %s-%d\n", precv_net_cmd->dst_devices_name, precv_net_cmd->dst_devices_no));
	debug_print(("src: %s-%d\n", precv_net_cmd->send_information.src_devices_name, precv_net_cmd->send_information.src_devices_no));
	debug_print(("ann: %d\n",precv_net_cmd->send_information.event_type_ann));
	debug_print(("intercom: %d\n",precv_net_cmd->send_information.event_type_intercom));
	if(precv_net_cmd->send_information.event_type_ann == MANUAL_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  manual ann: package number %d\n", precv_net_cmd->send_information.sequence));
		debug_print(("manual_active = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.manual_active));
		debug_print(("manual_begin_or_over = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.manual_begin_or_over));
		debug_print(("manual_pre_read = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.manual_pre_read));

		debug_print(("manual_response = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.manual_response));
		debug_print(("line_number = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.line_number));
		debug_print(("current_station_code = %d,\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.current_station_code));
		debug_print(("door_side = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.door_side));
		debug_print(("stop_or_leave = %d\n", precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave));
	}
	else if(precv_net_cmd->send_information.event_type_ann == EMERG_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  emerg ann:package number %d\n", precv_net_cmd->send_information.sequence));
		debug_print(("emerg_active = %d\n", precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_active));
		debug_print(("emerg_begin_or_over = %d\n", precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_begin_or_over));
		debug_print(("emerg_response = %d\n", precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_response));
		debug_print(("emerg_sequence = %d\n", precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_sequence));
		debug_print(("emerg_times = %d\n", precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_times));
	}
	else if(precv_net_cmd->send_information.event_type_ann == LIVE_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  live ann:package number %d\n", precv_net_cmd->send_information.sequence));
		debug_print(("live_active = %d\n", precv_net_cmd->send_information.event_info_ann.live_announce.live_active));
		debug_print(("live_begin_or_over = %d\n", precv_net_cmd->send_information.event_info_ann.live_announce.live_begin_or_over));
		debug_print(("live_response = %d\n", precv_net_cmd->send_information.event_info_ann.live_announce.live_response));
	}
	else if(precv_net_cmd->send_information.event_type_ann == TMS_AUTO_ANN_EVENT)
	{
		debug_print(("Debug cmd info:  tms auto ann:package number %d\n", precv_net_cmd->send_information.sequence));
		debug_print(("tms_auto_active = %d\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active));
		debug_print(("tms_auto_begin_or_over = %d\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_begin_or_over));
		debug_print(("tms_auto_response = %d\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_response));
		debug_print(("line_number = %d\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.line_number));
		debug_print(("current_station_code = %d,\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.current_station_code));
		debug_print(("door_side = %d\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.door_side));
		debug_print(("stop_or_leave = %d\n", precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave));
	}
	else if(precv_net_cmd->send_information.event_type_ann == ANN_IDLE)
	{
		if(precv_net_cmd->send_information.event_type_intercom == D2P_INTERCOMM_EVENT)
		{
			debug_print(("D2P active = %d\n",precv_net_cmd->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_active));
			debug_print(("D2P dst bcu no = %d\n",precv_net_cmd->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no));
			debug_print(("D2P dst pcu no = %d\n",precv_net_cmd->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no));
			debug_print(("D2P pending = %d\n",precv_net_cmd->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_pending));
			debug_print(("D2P request or over = %d\n",precv_net_cmd->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_request_or_over));
			debug_print(("D2P response = %d\n",precv_net_cmd->send_information.event_info_intercom.d2p_intercomm.d2p_intercomm_response));
		}
		else if(precv_net_cmd->send_information.event_type_intercom == D2D_INTERCOMM_EVENT)
		{
			debug_print(("D2D active = %d\n",precv_net_cmd->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_active));
			debug_print(("D2D dst bcu no = %d\n",precv_net_cmd->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_bcu_device_no));
			debug_print(("D2D ppt state = %d\n",precv_net_cmd->send_information.event_info_intercom.d2d_intercomm.d2d_ppt_state));
			debug_print(("D2D request or over = %d\n",precv_net_cmd->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over));
			debug_print(("D2D response = %d\n",precv_net_cmd->send_information.event_info_intercom.d2d_intercomm.d2d_intercomm_response));
		}
		else
		{
			debug_print(("Debug cmd info:  idle ann:package number %d\n", precv_net_cmd->send_information.sequence));
		}
	}
	else
	{
		debug_print(("This is a error package\n"));
	}

	return 0;
}

int GetDeviceType(char *devices_name)
{
	int ret = -1;

	if( strcmp(devices_name,"BCU") == 0)
	{
		ret = TYPE_OF_BCU;
	}
	else if(strcmp(devices_name,"CCU") == 0)
	{
		ret = TYPE_OF_CCU;
	}
	else if(strcmp(devices_name,"EMAP") == 0)
	{
		ret = TYPE_OF_EAMP;
	}
	else if(strcmp(devices_name,"PCU") == 0)
	{
		ret = TYPE_OF_PECU;
	}
	else
	{
		ret = -1;
	}

	return ret;
}

void CcuUpdateLocalGdTable(send_infomation_t  *precv_net_info)
{
	int src_dev_type = 0;
	int src_dev_no = 0;

	if(NULL==precv_net_info)
	{
		debug_print(("CcuUpdateLocalGdTable: null ptr\n"));
		return ;
	}

	src_dev_type = GetDeviceType(precv_net_info->src_devices_name);
	src_dev_no = precv_net_info->src_devices_no;
	switch(src_dev_type)
	{
	case TYPE_OF_BCU:
	{

	}
	break;

	}

	return ;
}

int CcuTestCreateTftpWhenPu(void)
{
	int server_id = 0;

	if ((server_id = tftp_server_start()) < 0)
	{
		debug_print( ("tftpd start error\n") );
		return -1;
	}

	return 0;
}

void CcuTftpdAlarmSyncWaiting(cyg_handle_t alarm, cyg_addrword_t data)	
{
	AlarmTimeOutProcess( (void *)data );
	//other application process
	return ;
}
void CcuAlarmControlWaiting(cyg_handle_t alarm, cyg_addrword_t data)	
{
	AlarmTimeOutProcess( (void *)data );
	//other application process
	return ;
}
void CcuAlarmControlPlay(cyg_handle_t alarm, cyg_addrword_t data)	
{
	AlarmTimeOutProcess( (void *)data );
	//other application process
	return ;
}
void CcuAlarmControlEnd(cyg_handle_t alarm, cyg_addrword_t data)	
{
	AlarmTimeOutProcess( (void *)data );
	//other application process
	return ;
}
void CcuAlarmOccBegin(cyg_handle_t alarm, cyg_addrword_t data)	
{
	AlarmTimeOutProcess( (void *)data );
	//other application process
	return ;
}

///< Thread Entry
void *CcuNetworkThreadEntry(void *arg)
{
	network_buffer_t network_buffer_info;

	network_buffer_info.udp_data_socket_buffer.udp_data_socket_send_buffer = CharBufferOpen("ccu-network-audio-Tx");
	network_buffer_info.udp_data_socket_buffer.udp_data_socket_recv_buffer = CharBufferOpen("ccu-network-audio-Rx");
	network_buffer_info.udp_cmd_socket_buffer.udp_cmd_socket_send_buffer = BlockBufferOpen("ccu-network-cmd-Tx");
	network_buffer_info.udp_cmd_socket_buffer.udp_cmd_socket_recv_buffer = BlockBufferOpen("ccu-network-cmd-Rx");
	network_buffer_info.udp_common_socket_buffer.udp_common_socket_send_buffer = BlockBufferOpen("network-udp-common-send");
	network_buffer_info.udp_common_socket_buffer.udp_common_socket_recv_buffer = BlockBufferOpen("network-udp-common-recv");

	network_buffer_info.udp_common_socket_big_buffer.udp_common_socket_send_big_buffer = BlockBufferOpen("network-udp-common-big-send");
	network_buffer_info.udp_common_socket_big_buffer.udp_common_socket_recv_big_buffer = BlockBufferOpen("network-udp-common-big-recv");	

	network_buffer_info.udp_heart_socket_buffer.udp_heart_socket_recv_buffer = BlockBufferOpen("network-udp-heart-recv");
	network_buffer_info.udp_heart_socket_buffer.udp_heart_socket_send_buffer = BlockBufferOpen("network-udp-heart-send");
	network_buffer_info.udp_dmu_socket_buffer.udp_dmu_socket_recv_buffer = 0;
	network_buffer_info.udp_dmu_socket_buffer.udp_dmu_socket_send_buffer = 0;
	network_buffer_info.udp_monitor_socket_buffer.udp_monitor_socket_recv_buffer = BlockBufferOpen("ccu-monitor-command-recv");
	network_buffer_info.udp_monitor_socket_buffer.udp_monitor_socket_send_buffer = BlockBufferOpen("ccu-monitor-command-send");

#ifdef CONFIG_CCU_VIDEOLINK
	network_buffer_info.udp_ccu_videolink_buffer.udp_ccu_videolink_buffer_rx =
			BlockBufferOpen("ccu-videolink-cmd-rx");
	network_buffer_info.udp_ccu_videolink_buffer.udp_ccu_videolink_buffer_tx  =
			BlockBufferOpen("ccu-videolink-cmd-tx");
#endif

	network_buffer_info.udp_shell_socket_buffer.udp_shell_socket_recv_buffer = BlockBufferOpen("network-udp-shell-recv");
	network_buffer_info.udp_shell_socket_buffer.udp_shell_socket_send_buffer = BlockBufferOpen("network-udp-shell-send");

	NetWorkMain(network_buffer_info);
	return 0;
}
void *CcuTftpQueueThreadEntry(void *arg)
{
	char db_server_ip[32] = {0};
	int loop_time = 0;
	unsigned char tftp_created = 0;
	unsigned char tftp_get_server_ip = 0;

	//int alarm_id_tftpd;	
	//alarm_id_tftpd = AlarmCreate("ccu-tftpd-alarm", 1000, 1000, CcuTftpdAlarmSyncWaiting );
	//AlarmEnable(alarm_id_tftpd);
	memset((void *)db_server_ip, 0, sizeof(db_server_ip));

	for(;;)
	{
		if(loop_time++ >= 10000)
		{
			//debug_print(("Ccu tftpD:  running\n"));
			loop_time = 0;
		}

		if ( 0==tftp_get_server_ip && 1==CcuGdIpTableIfHasRead() )
		{
			if( 1==CcuGdReadServerIp(db_server_ip, sizeof(db_server_ip)) ) ///< this is server-CCU
			{
				diag_printf("Ccu tftpD thread::  server ip = %s \n", db_server_ip);
				tftp_get_server_ip = 1;
			}
		}

		if( 1==tftp_get_server_ip ) ///< test tftp server
		{
			if( tftp_created==0 )
			{
				debug_print(("Ccu tftpD thread:  call TftpServerAPI, %s \n", db_server_ip));
				TftpServerAPI(db_server_ip);
				tftp_created = 1;
			}
		}

#if 0		
		if( 1==AlarmCheckTimeOut(alarm_id_tftpd) ) ///< test tftp server
		{
			debug_print(("Ccu tftpD thread::  timeout again\n"));
			if( tftp_created==0 )
			{
				AlarmDisable(alarm_id_tftpd);
				//CcuTestCreateTftpWhenPu();
				TftpServerAPI();
				tftp_created = 1;
			}
		}
#endif

#if 0
		///< test db:  read TIpMac table
		if( 0==test_read_db_tftp )
		{
			debug_print(("CCU,tftp:  read Iptable \n"));
			if( 0==CcuTestReadIpMacTable_tftpd(&test_row, &ip_table) )
			{
				test_read_db_tftp = 1;
			}
		}
		else if( 1==test_read_db_tftp )
		{
			if( 3==CcuGetReadTableDisplaySwitch() )
			{
				debug_print(("CCU,tfptd:  display Iptable %d, %08x \n", test_row, ip_table));
				//CcuGdDisplayTableTIpMac(test_row, ip_table);
				CcuGdDisplayTableTline(test_row, ip_table);
				CcuSetReadTableDisplaySwitch(4);
				free(ip_table);
				ip_table = NULL;
				test_row = 0;
				test_read_db_tftp = 2;
			}
		}	
#endif				

		sched_yield();
	}

	return 0;
}
#if 1
void *CcuAnalogSampleThreadEntry(void *arg)
{
	//	return NULL;
	int loop_count = 0;
	int audio_play_cmd_recv_buffer = 0;
	audio_play_control_t  audio_play_control;
	int current_event = 0;
	unsigned char new_play_cmd = 0;
	cyg_io_handle_t  *paudio_handle = NULL;

	audio_play_cmd_recv_buffer = BlockBufferOpen("ccu-audio-play-cmd-Rx");
	paudio_handle = GetSndCardHandle();
	memset((void *)&audio_play_control, 0, sizeof(audio_play_control));

	for(;;)
	{
		if(loop_count++ >= 5000)
		{
			loop_count = 0;
			debug_print(("CCU,analog play running\n"));
		}

		if( BlockBufferRead(audio_play_cmd_recv_buffer, &audio_play_control, sizeof(audio_play_control)) > 0 )
		{
			new_play_cmd = 1;
			debug_print(("CCU,analog play:  new cmd=%d, %d, %d, %d, %d \n", audio_play_control.current_event,
					audio_play_control.audio_play_buffer_id,
					audio_play_control.enable_play,
					audio_play_control.enable_sample,					
					audio_play_control.play_times));
		}

		if(audio_play_control.enable_sample) ///< now, just for OCC
		{
			paudio_handle = GetSndCardHandle(); ///<
			CcuAnalogAudioSample( (*paudio_handle), audio_play_control.send_audio_buffer_id, audio_play_control.audio_play_buffer_id);
			//CcuAnalogAudioPlay( (*paudio_handle), audio_play_control.audio_play_buffer_id, audio_play_control.pending_audio_buffer_id);
		}
		if(audio_play_control.enable_play)
		{
			paudio_handle = GetSndCardHandle();
			if( CcuPlayLocalMp3( (*paudio_handle), audio_play_control.audio_play_buffer_id) < 0 )
			{
				if( current_event == EMERG_ANN_EVENT )
				{
					CharBufferBrush(audio_play_control.audio_play_buffer_id);
					if(audio_play_control.play_times > 0)
					{
						audio_play_control.play_times--;
					}
					else
					{
						//memset((void *)&audio_play_control, 0, sizeof(audio_play_control));
					}
				}
				else
				{
					//memset((void *)&audio_play_control, 0, sizeof(audio_play_control));
				}
			}
		}

		if( 0==audio_play_control.enable_play && 0==audio_play_control.enable_sample)
		{
			if(new_play_cmd)
			{
				new_play_cmd = 0;
				memset((void *)&audio_play_control, 0, sizeof(audio_play_control));
			}
		}

		sched_yield();
	}

	return 0;
}
#endif

#ifdef CONFIG_CCU_VIDEOLINK
void CcuDoCmdFromVideoHost(void  *precv_buffer, int shell_cmd_tx_buffer_id)
{ 
	if( NULL==precv_buffer )
	{
		debug_print(("CcuDoCmdFromVideoHost: null ptr\n"));
		return ;
	}

	return ;
}
void CcuSendCmdToVideoHost_old(network_send_package_t *precv_net_cmd, int send_video_cmd_tx_buffer_id)
{
#if 0
	char send_buf[1024];
	int send_bytes = 0;

	if( NULL==precv_net_cmd )
	{
		debug_print(("CcuSendCmdToVideoLinkage: null ptr\n"));
		return ;
	}

	if( 0 == strcmp(precv_net_cmd->dst_devices_name, "EAMP") )
	{ ///< only for BCU-->EAMP cmd
		if( precv_net_cmd->send_information.event_type_ann==EMERG_ANN_EVENT )
		{
			if( precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_active == 1 )
			{
				send_buf[0] = 'E';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';
				send_buf[4] = precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_sequence;
				send_buf[5] = 0x01; ///< emerg_active = 1
				send_buf[6] = 0x02; ///< operator: <1>=save; <2>:display; <3>: disable
				send_bytes = 7;
				BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
			}
			else
			{
				send_buf[0] = 'E';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';
				send_buf[4] = precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_sequence;
				send_buf[5] = 0x00; ///< emerg_active = 0
				send_buf[6] = 0x03; ///< operator: must be <3>: disable
				send_bytes = 7;
				BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
			}
		}
		else if ( precv_net_cmd->send_information.event_type_ann==OCC_EVENT ||
				precv_net_cmd->send_information.event_type_ann==LIVE_ANN_EVENT )
		{
			send_buf[0] = 'E';
			send_buf[1] = 'M';
			send_buf[2] = 'V';
			send_buf[3] = 'L';
			send_buf[4] = 0xFF;
			send_buf[5] = 0x00; ///< emerg_active = 0
			send_buf[6] = 0x03; ///< operator: must be <3>: disable
			send_bytes = 7;
			BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
		}
		else if( precv_net_cmd->send_information.event_type_ann==MANUAL_ANN_EVENT ||
				precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
		{
			diag_printf("<0111>send vlink: MANUAL_ANN_EVENT or auto,recved\n");
			if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.manual_active ||
					1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active )
			{
				///< station_information_t (*pp_station_info)[32];
				station_information_t *pstation_info = ccu_state.pp_station_info[0];
				int station_no = 0;
				int next_no = 0; //andy
				int end_no = 0; //andy

				if( precv_net_cmd->send_information.event_type_ann==MANUAL_ANN_EVENT )
				{
					station_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.current_station_code;
					next_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.next_station_code;
					end_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code;
				}
				else if( precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
				{
					station_no = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.current_station_code;
					next_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.next_station_code;
					end_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code;
				}

				send_buf[0] = 'D';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';

				if( 0==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						0==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave ) ///< next
					send_buf[4] = 0x05;
				else if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave ) ///< we are now at
					send_buf[4] = 0x04;
				//                send_buf[5] = 0xff; ///< name len1337
				//                send_buf[6] = 0xff; ///< name len
				//                strcpy(&send_buf[7], pstation_info[station_no].station_chinese_name);
				//                send_bytes = 7+strlen(pstation_info[station_no].station_chinese_name);
				//                send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
				//                send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
				if(send_buf[4] == 0x05)
				{ //next
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( next_no>=MAX_STATION_NUMBER )
						return ;
					strcpy(&send_buf[7], pstation_info[next_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[next_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);

					send_buf[4] = 0x04;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( station_no>=MAX_STATION_NUMBER )
						return ;
					strcpy(&send_buf[7], pstation_info[station_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[station_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);

					send_buf[4] = 0x07;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( end_no>=MAX_STATION_NUMBER )
						return ;
					strcpy(&send_buf[7], pstation_info[end_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[end_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);

				}
				if(send_buf[4] == 0x04)
				{ //arrive
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( station_no>=MAX_STATION_NUMBER )
						return ;
					strcpy(&send_buf[7], pstation_info[station_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[station_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);

					send_buf[4] = 0x05;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( next_no>=MAX_STATION_NUMBER )
						return ;
					strcpy(&send_buf[7], pstation_info[next_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[next_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);

					send_buf[4] = 0x07;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( end_no>=MAX_STATION_NUMBER )
						return ;
					strcpy(&send_buf[7], pstation_info[end_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[end_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
				}

				// BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
				diag_printf("send vlink: byte=%d,station_no=%d,%d\n", send_bytes,
						station_no, strlen(pstation_info[station_no].station_chinese_name));
				diag_printf("send vlink-0531: station=%d,%d,%d\n", station_no, next_no, end_no);

				ann_station_report_t *pstation_led_info = pstation_info[station_no].p_station_info;
				send_buf[0] = 'D';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';
				send_buf[4] = 0x06; ///< scroll-text
				send_buf[5] = 0xff; ///< scroll-text bytes
				send_buf[6] = 0xff; ///< scroll-text bytes
				send_bytes = 7;
				if( 0==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						0==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
				{ ///< next
					int count = 0;
					audio_display_list *led_text_head = pstation_led_info->station_type[0].display;

					int line;
					int up_down;
					char* end_staton_display;
					char* end_staton_audio;
					int end_station_no;
					int ret_loop;
					int open_door;
					void *p_ann_report ;

					if( 0==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.manual_annnounce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.manual_annnounce.up_down;
						if( up_down>1 )
							up_down = up_down-1;
						end_staton_display =
								pstation_info[precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code].station_chinese_name;
						end_staton_audio =
								pstation_info[precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code].station_audio;
						end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
						ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						open_door = precv_net_cmd->send_information.event_info_ann.manual_annnounce.door_side;
						p_ann_report =
								(void *)pstation_info[station_no].p_station_info;
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
					}
					else if( 0==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.up_down;
						if( up_down>1 )
							up_down = up_down-1;
						end_staton_display =
								pstation_info[precv_net_cmd->send_information.event_info_ann.tms_auto_announce.end_station_code].station_chinese_name;
						end_staton_audio =
								pstation_info[precv_net_cmd->send_information.event_info_ann.tms_auto_announce.end_station_code].station_audio;
						end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
						ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						open_door = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.door_side;
						p_ann_report =
								(void *)pstation_info[station_no].p_station_info;
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
					}

					for(;;) ///< there may be 16 sections
					{
						if( NULL != (led_text_head+count) && 0 != led_text_head[count].audio_display[0] )
						{
							int index = 0;
							if( 0==led_text_head[count].available_or_not ) {
								int len = strlen(led_text_head[count].audio_display);
#if 0 ///< debug
								diag_printf("<2016-1039> count=%d,len=%d\n",count, len);
								for(index=0; index<len; ) {
									diag_printf("<2016-1039>text=%02x,%02x,%02x\n",
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++]
									);
								}
#endif
								if(send_bytes+len>1024)
									len = 1024 - send_bytes;
								strncpy(&send_buf[send_bytes], led_text_head[count].audio_display, len);
								send_bytes += strlen(led_text_head[count].audio_display);
							}
						}
						else
						{
							break;
						}
						count++;
					}
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
					diag_printf("send vlink,next station: byte=%d,station_no=%d\n", send_bytes, station_no);
				}
				else if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
				{  ///< we are now at
					int count = 0;
					audio_display_list *led_text_head = pstation_led_info->station_type[1].display;

					int line;
					int up_down;
					char* end_staton_display;
					char* end_staton_audio;
					int end_station_no;
					int ret_loop;
					int open_door;
					void *p_ann_report ;

					if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.manual_annnounce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.manual_annnounce.up_down;
						if( up_down>1 )
							up_down = up_down-1;
						end_staton_display =
								pstation_info[precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code].station_chinese_name;
						end_staton_audio =
								pstation_info[precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code].station_audio;
						end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
						ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						open_door = precv_net_cmd->send_information.event_info_ann.manual_annnounce.door_side;
						p_ann_report =
								(void *)pstation_info[station_no].p_station_info;
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
					}
					else if( 1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.up_down;
						if( up_down>1 )
							up_down = up_down-1;
						end_staton_display =
								pstation_info[precv_net_cmd->send_information.event_info_ann.tms_auto_announce.end_station_code].station_chinese_name;
						end_staton_audio =
								pstation_info[precv_net_cmd->send_information.event_info_ann.tms_auto_announce.end_station_code].station_audio;
						end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
						ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						open_door = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.door_side;
						p_ann_report =
								(void *)pstation_info[station_no].p_station_info;
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
					}

					for(;;) ///< there may be 16 sections
					{
						if( NULL != (led_text_head+count) && 0 != led_text_head[count].audio_display[0] )
						{
							int index = 0;
							if( 0==led_text_head[count].available_or_not ) {
								int len = strlen(led_text_head[count].audio_display);
								diag_printf("<2016-1039> count=%d,len=%d\n",count, len);
								for(index=0; index<len; ) {
									diag_printf("<2016-1039>text=%02x,%02x,%02x\n",
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++]
									);
								}
								if(send_bytes+len>1024)
									len = 1024 - send_bytes;
								strncpy(&send_buf[send_bytes], led_text_head[count].audio_display, len);
								send_bytes += strlen(led_text_head[count].audio_display);
							}
						}
						else
						{
							break;
						}
						count++;
					}
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
					diag_printf("send vlink,we are now station: byte=%d,station_no=%d\n", send_bytes, station_no);
				}
			}
		}
		else if( precv_net_cmd->send_information.event_type_ann==TMS_MANUAL_ANN_EVENT )
		{
			diag_printf("CcuSendCmdToVideoLinkage: TMS_MANUAL_ANN_EVENT\n");
		}
		else
		{
			diag_printf("CcuSendCmdToVideoLinkage: other EAMP's cmd: %d\n",
					precv_net_cmd->send_information.event_type_ann);
		}
	}
	else
	{
		debug_print(("CcuSendCmdToVideoLinkage: not EAMP's cmd,02: %d\n",
				precv_net_cmd->send_information.event_type_ann));
	}

	return ;
#endif
}

///< add begin, for LCD's timestamp
unsigned char media_time_sync_flag = 0;
cyg_handle_t counter_handle_media_time_sync;
cyg_handle_t alarm_handle_media_time_sync;
cyg_alarm alarm_object_media_time_sync;

void alarm_func_media_time_sync(cyg_handle_t alarm_handle, cyg_addrword_t data)
{
	media_time_sync_flag = 1;
}

void CreatMediaTimeSyncTimer()
{
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_media_time_sync);
	cyg_alarm_create(counter_handle_media_time_sync,alarm_func_media_time_sync,0,
			&alarm_handle_media_time_sync,&alarm_object_media_time_sync);
	cyg_alarm_initialize(alarm_handle_media_time_sync,cyg_current_time()+24000,24000);
	cyg_alarm_enable(alarm_handle_media_time_sync);
}
///< add end, for LCD's timestamp
void CcuSendCmdToVideoHost(network_send_package_t *precv_net_cmd, int send_video_cmd_tx_buffer_id)
{
	char send_buf[1024];
	int send_bytes = 0;
	int i;

	if( NULL==precv_net_cmd )
	{
		diag_printf(("CcuSendCmdToVideoLinkage: null ptr\n"));
		return ;
	}

	///<���������������������TCMS������������������������������TCMS������������
	if(1 == media_time_sync_flag)
	{	
		send_buf[0] = 'T';
		send_buf[1] = 'I';
		send_buf[2] = 'M';
		send_buf[3] = 'E';
		send_buf[4] = ccu_state.ccu_tms_car_running_info.current_time.year;
		send_buf[5] = ccu_state.ccu_tms_car_running_info.current_time.month; 
		send_buf[6] = ccu_state.ccu_tms_car_running_info.current_time.date;
		send_buf[7] = ccu_state.ccu_tms_car_running_info.current_time.hour;
		send_buf[8] = ccu_state.ccu_tms_car_running_info.current_time.minute;
		send_buf[9] = ccu_state.ccu_tms_car_running_info.current_time.second;
		send_bytes = 10;

		diag_printf("-------------CcuSendCmdToVideoLinkage->send time sync!!!\n");
		BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
		media_time_sync_flag = 0;
	}

	static unsigned char mm = 1;
	while(mm>0)
	{///<������������������������������LCD������������
		send_buf[0] = 'E';
		send_buf[1] = 'M';
		send_buf[2] = 'V';
		send_buf[3] = 'L';
		send_buf[4] = 0x00;
		send_buf[5] = 0x00; ///< emerg_active = 0
		send_buf[6] = 0x03; ///< operator: must be <3>: disable
		send_bytes = 7;
		BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
		mm--;
	}

	if( 0 == strcmp(precv_net_cmd->dst_devices_name, "EAMP") )
	{///<������������������������������������������OCC������������������������������������������������
		if( precv_net_cmd->send_information.event_type_ann==EMERG_ANN_EVENT )
		{
			if( precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_active == 1 )
			{
				send_buf[0] = 'E';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';
				send_buf[4] = precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_sequence;
				send_buf[5] = 0x01; ///< emerg_active = 1
				send_buf[6] = 0x02; ///< operator: <1>=save; <2>:display; <3>: disable
				send_bytes = 7;
				BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
			}
			else
			{
				send_buf[0] = 'E';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';
				send_buf[4] = precv_net_cmd->send_information.event_info_ann.emerg_announce.emerg_sequence;
				send_buf[5] = 0x00; ///< emerg_active = 0
				send_buf[6] = 0x03; ///< operator: must be <3>: disable
				send_bytes = 7;
				BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
			}
		}
		else if ( precv_net_cmd->send_information.event_type_ann==OCC_EVENT ||
				precv_net_cmd->send_information.event_type_ann==LIVE_ANN_EVENT )
		{
			send_buf[0] = 'E';
			send_buf[1] = 'M';
			send_buf[2] = 'V';
			send_buf[3] = 'L';
			send_buf[4] = 0xFF;
			send_buf[5] = 0x00; ///< emerg_active = 0
			send_buf[6] = 0x03; ///< operator: must be <3>: disable
			send_bytes = 7;
			BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
		}
		else if( precv_net_cmd->send_information.event_type_ann==MANUAL_ANN_EVENT ||
				precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
		{
			if( 1 == precv_net_cmd->send_information.event_info_ann.manual_annnounce.manual_active ||
					1 == precv_net_cmd->send_information.event_info_ann.tms_auto_announce.tms_auto_active )
			{
				station_information_t *pstation_info = ccu_state.pp_station_info[0]; ///< [0] for line-3
				int station_no = 0;
				int next_no = 0; //andy
				int end_no = 0; //andy

				send_buf[0] = 'D';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';

				if( precv_net_cmd->send_information.event_type_ann==MANUAL_ANN_EVENT )
				{
					station_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.current_station_code;
					//next_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.next_station_code;
					end_no = precv_net_cmd->send_information.event_info_ann.manual_annnounce.end_station_code;
				}
				else if( precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
				{
					station_no = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.current_station_code;
					//next_no = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.next_station_code;
					end_no = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.end_station_code;
				}

				if( 0==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						0==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
				{
					//next station is
					send_buf[4] = 0x05;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( (station_no > MAX_STATION_NUMBER) || (station_no < 1) )
					{
						diag_printf("(station_no > MAX_STATION_NUMBER) || (station_no < 1) --is  %d-->return!\n",station_no);
						return ;
					}
					send_bytes = 7;
					strcpy(&send_buf[send_bytes], pstation_info[station_no].station_chinese_name);
					send_bytes = send_bytes+strlen(pstation_info[station_no].station_chinese_name);
					send_buf[send_bytes] = 0x23;///<"#"
					send_bytes++;
					strcpy(&send_buf[send_bytes], pstation_info[station_no].station_english_name);
					send_bytes =send_bytes+strlen(pstation_info[station_no].station_english_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
					diag_printf(("send vlink:------------------leave station name!!!!\n"));
#if 0
					for(i=0;i<send_bytes;i++)
					{
						diag_printf("send_buf[%d]:0x%02x\n",i,send_buf[i]);
					}
#endif
#if 0
					send_buf[4] = 0x04;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( station_no>=MAX_STATION_NUMBER || station_no==0 )
						return ;
					strcpy(&send_buf[7], pstation_info[station_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[station_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
#endif

					send_buf[4] = 0x07; ///< terminal
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if ( ccu_state.current_line_no==3 )
						end_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
					else if ( ccu_state.current_line_no==4 )
						end_no = ccu_state.ccu_region_info[1].current_region_info.over_station_no;
					if( (end_no > MAX_STATION_NUMBER) || (end_no < 0) )
					{
						diag_printf("(end_no > MAX_STATION_NUMBER) || (end_no < 0)---->return!\n");
						return ;
					}

					if( precv_net_cmd->send_information.event_type_ann==MANUAL_ANN_EVENT )
					{
						if( precv_net_cmd->send_information.event_info_ann.manual_annnounce.up_down ==1 )
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Inner Loop");
								send_bytes =send_bytes+strlen("Inner Loop");
							}
							else
							{
								strcpy(&send_buf[7], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								strcpy(&send_buf[send_bytes+1], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+1+strlen(pstation_info[end_no].station_english_name);
							}
						}
						else
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Outer Loop");
								send_bytes =send_bytes+strlen("Outer Loop");
							}
							else
							{
								strcpy(&send_buf[7], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}
						}
					}
					else if( precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
					{
						if(precv_net_cmd->send_information.event_info_ann.tms_auto_announce.up_down ==1 )
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Inner Loop");
								send_bytes =send_bytes+strlen("Inner Loop");
							}
							else
							{
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}
						}
						else
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Outer Loop");
								send_bytes =send_bytes+strlen("Outer Loop");
							}
							else
							{
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}
						}
					}
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
				}
				else if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave ) ///< we are now at
				{
					//arrive
					send_buf[4] = 0x04;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( (station_no > MAX_STATION_NUMBER) || (station_no < 1) )
					{
						diag_printf("(station_no > MAX_STATION_NUMBER) || (station_no < 1)---->return!\n");
						return ;
					}
					send_bytes = 7;
					strcpy(&send_buf[send_bytes], pstation_info[station_no].station_chinese_name);
					send_bytes = send_bytes+strlen(pstation_info[station_no].station_chinese_name);
					send_buf[send_bytes] = 0x23;///<"#"
					send_bytes++;
					strcpy(&send_buf[send_bytes], &pstation_info[station_no].station_english_name);
					send_bytes =send_bytes+strlen(pstation_info[station_no].station_english_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
					diag_printf(("send vlink:------------------arrive station name!!!!\n"));
#if 0
					for(i=0;i<send_bytes;i++)
					{
						diag_printf("send_buf[%d]:0x%02x\n",i,send_buf[i]);
					}
#endif
#if 0
					send_buf[4] = 0x05;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if( next_no>=MAX_STATION_NUMBER || next_no==0 )
						return ;
					strcpy(&send_buf[7], pstation_info[next_no].station_chinese_name);
					send_bytes = 7+strlen(pstation_info[next_no].station_chinese_name);
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
#endif

					send_buf[4] = 0x07;
					send_buf[5] = 0xff; ///< name len1337
					send_buf[6] = 0xff; ///< name len
					if ( ccu_state.current_line_no==3 )
						end_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
					else if ( ccu_state.current_line_no==4 )
						end_no = ccu_state.ccu_region_info[1].current_region_info.over_station_no;
					if( (end_no > MAX_STATION_NUMBER) || (end_no < 1) )
					{
						diag_printf("(end_no>MAX_STATION_NUMBER) || (end_no < 1)---->return!\n");
						return ;
					}

					if( precv_net_cmd->send_information.event_type_ann==MANUAL_ANN_EVENT )
					{
						if(precv_net_cmd->send_information.event_info_ann.manual_annnounce.up_down ==1 )
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Inner Loop");
								send_bytes =send_bytes+strlen("Inner Loop");
							}
							else
							{
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}

						}
						else
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Outer Loop");
								send_bytes =send_bytes+strlen("Outer Loop");
							}
							else
							{
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}
						}
					}
					else if( precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
					{
						if(precv_net_cmd->send_information.event_info_ann.tms_auto_announce.up_down ==1 )
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Inner Loop");
								send_bytes =send_bytes+strlen("Inner Loop");
							}
							else
							{
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}
						}
						else
						{
							send_bytes = 7;
							if(1==ccu_state.ccu_region_info[ccu_state.current_line_no-3].current_region_info.whether_the_cycle_bus)
							{
								strcpy(&send_buf[send_bytes], "������");
								send_bytes = send_bytes+strlen("������");
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], "Outer Loop");
								send_bytes =send_bytes+strlen("Outer Loop");
							}
							else
							{
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_chinese_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_chinese_name);
								send_buf[send_bytes] = 0x23;///<"#"
								send_bytes++;
								strcpy(&send_buf[send_bytes], pstation_info[end_no].station_english_name);
								send_bytes = send_bytes+strlen(pstation_info[end_no].station_english_name);
							}
						}
					}

					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);

				}

				///<LCD������������ begin
				ann_station_report_t *pstation_led_info = pstation_info[station_no].p_station_info;
				send_buf[0] = 'D';
				send_buf[1] = 'M';
				send_buf[2] = 'V';
				send_buf[3] = 'L';
				send_buf[4] = 0x06; ///< scroll-text
				send_buf[5] = 0xff; ///< scroll-text bytes
				send_buf[6] = 0xff; ///< scroll-text bytes
				send_bytes = 7;
				if( 0==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						0==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
				{ ///< next
					int count = 0;
					audio_display_list *led_text_head = pstation_led_info->station_type[0].display;

					int line;
					int up_down;
					char* end_staton_display;
					char* end_staton_audio;
					int end_station_no;
					int ret_loop;
					int open_door;
					void *p_ann_report ;

					if( 0==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.manual_annnounce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.manual_annnounce.up_down;
						if( up_down>1 )
						{
							up_down = up_down-1;
						}

						if ( ccu_state.current_line_no==3 )
						{
							end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						}
						else if ( ccu_state.current_line_no==4 )
						{
							end_station_no = ccu_state.ccu_region_info[1].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus;
						}
						end_staton_display = pstation_info[end_station_no].station_chinese_name;
						end_staton_audio = pstation_info[end_station_no].station_audio;

						open_door = precv_net_cmd->send_information.event_info_ann.manual_annnounce.door_side-1;//wyy-xg
						p_ann_report = (void *)pstation_info[station_no].p_station_info;
						debug_printf_qc(("[1027-1430]: ,%d,%d,%d,%d,%d,%d,%d, 0x08x \n", line, up_down, end_staton_display, end_staton_audio,
								end_station_no, ret_loop, open_door, (unsigned int)p_ann_report));
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
						debug_printf_qc(("[1027-1430-1]: go here \n"));
					}
					else if( 0==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.up_down;
						if( up_down>1 )
						{
							up_down = up_down-1;
						}

						if ( ccu_state.current_line_no==3 )
						{
							end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						}
						else if ( ccu_state.current_line_no==4 )
						{
							end_station_no = ccu_state.ccu_region_info[1].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus;
						}
						end_staton_display = pstation_info[end_station_no].station_chinese_name;
						end_staton_audio = pstation_info[end_station_no].station_audio;
						open_door = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.door_side-1;//wyy-xg
						p_ann_report = (void *)pstation_info[station_no].p_station_info;
						debug_printf_qc(("[1027-1432]: ,%d,%d,%d,%d,%d,%d,%d \n", line, up_down, end_staton_display, end_staton_audio, end_station_no, ret_loop, open_door));
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
						debug_printf_qc(("[1027-1432-1]: go here \n"));
					}
					///<LCD������������ end

					debug_printf_qc(("[1027-1433]: ,%d,%d,%d \n", (unsigned int)led_text_head, count ));
#if 1
					for(;;) ///< there may be 16 sections
					{
						debug_printf_qc(("[1027-1433-2]: ,%d,%d,%d \n", (unsigned int)(led_text_head), count, led_text_head[count].available_or_not));
						if( NULL != (led_text_head+count) && 0 != led_text_head[count].audio_display[0] )
						{
							int index = 0;
							if( 0==led_text_head[count].available_or_not )
							{
								int len = strlen(led_text_head[count].audio_display);
#if 1 ///< debug
								diag_printf("<2016-1039> count=%d,len=%d\n",count, len);
								for(index=0; index<len;)
								{
									diag_printf("<2016-1039>text=%02x,%02x,%02x\n",
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++]
									);
								}
#endif
								if(send_bytes+len>1024)
								{
									len = 1024 - send_bytes;
								}
								strncpy(&send_buf[send_bytes], led_text_head[count].audio_display, len);
								//send_bytes += strlen(led_text_head[count].audio_display);
								send_bytes += len;
							}
						}
						else
						{
							break;
						}
						count++;
					}
#endif
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
					debug_printf_qc(("send vlink,next station: byte=%d,station_no=%d\n", send_bytes, station_no));
				}
				else if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave ||
						1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
				{  ///< we are now at
					int count = 0;
					audio_display_list *led_text_head = pstation_led_info->station_type[1].display;

					int line;
					int up_down;
					char* end_staton_display;
					char* end_staton_audio;
					int end_station_no;
					int ret_loop;
					int open_door;
					void *p_ann_report ;

					if( 1==precv_net_cmd->send_information.event_info_ann.manual_annnounce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.manual_annnounce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.manual_annnounce.up_down;
						if( up_down>1 )
						{
							up_down = up_down-1;
						}

						if ( ccu_state.current_line_no==3 )
						{
							end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						}
						else if ( ccu_state.current_line_no==4 )
						{
							end_station_no = ccu_state.ccu_region_info[1].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus;
						}

						end_staton_display = pstation_info[end_station_no].station_chinese_name;
						end_staton_audio = pstation_info[end_station_no].station_audio;

						open_door = precv_net_cmd->send_information.event_info_ann.manual_annnounce.door_side-1;//wyy-xg
						p_ann_report =
								(void *)pstation_info[station_no].p_station_info;
						diag_printf("[1027-1456]: ,%d,%d,%d,%d,%d,%d,%d \n", line, up_down, end_staton_display, end_staton_audio, end_station_no, ret_loop, open_door);
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
					}
					else if( 1==precv_net_cmd->send_information.event_info_ann.tms_auto_announce.stop_or_leave )
					{
						line = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.line_number;
						up_down = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.up_down;
						if( up_down>1 )
							up_down = up_down-1;
						if ( ccu_state.current_line_no==3 )
						{
							end_station_no = ccu_state.ccu_region_info[0].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[0].current_region_info.whether_the_cycle_bus;
						}
						else if ( ccu_state.current_line_no==4 )
						{
							end_station_no = ccu_state.ccu_region_info[1].current_region_info.over_station_no;
							ret_loop = ccu_state.ccu_region_info[1].current_region_info.whether_the_cycle_bus;
						}

						end_staton_display = pstation_info[end_station_no].station_chinese_name;
						end_staton_audio = pstation_info[end_station_no].station_audio;
						open_door = precv_net_cmd->send_information.event_info_ann.tms_auto_announce.door_side-1;//wyy-xg
						p_ann_report =
								(void *)pstation_info[station_no].p_station_info;
						diag_printf("[1027-1457]: ,%d,%d,%d,%d,%d,%d,%d \n", line, up_down, end_staton_display, end_staton_audio, end_station_no, ret_loop, open_door);
						FileAttributeInit(line, up_down, end_staton_display, end_staton_audio, end_station_no,
								ret_loop, open_door, p_ann_report);
					}

					for(;;) ///< there may be 16 sections
					{
						if( NULL != (led_text_head+count) && 0 != led_text_head[count].audio_display[0] )
						{
							int index = 0;
							if( 0==led_text_head[count].available_or_not )
							{
								int len = strlen(led_text_head[count].audio_display);
#if 0
								debug_printf_qc(("<2016-1039> count=%d,len=%d\n",count, len));
								for(index=0; index<len; )
								{
									debug_printf_qc(("<2016-1039>text=%02x,%02x,%02x\n",
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++],
											led_text_head[count].audio_display[index++]
									));
								}
#endif
								if(send_bytes+len>1024)
									len = 1024 - send_bytes;
								strncpy(&send_buf[send_bytes], led_text_head[count].audio_display, len);
								//send_bytes += strlen(led_text_head[count].audio_display);
								send_bytes += len;
							}
						}
						else
						{
							break;
						}
						count++;
					}
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
					debug_printf_qc(("send vlink,we are now station: byte=%d,station_no=%d\n", send_bytes, station_no));
				}
			}
		}
		else if( precv_net_cmd->send_information.event_type_ann==SKIP_STATION )
		{
			int station_no = 0;
			station_information_t *pstation_info_line3 = ccu_state.pp_station_info[0]; ///<
			station_information_t *pstation_info_line4 = ccu_state.pp_station_info[1]; ///<

			diag_printf("CcuSendCmdToVideoLinkage: SKIP_STATION\n"); ///< set or clear
			send_buf[0] = 'D';
			send_buf[1] = 'M';
			send_buf[2] = 'V';
			send_buf[3] = 'L';
			send_buf[4] = 0x08; ///< skip cmd
			send_buf[5] = 0xff; ///< total len
			send_buf[6] = 0xff; ///< total len
			send_bytes = 7;
			if( precv_net_cmd->send_information.event_info_ann.station_information.line_number == 3 )
			{
				station_no = precv_net_cmd->send_information.event_info_ann.station_information.line_number;
				if( 1 ) ///< up
				{
					int i = 0;
					for(i=station_no; i<MAX_STATION_NUMBER; i++)
					{
						if ( station_information[0][station_no].effective ==1 )
						{
							strcpy(&send_buf[send_bytes], pstation_info_line3[station_no].station_chinese_name);
							send_bytes += strlen(pstation_info_line3[station_no].station_chinese_name);
							send_buf[send_bytes] = ',';
							send_bytes++;
						}
					}
					send_bytes--;
					send_buf[send_bytes] = 0x00;
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
				}
				else ///< down
				{
					int i = 0;
					for(i=MAX_STATION_NUMBER; i>=station_no; i--)
					{
						if ( station_information[0][station_no].effective ==1 )
						{
							strcpy(&send_buf[send_bytes], pstation_info_line3[station_no].station_chinese_name);
							send_bytes += strlen(pstation_info_line3[station_no].station_chinese_name);
							send_buf[send_bytes] = ',';
							send_bytes++;
						}
					}
					send_bytes--;
					send_buf[send_bytes] = 0x00;
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
					send_buf[5] = (unsigned char)(send_bytes & 0xFF); ///< update real len
					send_buf[6] = (unsigned char)((send_bytes>>8) & 0xFF); ///< update real len
				}
			}
			else if( precv_net_cmd->send_information.event_info_ann.station_information.line_number == 4 )
			{
				station_no = precv_net_cmd->send_information.event_info_ann.station_information.line_number;
				//station_information[1][station_no].effective
			}
			else
			{
				return ;
			}

			BlockBufferWrite(send_video_cmd_tx_buffer_id, (void *)send_buf, send_bytes);
			diag_printf("[vlink, skp]: send_bytes=%ds \n", send_bytes);
		}
		else if( precv_net_cmd->send_information.event_type_ann==TMS_AUTO_ANN_EVENT )
		{
			debug_printf_qc(("CcuSendCmdToVideoLinkage: TMS_AUTO_ANN_EVENT\n"));
		}
		else
		{
			debug_printf_qc(("CcuSendCmdToVideoLinkage: other EAMP's cmd: %d\n",
					precv_net_cmd->send_information.event_type_ann));
		}
	}
	else
	{
		debug_print(("CcuSendCmdToVideoLinkage: not EAMP's cmd,02: %d\n",
				precv_net_cmd->send_information.event_type_ann));
	}

	return ;
}
#endif

void *CcuControlThreadEntry(void *arg)
{
	tms_to_ccu_t tms_cmd_recv_package;
	network_send_package_t recv_network_info_from_network;
	ann_state_param_t  	ann_state_fun_param;
	int loop_count = 0;
	int recv_cmd_from_tms = 0;
	int recv_cmd_from_net = 0;
	int net_cmd_send_buffer = 0;
	int local_cmd_recv_buffer = 0;
	int gd_net_cmd_tx_buffer_id = 0;
	int pa_volume_buffer_id = 0;
	unsigned char ccu_driver_key = 0;
	unsigned char input_event = 0;
	unsigned char new_ann_cmd = 0;
	int delay_alarm_id = 0;
	int delay_alarm_play_id = 0;
	int delay_alarm_end_id = 0;
	int i = 0;
	common_package_t common_package_recv;
	network_shell_package_t recv_shell_from_network;

	recv_cmd_from_net   = BlockBufferOpen("ccu-network-cmd-Rx");
	net_cmd_send_buffer = BlockBufferOpen("ccu-network-cmd-Tx");
	recv_cmd_from_tms = BlockBufferOpen("ccu-tms-cmd-Rx");
	gd_net_cmd_tx_buffer_id = BlockBufferOpen( "network-udp-common-send" );
	ann_state_fun_param.audio_play_control_buffer_id = BlockBufferOpen("ccu-audio-play-cmd-Rx");
	ann_state_fun_param.audio_play_control_buffer_id = BlockBufferOpen("ccu-audio-play-cmd-Rx");
	ann_state_fun_param.net_audio_send_buffer_id = CharBufferOpen("ccu-network-audio-Tx"); ///< add, 12-12
	ann_state_fun_param.local_audio_play_buffer_id = CharBufferOpen("ccu-local-audio-file"); ///< add, 12-12	
	ann_state_fun_param.audio_pending_buffer_id = CharBufferOpen("ccu-local-pending-audio-file"); ///< add, 12-12
	ccu_state.local_audio_file_buffer_id = CharBufferOpen("ccu-local-audio-file");
	ccu_state.local_pending_buffer_id  = CharBufferOpen("ccu-local-pending-audio-file");
	ccu_state.ccu_get_bcu_log_info_buffer_id = BlockBufferOpen("ccu-get-bcu-log-info");
	ccu_state.ccu_recv_audio_data_buffer_id = CharBufferOpen("ccu-network-audio-Rx");
	ccu_state.ccu_send_audio_data_buffer_id = CharBufferOpen("ccu-network-audio-Tx");
	ccu_state.ccu_recv_cmd_buffer_id = BlockBufferOpen("ccu-network-cmd-Rx");
	ccu_state.ccu_send_cmd_buffer_id = BlockBufferOpen("ccu-network-cmd-Tx");
	ccu_state.ccu_ddu_tms_simulate_id = BlockBufferOpen("ccu-ddu-tms-simulate-Rx");	///<ddu simulate add 20140104
	pa_volume_buffer_id = BlockBufferOpen("network-udp-common-dev-vol-recv");

#ifdef CONFIG_CCU_VIDEOLINK	
	int ccu_videolink_cmd_buff_tx = BlockBufferOpen("ccu-videolink-cmd-tx");
	int ccu_videolink_cmd_buff_rx = BlockBufferOpen("ccu-videolink-cmd-rx");
	char videolink_cmd[64];
#endif

	ZhwBufferRegister(BUFFER_TYPE_COMMON_BIG, COMMON_PACKAGE_TYPE_BCU_LOG_INFO, BUFFER_TYPE_RECV_FROM_NET, ccu_state.ccu_get_bcu_log_info_buffer_id);
	ZhwBufferRegister(BUFFER_TYPE_COMMON_NORMAL, COMMON_PACKAGE_TYPE_DEVICE_INFO, BUFFER_TYPE_RECV_FROM_NET, pa_volume_buffer_id);
	ZhwBufferRegister(BUFFER_TYPE_COMMON_NORMAL, COMMON_PACKAGE_TYPE_MONITOR_SPEAKER_CONTROL, BUFFER_TYPE_RECV_FROM_NET, pa_volume_buffer_id);
	ZhwBufferRegister(BUFFER_TYPE_COMMON_NORMAL,COMMON_PACKAGE_TYPE_DDU_SIMULATE,BUFFER_TYPE_RECV_FROM_NET,ccu_state.ccu_ddu_tms_simulate_id);
	ccu_state.ccu_send_to_media_cmd_buffer_id = BlockBufferOpen("ccu-media-cmd-Tx");
	ccu_state.key_status[0] = 0;
	ccu_state.key_status[1] = 0;
	ccu_driver_key = CcuCheckDriverKey();
	CcuSetLocalDevNumberFromDriver();
	ann_state_fun_param.precv_net_cmd = &recv_network_info_from_network;
	ann_state_fun_param.send_net_cmd_buffer_id = net_cmd_send_buffer;
	ann_state_fun_param.src_dev_no = local_ccu_dev_no;
	strcpy(ann_state_fun_param.src_dev_name, "CCU");
	if(ann_state_fun_param.src_dev_no==1)
	{
		ann_state_fun_param.is_db_server = 1;
	}
	else
	{
		ann_state_fun_param.is_db_server = 0;
	}
	ClearSkipStationInfo(3);
	ccu_state.tms_effective_flag = 1;
	unsigned char key_info_online_times = 0;
	unsigned char key_info_offline_times = 0;
	unsigned char train_status = 0;
	CreateEAMPNetworkHeartBetaTimer();

	CreateEAMPToSyncTimer(400);

	CreateManualPackageTimer(200);

	ann_state_fun_param.occ_cmd_flag = 0 ;

	for(;;)
	{

		if(loop_count++ >= 5000)
		{
			loop_count = 0;
//			diag_printf("<2016>CCU,control thread running, occ=%d,%d,%d,%d,%d,%d \n", ccu_read_occ_status(),ann_state_fun_param.occ_cmd_flag,AlarmCheckTimeOut(ccu_state.tcms_to_pa_timer_flag[0]),																				ccu_state.tms_effective_flag,key_info_online_times,key_info_offline_times );
//			diag_printf("ccu_state.is_this_server = %d\n",ccu_state.is_this_server);
//			diag_printf("ccu_state.ccu_info.devices_no = %d,ccu_state.tms_effective_flag = %d\n",ccu_state.ccu_info.devices_no,ccu_state.tms_effective_flag);
//			diag_printf("ccu_state.key_info = %d\n",ccu_state.key_info);
		}
		//������TCMS���������������������������������TCMS���������������������������������PIS������
		SendTCMSNetworkToPA();

		//���������������������������������
		SendSyncSignal();

		if(send_eamp_network_state == 1)
		{//modify 2015-5-25 wilson
			if(tcms_should_running_times <= 100)
			{
				tcms_should_running_times ++;
				if(tcms_should_running_times > 3)
				{
					ccu_state.tms_effective_flag = 0;
				}
			}
//			if((ccu_state.ccu_info.devices_no == 1  && ccu_state.tms_effective_flag == 1 && ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] == 1)||
//					(ZhwGetPrimaryServerNS() == 0 && ccu_state.ccu_info.devices_no == 2) ||
//					(OtherCCUTCMSState() == 0 && ccu_state.ccu_info.devices_no == 2))
//			&& ccu_state.tms_effective_flag == 1
			if((ccu_state.ccu_info.devices_no == 1 /*&& ccu_state.tms_effective_flag == 1 && ccu_state.key_info == 1 */)||
					(ccu_state.ccu_info.devices_no == 2  && ccu_state.tms_effective_flag == 1 && ccu_state.key_info == 2))
			{

				SendHeartBetaSignalToEAMP();
				ccu_state.is_this_server = 1;
				ccu_state.pa_running_info_to_tms[24] |= 0x01 ;
				ccu_state.pcu_state_to_monitor[0] |= 0x01;
				diag_printf("111,%d,,%d,%d\n",ZhwGetPrimaryServerNS(),OtherCCUTCMSState(),ccu_state.ccu_info.devices_no);
			}
			else
			{
				ccu_state.is_this_server = 0;
				ccu_state.pa_running_info_to_tms[24] &= 0xFE;
				diag_printf("222,%d,%d,%d\n",ZhwGetPrimaryServerNS(),OtherCCUTCMSState(),ccu_state.ccu_info.devices_no);
				ccu_state.pcu_state_to_monitor[0] &= 0xFE;
			}

			send_eamp_network_state = 0;
		}
		if(GetHardwareKeyStatus() == 1)
		{
			key_info_online_times = key_info_online_times + 1;
			key_info_offline_times = 0;
		}
		else
		{
			key_info_online_times = 0;
			key_info_offline_times = key_info_offline_times + 1;
		}
		if(key_info_online_times >= 5)
		{
			key_info_online_times = 0;
			GetKeyInfoNew(1);
		}
		else if(key_info_offline_times >= 5)
		{
			key_info_offline_times = 0;
			GetKeyInfoNew(0);
		}
		//		diag_printf("ccu_state.ccu_info.devices_no =%d,ccu_state.key_status[ccu_state.ccu_info.devices_no - 1] =%d\n",ccu_state.ccu_info.devices_no,ccu_state.key_status[ccu_state.ccu_info.devices_no - 1]);
		if(0 &&train_status != (ccu_read_tarin_status() & 0x1f)  )
		{
			train_status = ccu_read_tarin_status() & 0x1f ;
			if(train_status == CCU_30KM_TRUE )
				diag_printf("CCU_30KM_TRUE" );
			if(train_status == CCU_0KM_TRUE )
				diag_printf("CCU_0KM_TRUE \n" );
			if(train_status == CCU_CLOSE_DOOR_TRUE)
				diag_printf("CCU_CLOSE_DOOR_TRUE\n" );
			if(train_status == CCU_LEFT_OPEN_TRUE )
				diag_printf("CCU_LEFT_OPEN_TRUE \n" );
			if(train_status == CCU_RIGHT_OPEN_TRUE)
				diag_printf("CCU_RIGHT_OPEN_TRUE\n" );
			printf("STATUS %X \n",train_status  );
		}

		if(1==ann_state_fun_param.occ_cmd_flag && 1==ccu_read_occ_status() ) ///< OCC valid, tell EAMP
		{
			ann_state_fun_param.occ_cmd_flag = 0;
			strcpy(recv_network_info_from_network.dst_devices_name, "EAMP");
			recv_network_info_from_network.dst_devices_no = MUL_DST_NO;
			strcpy(recv_network_info_from_network.send_information.src_devices_name, "CCU");
			recv_network_info_from_network.send_information.src_devices_no = ann_state_fun_param.src_dev_no;
			recv_network_info_from_network.send_information.event_type_ann = OCC_EVENT;
			recv_network_info_from_network.send_information.event_type_intercom = INTERCOM_IDLE;
			recv_network_info_from_network.send_information.event_info_ann.occ_announce.occ_active = 0;
			recv_network_info_from_network.send_information.event_info_ann.occ_announce.occ_begin_or_over = 0;
			recv_network_info_from_network.send_information.event_info_ann.occ_announce.occ_response = 0;

			BlockBufferWrite(recv_cmd_from_net, (void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network));
			BlockBufferWrite(net_cmd_send_buffer, (void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network));
			strcpy(recv_network_info_from_network.dst_devices_name, "BCU");
			recv_network_info_from_network.dst_devices_no = MUL_DST_NO;
			BlockBufferWrite(net_cmd_send_buffer, (void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network));
			memset((void *)&recv_network_info_from_network, 0, sizeof(recv_network_info_from_network));
			diag_printf("Ccu, control:  send OCC over cmd --> EAMP/BCU \n");
		}
		else if(0==ann_state_fun_param.occ_cmd_flag && 0==ccu_read_occ_status() ) ///< OCC valid, tell EAMP
		{
			ann_state_fun_param.occ_cmd_flag = 1;
			strcpy(recv_network_info_from_network.dst_devices_name, "EAMP");
			recv_network_info_from_network.dst_devices_no = MUL_DST_NO;
			strcpy(recv_network_info_from_network.send_information.src_devices_name, "CCU");
			recv_network_info_from_network.send_information.src_devices_no = ann_state_fun_param.src_dev_no;
			recv_network_info_from_network.send_information.event_type_ann = OCC_EVENT;
			recv_network_info_from_network.send_information.event_type_intercom = INTERCOM_IDLE;
			recv_network_info_from_network.send_information.event_info_ann.occ_announce.occ_active = 1;
			recv_network_info_from_network.send_information.event_info_ann.occ_announce.occ_begin_or_over = 0;
			recv_network_info_from_network.send_information.event_info_ann.occ_announce.occ_response = 0;

			BlockBufferWrite(recv_cmd_from_net, (void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network));
			BlockBufferWrite(net_cmd_send_buffer, (void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network));
			memset((void *)&recv_network_info_from_network, 0, sizeof(recv_network_info_from_network));
			diag_printf("Ccu, control:  send OCC cmd --> EAMP \n");
		}

		if(BlockBufferRead(ccu_state.ccu_cmu_state_buffer_id,(void *)&common_package_recv,sizeof(common_package_recv)) > 0)
		{
			diag_printf("recv cmu request-%d\n",common_package_recv.common_data_u.tcms_cmu_state.get_or_set);
			if(common_package_recv.pkg_type == COMMON_PACKAGE_TYPE_TCMS_CMU_STATE && common_package_recv.common_data_u.tcms_cmu_state.get_or_set == 1 &&
					ccu_state.is_this_server == 1)
			{
				//SendTCMSCMUStateResponsePackage(common_package_recv);
			}
		}

		if(BlockBufferRead(recv_cmd_from_net, (void *)&recv_network_info_from_network, sizeof(recv_network_info_from_network))>0 &&
				strcmp(recv_network_info_from_network.send_information.src_devices_name,"EAMP")!= 0 &&
				recv_network_info_from_network.send_information.src_devices_no <= 12)
		{
			new_ann_cmd = 1;
			CcuDisplayRecvNetPackage( &recv_network_info_from_network );

#ifdef CONFIG_CCU_VIDEOLINK
//					CcuSendCmdToVideoHost(&recv_network_info_from_network, ccu_videolink_cmd_buff_tx);
#endif

			CcuUpdateLocalGdTable(&recv_network_info_from_network.send_information);

			if(recv_network_info_from_network.send_information.event_type_ann == ANN_IDLE &&
					recv_network_info_from_network.send_information.event_type_intercom == INTERCOM_IDLE)
			{
				if(recv_network_info_from_network.send_information.update_tcms_effective_flag == 1 &&
						recv_network_info_from_network.send_information.tcms_effective_flag_info.get_or_set_tcms_effective_flag == 1)
				{
					SendTCMSEffectivateFlagToBCU(tms_effective_cmd,ccu_state.tms_effective_flag);
				}
			}

			UpdatePCURequestLinkage(recv_network_info_from_network);///????PCU??????

			if(recv_network_info_from_network.send_information.event_type_ann == LINE_NUMBER_CHANGE_EVENT)
			{
				diag_printf("%s%d:get_or_set = %d\n",recv_network_info_from_network.send_information.src_devices_name,recv_network_info_from_network.send_information.src_devices_no,recv_network_info_from_network.send_information.event_info_ann.line_info.get_or_set);
				if(recv_network_info_from_network.send_information.event_info_ann.line_info.get_or_set == 1)
				{///< get the current line info
					if(ccu_state.is_this_server == 1)
					{
						SendCurrentCCULineInfo();
					}
				}
				else if(strcmp(recv_network_info_from_network.send_information.src_devices_name,"CCU") == 0)
				{
					CCUUpdateLineInfo(recv_network_info_from_network);
					ShowRegionInfo_diag_printf(ccu_state.current_line_no);
				}
			}
			else if(recv_network_info_from_network.send_information.event_type_ann == MANUAL_ANN_EVENT)
			{
				if(recv_network_info_from_network.send_information.event_info_ann.manual_annnounce.manual_active == 1 &&
						recv_network_info_from_network.send_information.event_info_ann.manual_annnounce.manual_begin_or_over == 1)
				{
					ccu_state.hard_signal_info.hard_signal_current_station_no = recv_network_info_from_network.send_information.event_info_ann.manual_annnounce.current_station_code;
					ccu_state.hard_signal_info.hard_signal_stop_or_leave =  recv_network_info_from_network.send_information.event_info_ann.manual_annnounce.stop_or_leave;
				}
			}
			else if(recv_network_info_from_network.send_information.event_type_ann == PA_KEY_INFORMATION)
			{
				if(strncmp(recv_network_info_from_network.dst_devices_name,"CCU",3) != 0)
				{

				}
				else if(recv_network_info_from_network.send_information.event_info_ann.pa_key_info.get_or_set_key_info == 1)
				{
					//diag_printf("key info:%d\n",recv_network_info_from_network.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.ccu_info.devices_no - 1]);
					if(recv_network_info_from_network.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.ccu_info.devices_no - 1] !=
							ccu_state.key_status[ccu_state.ccu_info.devices_no - 1])
					{
						diag_printf("SendKeyInfo\n");
						SendKeyInfo(ccu_state.key_status[ccu_state.ccu_info.devices_no - 1]);
					}
				}
				else if(recv_network_info_from_network.send_information.event_info_ann.pa_key_info.get_or_set_key_info == 0)
				{
					ccu_state.key_status[ccu_state.other_ccu_no - 1] =
							recv_network_info_from_network.send_information.event_info_ann.pa_key_info.bcu_status[ccu_state.other_ccu_no - 1];
					if(ccu_state.key_status[0] == 1)
					{
						ccu_state.key_info = 1;
					}
					else if(ccu_state.key_status[1] == 1)
					{
						ccu_state.key_info = 2;
					}
					else
					{
						ccu_state.key_info = 0;
					}
				}
			}
			else if(recv_network_info_from_network.send_information.event_type_ann == SKIP_STATION)
			{///<??????
				station_information[recv_network_info_from_network.send_information.event_info_ann.station_information.line_number - 3][recv_network_info_from_network.send_information.event_info_ann.station_information.station_no].effective =
						recv_network_info_from_network.send_information.event_info_ann.station_information.effective;
				SetSkipStationResponseToTCMS(recv_network_info_from_network.send_information.event_info_ann.station_information.line_number,
						recv_network_info_from_network.send_information.event_info_ann.station_information.station_no,
						recv_network_info_from_network.send_information.event_info_ann.station_information.effective);
			}
			else
			{
				if( 0==CcuAnnCmdParse((send_infomation_t *)&(ann_state_fun_param.precv_net_cmd->send_information)) )
				{
					if( 0==ann_state_fun_param.occ_cmd_flag && 1==ccu_read_occ_status() &&
							ann_state_fun_param.precv_net_cmd->send_information.event_type_ann == OCC_EVENT &&
							ann_state_fun_param.precv_net_cmd->send_information.event_info_ann.occ_announce.occ_active==1 )
					{ ///< local OCC is off
						diag_printf("local CCU occ off,but receive OCC111111\n");
					}
					else
					{
						CcuStopCurrentEvent(&ann_state_fun_param);
						CcuStartNewEvent(&ann_state_fun_param);
					}
				}
			}
			//diag_printf("recv_network_info_from_network.send_information.src_devices_name = %s, %d\n",recv_network_info_from_network.send_information.src_devices_name,recv_network_info_from_network.send_information.src_devices_no);
			//diag_printf("recv_network_info_from_network.send_information.src_devices_name = %s, %d\n", recv_network_info_from_network.dst_devices_name,recv_network_info_from_network.dst_devices_no);
#ifdef CONFIG_CCU_VIDEOLINK ///< for pids-skip-station
//			CcuSendCmdToVideoHost(&recv_network_info_from_network, ccu_videolink_cmd_buff_tx);
#endif

		}

		if(1 == ccu_state.is_this_server)
		{
			TransmitBoradcastMessageToMonitor();///<������������������������������,PCU������������
			ReceiveControlMessageFromMonitor();
		}

		CcuProcessCurrentEvent(&ann_state_fun_param);
		if( new_ann_cmd )
		{
			new_ann_cmd = 0;
			memset((void *)&recv_network_info_from_network, 0, sizeof(recv_network_info_from_network));
		}
		if(BlockBufferRead(ccu_state.ccu_ddu_tms_simulate_id, (void *)&common_package_recv, sizeof(common_package_recv))>0)
		{
			SetSimulatedduInfo(common_package_recv);
		}
		if(BlockBufferRead(ccu_state.ccu_intercomm_audio_data_record_recv_buffer_id, (void *)&common_package_recv, sizeof(common_package_recv))>0)
		{
			UpdateIntercommAudioDataRecordingFile(common_package_recv);
		}
		if(BlockBufferRead(pa_volume_buffer_id, (void *)&common_package_recv, sizeof(common_package_recv))>0)
		{
			UpdatePAVolumeInfo(common_package_recv);
		}
		GetCCUDeviceVersionInfo();
#ifdef CONFIG_CCU_VIDEOLINK
		if( BlockBufferRead(ccu_videolink_cmd_buff_rx, (void *)videolink_cmd, sizeof(videolink_cmd))>0)
		{
			CcuDoCmdFromVideoHost(videolink_cmd, ccu_videolink_cmd_buff_tx);
		}
#endif
		if(BlockBufferRead(ccu_state.shell_recv_buffer_id,(void *)&recv_shell_from_network,sizeof(recv_shell_from_network))>0)
		{
			int shell_loop;
			for(shell_loop = 0 ; shell_loop < SHELL_CMD_NUMBER ; shell_loop++)
			{
				if(recv_shell_from_network.cmd == shellcmd[shell_loop].cmd)
				{
					shellcmd[shell_loop].cmd_function(recv_shell_from_network);
					break;
				}
			}
		}
		sched_yield();
	}

	return NULL;
}

void *CCUCanRecvEntry(void *arg)
{
	//add by zhw 2016-1-1
#if 1

	cyg_uint32    len;
	cyg_can_event rx_event;
	//	cyg_can_event rx_event1;
	//	cyg_uint8     i;
	// eamp: 0x180
	//	cyg_can_msgbuf_info    msgbox_info;
	cyg_can_msgbuf_cfg     msgbox_cfg;
	//	cyg_uint8 train_status = 0;
	//
	msgbox_cfg.cfg_id = CYGNUM_CAN_MSGBUF_RESET_ALL;
	len = sizeof(msgbox_cfg);
	//	if (ENOERR != cyg_io_set_config( hCAN_Tbl[0], CYG_IO_SET_CONFIG_CAN_MSGBUF ,&msgbox_cfg, &len))
	//	{
	//		CYG_TEST_FAIL_FINISH("Error resetting message buffer configuration of /dev/can1");
	//	}
	cyg_can_filter rx_filter;


	rx_filter.cfg_id  = CYGNUM_CAN_MSGBUF_RX_FILTER_ADD;
	rx_filter.msg.id  = 0x180 ;
	rx_filter.msg.ext = CYGNUM_CAN_ID_STD;
	//rx_filter.msg.ext = CYGNUM_CAN_ID_EXT;

	len = sizeof(rx_filter);
	//	if (ENOERR != cyg_io_set_config(hCAN_Tbl[0], CYG_IO_SET_CONFIG_CAN_MSGBUF ,&rx_filter, &len))
	//	{
	//		CYG_TEST_FAIL_FINISH("Error writing config of /dev/can1");
	//	}
	//	else if (CYGNUM_CAN_MSGBUF_NA == rx_filter.handle)
	//	{
	//		CYG_TEST_FAIL_FINISH("Error setting up message filter for /dev/can1");
	//	}

	CreatCANTimer();
	while (1)
	{///<FDU ?????can?????????
		sem_wait(&sem_can_recv);
		len = sizeof(rx_event);
		if (ENOERR != cyg_io_read(hCAN_Tbl[0], &rx_event, &len))
		{
			CYG_TEST_FAIL_FINISH("Error reading from channel 1");
		}
		if(rx_event.msg.dlc)
		{
			//			diag_printf(" can1\n");
			//			for(i=0;i<rx_event.msg.dlc;i++)
			//				diag_printf(" %x ", rx_event.msg.data.bytes[i]); //rx_event.msg.data.bytes[%d]  i,
			//			diag_printf(" \ncan1\n");
			fdu_failure_flag = 0;//????���????????????
			ccu_state.ccu_per_dev_failure_info.fdu_failure_info = 0;
		}
	}
	CYG_TEST_PASS_FINISH("CAN rx/tx test OK");
	return NULL;

	//add by zhw 2016-1-1
#endif
}

void *CCUFailureStaticsEntry(void *arg)
{
	debug_print(("I am success entry failure statics handle thread!\n"));
	InitializeHeartBeatPackage();
	udp_heart_socket_buffer_t udp_heart_socket_buffer_info;
	udp_heart_socket_buffer_info.udp_heart_socket_recv_buffer = BlockBufferOpen("network-udp-heart-recv");
	udp_heart_socket_buffer_info.udp_heart_socket_send_buffer = BlockBufferOpen("network-udp-heart-send");
	ZhwDevTestModule(udp_heart_socket_buffer_info);
	return NULL;
}

int main(int argc, char **argv)
{
	//diag_printf("ccu_read_tc_key_in_status = %d\n",ccu_read_tc_key_in_status());
	int return_value_of_thread_create = 0;
	void *return_value_of_join_thread[16];
	int return_mount_romfs = 0;/*the value of mount romfs*/

	pthread_attr_t attr_of_network; 
	pthread_attr_t attr_of_control; 
	pthread_attr_t attr_of_ccu_sync; 		
	pthread_attr_t attr_of_tftp_queue; 	
	pthread_attr_t attr_of_mvb_recv;	
	pthread_attr_t attr_of_tms_cmd;
	pthread_attr_t attr_of_tms_send_cmd;
	pthread_attr_t attr_of_analog_play; ///<ccu play local mp3
	pthread_attr_t attr_of_media_and_monitor_serial_recv;
	pthread_attr_t attr_of_serial_send;
	pthread_attr_t attr_of_media_and_monitor_serial_cmd_parse;
	pthread_attr_t attr_of_failure_statics;
	pthread_attr_t attr_of_key_info;
	pthread_attr_t attr_of_log_info;
	/*Initialize the system*/
	CYG_TEST_INIT();
	if(ReadNum() == 1)
	{
		CCU_TCMS_ADDR1;
	}
	else
	{
		CCU_TCMS_ADDR0;
	}
	return_mount_romfs = MountYaffs2();
	if(return_mount_romfs == -1)
	{
		diag_printf("mount error!\n");
		pthread_exit(NULL);
	}
	diag_printf("mount ok!\n");
	//?????????
	DirCreateOfNandflash();
	diag_printf("after mount ok!-%d-%d\n",sizeof(common_big_package_t),GD_DEV_INFO_MAX_LEN);

	if (CcuBufferInit() < 0 )
	{
		debug_print(("CCU,main:  buff init error,exit main\n"));
		return 1;
	}

	if (SqliteOpen() < 0)
	{
		debug_print(("pu:  open db fail \n"));
		diag_printf("pu:  open db fail \n");
		return -1;
	}


	///<?????????���?????���
	InitGlobalArray();

	strncpy(ccu_state.pa_software_version.software_version,"683-PA-CCU-app-1-20180504",30);
	strncpy(ccu_state.pa_software_version.software_date,"V1.3.0",30);
	//strncpy(ccu_state.pa_software_version.software_version,"QD13-V1.0.2",30);
	//strncpy(ccu_state.pa_software_version.software_date,"664-PA-CCU-app-1-2011114",30);
	//strncpy(ccu_state.pa_software_version.software_version,"SH17-V1.0.22",30);
	//strncpy(ccu_state.pa_software_version.software_date,"638-PA-CCU-app-1-20161114",30);

	strncpy(ccu_state.pa_software_version.db_version_audio,ReadAnnVersion(),30);
	strncpy(ccu_state.pa_software_version.db_version_config,ReadCfgVersion(),30);

	diag_printf("%s\n",ccu_state.pa_software_version.software_version);
	diag_printf("%s\n",ccu_state.pa_software_version.software_date);
	diag_printf("ann:%s\n",ccu_state.pa_software_version.db_version_audio);
	diag_printf("config:%s\n",ccu_state.pa_software_version.db_version_config);

	InitSemaphore(&sem_wakeup_eamp_led);
	InitSemaphore(&sem_can_recv);
	InitSemaphore(&sem_wakeup_test);

	InitSemaphore(&sem_mvb_new_cmd);	

	ReadLocalDeviceVolumeInfo();//

	/*Lookup the sound card */
	LookupSndCard(&ccu_analog_audio_handle);
	OpenSndCard(&ccu_analog_audio_handle, "playback");

#if 1
	CCUSerialInit();//????????

	RS485_EN;
	//	RS485_DIS;

	CCUStateInit();//???????

	InitStationInformation();//??????????

	InitCCUDataBaseInfo();//??????????????????

	InitPCUDeviceState();//?????PCU?????

	GetAnnStationInfo();//???????????
	GetStationDistance();//?????????????????????
	GetDownDistanceInfo();//??????��?????????????????
	ShowDistanceInfo();//??????????

	CreateSYNCTimer();

	int u = 0;
	for(u =1;u <= 15;u++)
	{
		diag_printf("3-%d:%d,%d\n",u,ccu_state.pp_station_info[0][u].open_door_side[0],
				ccu_state.pp_station_info[0][u].open_door_side[1]);
		diag_printf("3-%d:%d,%d\n",u,ccu_state.pp_station_info[0][u].open_door_side[0],
				ccu_state.pp_station_info[0][u].open_door_side[1]);
		//		diag_printf("%d:%d,%d\n",u,ccu_state.ccu_tms_station_diatance[0][u].triger_dia_distance[0],ccu_state.ccu_tms_station_diatance[0][u].triger_dia_distance[1]);
	}
	//	for(u =1;u <= 26;u++)
	//	{
	//		diag_printf("%d:%d,%d\n",4,ccu_state.pp_station_info[1][u].open_door_side[0],ccu_state.pp_station_info[1][u].open_door_side[1]);
	//	}

#endif	

	CcuPriorListInit(); ///< add, 12-21

	CCUControlBufferInit();//ccu??????????

	ccu_state.is_this_server = 1;
	//    SqliteClose();

	if(db_err_status == 1)
	{
		ccu_state.ccu_info.devices_no = 200;
	}


	ThreadAttributionConfigure( &attr_of_network, CCU_PRIORIT,
			&thread_stack_of_network[sizeof(thread_stack_of_network)], sizeof(thread_stack_of_network));

	ThreadAttributionConfigure( &attr_of_control, CCU_PRIORIT,
			&thread_stack_of_control[sizeof(thread_stack_of_control)], sizeof(thread_stack_of_control));

	ThreadAttributionConfigure( &attr_of_ccu_sync, CCU_PRIORIT,
			&thread_stack_of_ccu_sync[sizeof(thread_stack_of_ccu_sync)], sizeof(thread_stack_of_ccu_sync));

	ThreadAttributionConfigure( &attr_of_tftp_queue, CCU_PRIORIT,
			&thread_stack_of_tftp_queue[sizeof(thread_stack_of_tftp_queue)], sizeof(thread_stack_of_tftp_queue));

	ThreadAttributionConfigure( &attr_of_mvb_recv, CCU_PRIORIT+3,
			&thread_stack_of_mvb_recv[sizeof(thread_stack_of_mvb_recv)], sizeof(thread_stack_of_mvb_recv));
	ThreadAttributionConfigure( &attr_of_tms_cmd, CCU_PRIORIT+4,
			&thread_stack_of_tms_cmd[sizeof(thread_stack_of_tms_cmd)], sizeof(thread_stack_of_tms_cmd));

	ThreadAttributionConfigure( &attr_of_tms_send_cmd, CCU_PRIORIT,
			&thread_stack_of_send_tms_cmd[sizeof(thread_stack_of_send_tms_cmd)], sizeof(thread_stack_of_send_tms_cmd));

	ThreadAttributionConfigure( &attr_of_analog_play, CCU_PRIORIT,
			&thread_stack_of_analog_sample[sizeof(thread_stack_of_analog_sample)], sizeof(thread_stack_of_analog_sample));

	ThreadAttributionConfigure( &attr_of_serial_send, CCU_PRIORIT+2,
			&thread_stack_of_serial_send[sizeof(thread_stack_of_serial_send)], sizeof(thread_stack_of_serial_send));

	ThreadAttributionConfigure( &attr_of_media_and_monitor_serial_recv, CCU_PRIORIT,
			&thread_stack_of_media_and_monitor_serial_recv[sizeof(thread_stack_of_media_and_monitor_serial_recv)], sizeof(thread_stack_of_media_and_monitor_serial_recv));

	ThreadAttributionConfigure( &attr_of_media_and_monitor_serial_cmd_parse, CCU_PRIORIT+5,
			&thread_stack_of_media_and_monitor_serial_cmd_parse[sizeof(thread_stack_of_media_and_monitor_serial_cmd_parse)], sizeof(thread_stack_of_media_and_monitor_serial_cmd_parse));

	ThreadAttributionConfigure( &attr_of_failure_statics, CCU_PRIORIT,
			&thread_stack_of_failure_statics[sizeof(thread_stack_of_failure_statics)], sizeof(thread_stack_of_failure_statics));

	ThreadAttributionConfigure( &attr_of_key_info, CCU_PRIORIT+1,
			&thread_stack_of_key_info[sizeof(thread_stack_of_key_info)], sizeof(thread_stack_of_key_info));

	ThreadAttributionConfigure( &attr_of_log_info, CCU_PRIORIT,
			&thread_stack_of_log_info[sizeof(thread_stack_of_log_info)], sizeof(thread_stack_of_log_info));

	return_value_of_thread_create = pthread_create(&thread_of_network,
			&attr_of_network,
			CcuNetworkThreadEntry,
			NULL);
	return_value_of_thread_create = pthread_create(&thread_of_control,
			&attr_of_control,
			CcuControlThreadEntry,
			NULL);
	return_value_of_thread_create = pthread_create(&thread_of_ccu_sync,
			&attr_of_ccu_sync,
			CcuSyncThreadEntry,
			NULL);
	return_value_of_thread_create = pthread_create(&thread_of_tftp_queue,
			&attr_of_tftp_queue,
			CcuTftpQueueThreadEntry,
			NULL);
	return_value_of_thread_create = pthread_create(&thread_of_tms_cmd,
			&attr_of_tms_cmd,
			CcuTmsHandleThreadEntry,
			NULL);
	return_value_of_thread_create =	pthread_create(&thread_of_analog_sample,
			&attr_of_analog_play,
			CcuAnalogSampleThreadEntry,
			NULL);

	return_value_of_thread_create = pthread_create(&thread_of_serial_send,///<led & tvb
			&attr_of_serial_send,
			FDUCanSendThreadEntry,
			NULL);
	return_value_of_thread_create = pthread_create(&thread_of_media_and_monitor_serial_recv,
			&attr_of_media_and_monitor_serial_recv,
			MediaAndMonitorSerialRecvThreadEntry,
			NULL);
	return_value_of_thread_create = pthread_create(&thread_of_media_and_monitor_serial_cmd_parse,
			&attr_of_media_and_monitor_serial_cmd_parse,
			MediaAndMonitorSerialCMDParseThreadEntry,
			NULL);

	return_value_of_thread_create = pthread_create(&thread_of_failure_statics,
			&attr_of_failure_statics,
			CCUFailureStaticsEntry,
			NULL);

	return_value_of_thread_create = pthread_create(&thread_of_key_info,
			&attr_of_key_info,
			CCUCanRecvEntry,///<can recv
			NULL);

	return_value_of_thread_create = pthread_create(&thread_of_log_info,
			&attr_of_log_info,
			CCULogInfoEntry,
			NULL);

	/*Wait the end of thread*/
	pthread_join(thread_of_network, &return_value_of_join_thread[0] );
	pthread_join(thread_of_control, &return_value_of_join_thread[1] );
	pthread_join(thread_of_tftp_queue, &return_value_of_join_thread[2] );
	pthread_join(thread_of_ccu_sync, &return_value_of_join_thread[3] );
	pthread_join(thread_of_tms_cmd, &return_value_of_join_thread[4] );
	pthread_join(thread_of_analog_sample, &return_value_of_join_thread[5] );
	pthread_join(thread_of_serial_send, &return_value_of_join_thread[6] );
	//	 pthread_join(thread_of_media_and_monitor_serial_recv, &return_value_of_join_thread[7] );
	pthread_join(thread_of_media_and_monitor_serial_cmd_parse, &return_value_of_join_thread[8] );
	pthread_join(thread_of_failure_statics, &return_value_of_join_thread[9] );
	pthread_join(thread_of_key_info, &return_value_of_join_thread[10] );
	pthread_join(thread_of_log_info, &return_value_of_join_thread[11] );
	///<160406 wds	 pthread_join(thread_of_mvb_recv, &return_value_of_join_thread[12] );

	debug_print(("CCU:  main thread exit\n"));

	return 0;
}

