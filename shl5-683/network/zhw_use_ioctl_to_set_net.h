/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_use_ioctl_to_set_net.h
 * 文件标识：
 * 摘    要： 设置网络的ip地址头文件
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2012-09-14
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef ZHW_USE_IOCTL_TO_SET_NET_H_
#define ZHW_USE_IOCTL_TO_SET_NET_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <network.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>

#ifdef TEST
#include <bootp.h>
#endif

#include "network_include_extern_head.h"
#include "zhw_network_config.h"

typedef struct net_info
{
	char *inf_name;
	char *addrs_ip;
	char *addrs_netmask;
	char *addrs_broadcast;
	char *addrs_gateway;
	char *addrs_server;
}net_info_t;

int ZhwSetIpToSys(net_info_t net_info);
int ZhwSetMacToSys( const char *interface, char mac_address[6] );
int ZhwSetAddToSys(net_info_t net_info,char addrs_local_mac[6]);
#endif /* ZHW_USE_IOCTL_TO_SET_NET_H_ */
