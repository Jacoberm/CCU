/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称：zhw_set_into_mul.h
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

#ifndef ZHW_SET_INTO_MUL_H_
#define ZHW_SET_INTO_MUL_H_
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>

#include "zhw_socket_init.h"
#include "zhw_network_config.h"

int ZhwSetMulitIp(int socke_fd, char *client_ip, char *mcast_ip, int port_num);
#endif /* ZHW_SET_INTO_MUL_H_ */
