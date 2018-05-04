/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： udp_send_fun.h
 * 文件标识：
 *
 * 当前版本： V1.0.1
 * 作    者： roben_ye
 * 摘    要： 3.添加while循环，将传送下来的数据全部发送完
 * 完成日期： 2012-09-19
 *
 * 取代版本： V1.0.0
 * 原作者  ： roben_ye
 * 摘    要： 1. 发送控制命令函数
 *			  2. 打包音频数据、发送音频等函数
 * 完成日期：2012-08-20
 */
#include "udp_send_fun.h"
#include "udp_type.h"
#include <stdlib.h>

#ifndef _LED
   #define debug_print(s) //printf s
#else
   #define debug_print(s)
#endif


int UdpSendFunDATA(int sockfd, void *buffer, int buffer_len, char SEND_IP[16],int send_port_num)
{
	struct sockaddr_in SendtoAdd = {0};
	struct sockaddr_in* SendAddress = NULL;

	//roben_ye,初始化发送地址
	SendAddress = InitSocketAdd(send_port_num, SEND_IP, &SendtoAdd);
	if (NULL == SendAddress)
	{
		debug_print(("wrong on the BindAddress after InitSocketAdd\n"));
		return -1;
	}
	//发送数据
	if (UdpSendBuffer(sockfd, buffer, buffer_len,SendAddress) < 0)
	{
		debug_print(("UdpSendBuffer\n"));
		WriteNetworkErrorToLog("UdpSendBuffer(sockfd, buffer, buffer_len,&SendtoAdd) < 0\n");
		return(-1);
	}

	return 0;
}

data_package_node_t data_package_node;

int UdpSendBuffer(int sockfd, void *buffer, int buffer_len,struct sockaddr_in *SendtoAdd)
{
	int ret = 0;
	int send_len = buffer_len;
	char *pToData = (char *)buffer;
	//申请每一包的大小
//	data_package_node_t *pToSendBuf = (data_package_node_t*)malloc(sizeof(data_package_node_t));
	data_package_node_t *pToSendBuf = &data_package_node;

	if(pToSendBuf == NULL)
	{
		debug_print(("malloc error\n"));
		return -1;
	}

	while(send_len)
	{
		//想要发送的长度 大于 本次包所能发送的长度
		if(send_len >= SIZE_LEN)
		{
			//网络包打包s
			InitSendPackage(pToSendBuf,(void *)pToData,SIZE_LEN);
			send_len -= SIZE_LEN;
			pToData += SIZE_LEN;
		}
		else
		{
			InitSendPackage(pToSendBuf,(void *)pToData,send_len);
			send_len = 0;
		}

		//系统函数调用
		if ((ret = sendto(sockfd, pToSendBuf, sizeof(data_package_node_t), 0, (struct sockaddr *)SendtoAdd, sizeof(*SendtoAdd))) < 0)
		{
			debug_print(("**** ret =%d\n",ret));
			debug_print(("sendto error!\n"));
			WriteNetworkErrorToLog("sendto()  < 0\n");
			return(-1);
		}

//		PrintPortAndIp(*SendtoAdd);
	}

//	free(pToSendBuf);
	return ret;
}

int InitSendPackage(data_package_node_t *pToSendBuf, void *buffer, int buffer_len)
{
	static int counter = 0;
	bzero(pToSendBuf, sizeof(data_package_node_t));

	pToSendBuf-> end_flags = 0;
	pToSendBuf->package_counter = counter++;
	memcpy(pToSendBuf->data_buffer, buffer, buffer_len);
	pToSendBuf->send_len = buffer_len;
//	if(buffer_len < SIZE_LEN)
//	{
//		pToSendBuf->end_flags = 1;
//	}
	gettimeofday(&(pToSendBuf->tv),NULL);
	return 0;
}




int UdpSendFunCMD(int sockfd, void *buffer, int buffer_len, char SEND_IP[16],int send_port_num)
{
	int ret = 0;

	struct sockaddr_in SendtoAdd = {0};
	struct sockaddr_in* SendAddress = NULL;
	SendAddress = InitSocketAdd(send_port_num, SEND_IP, &SendtoAdd);
	if (NULL == SendAddress)
	{
		debug_print(("wrong on the BindAddress after InitSocketAdd\n"));
//		diag_printf("SEND_IP = %s,%d\n",SEND_IP,send_port_num);
		return -1;
	}

	if ((ret = sendto(sockfd, buffer, buffer_len,0,(struct sockaddr *)SendAddress, sizeof(*SendAddress))) < 0)
	{
//		diag_printf("****sendto error ret =%d,%s\n",ret,SEND_IP);
		debug_print(("sendto error!\n"));
		WriteNetworkErrorToLog("sendto()  UdpSendFunCMD  < 0\n");
		return(-1);
	}
	else {
//		diag_printf("****sendto  %s success ,ret =%d\n",SEND_IP,ret);
	}
//	PrintPortAndIp(SendtoAdd);
	return ret;
}
