/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：zhw_network_config.h
 * 文件标识：
 * 摘    要：网络配置文件头文件
 *
 * 当前版本：V1.0.0
 * 作    者：周魏
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */

#ifndef ZHW_NETWORK_CONFIG_H_
#define ZHW_NETWORK_CONFIG_H_
//#include "network_include_extern_head.h"

#ifndef uchar
//#define uchar unsigned char
#endif

#define SOCKFD_EAMP_PORT 19999
#define SOCKFD_EAMP_DATA_PORT 19998
#define SOCKFD_PCU_CMD_PORT 19997
#define SOCKFD_PCU_DATA_PORT 19996

#define SOCKFD_HEART_PORT 20000

#if 1
#define net_debug_printf(s) //diag_printf s
#define net_debug_printf2(s) diag_printf s
//#define net_debug_printf(s)  //diag_printf s       ///<debug for wilson,on the platform of Beta1.0
#endif

//EAMP 接受音频数据的多播组
//#define MUL_IP_EAMP_DATA "230.10.10.66"

//EAMP 接受控制命令的多播组
//#define MUL_IP_EAMP_CMD "230.10.10.77"

//BCU 接受PCU请求命令的多播组
//#define MUL_IP_BCU_CMD "230.10.10.55"

//设备的心跳多播组
//#define MUL_IP_HEART "230.10.10.88"

#define LOOP_NUM 20
#define BACKLOG 40

#define CMD_SIZE 100
#define DATA_SIZE (1024)

#define IP_NUMBER 28
#define DEVICE_TYPE 4



typedef struct
{
	int udp_cmd_socket_send_buffer;
	int udp_cmd_socket_recv_buffer;
}udp_cmd_socket_buffer_t;

typedef struct
{
	int udp_data_socket_send_buffer;
	int udp_data_socket_recv_buffer;
}udp_data_socket_buffer_t;

typedef struct
{
	int udp_common_socket_send_buffer;
	int udp_common_socket_recv_buffer;
}udp_common_socket_buffer_t;

typedef struct
{
	int udp_common_socket_send_big_buffer;
	int udp_common_socket_recv_big_buffer;
}udp_common_socket_big_buffer_t;

typedef struct
{
	int udp_heart_socket_send_buffer;
	int udp_heart_socket_recv_buffer;
}udp_heart_socket_buffer_t;

typedef struct
{
	int udp_dmu_socket_send_buffer;
	int udp_dmu_socket_recv_buffer;
}udp_dmu_socket_buffer_t;

typedef struct
{
	int udp_monitor_socket_send_buffer;
	int udp_monitor_socket_recv_buffer;
}udp_monitor_socket_buffer_t;

#ifdef CONFIG_CCU_VIDEOLINK
typedef struct
{
	int udp_ccu_videolink_buffer_rx;
	int udp_ccu_videolink_buffer_tx;
}udp_ccu_videolink_buffer_t;
#endif


typedef struct
{
	int udp_shell_socket_send_buffer;
	int udp_shell_socket_recv_buffer;
}udp_shell_socket_buffer_t;

typedef struct
{
	udp_cmd_socket_buffer_t udp_cmd_socket_buffer;
	udp_data_socket_buffer_t udp_data_socket_buffer;
	udp_common_socket_buffer_t udp_common_socket_buffer;
	udp_common_socket_big_buffer_t udp_common_socket_big_buffer;
	udp_heart_socket_buffer_t udp_heart_socket_buffer;
	udp_dmu_socket_buffer_t udp_dmu_socket_buffer;
	udp_monitor_socket_buffer_t udp_monitor_socket_buffer;
	udp_shell_socket_buffer_t udp_shell_socket_buffer ;
#ifdef CONFIG_CCU_VIDEOLINK
	udp_ccu_videolink_buffer_t udp_ccu_videolink_buffer;
#endif
}network_buffer_t;


#endif /* ZHW_NETWORK_CONFIG_H_ */
