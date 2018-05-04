/*
 * tftp_server_fun.h
 *
 *  Created on: 2013-1-26
 *      Author: benty
 */

#ifndef TFTP_SERVER_FUN_H_
#define TFTP_SERVER_FUN_H_


#include <pkgconf/system.h>
#include <pkgconf/net.h>
#include <network.h>
#include <tftp_support.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "tftp_network_base.h"

struct tftp_test_fileops
{
    int (*open)(const char *, int);
    int (*close)(int);
    int (*write)(int, const void *, int);
    int (*read)(int, void *, int);
};
#ifndef TNR_OFF
# define TNR_OFF()
#endif
#ifndef TNR_ON
# define TNR_ON()
#endif
#ifndef TNR_INIT
# define TNR_INIT()
#endif
#ifndef TNR_PRINT_ACTIVITY
# define TNR_PRINT_ACTIVITY()
#endif

#ifndef TCP_MAX_CONNECT
#define TCP_MAX_CONNECT 1	//最多允许同时更新的数量。
#endif

//tftp 服务器打开接口
int tftp_test_server(void);

//拆成两个函数，tftp启动函数
int tftp_server_start(void);

//拆成两个函数，tftp关闭函数，现在没有使用
int tftp_server_end(int server_id);

//tftp 打开文件
int tftp_server_open(const char *, int);

//tftp 关闭
int tftp_server_close(int);

//tftp 更新文件
int tftp_server_write(int, const void *, int);

//tftp 读取文件
int tftp_server_read(int, void *, int);


//void *TftpTthreeadEntry(void *arg);

//void TftpThreadConfigure(pthread_attr_t *attr);

//int ServerSockCreateAndListen(struct addrinfo * addrinfo);

#endif /* TFTP_SERVER_FUN_H_ */
