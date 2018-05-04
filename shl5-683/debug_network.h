/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： debug_network.h
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

#ifndef DEBUG_NETWORK_H_
#define DEBUG_NETWORK_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cyg/infra/testcase.h>
#include <sys/socket.h>
#include "ccu_function.h"

//#define NETWORK_DEBUG

#define NETWORK_DEBUG_PORT 15811

extern int network_debug_sockfd;
extern struct sockaddr_in network_debug_their_addr,network_debug_here_addr;

void InitNetworkDebug();

void diag_printf_network(char *a);

void diag_printf_network_recv(char *buffer_address,int buffer_size);


#endif /* DEBUG_NETWORK_H_ */
