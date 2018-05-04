/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_socket_init.c
 * 文件标识：
 * 摘    要： 套接字初始化
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2013-5-5
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */
#include "zhw_socket_init.h"

/*  	@brief	本函数的功能是负责对套接字的地址结构进行初始化；
 *   	@param char *ip 需要绑定的ip地址；
 *   	@param int port 需要绑定的端口号；
 *   	@return  struct sockaddr_in ret_addr 地址结构体
**/
struct sockaddr_in IpAndPortInit(char *ip,int port)
{
	struct sockaddr_in ret_addr;
	memset(&ret_addr, 0, sizeof(ret_addr));
	ret_addr.sin_family=AF_INET;
	ret_addr.sin_len=sizeof(ret_addr);
	ret_addr.sin_port=htons(port);
	ret_addr.sin_addr.s_addr=inet_addr(ip);
	return ret_addr;
}

/*  	@brief	本函数的功能是比较两个socket值的大小；
 *   	@param int socket1 1号socket；
 *   	@param int socket2 2号socket；
 *   	@return  int max_fd 比较大的socket值；
**/
int MaxSocket(int socket1,int socket2)
{
	int max_fd;
	max_fd = (socket1>socket2?socket1:socket2);
	return max_fd;
}
