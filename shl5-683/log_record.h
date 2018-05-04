/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： log_record.h
 * 文件标识：
 * 摘    要：The head file of log record
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-11
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef LOG_RECORD_H_
#define LOG_RECORD_H_

#include <stdio.h>
#include "data_struct.h"

#define LOG_NUMBER 500


typedef struct
{
	int tv_sec_begin;
	int tv_usec_begin;
	int tv_sec_over;
	int tv_usec_over;
	int len_of_read_audio_data;
}log_time_t;

typedef struct
{
	log_time_t log_time;
	int first_free_number;
}log_mic_simple_t;

typedef struct
{
	unsigned char event_no;
	char event_name[50];
}mapping_table_t;

extern mapping_table_t mapping_table[256];

/** pexit function description
 *	It belongs to BCU
 *	Function:Initial the log,which is used to Print error message
 *
 *  @param   the string of error message
 *  @return  null
 */
//void pexit(char *s);

/** InitLogTime function description
 *  It belongs to BCU
 *	  Function:Initial the log,which is used to record time
 *
 *  @param   the array of the log
 *  @return  if create successfully return 0
 */
int InitLogTime(log_mic_simple_t log_mic_simple[]);

/** AddNewDataToLogTime function description
 *  It belongs to BCU
 *	  Function:Add new data into the array of log
 *
 *  @param   the array of the log
  *  @param   the type of the time,0:before 1:after
 *  @param   the second of the time
 *  @param   the seconds of the time
 *  @param   the length of audio data
 *  @return  if create successfully return 0
 */
int AddNewDataToLogTime(log_mic_simple_t log_mic_simple[],int type,int tv_sec,int tv_usec,int audio_data_length);

/** ShowDataOfLogTime function description
 *  It belongs to BCU
 *	  Function:show the data,which is in the array of log
 *
 *  @param   the array of the log
 *  @return  if create successfully return 0
 */
int ShowDataOfLogTime(log_mic_simple_t log_mic_simple[]);

/** DisplaySendCmd function description
 *	  Function:show the detail of cmd information
 *
 *  @param   cmd package
 *  @return  if show successfully return 0,else return -1
 */
int DisplaySendCmd(send_infomation_t  param_cmd_info);

/** DisplayNetworkCmd function description
 *	  Function:show the detail of cmd information
 *
 *  @param   cmd package
 *  @return  if show successfully return 0,else return -1
 */
int DisplayNetworkCmd(network_send_package_t  param_cmd_info);

/** InitMappingTable function description
 *	  Function:initial the mapping table
 *
 *  @param   cmd package
 *  @return  null
 */
void InitMappingTable();

/** WriteNetworkErrorToLog function description
 *	  Function:写错误信息到log
 *
 *  @param  错误信息
 *  @return  null
 */
void WriteNetworkErrorToLog(char *param_error_tip_info);
#endif /* LOG_RECORD_H_ */
