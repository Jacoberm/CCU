/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： debug_network.c
 * 文件标识：
 * 摘    要： 通过网络打印提示信息
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-10-10
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "debug_network.h"

int network_debug_sockfd;
struct sockaddr_in network_debug_their_addr,network_debug_here_addr;
unsigned char whether_this_socket_is_register = 0;
void InitNetworkDebug()
{
#ifdef NETWORK_DEBUG
	if((network_debug_sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1)
	{
		perror("sockfd");
	}
	memset(&network_debug_here_addr, 0, sizeof(network_debug_here_addr));
	network_debug_here_addr.sin_family = AF_INET;
	network_debug_here_addr.sin_len = sizeof(network_debug_here_addr);
	network_debug_here_addr.sin_port = htons(NETWORK_DEBUG_PORT);
	if(ccu_state.ccu_info.devices_no == 1)
	{
		network_debug_here_addr.sin_addr.s_addr = inet_addr("192.168.7.13");
	}
	else
	{
		network_debug_here_addr.sin_addr.s_addr = inet_addr("192.168.7.14");
	}


	if(bind(network_debug_sockfd,(struct sockaddr *)&network_debug_here_addr,sizeof(network_debug_here_addr)) < 0)
	{
		diag_printf("bind fail\n");
	}

	bzero(&(network_debug_their_addr.sin_zero),8);
	network_debug_their_addr.sin_family = AF_INET;
	network_debug_their_addr.sin_port = htons(NETWORK_DEBUG_PORT);
	if(ccu_state.ccu_info.devices_no == 1)
	{
		network_debug_their_addr.sin_addr.s_addr = inet_addr("192.168.7.113");
	}
	else
	{
		network_debug_their_addr.sin_addr.s_addr = inet_addr("192.168.7.114");
	}
	whether_this_socket_is_register = 1;
#endif
}

void diag_printf_network(char *a)
{

#ifdef NETWORK_DEBUG
	int numbytes;
	if(whether_this_socket_is_register != 0 && (numbytes = sendto(network_debug_sockfd,a,strlen(a),0,(struct sockaddr *)&network_debug_their_addr,sizeof(struct sockaddr))) == -1)
	{
		perror("sendto");
	}
#endif
}

void diag_printf_network_recv(char *buffer_address,int buffer_size)
{
	if(whether_this_socket_is_register != 0)
	{
		recvfrom(network_debug_sockfd,buffer_address,buffer_size,0,(struct sockaddr *)&network_debug_their_addr,sizeof(struct sockaddr));
		int k = 0;
		for(k = 0;k <= 67;k++)
		{
			if(buffer_address[k] != 0)
			{
				diag_printf("%d:%x\n",k,buffer_address[k]);
			}
		}
	}
	else
	{
		cyg_thread_delay(200);
	}
}
