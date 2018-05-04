/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_network_main.c
 * 文件标识：
 * 摘    要： 网络主函数
 *
 * 当前版本： V1.0.1
 * 作    者： zhouwei
 * 完成日期：
 *
 * 取代版本：v1.0.0
 * 原作者  ：zhouwei
 * 完成日期：2012-10-08
 *
 */
#include "zhw_network_main.h"

extern int db_err_status;
//wilson add
struct global_info dev_info_local;
buffer_pointer_t *p_buffer_pointer;
int net_recv_buffer_id_wilson = 0;
extern pthread_t thread_of_network;
cyg_flag_t cmd_flag_type ; //flag 4-1 add by zhw
#ifdef EAMP_DEV
extern pthread_t thread_of_control_processing;
extern pthread_t thread_of_network_processing;
extern unsigned char control_thread_flag;
#endif
unsigned int net_audio_send_total = 0; ///< debug
#if 1
unsigned int pack_number = 0;
unsigned char gd_thread_enter_ccu_db = 0;
void NetSimulationRecvPackage(int buffer_id)
{
        common_big_package_t common_package_buffer = {"\0", 0};
//	int i = 0;

        strcpy(common_package_buffer.dst_dev_name, "CCU");
        common_package_buffer.dst_dev_number = 2;
        strcpy(common_package_buffer.src_dev_name, "CCU");
        common_package_buffer.src_dev_number = 1;		
        common_package_buffer.pkg_type = 2;
        common_package_buffer.common_big_data_u.pa_common_cmd_info.sequence = pack_number++;
        BlockBufferWrite(buffer_id, &common_package_buffer, sizeof(common_package_buffer));
		
        return ;
}
#endif

int gd_udp_common_socket_send_buffer = 0; ///< debug
shell_package_t shellcmd[SHELL_CMD_NUMBER] = {
				{ProtocolTestStart,ZhwProTS},
				{CommQualityTest,ZhwCqcModule},
				{SystemReboot,NetShellReboot},
				{DevFirmwareUpdate,DFUProc},
				{OpenAtuo,ZhwBakFunc4},
				{CloseAtuo,ZhwBakFunc5},
				{OpenNetPrint,ZhwBakFunc6},
				{CloseNetPrint,ZhwBakFunc7},
				{ZhwFunc8,ZhwBakFunc8},
				{ZhwFunc9,ZhwBakFunc9}
};

//changed by zhw 2016-1-1
struct dev_type_and_no local_device_io;
struct global_info dev_info_local;
void ZhwSetNet() {
	local_device_io = ReadFromIO();

	char local_mac[6];
	int mac_loop;

	//init the global array---zhw_5.7
//	InitGlobalArray(); //全局设备信息数组初始化，已经移至初始化线程中使用。

	//read message from I/O
//	local_device_io = ReadFromIO();
//	net_debug_printf(("the dev type is :%d,the dev number is :%d\n",local_device_io.dev_type,local_device_io.dev_number));

//	diag_printf("db_err_status = %d\n",db_err_status);
	if( 1 == db_err_status )
	{
		dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,200,1);
		dev_info_local.dev_mac[0] = 0;
		dev_info_local.dev_mac[1] = 90;
		dev_info_local.dev_mac[2] = 27;
		dev_info_local.dev_mac[3] = 200;
		dev_info_local.dev_mac[4] = local_device_io.dev_type;
		dev_info_local.dev_mac[5] = local_device_io.dev_number;
	}
	else
	{
		dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,local_device_io.dev_number,1);
	}

	for(mac_loop = 0 ;mac_loop < 6 ;mac_loop++)
	{
		local_mac[mac_loop] = dev_info_local.dev_mac[mac_loop];
	}


#ifdef CYGHWR_NET_DRIVER_ETH0_MANUAL
	net_debug_printf(("system already define CYGHWR_NET_DRIVER_ETH0_MANUAL!!!\n"));
	net_info_t net_info;
	net_info.inf_name = LOCAL_DEV_NAME;
	net_info.addrs_ip = dev_info_local.dev_ip;
	net_info.addrs_netmask = NETMASK_ADDRS;
	net_info.addrs_broadcast = BROADCAST_ADDRS;
	net_info.addrs_gateway = GATEWAY_ADDRS;
	net_info.addrs_server = SERVER_ADDRS;

	if(ZhwSetAddToSys(net_info,local_mac)<0)
	{
		net_debug_printf(("init network is failed for %s !!!\n",net_info.inf_name));
		exit(0);
	}
	else {
		net_debug_printf(("init network is success for %s !!!\n",net_info.inf_name));
	}
#else
	net_debug_printf(("system not define CYGHWR_NET_DRIVER_ETH0_MANUAL!!!\n"));
	init_all_network_interfaces();
#endif
}

void NetWorkMain(network_buffer_t network_buffer)
{
	net_recv_buffer_id_wilson = network_buffer.udp_data_socket_buffer.udp_data_socket_recv_buffer;
//	net_debug_printf(("I am success entry network handle thread!\n"));
	int read_data_long = 0;

	int network_thread_loop_number = 0 ;

	int udp_cmd_socket;
	int udp_data_socket;
	int udp_common_socket;
	int udp_shell_socket;
	int udp_common_big_socket;
	int udp_heart_socket;
	// add by zhw 2015-12-8
	int udp_dmu_socket;
	//end of add

	//add by zhw 2015-12-23
	int tcp_connect_flag = 0 ;
	int tcp_client_monitor_socket;
	//end of add

//	int tcp_server_socket;
	int max_socket = 0;

	socklen_t len;

	int select_ret = 0;

	int udp_data_flag = 0;//1:表示传送命令，2：表示传送数据

	struct sockaddr_in udp_cmd_socket_addr;//用于提供创建udp命令sockfd的ip和端口
	struct sockaddr_in udp_data_socket_addr;//用于提供创建udp数据sockfd的ip和端口
	struct sockaddr_in udp_common_socket_addr;//用于提供创建udp普通命令sockfd的ip和端口
	struct sockaddr_in udp_shell_socket_addr;//用于提供创建udp shell命令sockfd的ip和端口
	struct sockaddr_in udp_common_big_socket_addr;
	struct sockaddr_in udp_heart_socket_addr;
	//add by zhw 2015-12-8
	struct sockaddr_in udp_dmu_socket_addr;
	//end of add

	//add by zhw 2015-12-23
	struct sockaddr_in tcp_client_monitor_socket_addr;
	//end of add

//	struct sockaddr_in tcp_server_socket_addr;//用于提供创建tcp服务器sockfd的ip和端口

	struct sockaddr_in udp_shell_client_addr; //保存shell通讯的客户端地址
	len = sizeof(udp_shell_client_addr);

	char data_buffer_send[DATA_SIZE];

	//传给银华的参数，用于获取全局变量，在获取中银华进行加锁。
	char device_name_send_data[30];
	int device_number_send_data = 0;



	network_send_package_t network_send_cmd_buf;	//用于网络发送控制信息缓存
	common_package_t common_package_buffer;		//用于存放通用套接字的数据缓存
	common_big_package_t common_big_package_buffer;
	network_heart_package_t heart_package_buffer;

	//add by zhw 2015-12-8
#ifdef EAMP_DEV
	eamp_to_LCD_t cmd_sendto_lcd_buffer;
	eamp_to_LCD_t cmd_recvfrom_lcd_buffer;
#endif
	//end of add

	//add by zhw 2015-12-23
	char message_to_monitor_buffer[40];
	char message_from_monitor_buffer[40];
	memset(message_to_monitor_buffer, 0, sizeof(message_to_monitor_buffer));
	memset(message_from_monitor_buffer, 0, sizeof(message_from_monitor_buffer));
	//end of add

	int shell_loop;
	char shell_recv_buffer[128];
	char shell_send_buffer[128];

//	struct dev_type_and_no local_device_io;
//	struct global_info dev_info_local;
	struct global_info tar_dev_info;
	char *set_ip;
//	char local_mac[6];
//	int mac_loop;
//
//	//init the global array---zhw_5.7
////	InitGlobalArray(); //全局设备信息数组初始化，已经移至初始化线程中使用。
//
//	//read message from I/O
////	local_device_io = ReadFromIO();
////	net_debug_printf(("the dev type is :%d,the dev number is :%d\n",local_device_io.dev_type,local_device_io.dev_number));
//
////	diag_printf("db_err_status = %d\n",db_err_status);
//	if( 1 == db_err_status )
//	{
//		dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,200,1);
//		dev_info_local.dev_mac[0] = 0;
//		dev_info_local.dev_mac[1] = 90;
//		dev_info_local.dev_mac[2] = 27;
//		dev_info_local.dev_mac[3] = 200;
//		dev_info_local.dev_mac[4] = local_device_io.dev_type;
//		dev_info_local.dev_mac[5] = local_device_io.dev_number;
//	}
//	else
//	{
//		dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,local_device_io.dev_number,1);
//	}
//
	set_ip = dev_info_local.dev_ip;
//	for(mac_loop = 0 ;mac_loop < 6 ;mac_loop++)
//	{
//		local_mac[mac_loop] = dev_info_local.dev_mac[mac_loop];
//	}

//#ifdef CYGHWR_NET_DRIVER_ETH0_MANUAL
//	net_debug_printf(("system already define CYGHWR_NET_DRIVER_ETH0_MANUAL!!!\n"));
//	net_info_t net_info;
//	net_info.inf_name = LOCAL_DEV_NAME;
//	net_info.addrs_ip = set_ip;
//	net_info.addrs_netmask = NETMASK_ADDRS;
//	net_info.addrs_broadcast = BROADCAST_ADDRS;
//	net_info.addrs_gateway = GATEWAY_ADDRS;
//	net_info.addrs_server = SERVER_ADDRS;
//
//	if(ZhwSetAddToSys(net_info,local_mac)<0)
//	{
//		net_debug_printf(("init network is failed for %s !!!\n",net_info.inf_name));
//		exit(0);
//	}
//	else {
//		net_debug_printf(("init network is success for %s !!!\n",net_info.inf_name));
//	}
//#else
//	net_debug_printf(("system not define CYGHWR_NET_DRIVER_ETH0_MANUAL!!!\n"));
//	init_all_network_interfaces();
//#endif

	net_debug_printf(("zhw ********** add for test 2015-12-9  !!!\n"));

	memset(&udp_cmd_socket_addr, 0, sizeof(udp_cmd_socket_addr));
	udp_cmd_socket_addr = IpAndPortInit(set_ip,UDP_CMD_PORT);

	memset(&udp_data_socket_addr, 0, sizeof(udp_data_socket_addr));
	udp_data_socket_addr = IpAndPortInit(set_ip,UDP_DATA_PORT);

	memset(&udp_common_socket_addr, 0, sizeof(udp_common_socket_addr));
	udp_common_socket_addr = IpAndPortInit(set_ip,UDP_COMMON_PORT);

	memset(&udp_shell_socket_addr, 0, sizeof(udp_shell_socket_addr));
	udp_shell_socket_addr = IpAndPortInit(set_ip,UDP_SHELL_PORT);

	memset(&udp_common_big_socket_addr,0,sizeof(udp_common_big_socket_addr));
	udp_common_big_socket_addr = IpAndPortInit(set_ip,UDP_COMMON_BIG_PORT);

	memset(&udp_heart_socket_addr,0,sizeof(udp_heart_socket_addr));
	udp_heart_socket_addr = IpAndPortInit(set_ip,UDP_HEART_PORT);

	//add by zhw 2015-12-8
	memset(&udp_dmu_socket_addr,0,sizeof(udp_dmu_socket_addr));
	udp_dmu_socket_addr = IpAndPortInit(set_ip,50151);
	//end of add

	//add by zhw 2015-12-23
	memset(&tcp_client_monitor_socket_addr,0,sizeof(tcp_client_monitor_socket_addr));
	if(strcmp(dev_info_local.dev_name,"CCU")==0) {
		if(dev_info_local.dev_number == 1) {
			tcp_client_monitor_socket_addr = IpAndPortInit(MONITOR_HOST_1_IP,TCP_MONITOR_PORT);
		}
		else if(dev_info_local.dev_number == 2) {
			tcp_client_monitor_socket_addr = IpAndPortInit(MONITOR_HOST_2_IP,TCP_MONITOR_PORT);
		}
	}
//	tcp_client_monitor_socket_addr = IpAndPortInit(MONITOR_HOST_1_IP,TCP_MONITOR_PORT);
	//end of add

//	memset(&tcp_server_socket_addr, 0, sizeof(tcp_server_socket_addr));
//	tcp_server_socket_addr = IpAndPortInit(set_ip,TCP_SERVER_PORT);

	udp_cmd_socket = SockfdServerInit(udp_cmd_socket_addr,"SOCK_DGRAM");//创建与eamp的套接字
	if(udp_cmd_socket<0)
	{
		net_debug_printf(("the udp socket of cmd init fail!!!\n"));
	}
	else
	{
		max_socket = MaxSocket(max_socket,udp_cmd_socket);
	}

	udp_data_socket = SockfdServerInit(udp_data_socket_addr,"SOCK_DGRAM");//创建与eamp的套接字
	if(udp_data_socket<0)
	{
		net_debug_printf(("the udp socket of data init fail!!!\n"));
	}
	else
	{
		max_socket = MaxSocket(max_socket,udp_data_socket);
	}

	udp_common_socket = SockfdServerInit(udp_common_socket_addr,"SOCK_DGRAM");//创建与eamp的套接字
	if(udp_common_socket<0)
	{
		net_debug_printf(("the udp socket of common init fail!!!\n"));
	}
	else
	{
		max_socket = MaxSocket(max_socket,udp_common_socket);
	}

	udp_common_big_socket = SockfdServerInit(udp_common_big_socket_addr,"SOCK_DGRAM");
	if(udp_common_big_socket < 0)
	{
		net_debug_printf(("the udp socket of udp_common_big_socket init fail!!!\n"));
	}
	else
	{
		max_socket = MaxSocket(max_socket,udp_common_big_socket);
	}

	udp_heart_socket = SockfdServerInit(udp_heart_socket_addr,"SOCK_DGRAM");
	if(udp_heart_socket < 0)
	{
		net_debug_printf(("the udp socket of udp_heart_socket init fail!!!\n"));
	}
	else
	{
		max_socket = MaxSocket(max_socket,udp_heart_socket);
	}

	//add by zhw 2015-12-8
	udp_dmu_socket = SockfdServerInit(udp_dmu_socket_addr,"SOCK_DGRAM");
	if(udp_dmu_socket < 0)
	{
		net_debug_printf(("the udp socket of udp_dmu_socket init fail!!!\n"));
	}
	else
	{
		max_socket = MaxSocket(max_socket,udp_dmu_socket);
	}
	//end of add

	//add by zhw 2015-12-23
	#ifdef ZHW_CONFIG_MONITOR_TCP
	tcp_client_monitor_socket = socket(AF_INET,SOCK_STREAM,0);
	if(tcp_client_monitor_socket < 0) {
		net_debug_printf(("the tcp socket of tcp_client_monitor_socket init fail!!!\n"));
	}
	else {
		max_socket = MaxSocket(max_socket,tcp_client_monitor_socket);
	}
	#endif
	//end of add

	//test ????????????????????????? zhw-6.6
	udp_shell_socket = SockfdServerInit(udp_shell_socket_addr,"SOCK_DGRAM");//创建与eamp的套接字
	if(udp_shell_socket<0)
	{
		net_debug_printf(("the udp shell sockfd init fail!!!\n"));
	}
	else
	{
		net_debug_printf(("the  udp shell socket is created !!!\n"));
		max_socket = MaxSocket(max_socket,udp_shell_socket);
	}

	#ifdef CONFIG_CCU_VIDEOLINK
	int udp_ccu_videolink_socket;
       struct sockaddr_in udp_ccu_videolink_socket_addr;	
	   
	memset(&udp_ccu_videolink_socket_addr, 0, sizeof(udp_ccu_videolink_socket_addr));
	udp_ccu_videolink_socket_addr = IpAndPortInit(set_ip, 52090);
	
	udp_ccu_videolink_socket = SockfdServerInit(udp_ccu_videolink_socket_addr, "SOCK_DGRAM");
	if(udp_ccu_videolink_socket<0) {
		net_debug_printf(("udp_ccu_videolink_socket:  init fail!!!\n"));
	} else {
		max_socket = MaxSocket(max_socket, udp_ccu_videolink_socket);
	}
	#endif

	if(strcmp(dev_info_local.dev_name,"BCU")==0)
	{
		if(ZhwSetMulitIp(udp_cmd_socket,set_ip,MUL_IP_BCU_CMD,UDP_CMD_PORT) < 0)
		{
			net_debug_printf(("udp_cmd_socket %d set mul_ip:%s,port :%d err!!!\n",udp_cmd_socket,MUL_IP_BCU_CMD,UDP_CMD_PORT));
		}
		if(ZhwSetMulitIp(udp_heart_socket,set_ip,MUL_IP_COMMON,UDP_HEART_PORT) < 0)
		{
			net_debug_printf(("udp_heart_socket %d set mul_ip:%s,port :%d err!!!\n",udp_heart_socket,MUL_IP_COMMON,UDP_HEART_PORT));
		}
		if(ZhwSetMulitIp(udp_common_big_socket, set_ip, MUL_IP_SYNC_COMMON_BCU, UDP_SYNC_COMMON_BCU_PORT) < 0) ///< add, 2013
		{
			net_debug_printf(("BCU, sync, udp_common_big_socket=%d, mul_ip=%s,port=%d, error\n", udp_common_big_socket, MUL_IP_SYNC_COMMON_BCU, UDP_SYNC_COMMON_BCU_PORT));
		}
		if(ZhwSetMulitIp(udp_data_socket, set_ip, MUL_IP_CC_DATA, UDP_DATA_PORT) < 0) ///< add, 0819, now, ccu is eamp_x
		{
			net_debug_printf(("udp_data_socket of BCU=%d, mul_ip=%s_port=%d, error!\n", udp_data_socket, MUL_IP_CC_DATA, UDP_DATA_PORT));
		}
		net_debug_printf(("BCU Multi is set ok!!!\n"));
	}
	else if(strcmp(dev_info_local.dev_name,"EAMP")==0)
	{
		if(ZhwSetMulitIp(udp_cmd_socket,set_ip,MUL_IP_EAMP_CMD,UDP_CMD_PORT) < 0)
		{
			net_debug_printf(("udp_cmd_socket %d set mul_ip:%s,port :%d err!!!\n",udp_cmd_socket,MUL_IP_EAMP_CMD,UDP_CMD_PORT));
		}
		if(ZhwSetMulitIp(udp_data_socket,set_ip,MUL_IP_EAMP_DATA,UDP_DATA_PORT) < 0)
		{
			net_debug_printf(("udp_data_socket %d set mul_ip:%s,port :%d err!!!\n",udp_data_socket,MUL_IP_EAMP_DATA,UDP_DATA_PORT));
		}
		if(ZhwSetMulitIp(udp_heart_socket,set_ip,MUL_IP_COMMON,UDP_HEART_PORT) < 0)
		{
			net_debug_printf(("udp_heart_socket %d set mul_ip:%s,port :%d err!!!\n",udp_heart_socket,MUL_IP_COMMON,UDP_HEART_PORT));
		}
		if(ZhwSetMulitIp(udp_common_big_socket, set_ip, MUL_IP_SYNC_COMMON_EAMP, UDP_SYNC_COMMON_EAMP_PORT) < 0) ///< add, 2013
		{
			net_debug_printf(("EAMP, sync, udp_common_big_socket=%d, mul_ip=%s, port=%d, error\n", udp_common_big_socket, MUL_IP_SYNC_COMMON_EAMP, UDP_SYNC_COMMON_EAMP_PORT));
		}		
		net_debug_printf(("EAMP Multi is set ok!!!\n"));
	}
	else if(strcmp(dev_info_local.dev_name,"PCU")==0)
	{
		if(ZhwSetMulitIp(udp_heart_socket,set_ip,MUL_IP_COMMON_PCU,UDP_HEART_PORT) < 0)
		{
			net_debug_printf(("udp_heart_socket %d set mul_ip:%s,port :%d err!!!\n",udp_heart_socket,MUL_IP_COMMON,UDP_HEART_PORT));
		}
		if(ZhwSetMulitIp(udp_shell_socket,set_ip,MUL_IP_SHELL,UDP_SHELL_PORT) < 0)
		{
			net_debug_printf(("udp_common_socket %d set mul_ip:%s,port :%d err!!!\n",udp_shell_socket,MUL_IP_SHELL,UDP_SHELL_PORT));
		}
		if(ZhwSetMulitIp(udp_common_big_socket, set_ip, MUL_IP_SYNC_COMMON_PCU, UDP_SYNC_COMMON_PCU_PORT) < 0) ///< add, 2013
		{
			net_debug_printf(("PCU, sync, comm_socket=%d, mul_ip=%s, port=%d, error\n", udp_common_big_socket, MUL_IP_SYNC_COMMON_PCU, UDP_SYNC_COMMON_PCU_PORT));
		}
		if(ZhwSetMulitIp(udp_cmd_socket, set_ip, MUL_IP_PCU_CMD, UDP_CMD_PORT) < 0) ///< wilson 2014-01-20
		{
			net_debug_printf(("PCU, sync, comm_socket=%d, mul_ip=%s, port=%d, error\n", udp_cmd_socket, MUL_IP_SYNC_COMMON_PCU, UDP_SYNC_COMMON_PCU_PORT));
		}
		net_debug_printf(("PCU Multi is set ok!!!\n"));
	}
	else if(strcmp(dev_info_local.dev_name,"CCU")==0)
	{
		if(ZhwSetMulitIp(udp_cmd_socket, set_ip, MUL_IP_EAMP_CMD, UDP_CMD_PORT) < 0) ///< add, 0819, now, ccu is eamp_x
		{
			net_debug_printf(("udp_cmd_socket of CCU=%d, mul_ip=%s_port=%d, error!\n", udp_cmd_socket, MUL_IP_EAMP_CMD, UDP_CMD_PORT));
		}
		if(ZhwSetMulitIp(udp_cmd_socket, set_ip, MUL_IP_BCU_CMD, UDP_CMD_PORT) < 0) ///< add, 0803, now, ccu is bcu_x
		{
			net_debug_printf(("udp_cmd_socket of CCU=%d, mul_ip=%s_port=%d, error!\n", udp_cmd_socket, MUL_IP_BCU_CMD, UDP_CMD_PORT));
		}	
		if(ZhwSetMulitIp(udp_common_big_socket, set_ip, MUL_IP_SYNC_COMMON_CCU, UDP_SYNC_COMMON_CCU_PORT) < 0) ///< add, 20131231
		{
			net_debug_printf(("udp_common_socket %d, CCU SYNC, set mul_ip:%s,port :%d err!!!\n", udp_common_socket, MUL_IP_SYNC_COMMON_CCU, UDP_SYNC_COMMON_CCU_PORT));
		}
		if(ZhwSetMulitIp(udp_heart_socket,set_ip,MUL_IP_COMMON,UDP_HEART_PORT) < 0)
		{
			net_debug_printf(("udp_heart_socket %d set mul_ip:%s,port :%d err!!!\n",udp_heart_socket,MUL_IP_COMMON,UDP_HEART_PORT));
		}
		if(ZhwSetMulitIp(udp_heart_socket,set_ip,MUL_IP_COMMON_PCU,UDP_HEART_PORT) < 0)
		{
			net_debug_printf(("udp_heart_socket %d set mul_ip:%s,port :%d err!!!\n",udp_heart_socket,MUL_IP_COMMON_PCU,UDP_HEART_PORT));
		}
		if(ZhwSetMulitIp(udp_cmd_socket,set_ip,"230.10.10.44",UDP_COMMON_PORT) < 0)///<just for test
		{
			net_debug_printf(("bcu send  %d set mul_ip:%s,port :%d err!!!\n",udp_common_socket,MUL_IP_COMMON,UDP_COMMON_PORT));
		}
		if(ZhwSetMulitIp(udp_data_socket, set_ip, MUL_IP_CC_DATA, UDP_DATA_PORT) < 0) ///< add, 0819, now, ccu is eamp_x
		{
			net_debug_printf(("udp_data_socket of CCU=%d, mul_ip=%s_port=%d, error!\n", udp_data_socket, MUL_IP_CC_DATA, UDP_DATA_PORT));
		}
		if(ZhwSetMulitIp(udp_ccu_videolink_socket, set_ip, MUL_IP_MONITOR_CMD, UDP_MONITOR_PORT) < 0) ///< add, 0819, now, ccu is eamp_x
		{
			net_debug_printf(("udp_ccu_videolink_socket of CCU=%d, mul_ip=%s_port=%d, error!\n", udp_ccu_videolink_socket, MUL_IP_MONITOR_CMD, UDP_MONITOR_PORT));
		}
//		net_debug_printf(("CCU Multi is set ok!!!\n"));
	}
	else if(strcmp(dev_info_local.dev_name,"CMU")==0)
	{
		if(ZhwSetMulitIp(udp_cmd_socket,set_ip,MUL_IP_CMU_CMD,UDP_CMD_PORT) < 0)
		{
			net_debug_printf(("mcu cmd socket  %d set mul_ip:%s,port :%d err!!!\n",udp_cmd_socket,MUL_IP_CMU_CMD,UDP_CMD_PORT));
		}
		if(ZhwSetMulitIp(udp_data_socket,set_ip,MUL_IP_EAMP_DATA,UDP_DATA_PORT) < 0)
		{
			net_debug_printf(("mcu data socket  %d set mul_ip:%s,port :%d err!!!\n",udp_data_socket,MUL_IP_EAMP_DATA,UDP_DATA_PORT));
		}
		if(ZhwSetMulitIp(udp_data_socket,set_ip,MUL_IP_CC_DATA,UDP_DATA_PORT) < 0)
		{
			net_debug_printf(("mcu data socket  %d set mul_ip:%s,port :%d err!!!\n",udp_data_socket,MUL_IP_CC_DATA,UDP_DATA_PORT));
		} // changed by zhw-2015-5-18
		if(ZhwSetMulitIp(udp_common_socket,set_ip,MUL_IP_CMU_COMMON,UDP_COMMON_PORT) < 0)
		{
			net_debug_printf(("mcu data socket  %d set mul_ip:%s,port :%d err!!!\n",udp_data_socket,MUL_IP_CC_DATA,UDP_DATA_PORT));
		}
		// add by zhw-2015-5-18 for cmu data
		if(ZhwSetMulitIp(udp_data_socket,set_ip,MUL_IP_CMU_DATA,UDP_DATA_PORT) < 0)
		{
			net_debug_printf(("mcu data socket  %d set mul_ip:%s,port :%d err!!!\n",udp_data_socket,MUL_IP_CC_DATA,UDP_DATA_PORT));
		}
		//end of add
	}

	fd_set frd;
	fd_set fwd;
	fd_set fad;
	FD_ZERO(&fad);

	FD_SET(udp_cmd_socket,&fad);
	FD_SET(udp_data_socket,&fad);
	FD_SET(udp_common_socket,&fad);
	FD_SET(udp_shell_socket,&fad);
	FD_SET(udp_common_big_socket,&fad);
	FD_SET(udp_heart_socket,&fad);

	//add by zhw 2015-12-8
	FD_SET(udp_dmu_socket,&fad);
	//end of add

	//add by zhw 2015-12-23
	#ifdef ZHW_CONFIG_MONITOR_TCP
	FD_SET(tcp_client_monitor_socket,&fad);
	#endif
	//end of add

//	FD_SET(tcp_server_socket,&fad);

#ifdef CONFIG_CCU_VIDEOLINK
	FD_SET(udp_ccu_videolink_socket, &fad);
#endif

	memset(&data_buffer_send,'\0',sizeof(data_buffer_send));//初始化发送音频数据的buffer；从别处移来，防止清除发送数据——zhw
	memset(&shell_recv_buffer,'\0',sizeof(shell_recv_buffer));
	memset(&shell_send_buffer,'\0',sizeof(shell_send_buffer));

	buffer_pointer_t	CachePoint = CacheCreate();
	p_buffer_pointer = &CachePoint;


//			if(0 == tcp_connect_flag) {
//				  if(connect(tcp_client_monitor_socket,(struct sockaddr *)&tcp_client_monitor_socket_addr,sizeof(tcp_client_monitor_socket_addr))==-1) {
//	//			       diag_printf("connect() error\n");
//				  }
//				  else {
//					  tcp_connect_flag = 1;
//				  }
//			}

#ifdef TCP_IP_TEST
	ZhwTcpIpTestAlarmInit();
#endif

//	ZhwShellInit(); //add by zhw_8-21 ;初始化shell命令

	gd_udp_common_socket_send_buffer = network_buffer.udp_common_socket_buffer.udp_common_socket_send_buffer;
#ifdef	EAMP_DEV
	CreateNetworkTimer();
	PthreadPriorityChangeForSchedRr(thread_of_network_processing, 10+9);
#endif
	for(;;)
	{
		if( network_thread_loop_number == 5000 )
		{
			network_thread_loop_number = 0 ;
			net_debug_printf(("the network module is run ok!!!\n"));
		}
		else
		{
			network_thread_loop_number = network_thread_loop_number + 1 ;
		}

              #ifdef ZHW_CONFIG_MONITOR_TCP
		if(0 == tcp_connect_flag) {
			  if(connect(tcp_client_monitor_socket,(struct sockaddr *)&tcp_client_monitor_socket_addr,sizeof(tcp_client_monitor_socket_addr))==-1) {
//			       diag_printf("connect() error\n");
			  }
			  else {
				  tcp_connect_flag = 1;
			  }
		}
		#endif

		FD_ZERO(&frd);
		FD_ZERO(&fwd);
		frd=fad;//将可读标志位赋值
		fwd=fad;
		select_ret = select(max_socket+1,&frd,&fwd,NULL,0);//监听所有套接口套接口，看是否有数据
		if(select_ret==-1)
		{
			net_debug_printf(("the select is error!\n"));
			exit(1);
		}
		else
		{
			if(FD_ISSET(udp_cmd_socket,&frd))//检测udp的cmd套接字是否有数据可以接受
			{
				net_debug_printf(("There have cmd to receive\n"));
				udp_data_flag=1;
				UdpRecvFun(udp_cmd_socket,udp_data_flag,&CachePoint,network_buffer.udp_cmd_socket_buffer.udp_cmd_socket_recv_buffer);
		#ifdef	EAMP_DEV
				ShowCurrentTime(0);
				control_thread_flag = 0;
				PthreadPriorityChangeForSchedRr(thread_of_control_processing, 18);
			#endif
				if(strcmp(dev_info_local.dev_name,"CMU")==0)
				{
					cyg_flag_setbits(&cmd_flag_type,0x40);
				}
			} //end of FD_ISSET
			if(FD_ISSET(udp_data_socket,&frd))//检测udp的data套接字是否有数据接收
			{
//				net_debug_printf(("There have audio data to receive!!!\n"));
				udp_data_flag = 2;
				UdpRecvFun(udp_data_socket,udp_data_flag,&CachePoint,network_buffer.udp_data_socket_buffer.udp_data_socket_recv_buffer);
//				net_debug_printf(("out of udp_data_flag = 2\n"));
			}

			if(FD_ISSET(udp_common_socket,&frd))//检测udp的common套接字是否有数据接收
			{
#ifdef TCP_IP_TEST
				tcp_ip_test_count = 10 ;
#endif
				net_debug_printf(("There have common cmd to receive\n"));
				udp_data_flag = 1;

				int ret = 0;
				struct sockaddr_in recv_add = {0};
				int len_num = sizeof(common_package_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_common_socket, (void *)&common_package_buffer, len_num, 0, (struct sockaddr *)&recv_add, (socklen_t*)pToPackLen);
              if ( ret > 0)
              {
            	  net_debug_printf(("Net : receive common cmd ok \n"));
            	  ret = BlockBufferWrite(network_buffer.udp_common_socket_buffer.udp_common_socket_recv_buffer, (void *)&common_package_buffer, sizeof(common_package_buffer));
            	  if ( ret < 0 )
            	  {
            		  net_debug_printf(("Net : receive common cmd ok, but BlockBufferWrite()<0 \n"));
            	  }
              }
              else
              {
            	  net_debug_printf(("Net, comm_socket:  recvfrom() < 0 \n"));
              }

				//????添加分类接受函数
				if( ZhwNetworkRecvForward(network_buffer.udp_common_socket_buffer.udp_common_socket_recv_buffer,1) < 0)
				{
					net_debug_printf(("send package to apply is error!!!\n"));
				}
			}

			if(FD_ISSET(udp_common_big_socket,&frd))//检测udp的big common套接字是否有数据接收
			{
				net_debug_printf(("There have big common cmd to receive\n"));
				udp_data_flag = 1;

				int ret = 0;
				struct sockaddr_in recv_add = {0};
				int len_num = sizeof(common_big_package_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_common_big_socket, (void *)&common_big_package_buffer, len_num, 0, (struct sockaddr *)&recv_add, (socklen_t*)pToPackLen);
              if ( ret > 0)
              {
//            	  net_debug_printf(("Net : receive big common cmd ok \n"));
            	  ret = BlockBufferWrite(network_buffer.udp_common_socket_big_buffer.udp_common_socket_recv_big_buffer, (void *)&common_big_package_buffer, sizeof(common_big_package_buffer));
            	  if ( ret < 0 )
            	  {
            		  net_debug_printf(("Net : receive common cmd ok, but BlockBufferWrite()<0 \n"));
            	  }
              }
              else
              {
            	  net_debug_printf(("Net, comm_socket:  recvfrom() < 0 \n"));
              }

				//????添加分类接受函数
				if( ZhwNetworkRecvForward(network_buffer.udp_common_socket_big_buffer.udp_common_socket_recv_big_buffer,2) < 0)
				{
					net_debug_printf(("send package to apply is error!!!\n"));
				}
			}

			if(FD_ISSET(udp_heart_socket,&frd))//检测heart套接字是否有数据接收
			{
				net_debug_printf(("There have heart package to receive\n"));
				udp_data_flag = 1;

				int ret = 0;
				struct sockaddr_in recv_add = {0};
				int len_num = sizeof(heart_package_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_heart_socket, (void *)&heart_package_buffer, len_num, 0, (struct sockaddr *)&recv_add, (socklen_t*)pToPackLen);
              if ( ret > 0)
              {
            	  net_debug_printf(("Net : receive heart package ok--%s-%d-%d \n",heart_package_buffer.src_dev_name,heart_package_buffer.src_dev_number,heart_package_buffer.heart_package.package_type));
            	  ret = BlockBufferWrite(network_buffer.udp_heart_socket_buffer.udp_heart_socket_recv_buffer, (void *)&heart_package_buffer, sizeof(heart_package_buffer));
            	  if ( ret < 0 )
            	  {
            		  net_debug_printf(("Net : receive heart package ok, but BlockBufferWrite()<0 \n"));
            	  }

              }
              else
              {
            	  net_debug_printf(("Net, heart_socket:  recvfrom() < 0 \n"));
              }
			}

			//add by zhw 2015-12-8
#ifdef EAMP_DEV
			if(FD_ISSET(udp_dmu_socket,&frd)) {//检测dmu套接字是否有数据接收
				diag_printf("There have dmu package to receive\n");
//				udp_data_flag = 1;
				int ret = 0;
				struct sockaddr_in dmu_recv_add = {0};
				int len_num = sizeof(cmd_recvfrom_lcd_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_dmu_socket, (void *)&cmd_recvfrom_lcd_buffer, len_num, 0, (struct sockaddr *)&dmu_recv_add, (socklen_t*)pToPackLen);
              if ( ret > 0) {
//            	  net_debug_printf(("Net : receive dmu package ok--%s-%d-%d \n",cmd_recvfrom_lcd_buffer.,heart_package_buffer.src_dev_number,heart_package_buffer.heart_package.package_type));
            	  ret = BlockBufferWrite(network_buffer.udp_dmu_socket_buffer.udp_dmu_socket_recv_buffer, (void *)&cmd_recvfrom_lcd_buffer, sizeof(cmd_recvfrom_lcd_buffer));
            	  if ( ret < 0 ) {
            		  net_debug_printf(("Net : receive dmu package ok, but BlockBufferWrite()<0 \n"));
            	  }
              }
             else {
            	  net_debug_printf(("Net, dmu_socket:  recvfrom() < 0 \n"));
              }
			}
#endif
			//end of add

			//add by zhw 2015-12-8
//			if(FD_ISSET(udp_monitor_socket,&frd)) {//检测monitor套接字是否有数据接收
//				diag_printf("There have monitor package to receive\n");
////				udp_data_flag = 1;
//				int ret = 0;
//				struct sockaddr_in monitor_recv_add = {0};
//				int len_num = sizeof(cmd_recvfrom_lcd_buffer);
//				int *pToPackLen = &len_num;
//				ret = recvfrom(udp_dmu_socket, (void *)&cmd_recvfrom_lcd_buffer, len_num, 0, (struct sockaddr *)&monitor_recv_add, (socklen_t*)pToPackLen);
//              if ( ret > 0) {
////            	  net_debug_printf(("Net : receive dmu package ok--%s-%d-%d \n",cmd_recvfrom_lcd_buffer.,heart_package_buffer.src_dev_number,heart_package_buffer.heart_package.package_type));
//            	  ret = BlockBufferWrite(network_buffer.udp_dmu_socket_buffer.udp_dmu_socket_recv_buffer, (void *)&cmd_recvfrom_lcd_buffer, sizeof(cmd_recvfrom_lcd_buffer));
//            	  if ( ret < 0 ) {
//            		  net_debug_printf(("Net : receive dmu package ok, but BlockBufferWrite()<0 \n"));
//            	  }
//              }
//             else {
//            	  net_debug_printf(("Net, dmu_socket:  recvfrom() < 0 \n"));
//              }
//			}
			//end of add

			//add by zhw_2013_10_21 接受从交换机trap套接字收到的trap数据，并封装成common通用包，上传到应用层。
//			if(strcmp(dev_info_local.dev_name,"CCU")==0)
//			{
//				if(FD_ISSET(trap_recv_socket,&frd))
//				{
//					net_debug_printf(("There have switch trap cmd to receive\n"));
//					if(recvfrom(trap_recv_socket,&switch_trap_info_t,sizeof(switch_trap_info_t),0,(struct sockaddr *)&trap_recv_addr,&trap_len) < 0)
//					{
//						net_debug_printf(("trap_recv_socket receive data is err!!!\n"));
//					}
//					else
//					{
//						common_pkg_trap_info.pkg_type = 1 ;
//						common_pkg_trap_info.common_data_u.heart_package.package_type = 3 ;
//						common_pkg_trap_info.common_data_u.heart_package.dev_info_u.switch_info.switch_status = switch_trap_info_t ;
//						if ( (BlockBufferWrite(network_buffer.udp_common_socket_buffer.udp_common_socket_recv_buffer, common_pkg_trap_info, len)) < 0 )
//						{
//							net_debug_printf(("BlockBufferWrite: write switch trap data to network common buffer is error\n" ));
//						}
//						else
//						{
//							if( ZhwNetworkRecvForward(network_buffer.udp_common_socket_buffer.udp_common_socket_recv_buffer) < 0)
//							{
//								net_debug_printf(("send switch trap package to apply is error!!!\n"));
//							}
//						}
//					}
//				}
//			}

			if( FD_ISSET(udp_ccu_videolink_socket, &frd) ) {
				//diag_printf("There have monitor package to receive\n");
				int ret = 0;
				struct sockaddr_in monitor_recv_add = {0};
				int len_num = sizeof(message_from_monitor_buffer);
				int *pToPackLen = &len_num;
				ret = recvfrom(udp_ccu_videolink_socket, (void *)&message_from_monitor_buffer, len_num, 0, (struct sockaddr *)&monitor_recv_add, (socklen_t*)pToPackLen);
	         	if ( ret > 0) {
	            	  //net_debug_printf2(("Net : receive monitor package ok--%d-%d-%d \n",message_from_monitor_buffer[0],message_from_monitor_buffer[1],message_from_monitor_buffer[2]));
		       	  ret = BlockBufferWrite(network_buffer.udp_monitor_socket_buffer.udp_monitor_socket_recv_buffer, (void *)&message_from_monitor_buffer, sizeof(message_from_monitor_buffer));
		       	  if ( ret < 0 ) {
		       		  //net_debug_printf2(("Net : receive monitor package ok, but BlockBufferWrite()<0 \n"));
		       	  }
		         }
	            else {
	           	  net_debug_printf2(("Net, monitor_socket:  recvfrom() < 0 \n"));
	             }
			}

			if(FD_ISSET(udp_shell_socket,&frd))//检测udp的shell套接字是否有数据接收？？？？？？？
			{
			    int shell_recv_bytes = 0;
				net_debug_printf(("There shell socket have data to receive-2016\n"));
				net_debug_printf2(("There shell socket have data to receive-2016\n"));
				shell_recv_bytes = recvfrom(udp_shell_socket, (void *)shell_recv_buffer,
						                    sizeof(shell_recv_buffer),0,
						                    (struct sockaddr *)&udp_shell_client_addr, &len);
				if( shell_recv_bytes < 0 )
				{
					net_debug_printf2(("udp_shell_socket receive data is err!!!\n"));
				}
				else
				{
					//if(strlen(shell_recv_buffer) > sizeof(shell_recv_buffer) )
					if( shell_recv_bytes > sizeof(shell_recv_buffer) )
					{
						//net_debug_printf(("the mast shell common long is 20 ,now the common of into is :%d!!!\n",strlen(shell_recv_buffer)));
						net_debug_printf2(("the mast shell common long is 20 ,now the common of into is :%d!!!\n", shell_recv_bytes));
					}
					else
					{
					       if( shell_recv_buffer[0] >= 0x7A ) 
					       {
						       	int jj = 0;
						       	net_debug_printf2(("<QQ-0311,shell TCMS test> len=%d\n", shell_recv_bytes));
							//memcpy((void *)test_fromPTU_to_tcms,(void *)shell_recv_buffer,51);
							for(jj=0; jj<shell_recv_bytes; jj++)
							{
							    if( jj%8==0 )
							    	net_debug_printf2(("\n"));
								net_debug_printf2(("0x%02x,", shell_recv_buffer[jj]));
							}
						}
					       else 
						{
							int ret = BlockBufferWrite(network_buffer.udp_shell_socket_buffer.udp_shell_socket_recv_buffer, (void *)&shell_recv_buffer, sizeof(shell_recv_buffer));
							if ( ret < 0 )
							{                      
								 net_debug_printf(("Net : receive shell cmd ok, but BlockBufferWrite()<0 \n"));
							}
						}
					}
				}
			}

			if(FD_ISSET(udp_cmd_socket,&fwd))
			{
				//发送控制信息
				if(BlockBufferRead(network_buffer.udp_cmd_socket_buffer.udp_cmd_socket_send_buffer,(void *)&network_send_cmd_buf,sizeof(network_send_cmd_buf))>0)
				{
					net_debug_printf2(("!!!!!!!!!!!!!!-%s\n",network_send_cmd_buf.dst_devices_name));

					tar_dev_info = ReadGlobalArray(0,network_send_cmd_buf.dst_devices_name,network_send_cmd_buf.dst_devices_no,2);
					net_debug_printf2(("BCU,net cmd:  tar_ip is : %s\n",tar_dev_info.dev_ip));
					UdpSendFunCMD(udp_cmd_socket,&network_send_cmd_buf,sizeof(network_send_package_t),tar_dev_info.dev_ip,UDP_CMD_PORT);//发送控制数据
				}
			}

			if(FD_ISSET(udp_data_socket,&fwd))
			{
				//发送音频信息
				read_data_long = CharBufferRead(network_buffer.udp_data_socket_buffer.udp_data_socket_send_buffer,(void *)data_buffer_send,sizeof(data_buffer_send));
				if( read_data_long > 0)
				{
//					net_debug_printf(("BCU,net:  send audio, bytes = %d\n",read_data_long));
					DSTDeviceNameAndNumber(device_name_send_data,&device_number_send_data);
					tar_dev_info = ReadGlobalArray(0,device_name_send_data,device_number_send_data,2);
//					net_debug_printf(("BCU,net:  send audio, dst-ip = %s\n", tar_dev_info.dev_ip));
					UdpSendFunDATA(udp_data_socket,data_buffer_send,read_data_long,tar_dev_info.dev_ip,UDP_DATA_PORT);//发送音频数据给EAMP
					if(strcmp("PCU",device_name_send_data) == 0)
					{
						tar_dev_info = ReadGlobalArray(0,"CC",230,2);
						UdpSendFunDATA(udp_data_socket,data_buffer_send,read_data_long,tar_dev_info.dev_ip,UDP_DATA_PORT);//发送音频数据给CC
					}
				}
			}

			if(FD_ISSET(udp_common_socket, &fwd))
			{
				//发送common 数据
				if(BlockBufferRead(network_buffer.udp_common_socket_buffer.udp_common_socket_send_buffer,(void *)&common_package_buffer,sizeof(common_package_buffer))>0)
				{
					net_debug_printf(("Comm: !!!!!!!!!!!!!!-%s_%d\n",common_package_buffer.dst_dev_name, common_package_buffer.dst_dev_number));
					net_debug_printf(("Comm: !!!!!!!!!!!!!!-pkg_type=%d\n",common_package_buffer.pkg_type));

					tar_dev_info = ReadGlobalArray(0,common_package_buffer.dst_dev_name,common_package_buffer.dst_dev_number,2);
					net_debug_printf(("Comm:  tar_ip is : %s\n",tar_dev_info.dev_ip));
					UdpSendFunCMD(udp_common_socket,&common_package_buffer,sizeof(common_package_t),tar_dev_info.dev_ip,UDP_COMMON_PORT);//发送控制数据
				}
			}

			if(FD_ISSET(udp_common_big_socket, &fwd))
			{
				//发送big common 数据
				if(BlockBufferRead(network_buffer.udp_common_socket_big_buffer.udp_common_socket_send_big_buffer,(void *)&common_big_package_buffer,sizeof(common_big_package_buffer))>0)
				{
					net_debug_printf(("Big Comm: !!!!!!!!!!!!!!-%s_%d\n",common_big_package_buffer.dst_dev_name, common_big_package_buffer.dst_dev_number));
					if(12 == common_big_package_buffer.pkg_type)
					{
						UdpSendFunCMD(udp_common_big_socket,&common_big_package_buffer,sizeof(common_big_package_t),"192.168.10.2",UDP_COMMON_BIG_PORT);//发送控制数据
					}
					else
					{
						tar_dev_info = ReadGlobalArray(0,common_big_package_buffer.dst_dev_name,common_big_package_buffer.dst_dev_number,2);
						UdpSendFunCMD(udp_common_big_socket,&common_big_package_buffer,sizeof(common_big_package_t),tar_dev_info.dev_ip,UDP_COMMON_BIG_PORT);//发送控制数据
					}
				}
			}

			if(FD_ISSET(udp_heart_socket, &fwd))
			{
				//发送heart 数据
				if(BlockBufferRead(network_buffer.udp_heart_socket_buffer.udp_heart_socket_send_buffer,(void *)&heart_package_buffer,sizeof(heart_package_buffer))>0)
				{
					net_debug_printf(("Heart send: !!!!!!!!!!!!!!-%s_%d\n",heart_package_buffer.dst_dev_name, heart_package_buffer.dst_dev_number));
					tar_dev_info = ReadGlobalArray(0,heart_package_buffer.dst_dev_name,heart_package_buffer.dst_dev_number,2);
					net_debug_printf(("Heart:  tar_ip is : %s\n",tar_dev_info.dev_ip));
					UdpSendFunCMD(udp_heart_socket,&heart_package_buffer,sizeof(network_heart_package_t),tar_dev_info.dev_ip,UDP_HEART_PORT);
				}
			}

			//add by zhw 2015-12-8
#ifdef EAMP_DEV
			if(FD_ISSET(udp_dmu_socket,&fwd)) {//发送dmu data信息
				if(BlockBufferRead(network_buffer.udp_dmu_socket_buffer.udp_dmu_socket_send_buffer,(void *)&cmd_sendto_lcd_buffer,sizeof(cmd_sendto_lcd_buffer))>0)
				{
					diag_printf("network read dmu data from control is ok !\n");
					tar_dev_info = ReadGlobalArray(0,"DMU",225,2);
					net_debug_printf(("BCU,net cmd:  tar_ip is : %s\n",tar_dev_info.dev_ip));
					UdpSendFunCMD(udp_dmu_socket,&cmd_sendto_lcd_buffer,sizeof(eamp_to_LCD_t),"192.168.10.255",UDP_DMU_PORT);//发送控制数据
				}
			}
#endif
			//end of add

			//add by zhw 2015-12-23
			#ifdef ZHW_CONFIG_MONITOR_TCP 
			if(FD_ISSET(tcp_client_monitor_socket,&fwd)) {//发送monitor data信息
				if(CharBufferRead(network_buffer.udp_monitor_socket_buffer.udp_monitor_socket_send_buffer,(void *)message_to_monitor_buffer,sizeof(message_to_monitor_buffer))>0)
				{
					diag_printf("network read dmu data from control is ok !\n");
//					tar_dev_info = ReadGlobalArray(0,"DMU",225,2);
//					net_debug_printf(("BCU,net cmd:  tar_ip is : %s\n",tar_dev_info.dev_ip));
					send(tcp_client_monitor_socket,message_to_monitor_buffer,sizeof(message_to_monitor_buffer),0);
//					UdpSendFunCMD(udp_monitor_socket,message_to_monitor_buffer,sizeof(message_to_monitor_buffer),MONITOR_HOST_1_IP,UDP_MONITOR_PORT);//发送控制数据
//					UdpSendFunCMD(udp_monitor_socket,message_to_monitor_buffer,sizeof(message_to_monitor_buffer),MONITOR_HOST_2_IP,UDP_MONITOR_PORT);//发送控制数
				}
			}
			#endif
			//end of add

                     #ifdef CONFIG_CCU_VIDEOLINK
			   if( FD_ISSET(udp_ccu_videolink_socket, &fwd) ) {
			   	 char send_vlink_buffer[1024];   
				 int cmd_len = BlockBufferRead(network_buffer.udp_ccu_videolink_buffer.udp_ccu_videolink_buffer_tx, 
				 	                       (void *)send_vlink_buffer, sizeof(send_vlink_buffer));
				 if( cmd_len >0)
				{
				       if( cmd_len >= 1024 )
					   	cmd_len = 1024;
					diag_printf("send_vlink_buffer: cmd_len=%d, to=%s_%d\n", cmd_len,
						    "230.20.20.77", 52089);
					UdpSendFunCMD(udp_ccu_videolink_socket, send_vlink_buffer, cmd_len,
						"230.20.20.77", 52089);
				}
				 cmd_len = BlockBufferRead(network_buffer.udp_monitor_socket_buffer.udp_monitor_socket_send_buffer,
				 	   (void *)message_to_monitor_buffer, sizeof(message_to_monitor_buffer));
				 if( cmd_len > 0)
				 {
					diag_printf("send to monitor: cmd_len=%d, to=%s_%d\n", cmd_len,
						    "230.20.20.77", 52089);
                                        #if 0 ///< debug
					int j = 0;
	                                for(j=0; j<cmd_len; ) {
                                            diag_printf("<2016-net>send: %02x, %02x, %02x\n", 
						 message_to_monitor_buffer[j++], 
                                                 message_to_monitor_buffer[j++], 
                                                 message_to_monitor_buffer[j++]);
	                                }
                                        #endif
					UdpSendFunCMD(udp_ccu_videolink_socket, message_to_monitor_buffer, cmd_len,
						"230.20.20.77", 52089);
				 }
			   }                        			
                     #endif

			if(FD_ISSET(udp_shell_socket,&fwd))
			{
				//发送shell控制信息
				//if(BlockBufferRead(network_buffer.udp_shell_socket_buffer.udp_shell_socket_send_buffer,&network_send_cmd_buf,sizeof(network_send_cmd_buf))>0)
				
				memset(shell_send_buffer,0,sizeof(shell_send_buffer));
				if(BlockBufferRead(network_buffer.udp_shell_socket_buffer.udp_shell_socket_send_buffer,(void *)&shell_send_buffer,sizeof(shell_send_buffer))>0)
				{
					net_debug_printf(("There shell socket have data to send-2016\n"));
					int ret = sendto(udp_shell_socket, (void *)shell_send_buffer,
													sizeof(network_shell_package_t),0,
													(struct sockaddr *)&udp_shell_client_addr, len);
					if(ret < 0)
					{
						net_debug_printf(("udp_shell_socket send data is err!!!\n"));
					}
				}
			}
		}
#ifdef BCU_DEV
        PthreadPriorityChangeForSchedRr(thread_of_network, 10);
#endif
#ifdef PCU_DEV
        PthreadPriorityChangeForSchedRr(thread_of_network, 10);
#endif
        sched_yield();//释放此次线程控制权
	}
	
	CacheFree(CachePoint);
}

char *zhw_network_get_local_ip(void)
{
	struct dev_type_and_no local_device_io;
	struct global_info local_dev_info_local;
	
	local_device_io = ReadFromIO();
	local_dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,local_device_io.dev_number,1);
	debug_print(("zhw_networ_get_local_ip: %d-%d-%s\n", local_device_io.dev_type, 
		               local_device_io.dev_number, local_dev_info_local.dev_ip));

	return  local_dev_info_local.dev_ip;
}
int zhw_dev_weather_server(int a)
{
	struct dev_type_and_no local_device_io;
	struct global_info local_dev_info_local;
	
	local_device_io = ReadFromIO();
	local_dev_info_local = ReadGlobalArray(local_device_io.dev_type,NULL,local_device_io.dev_number,1);
	debug_print(("zhw_dev_weather_server: %d-%d-%s\n", local_device_io.dev_type, local_device_io.dev_number, local_dev_info_local.dev_ip));

	return  local_dev_info_local.is_server;

}
char *zhw_server_dev_info(int dev_type, int dev_number)
{
	struct global_info local_dev_info_local;
	
	local_dev_info_local = ReadGlobalArray(dev_type, NULL, dev_number, 1);
	debug_print(("zhw_server_dev_info: %d_%d_%s \n", dev_type, dev_number, local_dev_info_local.dev_ip));

	return  local_dev_info_local.dev_ip;

}

void ForceBUfferData_wilson()
{
//	BufferPoint->ArrayToHandle,&(BufferPoint->pToData
	ForceClearDataCache(p_buffer_pointer->ArrayToHandle,&(p_buffer_pointer->pToData));
}
void ForceGetBufferData_wilson()
{
	ForcePushDataCacheToBuffer(p_buffer_pointer->ArrayToHandle,&(p_buffer_pointer->pToData),net_recv_buffer_id_wilson);
}

struct global_info zhw_server_info(int a)
{
	struct global_info local_dev_info_local = {0};
	if( (a==1) || (a==2) )
	{
		local_dev_info_local = ResolveUseServerNumber(a);
	}
	 
	return local_dev_info_local;
}

