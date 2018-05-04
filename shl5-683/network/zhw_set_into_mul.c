/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称：zhw_set_into_mul.c
*文件标识：
*摘  要：主要是用于服务器套接字的创建，并绑定到相应的服务器IP和已知端口上，并将创建好的套接字返回给用户；
*
*当前版本：00.00.01
*作  者：周魏
*完成日期：
*
*取代版本：原版本号
*原作者 ：
*完成时间：
*/
#include "zhw_set_into_mul.h"

int ZhwSetMulitIp(int socke_fd, char client_ip[16], char mcast_ip[16], int port_num)
{
	net_debug_printf(("I am success entry ZhwSetMulitIp function!\n"));
	int ret = 0;
	unsigned char loop = 0;
	struct ip_mreq mreq;
	bzero(&mreq, sizeof(struct ip_mreq));
//	mreq.imr_multiaddr.s_addr = inet_addr(mcast_ip);
//	mreq.imr_interface.s_addr = inet_addr(client_ip);

	if (setsockopt(socke_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
	{
		net_debug_printf(("setsockopt IP_MULTICAST_LOOP error\n"));
		return -1;
	}

	if (inet_pton(AF_INET, client_ip, (char *)&mreq.imr_interface.s_addr) <= 0)
	{
		net_debug_printf((" ERROR in  transforming the IP add\n"));
		return(-1);
	 }
	if (inet_pton(AF_INET, mcast_ip, (char *)&mreq.imr_multiaddr.s_addr) <= 0)
	{
		net_debug_printf((" ERROR in  transforming the IP add\n"));
		return(-1);
	 }

	if ((ret=setsockopt(socke_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP,&mreq, sizeof(struct ip_mreq))) == -1)
	{
		net_debug_printf(("the ret value is %d\n",ret));
		net_debug_printf(("stop on the setsockopt IP_ADD_MEMBERSHIP\n"));
		return(-1);
	}

	net_debug_printf(("I am success exit ZhwSetMulitIp function!\n"));
	return 0;
}


