/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： file_operate.h
 * 文件标识：
 * 摘    要： 文件操作头文件
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期： 2014-03-03
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef FILE_OPERATE_H_
#define FILE_OPERATE_H_

#include <stdio.h>
#include <unistd.h>

#define FS_Print
#ifndef FS_Print
   #define debug_fs_print(s)
#else
   #define debug_fs_print(s) //diag_printf s
#endif

extern FILE *fp_recording;
extern FILE *fp_bcu1_log_info;
extern FILE *fp_bcu2_log_info;
extern FILE *fp_intercomm_audio_info;

/** CreateNewDir function description
 *  This function is used to creat new direction
 *
 *  @param   the route of new floder
 *  @param   the mode of creat
 *  @return  if success return 0
 */
int CreateNewFloder(char *new_folder,int amode);

/** OpenNewFile function description
 *  This function is used to open new file
 *
 *	@param   the file pointer
 *  @param   the route of new file
 *  @param   the mode of file open
 *  @return  if success return 0
 */
int OpenNewFile(FILE  **fpp,char *new_file,char *fiel_mode);

/** CloseFile function description
 *  This function is used to close new file
 *
 *  @param   the file pointer
 *  @return  if success return 0
 */
int CloseFile(FILE *fp);

/** WriteAudioDataToFile function description
 *  This function is used to write audio data to file
 *
 *  @param   the file pointer
 *  @param   需要写入的信息
 *  @param   需要写入信息的长度
 *  @return  if success return 0
 */
int WriteAudioDataToFile(FILE *fp, const unsigned char *str,unsigned int buffer_len);

/** WriteTCMSLogToFile function description
 *  This function is used to write tcms log to file
 *
 *  @param   the file pointer
 *  @param   需要写入的信息
 *  @param   需要写入信息的长度
 *  @return  if success return 0
 */
int WriteTCMSLogToFile(FILE *fp, const unsigned char *str,unsigned int buffer_len);

/** ReadFileSize function description
 *  This function is used to read the size  of audio data
 *
 *  @param   the file path
 *  @return  if file has been existed return the size of this file,otherwise retuen -1
 */
int ReadFileSize(char *file_path);

/** ShowFileContest function description
 *  This function is used to show the content of file
 *
 *  @param   the file path
 *  @return  if  ok return 0,otherwise retuen -1
 */
int ShowFileContest(char *file_path);

/** ReadLocalFileToBuffer function description
 *  读取本地文件到缓存区中
 *
 *  @param   buffer id
 *  @return  if  ok return 0,otherwise retuen -1
 */
int ReadLocalFileToBuffer(const char *file_path,int buffer_id);

/** WriteLogInfoToFile function description
 *  将错误信息写入到文件
 *
 *  @param   文件描述符
 *  @param   内容
 *  @param   长度
 *  @return  if  ok return 0,otherwise retuen -1
 */
int WriteLogInfoToFile(FILE *fp, const unsigned char *str,unsigned int buffer_len);

/** FileExist function description
 *  判断文件是否存在
 *
 *  @param   文件路径
 *  @return  if  ok return 0,otherwise retuen -1
 */
int FileExist(char *file_path);

#endif /* FILE_OPERATE_H_ */
