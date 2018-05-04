/*
 * tftp_client_test.h
 *
 *  Created on: 2013-1-26
 *      Author: benty
 */

#ifndef TFTP_CLIENT_FUN_H_
#define TFTP_CLIENT_FUN_H_

#include <network.h>
#include <tftp_support.h>
#include <stdio.h>
#include <network.h>
#include <tftp_support.h>
#include <arpa/inet.h>
#include <string.h>
#include <fcntl.h>

#include "tftp_update_protect_confige.h"
#include "tftp_network_base.h"


//int ForceDataFromBufToDisk(FILE *file_stream);


//接收，存储标准tftp包

int TftpControlFunction(struct sockaddr_in *udp_PTT_recv_addr,
							FILE *write_stream,
							char *down_file_name);
							
							

//tftp下载、存储接口。
int DownloadFileFromTFTP(char *down_file_name, char *tftp_server_ip, char *save_file_name);


//int RefreshDatabaseFile(FILE *writw_file_stream, char *buf, int len);

//int WriteDataToFile(FILE *writw_file_stream, char *buf, int len);

//tftp服务器端地址打包
int InitRecvAddr(struct sockaddr_in *udp_PTT_recv_addr, char *tftp_server_ip);

//int InitAndSendPackage(int local_sock_send, int local_dev_num, int type);

//修改配置文件中音频文件更新标志的状态。
int ModifyConfigfileState(char *config_file_name, int want_chang_file, int want_change_state);


#endif /* TFTP_CLIENT_FUN_H_ */
