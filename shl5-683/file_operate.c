/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： file_operate.c
 * 文件标识：
 * 摘    要： 文件操作主函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期： 2014-03-03
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "file_operate.h"
#include "struct_type.h"
FILE *fp_recording = NULL;
FILE *fp_read_recording_file;
FILE *fp_bcu1_log_info = NULL;
FILE *fp_bcu2_log_info = NULL;
FILE *fp_intercomm_audio_info = NULL;

int CreateNewFloder(char *new_folder,int amode)
{
	if(access(new_folder,0) == -1)
	{
		if(mkdir(new_folder,amode))
		{
			debug_fs_print(("create dir :%s is fail !!!\n",new_folder));
			perror("the error code is :\n");
			return -1;
		}
		else
		{
			debug_fs_print(("create new dir : %s is OK !!!\n",new_folder));
		}
		return 0 ;
	}
	else
	{
		debug_fs_print(("the dir : %s is exists !!!\n",new_folder));
	}
	return 0 ;
}

int OpenNewFile(FILE  **fpp,char *new_file,char *fiel_mode)
{
	return 0 ;
	if(*fpp != NULL)
	{
		fclose(*fpp);
		*fpp = NULL;
	}
//	if(access(new_file,F_OK) == -1)
	if(1)
	{
		*fpp = fopen(new_file,fiel_mode);
		if(*fpp == NULL)
		{
			debug_fs_print(("fopen file :%s is fail !!!\n",new_file));
			perror("the error code is :\n");
			return -1;
		}
		else
		{
			debug_fs_print(("fopen new file : %s is OK !!!\n",new_file));
		}
		return 0 ;
	}
	else
	{
		debug_fs_print(("the file : %s is exists !!!\n",new_file));
	}
	return 0 ;
}


int CloseFile(FILE *fp)
{
	return fclose(fp);
}
int WriteAudioDataToFile(FILE *fp, const unsigned char *str,unsigned int buffer_len)
{
	return 0;
	int ret = -1;
	ret = fwrite(str, sizeof(char), buffer_len, fp);
	fs_debug_print(("WriteAudioDataToFile :ret = %d\n",ret));
	if(ret >= 0)
	{
		fflush(fp);
//		ret = 0;
	}
	else
	{
		ret =  -1;
	}
	return ret;
}

int WriteTCMSLogToFile(FILE *fp, const unsigned char *str,unsigned int buffer_len)
{
	int ret = -1;
	ret = fwrite(str, sizeof(char), buffer_len, fp);
	if(ret >= 0)
	{
		fflush(fp);
	}
	else
	{
		ret =  -1;
	}
	return ret;
}

int ReadFileSize(char *file_path)
{
	unsigned long filesize = -1;
	FILE *fpq;
	fpq = fopen(file_path, "r");
	if(fpq != NULL)
	{
		fseek(fpq, 0L, SEEK_END);
		filesize = ftell(fpq);
		fclose(fpq);
	}
	fs_debug_print(("filesize = %d\n",filesize));
	return filesize;
}

int ShowFileContest(char *file_path)
{
	FILE *fpq;
	unsigned char str[100];
	fpq = fopen(file_path, "r");
	if(fpq == NULL)
	{
		return -1;
	}
	fgets(str,100,fpq);
	int i = 0;

	for(;i < 100;i++)
	{
		fs_debug_print(("%d:%02X ",i,str[i]));
	}
	fclose(fpq);
	return 0;
}


int ReadRecordingFile(char *file_path,unsigned int buffer_id,int flag)
{
	if(flag == 0)
	{
		fp_read_recording_file = fopen(file_path, "r");

		if(fp_read_recording_file == NULL)
		{
			fs_debug_print(("open file %s failed\n",file_path));
			return -1;
		}
		fs_debug_print(("open file %s success\n",file_path));
	}
	if(fp_read_recording_file != NULL)
	{
		if(!feof(fp_read_recording_file))
		{
			int read_len = 0;
			char tmp_buffer[1024];
			read_len = fread(tmp_buffer, sizeof(char), sizeof(tmp_buffer), fp_read_recording_file);
			if(read_len > 0)
			{
				CharBufferWrite(buffer_id, tmp_buffer, read_len);
			}
			fs_debug_print(("read_len = %d\n",read_len));
		}
	}
	else
	{
		fclose(fp_read_recording_file);
		fp_read_recording_file = NULL;
	}
	return 0;
}

int ReadLocalFileToBuffer(const char *file_path,int buffer_id)
{
	FILE *fp;
	int ret=-1;
	unsigned char temp_buffer[512];
	int read_len = 0;
	fp = fopen(file_path, "r");

	if(fp == NULL)
	{
		fs_debug_print(("open file %s failed\n",file_path));
		return -1;
	}
	while(!feof(fp))
	{
		read_len = fread(temp_buffer, sizeof(char), sizeof(temp_buffer), fp);
		fs_debug_print(("read_len = %d\n",read_len));
		if(read_len > 0)
		{
			CharBufferWrite(buffer_id, (const void *)temp_buffer, read_len);
		}
	}
	fclose(fp);
	return ret;
}

int WriteLogInfoToFile(FILE *fp, const unsigned char *str,unsigned int buffer_len)
{
	int ret = -1;
	ret = fwrite(str, sizeof(char), buffer_len, fp);
	if(ret >= 0)
	{
		fflush(fp);
	}
	else
	{
		ret =  -1;
	}
	return ret;
}

int FileExist(char *file_path)
{
	int ret = 0;
	if(access(file_path,F_OK) == -1)
	{
		ret = 0;
	}
	else
	{
		ret = 1;
	}
	return ret;
}
