/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称：zhw_sockfd_server_init.h
*文件标识：
*摘  要：服务器socket初始化头文件
*
*当前版本：00.00.01
*作  者：周魏
*完成日期：
*
*取代版本：原版本号
*原作者 ：
*完成时间：
*/

#ifndef ZHW_SOCKFD_SERVER_INIT_H_
#define ZHW_SOCKFD_SERVER_INIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cyg/infra/testcase.h>
#include "network_include_extern_head.h"

#include <sys/socket.h>

int SockfdServerInit(struct sockaddr_in server_addr,char *sock_model);

#endif /* ZHW_SOCKFD_SERVER_INIT_H_ */
