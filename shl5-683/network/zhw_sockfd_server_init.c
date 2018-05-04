/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称：zhw_sockfd_server_init.c
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
#include "zhw_sockfd_server_init.h"
#include "zhw_network_config.h"
//#define BACKLOG 20
void pexitt(char *s)
{
    CYG_TEST_FAIL_FINISH( s );
}

/*
 *   	@brief	本函数的功能是负责pcu设备网络信息的发送和接受；
 *     @param server_addr 是struct sockaddr_in 结构体，是服务器需要绑定的IP及端口号；
 *     @param sock_model 	是sock的模式可为SOCK_DGRAM或SOCK_STREAM
 *     @return    将新创建好的套接字返回，为整型数；如果是0表示创建出错
 */
int SockfdServerInit(struct sockaddr_in server_addr,char *sock_model)
{
	int listenfd = 0;
	int on = 1;
	struct sockaddr_in my_addr;//their_addr_TCP;

	if(strcmp(sock_model,"SOCK_STREAM") == 0)
	{
		if((listenfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
		{
			net_debug_printf(("socket fail\n"));
			exit(1);
		}
//		net_debug_printf(("the sockfd_tcp is:%d !\n",listenfd));
	}

	if(strcmp(sock_model,"SOCK_DGRAM") == 0)
	{
		if((listenfd = socket(AF_INET,SOCK_DGRAM,0)) < 0)
		{
			net_debug_printf(("socket fail\n"));
			exit(1);
		}
//		net_debug_printf(("the sockfd_udp is:%d !\n",listenfd));
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)))
	{
		pexitt("setsockopt /source/ SO_REUSEADDR");
	}

	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &on, sizeof(on)))
	{
		pexitt("setsockopt /source/ SO_REUSEPORT");
	}

	int a = 1;
	 if(setsockopt(listenfd, SOL_SOCKET, SO_BROADCAST,(char*)&a,sizeof(int))<0)
    {
		 diag_printf("%%%%%% set socket failed\n");
    }
	 else {
		 diag_printf("&&&&&&&&&& set socket success!!!\n");
	 }

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_len = sizeof(my_addr);
	my_addr.sin_port = server_addr.sin_port;
//	my_addr.sin_addr.s_addr = server_addr.sin_addr.s_addr;
	my_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
//	net_debug_printf(("i want to binding:%s\n",inet_ntoa(my_addr.sin_addr)));

	if(bind(listenfd,(struct sockaddr *)&my_addr,sizeof(my_addr)) < 0)
	{
		net_debug_printf(("bind fail\n"));
		exit(1);
//				continue;
	}

	if(strcmp(sock_model,"SOCK_STREAM") == 0)
	{
		if(listen(listenfd,BACKLOG) == -1)
		{
			net_debug_printf(("listen fail\n"));
			exit(1);
		}
	}

	return listenfd;
}
