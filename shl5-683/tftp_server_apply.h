/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：ccu_database_update.h
 * 文件标识：
 * 摘    要：CCU database update function declare
 * 			 CCU 数据库更新函数声明和数据结构声明
 *
 * 当前版本：V1.0.0
 * 作    者：roben
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */


#ifndef TFTP_SERVER_PART_H_
#define TFTP_SERVER_PART_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h> // sleep()
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>

#include "network/udp_recv_fun.h"
#include "tftp_server_fun.h"

//tftp 服务器entry。
int TftpServerAPI(char *tftp_server_ip);

//tftp 服务器控制函数，控制链接数量。
//int TftpServerCmdFun(char *tftp_server_ip);

//int CreateTFTPThread(void);

//int AcceptConnectSocket(int server_socket, int *max_sock_num);

//int ConnectSockRecv(int ret, int total_connect_num);


#endif /* TEST_DATABASE_UPDATE_H_ */
