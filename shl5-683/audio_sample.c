/*
 * audio_sample.c
 *
 *  Created on: 2013-11-21
 *      Author: benty
 */

#include "audio_sample.h"

int PlayAudioTwice(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id)
{///<????,??OCC
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
	if( current_total_bytes <=  border_of_audia_data_size )
	{
		//debug_print(("PlayAudioTwice:  2 buffer < 2K\n",current_total_bytes));
		return -5;
	}

	if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )
	{
		 debug_print(("PlayAudio: before bwrite, buffer cleared: %d,so return -1\n",current_total_bytes));
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
		   debug_print(("PlayAudioTwice:  read pending=%d, This is unlikely\n", ret));
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
//				debug_print(("PlayAudioTwice:  read main=%d, This is unlikely\n", ret));
//				return -8;
			}
		}

		if( total_read_len < border_of_audia_data_size)
		{
			  debug_print(("PlayAudioTwice:  total(%d) < 2KiB, This is unlikely\n", total_read_len));
			  return -6;
		}
		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) )///<2013-11-11
		{
			 debug_print(("PlayAudio: before bwrite, buffer cleared-02\n"));
			 return -1;
		}
//		if( pending_buffer_clear_flag != CharBufferCheckClearFlag(pending_audio_buffer_id) )
//		{
//			 debug_print(("PlayAudioTwice: pending buffer cleared-01\n"));
//			 return -1;
//		}

		//debug_print(("eamp, playTwice:  play total=%d \n", total_read_len));
		len = (cyg_uint32)total_read_len;
		len_backup = len;
		snd_set_local = (1<<8) | CYG_SND_AUDIO_STREAM;

//		if(begin_to_broadcast_audio_data == 1)
		{
			debug_print(("Before cyg_io_bwrite,%d\n",len));
			ret_bwrite = cyg_io_bwrite( audio_handle, (cyg_uint8 *)tmp_buff, &len, snd_set_local);
			debug_print(("Before cyg_io_bwrite\n"));
		}


		if( buffer_clear_flag != CharBufferCheckClearFlag(audio_source_buffer_id) ) //add, 2013
		{
			debug_print(("PlayAudioTwice: after bwrite, buffer cleared,so return -1\n"));
			 return -1;
		}

		debug_print(("eamp: bw=%d, left=%d,%d,dec=%d\n", ret_bwrite, len, len_backup,len_backup - len));

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
