/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称：zhw_apply_common_func.c
*文件标识：应用层共用的函数
*摘  要：将一些应用层读需要用的函数统一放置。
*接受参数：无
*返回数据：
*
*当前版本：00.00.01
*作  者：周魏
*完成日期：
*
*取代版本：原版本号
*原作者 ：
*完成时间：
*/
#include "zhw_apply_common_func.h"

/*
 *   	@brief	本函数的功能是:为应用层创建buffer；
 *     @param buffer_type 创建的buffer的类型（1表示block buffer，2表示char buffer）
 *     @param buffer_number 需要创建的应用层的包数量(当buffer_type 为1时有效，为2时可以不用输入为0即可)
 *     @param buffer_size 应用层创建的buffer大小
 *     @param buffer_name 应用层创建的buffer名称
 *     @return 创建成功返回buffer id 号，创建失败返回 -1；需要创建的buffer类型有误返回 -2；
 */
int ZhwApplyBufferCreate(int buffer_type,int buffer_number,int buffer_size,char *buffer_name)
{
	int ret_buffer_id = 0 ;
	switch(buffer_type)
	{
	case 1:
		ret_buffer_id = BlockBufferCreate(buffer_number, buffer_size, buffer_name);//创建block buffer
		if (0 == ret_buffer_id )
		{
			net_debug_printf(("create %s error\n",buffer_name));
			return -1;
		}
		break;
	case 2:
		ret_buffer_id = CharBufferCreate( buffer_size, buffer_name );//创建char buffer
		if (0 == ret_buffer_id )
		{
			net_debug_printf(("create %s error\n",buffer_name));
			return -1;
		}
		break;
	default:
		net_debug_printf(("please check the 'buffer_type' which of your import,the right number must be 1 or 2 !!!\n"));
		return -2 ;
		break;
	}

	return ret_buffer_id;
}

