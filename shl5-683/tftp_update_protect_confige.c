/*
 * tftp_update_protect_confige.c
 *
 *  Created on: 2013-4-16
 *      Author: benty
 */

#include <stdio.h>
#include "tftp_update_protect_confige.h"
#include <errno.h>



//读取需要修改行的信息。以行为单位,
int GetFileLine(FILE *stream, void *ptoline, int get_len, int flage)
{
	while(flage--)
	{
		//如果读出的数据大于get_len，getline()函数会自己重新申请更大的空间，但是会影响效率。
		if (fgets((char *)ptoline, get_len, stream) < 0)//fgets、getline函数移动文件指针
		{
			//在调用getline获取ptoline3之前调用feof(stream)函数，是不会显示到达了文件尾的。
			if (feof(stream))
			{
				tftp_print( ("end of the file\n") );
				break;
			}
			else
			{
				tftp_print( ("wrong at GetFileLine-000\n") );
				fclose( stream );
				return -1;
			}
		}

	}

	return 0;
}

//移动文件指针,主要用于动作的文件指针与前面的动作的文件指针不相同.
//特殊的一行为单位,并且每行字数相同.
int FseekFileFP(FILE *stream, char *ptoline, int flage)
{
	if (1 == flage)
	{
		if (fseek(stream, 0, 0) != 0)
		{
			tftp_print( ("error on fseek1\n") );
			return -1;
		}
	}
	else
	{
		if (fseek(stream, 0, 0) != 0)
		{
			tftp_print( ("error on fseek1\n") );
			return -1;
		}
		while(--flage)
		{
			if (fseek(stream, strlen(ptoline), 0) != 0)
			{
				tftp_print( ("error on fseek1\n") );
				return -1;
			}
		}
	}

	return 0;
}

//将数据写向硬件中的存储位子.
int WriteToFileToDisk(FILE *stream , char *ptoline )
{
	int fp_id = 0;
	int ret = 0;

	if ((ret = fwrite(ptoline, strlen(ptoline), 1, stream)) < 0)
	{
		tftp_print( ("error on fwirte fun \n") );
		return -1;
	}
	//将内存缓存区内容同步内核缓存区。
	if (fflush(stream) == EOF )//	fsync();sync();函数不适用
	{
		tftp_print( ("error on fflush fun \n") );
		return -1;
	}
	//FILE *文件指针转换为int类型。
	if ((fp_id = fileno(stream)) < 0)//支持
	{
		tftp_print( ("error on the fileno fun\n") );
		return -1;
	}
	//将内核缓存区写道磁盘.
	if (fsync(fp_id) < 0)//支持
	{
		tftp_print( ("error on the fsync fun\n") );
		return -1;
	}

	return 0;
}
//具体的读,写动作.
int ModifyFileActionEntry(FILE *stream, int want_chang_file, char want_chang_mode)
{
	char *ptoline = NULL;
	char *p_modify_place = NULL;
	//如果读出的数据大于200，getline函数会自己重新申请更大的空间，但是会影响效率。
	int get_len = MAX_LINE_SIZE;

	tftp_print( ("ModifyFileActionEntry--111\n") );
	ptoline = LocalTftpMalloc(get_len + 1);
	tftp_print( ("ModifyFileActionEntry--2222\n") );
	//把want_chang_file当作覆盖读取的次数，最后一次结果，就是要读取的内容。
	if (GetFileLine(stream, ptoline, get_len, want_chang_file) < 0)//fgets、getline函数移动文件指针
	{
		tftp_print( ("wrong at GetFileLine--111\n") );
		return -1;
	}

	tftp_print( ("ModifyFileActionEntry--3333\n") );
	//返回的指针指向查找到的字符串的开头点。
	p_modify_place = strstr(ptoline, FIND_STATE_STRING);//返回的指针指向查找到的字符串的开头点。
																											//如果找不到，则认为配置文件出错。
	if(NULL == p_modify_place)
	{
		tftp_print( ("dont find the string in the config file\n") );
		return -1;
	}

	tftp_print( ("ModifyFileActionEntry--4444\n") );

	//“-1”指向匹配字符串尾，“+1”指向匹配字符串之后的字符串
	p_modify_place = p_modify_place + strlen(FIND_STATE_STRING) - 1 + 1;

	tftp_print( ("ModifyFileActionEntry--555\n") );
	//仅修改当前字符。
	*p_modify_place = want_chang_mode;

	tftp_print( ("ModifyFileActionEntry--6666\n") );

	//移动文件指针,到需要修改的位置.
	if (FseekFileFP(stream,ptoline,want_chang_file) < 0)
	{
		tftp_print( ("error on FseekFileFP fun\n") );
		return -1;
	}

	tftp_print( ("ModifyFileActionEntry--7777\n") );

	//将数据写向buffer,并强制刷新到disk上.
	WriteToFileToDisk(stream ,ptoline);

	tftp_print( ("ModifyFileActionEntry--888\n") );
	LocalTftpFree(ptoline);

	tftp_print( ("ModifyFileActionEntry--9999\n") );
	return 0;
}

//main enter.roben_ye.
//主要还是在做初始化的工作.修改配置文件标志状态
int ConfigFileModify(char *config_file_path, int want_chang_file, int want_chang_mode)
{

	if ( (want_chang_mode <= start_modify_state) || (want_chang_mode >= end_modify_state) )
	{
		tftp_print( ("no such state we can change\n") );
		return -1;
	}
	if ( (want_chang_file >= end_modify_file) || (want_chang_file <= start_modify_file) )
	{
		tftp_print( ("no such file to modify\n") );
		return -1;
	}

	tftp_print( ("ConfigFileModify---1111\n") );
	//"r+",文件必须已经存在
	//同时是指向哪儿就覆盖哪儿，覆盖长度仅为字符串长度
	tftp_print( ("fopen config file:%s\n",config_file_path) );
	FILE *stream = fopen(config_file_path, "r+");
	tftp_print( ("ConfigFileModify---0000\n") );
	if (stream == NULL)
	{
		tftp_print( ("erro on fopen-ConfigFileModify:%d\n",errno) );
		return -1;
	}

	tftp_print( ("ConfigFileModify---2222\n") );
	if (ModifyFileActionEntry(stream, want_chang_file, want_chang_mode) < 0)
	{
		tftp_print( ("error on ModifyFileEntry FILE_ONE\n") );
		fclose(stream);
		stream = NULL;
		return -1;
	}

	tftp_print( ("ConfigFileModify---3333\n") );
	fclose(stream);
	stream = NULL;

	tftp_print( ("ConfigFileModify-----end\n") );

	return 0;
}

//读取想要读取行的状态内容.
int ReadConfigFileState(char *config_file_path, int want_read_file)
{
	if ( (want_read_file >= end_modify_file) || (want_read_file <= start_modify_file) )
	{
		tftp_print( ("no such file to modify\n") );
		return -1;
	}

	int ret = 0;
	FILE *stream = fopen(config_file_path, "r");
	if (stream == NULL)
	{
		tftp_print( ("erro on fopen a file--ReadConfigFileState \n") );
		return -1;
	}

	if ( (ret = FindFileState(stream, want_read_file, FIND_STATE_STRING)) < 0)
	{
		tftp_print( ("error on ModifyFileEntry FILE_ONE\n") );
		return -1;
	}

	fclose(stream);
	stream = NULL;
	return ret;
}
//#if 1
//char *config_file_path, int want_read_file
//char *save_file_name,必须为已经malloc的空间，否者不能存储信息。
//int ReadConfigFilePath(char *config_file_name, char *save_file_name, int want_read_file)
//{
//	if ( (want_read_file >= end_modify_file) || (want_read_file <= start_modify_file) )
//	{
//		printf("no such file to modify\n");
//		return -1;
//	}
//
//	int ret = 0;
//	FILE *stream = fopen(config_file_name, "r");
//	if (stream == NULL)
//	{
//		printf("erro on fopen a file \n");
//		return -1;
//	}
//
//	if ( (ret = FindFilePath(stream, want_read_file, FIND_FILE_STRING, save_file_name)) < 0)
//	{
//		perror("error on ModifyFileEntry FILE_ONE\n");
//		return -1;
//	}
//
//	fclose(stream);
//	stream = NULL;
//	return ret;
//}
//
//#endif
//查询文件路径。
//int FindFilePath(FILE *stream, int want_read_file, char *find_string, char *save_file_name)
//{
//	int ret = 0;
//	char *ptoline = NULL;
//	char *p_modify_place = NULL;
//	char *p_end_palace = NULL;
//	char *p_save_file = save_file_name;
//
//	//如果读出的数据大于200，getline函数会自己重新申请更大的空间，但是会影响效率。
//	int get_len = MAX_LINE_SIZE;
//
//	ptoline = LocalTftpMalloc(get_len + 1);
//
//	//把want_chang_file当作覆盖读取的次数，最后一次结果，就是要读取的内容。
//	if ( GetFileLine(stream, ptoline, get_len, want_read_file) < 0)//fgets、getline函数移动文件指针
//	{
//		tftp_print( ("wrong at fgets ptoline--111\n") );
//		return -1;
//	}
//
////	printf("ptoline:%s\n",ptoline);
//
//	//返回的指针指向查找到的字符串的开头点。
//	p_modify_place = strstr(ptoline, find_string);//返回的指针指向查找到的字符串的开头点。
//
//	//“-1”，指向匹配字符串尾，“+1”指向匹配字符串之后的字符串
//	p_modify_place = p_modify_place + strlen(find_string) - 1 + 1;
//
//	//add,查找文件名的末尾位置。
//	p_end_palace =  strstr(ptoline, FIND_END_STRING);
//
//	//复制文件路径，到参数。
//	while(p_modify_place != p_end_palace)
//	{
//		strncpy(p_save_file,p_modify_place,1);
//		p_modify_place++;
//		p_save_file++;
//	}
//
////	printf("save_file_name:%s\n",save_file_name);
//
//	LocalTftpFree(ptoline);
//
//	return ret;
//}

//查询文件状态信息。
int FindFileState(FILE *stream, int want_read_file, char *find_string)
{
	int ret = 0;
	char *ptoline = NULL;
	char *p_modify_place = NULL;
	//如果读出的数据大于200，getline函数会自己重新申请更大的空间，但是会影响效率。
	int get_len = MAX_LINE_SIZE;

	ptoline = LocalTftpMalloc(get_len + 1);

	//把want_chang_file当作覆盖读取的次数，最后一次结果，就是要读取的内容。
	if ( GetFileLine(stream, ptoline, get_len, want_read_file) < 0)//fgets、getline函数移动文件指针
	{
		tftp_print( ("wrong at fgets ptoline--111\n") );
		return -1;
	}

	//返回的指针指向查找到的字符串的开头点。
	p_modify_place = strstr(ptoline, find_string);//返回的指针指向查找到的字符串的开头点。
																												//如果找不到，则认为配置文件出错。
	if(NULL == p_modify_place)
	{
		tftp_print( ("dont find the string in the config file\n") );
		return -1;
	}

	//“-1”指向匹配字符串尾，“+1”指向匹配字符串之后的字符串
	p_modify_place = p_modify_place + strlen(find_string) - 1 + 1;

	//仅修改当前字符。
	ret = *p_modify_place;

	LocalTftpFree(ptoline);

	return ret;
}
