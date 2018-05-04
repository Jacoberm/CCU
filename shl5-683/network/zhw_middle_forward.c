/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_middle_forward.c
 * 文件标识：
 * 摘    要： 通用数据包转发程序
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2013-03-26
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */
#include "zhw_middle_forward.h"

dev_status_info_t dev_status_info;

int ZhwNetworkRecvForward(int recv_buffer_id,int buffer_type)
{
	int low_recv_buffer;
	int low_buffer_type = 0 ;
	low_recv_buffer = recv_buffer_id;
	low_buffer_type = buffer_type;
	int apply_recv_buffer;
	common_package_t forward_recv_buffer;
	int low_recv_big_buffer = recv_buffer_id;
	common_big_package_t forward_recv_big_buffer;

	switch(low_buffer_type)
	{
	case 1:
		if(BlockBufferRead(low_recv_buffer,(void *)&forward_recv_buffer,sizeof(forward_recv_buffer)) > 0)
		{
			net_debug_printf(("the forward receive data from the low network buffer is ok!!!\n"));

			if((apply_recv_buffer = ZhwBufferInq(low_buffer_type,forward_recv_buffer.pkg_type,1)) < 0)
			{
				net_debug_printf(("the receive package type is unknown!!!-%d\n",forward_recv_buffer.pkg_type));
				return -1;
			}
			else
			{
				if(BlockBufferWrite( apply_recv_buffer, (void *)&forward_recv_buffer, sizeof(forward_recv_buffer) ) < 0)
				{
					net_debug_printf(("send package to apply buffer is error APPLY BUFFER IS FULL !!!\n"));
					return -1;
				}
			}

			return 1;
		}
		else
		{
			net_debug_printf(("the forward receive data from the low network buffer is error!!!\n"));
		}
		break;
	case 2:
		if(BlockBufferRead(low_recv_big_buffer,(void *)&forward_recv_big_buffer,sizeof(forward_recv_big_buffer)) > 0)
		{
			net_debug_printf(("the forward receive data from the low network buffer is ok!!!\n"));

			if((apply_recv_buffer = ZhwBufferInq(low_buffer_type,forward_recv_big_buffer.pkg_type,1)) < 0)
			{
				net_debug_printf(("the receive package type is unknown!!!-%d\n",forward_recv_big_buffer.pkg_type));
				return -1;
			}
			else
			{
				if(BlockBufferWrite( apply_recv_buffer, (void *)&forward_recv_big_buffer, sizeof(forward_recv_big_buffer) ) < 0)
				{
					net_debug_printf(("send package to heart apply buffer is error APPLY HEART BUFFER IS FULL !!!\n"));
					return -1;
				}
			}

			return 1;
		}
		else
		{
			net_debug_printf(("the forward receive data from the low network buffer is error!!!\n"));
		}
		break;
	default:
		net_debug_printf(("the buffer type must be big common or common!!!\n"));
		break;
	}

	return -1;
}

//send_big_buffer_type :1--common;2--common-big.
int ZhwNetworkSendForward(int send_buffer_id,int send_big_buffer_type)
{
	int low_send_buffer;
	int upper_send_buffer;
	common_package_t forward_send_buffer;

	int send_buffer_type = 0 ;
	int low_send_big_buffer;
	common_big_package_t forward_send_big_buffer;

	send_buffer_type = send_big_buffer_type;

	if((upper_send_buffer = send_buffer_id) < 0)
	{
		net_debug_printf(("the %d buffer is err!!!\n",send_buffer_id));
		return -1;
	}

	switch(send_buffer_type)
	{
	case 1:
		if((low_send_buffer = BlockBufferOpen("network-udp-common-send")) < 0)
		{
			net_debug_printf(("open the %s buffer is err!!!\n","network-udp-common-send"));
			return -1;
		}
		if(BlockBufferRead(upper_send_buffer,(void *)&forward_send_buffer,sizeof(forward_send_buffer)) > 0)
		{
			if(BlockBufferWrite( low_send_buffer, (void *)&forward_send_buffer, sizeof(forward_send_buffer) ) < 0)
			{
				net_debug_printf(("forward level send the apply package to low common buffer is error!!!\n"));
				return -1;
			}
			return 1;
		}
		break;
	case 2:
		if((low_send_big_buffer = BlockBufferOpen("network-udp-common-big-send")) < 0)
		{
			net_debug_printf(("open the %s buffer is err!!!\n","network-udp-common-big-send"));
			return -1;
		}
		if(BlockBufferRead(upper_send_buffer,(void *)&forward_send_big_buffer,sizeof(forward_send_big_buffer)) > 0)
		{
			if(BlockBufferWrite( low_send_big_buffer, (void *)&forward_send_big_buffer, sizeof(forward_send_big_buffer) ) < 0)
			{
				net_debug_printf(("forward level send the apply package to low big common buffer is error!!!\n"));
				return -1;
			}
			return 1;
		}
		break;
	default:
		break;
	}

	return -1;
}
