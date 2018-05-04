/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_dev_selftest_module.c
 * 文件标识：
 * 摘    要： 设备自检模块主函数
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2013-03-26
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */
#include "zhw_dev_selftest_module.h"

extern int eamp_number;
extern int bcu_number;
extern int ccu_number;
extern int pcu_number;
extern int cmu_number;
//int heart_beat_other_ccu_no;
cyg_handle_t heart_counter;
cyg_handle_t heart_alarm;
cyg_alarm heart_alarm_buffer;

//add for test --2015-1-9
//unsigned char chang_send_cmd_type = 0 ;
//unsigned char cmd_changed = 0 ;
//end of add for test --2015-1-9

char *bcu_name = "BCU";
char *eamp_name = "EAMP";
char *pcu_name = "PCU";
char *cmu_name = "CMU";

char *heart_dst_name = NULL;
int heart_dst_num = 0;
int malloc_loop = 0 ;

char *current_dev_name = NULL;
int current_dev_num = 0 ;

//add by zhw dev status CCU send to BCU
g_dev_fault_info_t g_dev_fault_info = {0};

//add by zhw 2014-5-20
current_time_t g_time_info;

int heart_type = 0 ;
int heart_beat = 0;
int send_to_bcu = SEND_TO_BCU ;
int heart_beat_dev_status = 0 ;

int heart_thread_loop_number = 0;

network_heart_package_t heart_send_buffer;
network_heart_package_t heart_recv_buffer;
common_big_package_t dev_fault_send_buffer;

int heart_recv_buffer_id;
int heart_send_buffer_id;
int dev_fault_send_buffer_id;

//用于表示网络链接状态的变量,上电默认在线.
int client_net_state = 1 ; // client network status
int primary_server_online = 1 ; // server network status
int other_ccu_tcms_flag = 1;

int ZhwGetClientNS()///<0:off line  1:on line
{
	return client_net_state ;
}
int ZhwSetClientNS(int set_net_state)
{
	client_net_state = set_net_state;
	return 0 ;
}

int ZhwGetPrimaryServerNS(void)
{
	return primary_server_online ; ///< <0>: server CCU is offline;  <1>: sever CCU is online
}

int OtherCCUTCMSState()
{
	return  other_ccu_tcms_flag;
}

int ZhwSetPrimaryServerNS(int primary_server_net_state)
{
	primary_server_online = primary_server_net_state;
	return 0 ;
}

/*
 *   	@brief	本函数的功能是打印设备状态表中的设备状态，每次只能打印一种类型的设备；
 *     @param dev_name 是字符指针，表示要打印的设备类型名；
 *     @param dev_list_head 	是设备类型表的头指针，根据不同的设备类型，其类型也不一样
 *     @return    1:运行正常，退出；
 *     				-1：设备名异常；
 */
int ZhwDevStatusPrint(char *dev_name,void *dev_list_head)
{
//	return 0;
	if(strcmp(dev_name,"EAMP") == 0 )
	{
		eamp_dev_info_t *eamp_dev_info_head;
		eamp_dev_info_head = (eamp_dev_info_t *)dev_list_head;
		while(eamp_dev_info_head != NULL)
		{
			if((eamp_dev_info_head->dev_number > 0) && (eamp_dev_info_head->dev_number < 200) )//&& (eamp_dev_info_head->reply == 1) )
			{
				net_debug_printf(("device info = %s-%d-%d\n",dev_name,eamp_dev_info_head->dev_number,eamp_dev_info_head->eamp_info.online));
//				diag_printf("per-dev-info=%d-%d-%d-%d-%d-%d-%d\n",
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.dm_failure_info,
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.in_sider_led1_failure_info,
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.in_sider_led2_failure_info,
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.nd1_failure_info,
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.nd2_failure_info,
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.outer_sider_led1_failure_info,
//						eamp_dev_info_head->eamp_info.eamp_per_dev_failure_info.outer_sider_led2_failure_info);
			}
			eamp_dev_info_head = eamp_dev_info_head->next ;
		}
	}
	else if(strcmp(dev_name,"BCU") == 0 )
	{
		bcu_dev_info_t *bcu_dev_info_head;
		bcu_dev_info_head = (bcu_dev_info_t *)dev_list_head;
		while(bcu_dev_info_head != NULL)
		{
			if( (bcu_dev_info_head->dev_number > 0 ) && (bcu_dev_info_head->dev_number < 200 ) )//&& (bcu_dev_info_head->reply == 1))
			{
				net_debug_printf(("device info = %s-%d-%d\n",dev_name,bcu_dev_info_head->dev_number,bcu_dev_info_head->bcu_info.online));
			}
			bcu_dev_info_head = bcu_dev_info_head->next ;
		}
	}
	else if(strcmp(dev_name,"CCU") == 0 )
	{
		ccu_dev_info_t *ccu_dev_info_head ;
		ccu_dev_info_head = (ccu_dev_info_t *)dev_list_head ;
		while(ccu_dev_info_head != NULL)
		{
			if( (ccu_dev_info_head->dev_number > 0) && (ccu_dev_info_head->dev_number < 200) )//&& (ccu_dev_info_head->reply == 1))
			{
				net_debug_printf(("device info = %s-%d-%d\n",dev_name,ccu_dev_info_head->dev_number,ccu_dev_info_head->ccu_info.online));
//				diag_printf("per-dev-info=%d\n",	ccu_dev_info_head->ccu_info.ccu_per_dev_failure_info.fdu_failure_info);
			}
			ccu_dev_info_head = ccu_dev_info_head->next ;
		}
	}
	else if(strcmp(dev_name,"PCU") == 0 )
	{
		pcu_dev_info_t *pcu_dev_info_head;
		pcu_dev_info_head = (pcu_dev_info_t *)dev_list_head;
		while(pcu_dev_info_head != NULL)
		{
			if( (pcu_dev_info_head->dev_number > 0) && (pcu_dev_info_head->dev_number < 200) )//&& (pcu_dev_info_head->reply == 1))
			{
				net_debug_printf(("device info = %s-%d-%d\n",dev_name,pcu_dev_info_head->dev_number,pcu_dev_info_head->pcu_info.online));
			}
			pcu_dev_info_head = pcu_dev_info_head->next ;
		}
	}
	else if(strcmp(dev_name,"CMU") == 0 )
	{
		cmu_dev_info_t *cmu_dev_info_head;
		cmu_dev_info_head = (cmu_dev_info_t *)dev_list_head;
		while(cmu_dev_info_head != NULL)
		{
			if( (cmu_dev_info_head->dev_number > 0) && (cmu_dev_info_head->dev_number < 200) )//&& (pcu_dev_info_head->reply == 1))
			{
				net_debug_printf(("device info = %s-%d-%d\n",dev_name,cmu_dev_info_head->dev_number,cmu_dev_info_head->cmu_info.online));
			}
			cmu_dev_info_head = cmu_dev_info_head->next ;
		}
	}
	else
	{
		net_debug_printf(("this device name :%s is unknown\n",dev_name));
		return -1 ;
	}
	return 1;
}

void ZhwSendDevStatusToBCUOrCCU(int dev_fault_send_buffer_id,struct global_info dev_info,char *dev_name)
{
	dev_status_info_t get_local_dev_info;
	get_local_dev_info.bcu_dev_info_head = dev_status_info.bcu_dev_info_head;
	get_local_dev_info.ccu_dev_info_head = dev_status_info.ccu_dev_info_head;
	get_local_dev_info.eamp_dev_info_head = dev_status_info.eamp_dev_info_head;
	get_local_dev_info.pcu_dev_info_head = dev_status_info.pcu_dev_info_head;
	get_local_dev_info.cmu_dev_info_head = dev_status_info.cmu_dev_info_head; //add by zhw 2014-12-12
	int loop = 0 ;

	ZhwDevStatusPrint("EAMP",dev_status_info.eamp_dev_info_head);
	ZhwDevStatusPrint("CCU",dev_status_info.ccu_dev_info_head);
	ZhwDevStatusPrint("BCU",dev_status_info.bcu_dev_info_head);
	ZhwDevStatusPrint("PCU",dev_status_info.pcu_dev_info_head);
	ZhwDevStatusPrint("CMU",dev_status_info.cmu_dev_info_head);

	for(loop = 0 ; loop < CONFIG_BCU_NUM ; loop ++)
	{
		while(get_local_dev_info.bcu_dev_info_head != NULL)
		{
			if(get_local_dev_info.bcu_dev_info_head->dev_number == (loop + 1))
			{
				g_dev_fault_info.bcu_info[loop] = get_local_dev_info.bcu_dev_info_head->bcu_info;
//				get_local_dev_info.bcu_dev_info_head->bcu_info.online = 0 ;
				break;
			}
			else
			{
				get_local_dev_info.bcu_dev_info_head = get_local_dev_info.bcu_dev_info_head->next ;
			}
		}
		get_local_dev_info.bcu_dev_info_head = dev_status_info.bcu_dev_info_head;
	}
	for(loop = 0 ; loop < CONFIG_CCU_NUM ; loop ++)
	{
		while(get_local_dev_info.ccu_dev_info_head != NULL)
		{
			if(get_local_dev_info.ccu_dev_info_head->dev_number == (loop + 1))
			{
				g_dev_fault_info.ccu_info[loop] = get_local_dev_info.ccu_dev_info_head->ccu_info;
				//add by zhw for test 2014-11-12
//				if(dev_info.dev_number != get_local_dev_info.ccu_dev_info_head->dev_number)
//				{
//					get_local_dev_info.ccu_dev_info_head->ccu_info.online = 0 ;
//				}
//				get_local_dev_info.ccu_dev_info_head->ccu_info.online = 0 ;
				break;
			}
			else
			{
				get_local_dev_info.ccu_dev_info_head = get_local_dev_info.ccu_dev_info_head->next ;
			}
		}
		get_local_dev_info.ccu_dev_info_head = dev_status_info.ccu_dev_info_head;
	}
	for(loop = 0 ; loop < CONFIG_EAMP_NUM ; loop ++)
	{
		//diag_printf("test eamp loop time,now the cmd_changed number =%d\n",cmd_changed);
		while(get_local_dev_info.eamp_dev_info_head != NULL)
		{
			if(get_local_dev_info.eamp_dev_info_head->dev_number == (loop + 1))
			{
				g_dev_fault_info.eamp_info[loop] = get_local_dev_info.eamp_dev_info_head->eamp_info;

				//add for test --2015-1-19
//				if(0 == (cmd_changed%3) )
//				{
//					if(3 == get_local_dev_info.eamp_dev_info_head->dev_number)
//					{
//						g_dev_fault_info.eamp_info[loop].online = 1 ;
//					}
//					else
//					{
//						g_dev_fault_info.eamp_info[loop].online = 0 ;
//					}
//				}
//				else if(1 == (cmd_changed%3))
//				{
//					g_dev_fault_info.eamp_info[loop].online = 1 ;
//				}
//				else if(2 == (cmd_changed%3))
//				{
//					g_dev_fault_info.eamp_info[loop].online = 0 ;
//				}
				//end of add for test --2015-1-9

				//				get_local_dev_info.eamp_dev_info_head->eamp_info.online = 0 ;
				break;
			}
			else
			{
				get_local_dev_info.eamp_dev_info_head = get_local_dev_info.eamp_dev_info_head->next ;
			}
		}
		get_local_dev_info.eamp_dev_info_head = dev_status_info.eamp_dev_info_head;
	}
	for(loop = 0 ; loop < CONFIG_PECU_NUM ; loop++)
	{
		while(get_local_dev_info.pcu_dev_info_head != NULL)
		{
			if(get_local_dev_info.pcu_dev_info_head->dev_number == (loop + 1))
			{
				g_dev_fault_info.pcu_info[loop] = get_local_dev_info.pcu_dev_info_head->pcu_info;
//				get_local_dev_info.pcu_dev_info_head->pcu_info.online = 0 ;

				//add for test --2015-1-9
//				if(0 == (cmd_changed%3) )
//				{
//					if(2 == get_local_dev_info.pcu_dev_info_head->dev_number)
//					{
//						g_dev_fault_info.pcu_info[loop].online = 1 ;
//					}
//					else
//					{
//						g_dev_fault_info.pcu_info[loop].online = 0 ;
//					}
//				}
//				else if(1 == (cmd_changed%3))
//				{
//					g_dev_fault_info.pcu_info[loop].online = 0 ;
//				}
//				else if(2 == (cmd_changed%3))
//				{
//					g_dev_fault_info.pcu_info[loop].online = 1 ;
//				}
				//end of add for test --2015-1-9

				break;
			}
			else
			{
				get_local_dev_info.pcu_dev_info_head = get_local_dev_info.pcu_dev_info_head->next ;
			}
		}
		get_local_dev_info.pcu_dev_info_head = dev_status_info.pcu_dev_info_head;
	}
	for(loop = 0 ; loop < cmu_number ; loop ++)
	{
		while(get_local_dev_info.cmu_dev_info_head != NULL)
		{
			if(get_local_dev_info.cmu_dev_info_head->dev_number == (loop + 1))
			{
				g_dev_fault_info.cmu_info[loop] = get_local_dev_info.cmu_dev_info_head->cmu_info;
				break;
			}
			else
			{
				get_local_dev_info.cmu_dev_info_head = get_local_dev_info.cmu_dev_info_head->next ;
			}
		}
		get_local_dev_info.cmu_dev_info_head = dev_status_info.cmu_dev_info_head;
	}

	//for test--print the device info of the send buffer--add by zhw-2015-1-13
	{
//		diag_printf("bcu1-%d,bcu2-%d\n",g_dev_fault_info.bcu_info[0].online,g_dev_fault_info.bcu_info[1].online);
//		diag_printf("ccu1-%d,ccu2-%d\n",g_dev_fault_info.ccu_info[0].online,g_dev_fault_info.ccu_info[1].online);
//		diag_printf("eamp1-%d,eamp2-%d,eamp3-%d,eamp4-%d,eamp5-%d,eamp6-%d\n",
//				g_dev_fault_info.eamp_info[0].online,g_dev_fault_info.eamp_info[1].online,
//				g_dev_fault_info.eamp_info[2].online,g_dev_fault_info.eamp_info[3].online,
//				g_dev_fault_info.eamp_info[4].online,g_dev_fault_info.eamp_info[5].online);
	}

	dev_fault_send_buffer.common_big_data_u.dev_falut_info = g_dev_fault_info;
	dev_fault_send_buffer.pkg_type = COMMON_PACKAGE_TYPE_ALL_DEV_STATE ;
	strcpy(dev_fault_send_buffer.src_dev_name , dev_info.dev_name);
//	strcpy(dev_fault_send_buffer.dst_dev_name , "BCU");
	strcpy(dev_fault_send_buffer.dst_dev_name , dev_name);
	dev_fault_send_buffer.src_dev_number = dev_info.dev_number;
	dev_fault_send_buffer.dst_dev_number = MUL_SYNC_DST_NO;

	if(BlockBufferWrite( dev_fault_send_buffer_id, (void *)&dev_fault_send_buffer, sizeof(dev_fault_send_buffer) ) > 0)
	{
		if(ZhwNetworkSendForward(dev_fault_send_buffer_id,2) < 0)
		{
			net_debug_printf(("apply send dev_fault_to_bcu package to network is error !!!\n"));
		}
		else
		{
			net_debug_printf(("send a dev_fault package\n"));
		}
	}
	else {
		net_debug_printf(("send a dev_fault package error\n"));
	}
}

int ZhwServiceBufferSet(void)
{
	int dev_status_buffer_id = 0 ;
	dev_status_buffer_id = ZhwApplyBufferCreate(1,2,sizeof(common_big_package_t),"dev-status-send-buffer");
	if(dev_status_buffer_id > 0 )
	{
		ZhwBufferRegister(2,8,2,dev_status_buffer_id);
	}
	return dev_status_buffer_id ;
}

/*
 *   	@brief	本函数的功能是向heart服务器ccu1发送客户端查询包；
 *     @param dev_info 		struct global_info，本地设备信息；
 *     @param pkg_counte 		int，已发送查询包号 ；
 *     @return    int:1--表示成功，-1--表示socket创建失败；-2--表示发送指令给server失败；-3--表示发送指令给自己失败；-4--表示发送
 *     指令给server和自己都失败。
 */
int ZhwProtocolTest(struct global_info dev_info,int pkg_counte)
{
	int sock_protocol_test = 0 ;
	int send_to_server_ret = 1 ;
	int send_to_itself_ret = 1 ;
	struct sockaddr_in sock_protocol_test_addr ;

	struct global_info server_info;

	server_info = ReadGlobalArray(0,"CCU",1,2);

	if( (sock_protocol_test = socket(AF_INET,SOCK_DGRAM,0) ) < 0 )
	{
//		diag_printf("test socket create failed!!!\n");
		return -1 ;
	}

	sock_protocol_test_addr = IpAndPortInit(server_info.dev_ip,(30004));
	ZhwServiceSendBufferInit("CCU",1,dev_info.dev_name,dev_info.dev_number,1,3,pkg_counte);
	if( (sendto(sock_protocol_test,&heart_send_buffer,sizeof(heart_send_buffer),0,(struct sockaddr *)&sock_protocol_test_addr,sizeof(struct sockaddr_in)) ) < 0)
	{
		send_to_server_ret = 0 ;
		perror("send client request common to heart-server ccu-1 is failed!!!--- :\n");
	}
	else
	{
//		diag_printf("send data to server is OK!!!\n");
	}

	sock_protocol_test_addr = IpAndPortInit(dev_info.dev_ip,(30004));
	ZhwServiceSendBufferInit(dev_info.dev_name,dev_info.dev_number,dev_info.dev_name,dev_info.dev_number,1,3,pkg_counte);
	if( (sendto(sock_protocol_test,&heart_send_buffer,sizeof(heart_send_buffer),0,(struct sockaddr *)&sock_protocol_test_addr,sizeof(struct sockaddr_in)) ) < 0)
	{
		send_to_itself_ret = 0 ;
		perror("send data to own is also failed!!!--- :\n");
	}
	else
	{
//		diag_printf("send data to own is OK!!!\n");
	}

	if( (send_to_server_ret  && send_to_itself_ret) == 1 )
	{
		close(sock_protocol_test);
		return 1;
	}
	else if( (send_to_server_ret  || send_to_itself_ret) == 0)
	{
		close(sock_protocol_test);
		return -4;
	}
	else if(send_to_server_ret == 0)
	{
		close(sock_protocol_test);
		return -2;
	}
	else
	{
		close(sock_protocol_test);
		return -3;
	}
}


void ZhwAlarmFunction(cyg_handle_t alarm,cyg_addrword_t data)
{
//	heart_beat = 1 ;
	switch(heart_type)
	{
	case FAIL_TIME_HEART_TYPE:
		SetFailureTimeFlag();
		heart_type++;
		heart_beat = 1 ;
		break;
	case SEND_TO_BCU_TYPE:
//		send_to_bcu = 1 ;
		send_to_bcu--;
		heart_type++;
		break;
	case (MAX_TYPE + 1):
		heart_type = 0 ;
		heart_beat = 1 ;
		heart_beat_dev_status = 1 ; // add by zhw-2015-3-18 for send mess to tcms
		break;
	default:
		heart_beat = 1 ;
		if(heart_type > (MAX_TYPE + 1))
		{
			perror("the heart type is error!!!\n");
		}
		else
		{
			heart_type++;
		}
		break;
	}
}

void ZhwAlarmInit(void)
{
	cyg_clock_to_counter(cyg_real_time_clock(),&heart_counter);
	cyg_alarm_create(heart_counter,ZhwAlarmFunction,0,&heart_alarm,&heart_alarm_buffer);
	cyg_alarm_initialize(heart_alarm,cyg_current_time()+HEART_TIME,HEART_TIME);
}

/*
 *   	@brief	本函数的功能是初始化发送buffer的头信息，具体的数据由个设备自行赋值；
 *     @param dst_dev_name 		char*，表示发送的目的设备名；
 *     @param dst_dev_number 	int，表示发送的目的设备号 ；
 *     @param src_dev_name 		char*，表示发送的源设备名；
 *     @param src_dev_number 	int，表示发送的源设备号；
 *     @param pkg_type 			int，表示发送的通用包类型（只能是1表示是心跳包）；
 *     @param heart_pkg_type 	int，表示发送的通用包里的心跳包类型（1表示查询包，2表示应答包）；
 *     @param heart_pkg_number 	int，表示发送的心跳包的包号；
 *     @return    void
 */
void ZhwServiceSendBufferInit(char *dst_dev_name,int dst_dev_number,char *src_dev_name,int src_dev_number,
		int pkg_type,int heart_pkg_type,int heart_pkg_number)
{
		g_time_info = GetCurrentTCMSTime();

	heart_send_buffer.dst_dev_number = dst_dev_number;
	strcpy(heart_send_buffer.dst_dev_name, dst_dev_name);
	strcpy(heart_send_buffer.src_dev_name , src_dev_name);
	heart_send_buffer.src_dev_number = src_dev_number;
		heart_send_buffer.heart_package.current_time.date = g_time_info.date ;
		heart_send_buffer.heart_package.current_time.hour = g_time_info.hour ;
		heart_send_buffer.heart_package.current_time.minute = g_time_info.minute ;
		heart_send_buffer.heart_package.current_time.month = g_time_info.month ;
		heart_send_buffer.heart_package.current_time.second = g_time_info.second ;
		heart_send_buffer.heart_package.current_time.year = g_time_info.year ;
	heart_send_buffer.heart_package.package_type = heart_pkg_type;
	heart_send_buffer.heart_package.package_number = heart_pkg_number;
//	diag_printf("heart_send_buffer = s:%s--n:%d--d:%s--n:%d\n",heart_send_buffer.src_dev_name,heart_send_buffer.src_dev_number,heart_send_buffer.dst_dev_name,heart_send_buffer.dst_dev_number);
}

void ZhwClientSendBufferInit(char *dst_dev_name,int dst_dev_number,char *src_dev_name,int src_dev_number,
		int pkg_type,int heart_pkg_type,int heart_pkg_number)
{
	int info_size = ((sizeof(heart_send_buffer.heart_package.dev_info_u.eamp_info.eamp_per_dev_failure_info)
			> sizeof(heart_send_buffer.heart_package.dev_info_u.ccu_info.ccu_per_dev_failure_info))
			? sizeof(heart_send_buffer.heart_package.dev_info_u.eamp_info.eamp_per_dev_failure_info)
					: sizeof(heart_send_buffer.heart_package.dev_info_u.ccu_info.ccu_per_dev_failure_info));
	unsigned char client_dev_info[info_size];

	if( (strcmp(src_dev_name,"CCU") == 0) || (strcmp(src_dev_name,"EAMP") == 0) )
	{
		GetPerDevFailureInfo(client_dev_info,info_size);
	}

	heart_send_buffer.dst_dev_number = dst_dev_number;
	strcpy(heart_send_buffer.dst_dev_name, dst_dev_name);
	strcpy(heart_send_buffer.src_dev_name , src_dev_name);
	heart_send_buffer.src_dev_number = src_dev_number;
	heart_send_buffer.heart_package.package_type = heart_pkg_type;
	heart_send_buffer.heart_package.package_number = heart_pkg_number;
	memset(&heart_send_buffer.heart_package.current_time,
			0,sizeof(heart_send_buffer.heart_package.current_time));

		if(strcmp(src_dev_name,"BCU") == 0)
		{
			heart_send_buffer.heart_package.dev_info_u.bcu_info.online = 1;
		}
		else if(strcmp(src_dev_name,"EAMP") == 0)
		{
			heart_send_buffer.heart_package.dev_info_u.eamp_info.online = 1 ;
			memcpy(&heart_send_buffer.heart_package.dev_info_u.eamp_info.eamp_per_dev_failure_info
					,client_dev_info,sizeof(heart_send_buffer.heart_package.dev_info_u.eamp_info.eamp_per_dev_failure_info));
		}
		else if(strcmp(src_dev_name,"CCU") == 0)
		{
			heart_send_buffer.heart_package.dev_info_u.ccu_info.online = 1 ;
			memcpy(&heart_send_buffer.heart_package.dev_info_u.ccu_info.ccu_per_dev_failure_info
					,client_dev_info,sizeof(heart_send_buffer.heart_package.dev_info_u.ccu_info.ccu_per_dev_failure_info));
		}
		else if(strcmp(src_dev_name,"PCU") == 0)
		{
			heart_send_buffer.heart_package.dev_info_u.pcu_info.online = 1 ;
		}
		else if(strcmp(src_dev_name,"CMU") == 0)
		{
			heart_send_buffer.heart_package.dev_info_u.cmu_info.online = 1 ;
		}
		else
		{
				net_debug_printf(("something is wrong when send client heart package!!!\n"));
		}
}


/*
 *   	@brief	本函数的功能是g为每一类设备初始化一个本地设备状态信息表；
 *     @param dev_name 是字符指针类型，表示需要进行创建的设备的类型名；
 *     @param dev_number 	整型，表示需要创建的数量；
 *     @return    void *:运行正常，根据传入的dev_name类型，返回相应类型的表头指针；
 *     				NULL：需要创建的设备类型名或需要创建的数量存在问题；
 */
void * ZhwDevStatusBufferInit(char *dev_name,int dev_number)
{
//	net_debug_printf(("run into the ZhwDevStatusBufferInit() function \n"));
	int dev_number_loop;
	if( (strcmp(dev_name,"EAMP") == 0) && (dev_number > 0) )
	{
		eamp_dev_info_t *buffer_head = (eamp_dev_info_t *)malloc(sizeof(eamp_dev_info_t));
		bzero(buffer_head,sizeof(eamp_dev_info_t));
		buffer_head->set_online = DEV_STATUS_SET ;
		buffer_head->reply = 0 ;
		buffer_head->dev_number = 1 ;
		buffer_head->eamp_info.online = 1 ;
		eamp_dev_info_t *buffer_point ;
		buffer_point = buffer_head ;
		for(dev_number_loop = 2;dev_number_loop < (dev_number+1) ;dev_number_loop++)
		{
			buffer_point->next = (eamp_dev_info_t *)malloc(sizeof(eamp_dev_info_t));
			bzero(buffer_point->next,sizeof(eamp_dev_info_t));
			buffer_point = buffer_point->next ;
			buffer_head->set_online = DEV_STATUS_SET ;
			buffer_point->reply = 0 ;
			buffer_point->dev_number = dev_number_loop ;
			buffer_point->eamp_info.online = 1;
		}
		buffer_point->next = NULL ;
		return (eamp_dev_info_t *)buffer_head;
	}
	else if( (strcmp(dev_name,"BCU") == 0) && (dev_number > 0) )
	{
		bcu_dev_info_t *buffer_head = (bcu_dev_info_t *)malloc(sizeof(bcu_dev_info_t)) ;
		bzero(buffer_head,sizeof(bcu_dev_info_t));
		buffer_head->set_online = DEV_STATUS_SET ;
		buffer_head->reply = 0 ;
		buffer_head->dev_number = 1 ;
		buffer_head->bcu_info.online = 1;
		bcu_dev_info_t *buffer_point ;
		buffer_point = buffer_head ;
		for(dev_number_loop = 2;dev_number_loop < (dev_number+1) ;dev_number_loop++)
		{
			buffer_point->next = (bcu_dev_info_t *)malloc(sizeof(bcu_dev_info_t));
			bzero(buffer_point->next,sizeof(bcu_dev_info_t));
			buffer_point = buffer_point->next ;
			buffer_head->set_online = DEV_STATUS_SET ;
			buffer_point->reply = 0 ;
			buffer_point->dev_number = dev_number_loop ;
			buffer_point->bcu_info.online = 1;
		}
		buffer_point->next = NULL ;
		return (bcu_dev_info_t *)buffer_head;
	}
	else if( (strcmp(dev_name,"CCU") == 0) && (dev_number > 0) )
	{
		ccu_dev_info_t *buffer_head = (ccu_dev_info_t *)malloc(sizeof(ccu_dev_info_t));
		bzero(buffer_head,sizeof(ccu_dev_info_t));
		buffer_head->set_online = DEV_STATUS_SET ;
		buffer_head->reply = 0 ;
		buffer_head->dev_number = 1 ;
		buffer_head->ccu_info.online = 1;
		ccu_dev_info_t *buffer_point ;
		buffer_point = buffer_head ;
		for(dev_number_loop = 2;dev_number_loop < (dev_number+1) ;dev_number_loop++)
		{
			buffer_point->next = (ccu_dev_info_t *)malloc(sizeof(ccu_dev_info_t));
			bzero(buffer_point->next,sizeof(ccu_dev_info_t));
			buffer_point = buffer_point->next ;
			buffer_head->set_online = DEV_STATUS_SET ;
			buffer_point->reply = 0;
			buffer_point->dev_number = dev_number_loop ;
			buffer_point->ccu_info.online = 1;
		}
		buffer_point->next = NULL ;
		return (ccu_dev_info_t *)buffer_head;
	}
	else if( (strcmp(dev_name,"PCU") == 0) && (dev_number > 0) )
	{
		pcu_dev_info_t *buffer_head = (pcu_dev_info_t *)malloc(sizeof(pcu_dev_info_t));
		bzero(buffer_head,sizeof(pcu_dev_info_t));
		buffer_head->set_online = DEV_STATUS_SET ;
		buffer_head->reply = 0 ;
		buffer_head->dev_number = 1 ;
		buffer_head->pcu_info.online = 1 ;
		pcu_dev_info_t *buffer_point ;
		buffer_point = buffer_head ;
		for(dev_number_loop = 2;dev_number_loop < (dev_number+1) ;dev_number_loop++)
		{
			buffer_point->next = (pcu_dev_info_t *)malloc(sizeof(pcu_dev_info_t));
			bzero(buffer_point->next,sizeof(pcu_dev_info_t));
			buffer_point = buffer_point->next ;
			buffer_head->set_online = DEV_STATUS_SET ;
			buffer_point->reply = 0;
			buffer_point->dev_number = dev_number_loop ;
			buffer_point->pcu_info.online = 1 ;
		}
		buffer_point->next = NULL ;
		return (pcu_dev_info_t *)buffer_head;
	}
	else if( (strcmp(dev_name,"CMU") == 0) && (dev_number > 0) )
	{
		cmu_dev_info_t *buffer_head = (cmu_dev_info_t *)malloc(sizeof(cmu_dev_info_t));
		bzero(buffer_head,sizeof(cmu_dev_info_t));
		buffer_head->set_online = DEV_STATUS_SET ;
		buffer_head->reply = 0 ;
		buffer_head->dev_number = 1 ;
		buffer_head->cmu_info.online = 1 ;
		cmu_dev_info_t *buffer_point ;
		buffer_point = buffer_head ;
		for(dev_number_loop = 2;dev_number_loop < (dev_number+1) ;dev_number_loop++)
		{
			buffer_point->next = (cmu_dev_info_t *)malloc(sizeof(cmu_dev_info_t));
			bzero(buffer_point->next,sizeof(cmu_dev_info_t));
			buffer_point = buffer_point->next ;
			buffer_head->set_online = DEV_STATUS_SET ;
			buffer_point->reply = 0;
			buffer_point->dev_number = dev_number_loop ;
			buffer_point->cmu_info.online = 1 ;
		}
		buffer_point->next = NULL ;
		return (cmu_dev_info_t *)buffer_head;
	}
	else
	{
		return NULL ;
	}
}

/*
 *   	@brief	本函数的功能是更新设备类型表中的设备状态信息；
 *     @param heart_recv_buffer 是network_heart_package_t数据类型，表示保存的接受到的数据信息；
 *     @param dev_status_info 	是保存本地设备类型表头指针的变量；
 *     @return    1:运行正常，退出；
 *     				-1：需要更新的设备类型名异常；
 *     				-2：需要更新的设备类型号异常；
 */
int ZhwDevStatusUpdate(network_heart_package_t heart_recv_buffer,dev_status_info_t dev_status_info)
{
	int dev_number;
	dev_number = heart_recv_buffer.src_dev_number;

	if(strcmp(heart_recv_buffer.src_dev_name,"EAMP") == 0 )
	{
//		net_debug_printf(("the number of send heart reply is :%d!!!\n",heart_recv_buffer.src_dev_number));
		eamp_dev_info_t *eamp_dev_info_poinf;
		eamp_dev_info_poinf = dev_status_info.eamp_dev_info_head ;
		while(eamp_dev_info_poinf != NULL)
		{
			if((heart_recv_buffer.src_dev_number ) == eamp_dev_info_poinf->dev_number)
			{
				eamp_dev_info_poinf->set_online = DEV_STATUS_SET ;
				eamp_dev_info_poinf->reply = 1;
				memcpy(&eamp_dev_info_poinf->eamp_info,&heart_recv_buffer.heart_package.dev_info_u.eamp_info,sizeof(eamp_info_t));
				return 1 ;
			}
			eamp_dev_info_poinf = eamp_dev_info_poinf->next ;
		}
		return -2 ;//设备号无法查到
	}
	else if(strcmp(heart_recv_buffer.src_dev_name,"BCU") == 0 )
	{
		bcu_dev_info_t *bcu_dev_info_point;
		bcu_dev_info_point = dev_status_info.bcu_dev_info_head ;
		while(bcu_dev_info_point != NULL)
		{
			if((heart_recv_buffer.src_dev_number ) == bcu_dev_info_point->dev_number)
			{
				bcu_dev_info_point->set_online = DEV_STATUS_SET ;
				bcu_dev_info_point->reply = 1;
				memcpy(&bcu_dev_info_point->bcu_info,&heart_recv_buffer.heart_package.dev_info_u.bcu_info,sizeof(bcu_info_t));
				return 1 ;
			}
			bcu_dev_info_point = bcu_dev_info_point->next ;
		}
		return -2 ;//设备号无法查到
	}
	else if(strcmp(heart_recv_buffer.src_dev_name,"CCU") == 0 )
	{
		ccu_dev_info_t *ccu_dev_info_point;
		ccu_dev_info_point = dev_status_info.ccu_dev_info_head;
		while(ccu_dev_info_point != NULL)
		{
			if((heart_recv_buffer.src_dev_number ) == ccu_dev_info_point->dev_number)
			{
				ccu_dev_info_point->set_online = DEV_STATUS_SET ;
				ccu_dev_info_point->reply = 1;
				memcpy(&ccu_dev_info_point->ccu_info,&heart_recv_buffer.heart_package.dev_info_u.ccu_info,sizeof(ccu_info_t));
				return 1 ;
			}
			ccu_dev_info_point = ccu_dev_info_point->next;
		}
		return -2 ;//设备号无法查到
	}
	else if(strcmp(heart_recv_buffer.src_dev_name,"PCU") == 0 )
	{
		pcu_dev_info_t *pcu_dev_info_point ;
		pcu_dev_info_point = dev_status_info.pcu_dev_info_head ;

		while(pcu_dev_info_point != NULL)
		{
			if((heart_recv_buffer.src_dev_number ) == pcu_dev_info_point->dev_number )
			{
				pcu_dev_info_point->set_online = DEV_STATUS_SET ;
				pcu_dev_info_point->reply = 1;
				memcpy(&pcu_dev_info_point->pcu_info,&heart_recv_buffer.heart_package.dev_info_u.pcu_info,sizeof(pcu_info_t));
				return 1 ;
			}
			pcu_dev_info_point = pcu_dev_info_point->next;
		}
		return -2 ;//设备号无法查到
	}
	else if(strcmp(heart_recv_buffer.src_dev_name,"CMU") == 0 )
	{
		cmu_dev_info_t *cmu_dev_info_point ;
		cmu_dev_info_point = dev_status_info.cmu_dev_info_head ;

		while(cmu_dev_info_point != NULL)
		{
			if((heart_recv_buffer.src_dev_number ) == cmu_dev_info_point->dev_number )
			{
				cmu_dev_info_point->set_online = DEV_STATUS_SET ;
				cmu_dev_info_point->reply = 1;
				memcpy(&cmu_dev_info_point->cmu_info,&heart_recv_buffer.heart_package.dev_info_u.cmu_info,sizeof(cmu_info_t));
				return 1 ;
			}
			cmu_dev_info_point = cmu_dev_info_point->next;
		}
		return -2 ;//设备号无法查到
	}
	else
	{
		return -1 ; //需要更新的设备名不存在
	}
}

void ZhwHeartPrimaryServer(int recv_buffer,int send_buffer,struct global_info dev_info)
{
//	net_debug_printf(("run into ZhwHeartPrimaryServer() function!!!\n"));
	int curr_pkg_number = 0;
	int pkg_counte = 0;

	int get_dev_status = 2 ;
	int clean_backup_server_status = 5;

	int err_ret = 0 ;

	eamp_dev_info_t *eamp_dev_info_point;
	bcu_dev_info_t *bcu_dev_info_point;
	ccu_dev_info_t *ccu_dev_info_point;
	pcu_dev_info_t *pcu_dev_info_point;
	cmu_dev_info_t *cmu_dev_info_point;

	eamp_number = ZhwGetDevNumber(EAMP_NAME);
	bcu_number = ZhwGetDevNumber(BCU_NAME);
	ccu_number = ZhwGetDevNumber(CCU_NAME);
	pcu_number = ZhwGetDevNumber(PCU_NAME);
	cmu_number = ZhwGetDevNumber(CMU_NAME);
	net_debug_printf(("the number of eamp is %d;\n the number of bcu is %d;\n the number of ccu is %d;\n the number of pcu is %d;\n the number of cmu is %d.\n",eamp_number,bcu_number,ccu_number,pcu_number,cmu_number));

	dev_status_info.eamp_dev_info_head = (eamp_dev_info_t *)ZhwDevStatusBufferInit(EAMP_NAME,eamp_number);
	dev_status_info.bcu_dev_info_head = (bcu_dev_info_t *)ZhwDevStatusBufferInit(BCU_NAME,bcu_number);
	dev_status_info.ccu_dev_info_head = (ccu_dev_info_t *)ZhwDevStatusBufferInit(CCU_NAME,ccu_number);
	dev_status_info.pcu_dev_info_head = (pcu_dev_info_t *)ZhwDevStatusBufferInit(PCU_NAME,pcu_number);
	dev_status_info.cmu_dev_info_head = (cmu_dev_info_t *)ZhwDevStatusBufferInit(CMU_NAME,cmu_number);

	dev_fault_send_buffer_id = ZhwServiceBufferSet();
	if(dev_fault_send_buffer_id < 0 )
	{
		net_debug_printf(("in heart primary service create dev_fault_send_buffer fail.\n"));
	}

	ZhwAlarmInit();

	while(1)
	{
		if(heart_thread_loop_number == 10000)
		{
			heart_thread_loop_number = 0;
			net_debug_printf(("Primary Server Dev--Selftest module run OK !!!\n"));
		}
		else
		{
			heart_thread_loop_number = heart_thread_loop_number + 1 ;
		}
		eamp_dev_info_point = dev_status_info.eamp_dev_info_head ;
		bcu_dev_info_point = dev_status_info.bcu_dev_info_head ;
		ccu_dev_info_point = dev_status_info.ccu_dev_info_head ;
		pcu_dev_info_point = dev_status_info.pcu_dev_info_head ;
		cmu_dev_info_point = dev_status_info.cmu_dev_info_head ;

		if(heart_beat == 1) //定时发送心跳包
		{
			heart_beat = 0;

			if(clean_backup_server_status)
			{
				clean_backup_server_status = (clean_backup_server_status - 1);
			}

			if(pkg_counte < 65535)
			{
				pkg_counte++;
			}
			else
			{
				pkg_counte = (pkg_counte - 65535 + 1);
			}

			//set the device name in the first send time  -- start from bcu device.zhw-2014-11-12
			if(current_dev_name == NULL)
			{
				if( 0 == malloc_loop )
				{
					malloc_loop = 1;
					current_dev_name = (char *)malloc(10);
					heart_dst_name = (char *)malloc(10);
					bzero(current_dev_name,10);
					bzero(heart_dst_name,10);
				}
				memcpy(current_dev_name ,bcu_name,sizeof(bcu_name));
			}

			{
				if( 0 == (strcmp(current_dev_name,BCU_NAME)) )
				{
					if(current_dev_num < bcu_number)
					{
						heart_dst_num = ( current_dev_num + 1 );
						memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
						current_dev_num++;
						while(bcu_dev_info_point != NULL)
						{
							if(bcu_dev_info_point->dev_number == heart_dst_num)
							{
								if(0 == bcu_dev_info_point->set_online)
								{
									bcu_dev_info_point->bcu_info.online = 0 ;
								}
								else
								{
									bcu_dev_info_point->set_online = ( bcu_dev_info_point->set_online - 1 ) ;
								}
								bcu_dev_info_point->reply = 0 ;
//								bcu_dev_info_point->bcu_info.online = 0;
								break;
							}
							else
							{
								bcu_dev_info_point = bcu_dev_info_point->next ;
							}
						}
						bcu_dev_info_point = dev_status_info.bcu_dev_info_head ;
					}
					else
					{
						current_dev_num = 0 ;
						heart_dst_num = 0 ;
						bzero(current_dev_name,10);
						memcpy(current_dev_name,eamp_name,sizeof(eamp_name));
					}
				}
				else if( 0 == (strcmp(current_dev_name,EAMP_NAME)) )
				{
					if(current_dev_num < eamp_number)
					{
						heart_dst_num = ( current_dev_num + 1 );
						memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
						current_dev_num++;
						while(eamp_dev_info_point != NULL)
						{
							if( eamp_dev_info_point->dev_number == heart_dst_num )
							{
								if(0 == eamp_dev_info_point->set_online)
								{
									eamp_dev_info_point->eamp_info.online = 0 ;
								}
								else
								{
									eamp_dev_info_point->set_online = ( eamp_dev_info_point->set_online - 1 ) ;
								}
								eamp_dev_info_point->reply = 0 ;
//								eamp_dev_info_point->eamp_info.online = 0 ;
								break ;
							}
							else
							{
								eamp_dev_info_point = eamp_dev_info_point->next ;
							}
						}
						eamp_dev_info_point = dev_status_info.eamp_dev_info_head ;
					}
					else
					{
						current_dev_num = 0 ;
						heart_dst_num = 0 ;
						bzero(current_dev_name,10);
						memcpy(current_dev_name,pcu_name,sizeof(pcu_name));
					}
				}
				else if( 0 == (strcmp(current_dev_name,PCU_NAME)) )
				{
					if(current_dev_num < pcu_number)
					{
						heart_dst_num = ( current_dev_num + 1 );
						memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
						current_dev_num++;
						while(pcu_dev_info_point != NULL)
						{
							if(pcu_dev_info_point->dev_number == heart_dst_num)
							{
								if(0 == pcu_dev_info_point->set_online)
								{
									pcu_dev_info_point->pcu_info.online = 0 ;
								}
								else
								{
									pcu_dev_info_point->set_online = ( pcu_dev_info_point->set_online - 1 ) ;
								}
								pcu_dev_info_point->reply = 0 ;
//								pcu_dev_info_point->pcu_info.online = 0 ;
								break;
							}
							else
							{
								pcu_dev_info_point = pcu_dev_info_point->next ;
							}
						}
						pcu_dev_info_point = dev_status_info.pcu_dev_info_head ;
					}
					else
					{
						current_dev_num = 0 ;
						heart_dst_num = 0 ;
						bzero(current_dev_name,10);
						memcpy(current_dev_name,cmu_name,sizeof(cmu_name));
					}
				}
				else if( 0 == (strcmp(current_dev_name,CMU_NAME)) )
				{
					if(current_dev_num < cmu_number)
					{
						heart_dst_num = ( current_dev_num + 1 );
						memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
						current_dev_num++;
						while(cmu_dev_info_point != NULL)
						{
							if(cmu_dev_info_point->dev_number == heart_dst_num)
							{
								if(0 == cmu_dev_info_point->set_online)
								{
									cmu_dev_info_point->cmu_info.online = 0 ;
								}
								else
								{
									cmu_dev_info_point->set_online = ( cmu_dev_info_point->set_online - 1 ) ;
								}
								cmu_dev_info_point->reply = 0 ;
//								cmu_dev_info_point->cmu_info.online = 0 ;
								break;
							}
							else
							{
								cmu_dev_info_point = cmu_dev_info_point->next ;
							}
						}
						cmu_dev_info_point = dev_status_info.cmu_dev_info_head ;
					}
					else
					{
						current_dev_num = 0 ;
						heart_dst_num = 0 ;
						bzero(current_dev_name,10);
						memcpy(current_dev_name,bcu_name,sizeof(bcu_name));
					}
				}
				else
				{
					net_debug_printf(("the dst dev name of heart package is error\n"));
					current_dev_num = 0 ;
					heart_dst_num = 0 ;
					bzero(current_dev_name,10);
					memcpy(current_dev_name,bcu_name,sizeof(bcu_name));
				}
			}

			if(NULL != current_dev_name)
			{
				if(heart_dst_num != 0)
				{
					while(ccu_dev_info_point != NULL)
					{
						if(ccu_dev_info_point->dev_number == 2)
						{
							if(0 == clean_backup_server_status)
							{
								ccu_dev_info_point->reply = 0 ;
								ccu_dev_info_point->ccu_info.online = 0;
							}
							break;
						}
						else
						{
							ccu_dev_info_point = ccu_dev_info_point->next ;
						}
					}
					ccu_dev_info_point = dev_status_info.ccu_dev_info_head ;
					curr_pkg_number = pkg_counte;

//					diag_printf("heart_dst_name = %s,heart_dst_num = %d\n",heart_dst_name,heart_dst_num);

					ZhwServiceSendBufferInit(heart_dst_name,heart_dst_num,dev_info.dev_name,dev_info.dev_number,1,1,pkg_counte);
					if(BlockBufferWrite( send_buffer, (void *)&heart_send_buffer, sizeof(heart_send_buffer) ) < 0)
					{
						net_debug_printf(("apply send heart request package to network is error !!!\n"));
					}
					else
					{
//						net_debug_printf(("send a heart package the package number is %d\n",pkg_counte));
					}

					ZhwServiceSendBufferInit("CCU",2,dev_info.dev_name,dev_info.dev_number,1,1,pkg_counte);
#ifdef CCU_DEV
					heart_send_buffer.heart_package.dev_info_u.ccu_info.tcms_effective_flag = ccu_state.tms_effective_flag;
#endif
					if(BlockBufferWrite( send_buffer, (void *)&heart_send_buffer, sizeof(heart_send_buffer) ) < 0)
					{
						net_debug_printf(("apply send heart request package to network is error !!!\n"));
					}
					else
					{
//						net_debug_printf(("send a heart package the package number is %d\n",pkg_counte));
					}
				}
			}
		}
//		diag_printf("SendHeartBetaToPeripheral-%d\n",heart_beat_dev_status);

		SendHeartBetaToPeripheral();
		if(heart_beat_dev_status == 1)
		{
			heart_beat_dev_status = 0;
			if(0 == get_dev_status)
			{
				get_dev_status = 2 ;
#ifdef CCU_DEV
				CCUGetAllPerDevFailureInfo();
#endif
			}
			else
			{
				get_dev_status--;
			}
		}


		if(BlockBufferRead(recv_buffer,(void *)&heart_recv_buffer,sizeof(heart_recv_buffer)) > 0) //从接受buffer中接受数据
		{
//			if( (0 != strcmp(heart_recv_buffer.src_dev_name,dev_info.dev_name)) && (heart_recv_buffer.src_dev_number != dev_info.dev_number) ) //
			{
				net_debug_printf(("%s--%d--%d--%d\n"
						,heart_recv_buffer.src_dev_name
						,heart_recv_buffer.src_dev_number
						,heart_recv_buffer.heart_package.package_type
						,heart_recv_buffer.heart_package.package_number));

				switch(heart_recv_buffer.heart_package.package_type)
				{
				case 1: //收到心跳查询包：说明备用服务器已经启用，发送查询命令让其关闭。
					ZhwServiceSendBufferInit(heart_recv_buffer.src_dev_name,heart_recv_buffer.src_dev_number,dev_info.dev_name,dev_info.dev_number,1,1,pkg_counte);
					if(BlockBufferWrite( send_buffer, (void *)&heart_send_buffer, sizeof(heart_send_buffer) ) < 0)
					{
						net_debug_printf(("apply send heart request package to backup heart server is error !!!\n"));
					}
					else
					{
						net_debug_printf(("close backup service\n"));
					}
					break;
				case 2: //收到应答包，处理程序。
					if(strcmp(heart_recv_buffer.src_dev_name,"EAMP") == 0 )
					{
						if(heart_recv_buffer.heart_package.package_number == curr_pkg_number) //收到的应答包号与发送包号相等，直接更新数据
						{
							if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
							{
								net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
							}
						}
						else
						{
							if(dev_status_info.eamp_dev_info_head->reply != 1)
							{
								if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
								{
									net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
								}
							}
						}
						break;
					}
					else if(strcmp(heart_recv_buffer.src_dev_name,"BCU") == 0 )
					{
						if(heart_recv_buffer.heart_package.package_number == curr_pkg_number) //收到的应答包号与发送包号相等，直接更新数据
						{
							if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
							{
								net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
							}

						}
						else
						{
							if(dev_status_info.bcu_dev_info_head->reply != 1)
							{
								if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
								{
									net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
								}

							}
						}
						break;
					}
					else if(strcmp(heart_recv_buffer.src_dev_name,"CCU") == 0 )
					{
						clean_backup_server_status = 5 ;
						if(heart_recv_buffer.heart_package.package_number == curr_pkg_number) //收到的应答包号与发送包号相等，直接更新数据
						{
							if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
							{
								net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
							}
						}
						else
						{
							if(dev_status_info.ccu_dev_info_head->reply != 1)
							{
								if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
								{
									net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
								}

							}
						}
						break;
					}
					else if(strcmp(heart_recv_buffer.src_dev_name,"PCU") == 0 )
					{
						if(heart_recv_buffer.heart_package.package_number == curr_pkg_number) //收到的应答包号与发送包号相等，直接更新数据
						{
							if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
							{
								net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
							}
						}
						else
						{
							if(dev_status_info.pcu_dev_info_head->reply != 1)
							{
								if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
								{
									net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
								}

							}
						}
						break;
					}
					else if(strcmp(heart_recv_buffer.src_dev_name,"CMU") == 0 )
					{
						if(heart_recv_buffer.heart_package.package_number == curr_pkg_number) //收到的应答包号与发送包号相等，直接更新数据
						{
							if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
							{
								net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
							}
						}
						else
						{
							if(dev_status_info.cmu_dev_info_head->reply != 1)
							{
								if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
								{
									net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
								}

							}
						}
						break;
					}
					else
					{
						net_debug_printf(("the device name of send this heart reply package %s,is unknown!!!\n",heart_recv_buffer.src_dev_name));
						break;
					}
				case 3: //add by zhw_8-21 test protocol
					net_debug_printf(("receive a client request package the device could not receive heart inquiry package !!!\n"));
					break;
				default :
					net_debug_printf(("the number of heart package is %d,is not right!!!\n",heart_recv_buffer.heart_package.package_type));
					break;
				}
			}	//接受心跳包结束。
		}

		if(send_to_bcu == 0)
		{
			send_to_bcu = SEND_TO_BCU ;
			// add for test --2015-1-9
//			if(10 == chang_send_cmd_type)
//			{
//				chang_send_cmd_type = 0 ;
//				if(cmd_changed < 20)
//				{
//					cmd_changed = ( cmd_changed + 1 ) ;
//				}
//				else
//				{
//					cmd_changed = 0 ;
//				}
//			}
//			else if(chang_send_cmd_type < 10)
//			{
//				chang_send_cmd_type++ ;
//			}
//			else
//			{
//				chang_send_cmd_type = 0 ;
//			}
			//end of add for test --2015-1-9

//			ZhwSendDevStatusToBCU(dev_fault_send_buffer_id,dev_info);
			ZhwSendDevStatusToBCUOrCCU(dev_fault_send_buffer_id,dev_info,"BCU");
			ZhwSendDevStatusToBCUOrCCU(dev_fault_send_buffer_id,dev_info,"CCU");
		}
		sched_yield();
	}
}

void ZhwHeartBackupServer(int recv_buffer,int send_buffer,struct global_info dev_info)
{
	int primary_server_down = 5;
//	int backup_server_up = 0;
	int get_dev_status = 0 ;
	int curr_pkg_number = 0;
	int pkg_counte = 0;
	int dev_number;

	eamp_dev_info_t *eamp_dev_info_point;
	bcu_dev_info_t *bcu_dev_info_point;
	ccu_dev_info_t *ccu_dev_info_point;
	pcu_dev_info_t *pcu_dev_info_point;
	cmu_dev_info_t *cmu_dev_info_point;

#ifdef UPTEST
	int status = 0;
#endif

	int err_ret = 0 ;

	cmu_number = ZhwGetDevNumber(CMU_NAME);

	if((eamp_number = ZhwGetDevNumber("EAMP")) < 0 )
	{
		net_debug_printf(("get EAMP number is fail err code is %d\n",eamp_number));
	}
	if((bcu_number = ZhwGetDevNumber("BCU")) < 0 )
	{
		net_debug_printf(("get BCU number is fail err code is %d\n",bcu_number));
	}
	if((ccu_number = ZhwGetDevNumber("CCU")) < 0 )
	{
		net_debug_printf(("get CCU number is fail err code is %d\n",ccu_number));
	}
	if((pcu_number = ZhwGetDevNumber("PCU")) < 0 )
	{
		net_debug_printf(("get PCU number is fail err code is %d\n",pcu_number));
	}

	if((dev_status_info.eamp_dev_info_head = (eamp_dev_info_t *)ZhwDevStatusBufferInit("EAMP",eamp_number)) == NULL )
	{
		net_debug_printf(("ZhwDevStatusBufferInit() function init EAMP buffer fail the return is NULL\n"));
	}
	if((dev_status_info.bcu_dev_info_head = (bcu_dev_info_t *)ZhwDevStatusBufferInit("BCU",bcu_number)) == NULL )
	{
		net_debug_printf(("ZhwDevStatusBufferInit() function init BCU buffer fail the return is NULL\n"));
	}
	if((dev_status_info.ccu_dev_info_head = (ccu_dev_info_t *)ZhwDevStatusBufferInit("CCU",ccu_number)) == NULL )
	{
		net_debug_printf(("ZhwDevStatusBufferInit() function init CCU buffer fail the return is NULL\n"));
	}
	if((dev_status_info.pcu_dev_info_head = (pcu_dev_info_t *)ZhwDevStatusBufferInit("PCU",pcu_number)) == NULL )
	{
		net_debug_printf(("ZhwDevStatusBufferInit() function init PCU buffer fail the return is NULL\n"));
	}

	dev_status_info.cmu_dev_info_head = (cmu_dev_info_t *)ZhwDevStatusBufferInit(CMU_NAME,cmu_number);

	ZhwAlarmInit();
	dev_fault_send_buffer_id = ZhwServiceBufferSet();
	if(dev_fault_send_buffer_id < 0)
	{
		net_debug_printf(("dev fault send buffer create fail\n"));
	}

	while(1)
	{
		if(heart_thread_loop_number == 1000)
		{
			heart_thread_loop_number = 0;
//			net_debug_printf(("BackUp Server Dev--Selftest module run OK !!!\n"));
		}
		else
		{
			heart_thread_loop_number = heart_thread_loop_number + 1 ;
		}

		eamp_dev_info_point = dev_status_info.eamp_dev_info_head ;
		bcu_dev_info_point = dev_status_info.bcu_dev_info_head ;
		ccu_dev_info_point = dev_status_info.ccu_dev_info_head ;
		pcu_dev_info_point = dev_status_info.pcu_dev_info_head ;
		cmu_dev_info_point = dev_status_info.cmu_dev_info_head ;

		if(heart_beat == 1)
		{
			heart_beat = 0 ;
			if(primary_server_down != 0)
			{
				primary_server_down = (primary_server_down -1);
			}
			else
			{
				ZhwSetPrimaryServerNS(0);
					if(pkg_counte < 65535)
					{
						pkg_counte++;
					}
					else
					{
						pkg_counte = (pkg_counte - 65535 + 1);
					}

					if(current_dev_name == NULL)
					{
						if( 0 == malloc_loop )
						{
							malloc_loop = 1;
							current_dev_name = (char *)malloc(10);
							heart_dst_name = (char *)malloc(10);
							bzero(current_dev_name,10);
							bzero(heart_dst_name,10);
						}
						memcpy(current_dev_name ,bcu_name,sizeof(bcu_name));
					}

					{
						if( 0 == (strcmp(current_dev_name,BCU_NAME)) )
						{
							if(current_dev_num < bcu_number)
							{
								heart_dst_num = ( current_dev_num + 1 );
								memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
								current_dev_num++;
								while(bcu_dev_info_point != NULL)
								{
									if(bcu_dev_info_point->dev_number == heart_dst_num)
									{
										if(0 == bcu_dev_info_point->set_online)
										{
											bcu_dev_info_point->bcu_info.online = 0 ;
										}
										else
										{
											bcu_dev_info_point->set_online = ( bcu_dev_info_point->set_online - 1 ) ;
										}
										bcu_dev_info_point->reply = 0 ;
//										bcu_dev_info_point->bcu_info.online = 0 ;
										break;
									}
									else
									{
										bcu_dev_info_point = bcu_dev_info_point->next ;
									}
								}
								bcu_dev_info_point = dev_status_info.bcu_dev_info_head ;
							}
							else
							{
								current_dev_num = 0 ;
								heart_dst_num = 0 ;
								bzero(current_dev_name,10);
								memcpy(current_dev_name,eamp_name,sizeof(eamp_name));
							}
						}
						else if( 0 == (strcmp(current_dev_name,EAMP_NAME)) )
						{
							if(current_dev_num < eamp_number)
							{
								heart_dst_num = ( current_dev_num + 1 );
								memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
								current_dev_num++;
								while(eamp_dev_info_point != NULL)
								{
									if( eamp_dev_info_point->dev_number == heart_dst_num )
									{
										if(0 == eamp_dev_info_point->set_online)
										{
											eamp_dev_info_point->eamp_info.online = 0 ;
										}
										else
										{
											eamp_dev_info_point->set_online = ( eamp_dev_info_point->set_online - 1 ) ;
										}
										eamp_dev_info_point->reply = 0 ;
//										eamp_dev_info_point->eamp_info.online = 0 ;
										break ;
									}
									else
									{
										eamp_dev_info_point = eamp_dev_info_point->next ;
									}
								}
								eamp_dev_info_point = dev_status_info.eamp_dev_info_head ;
							}
							else
							{
								current_dev_num = 0 ;
								heart_dst_num = 0 ;
								bzero(current_dev_name,10);
								memcpy(current_dev_name,pcu_name,sizeof(pcu_name));
							}
						}
						else if( 0 == (strcmp(current_dev_name,PCU_NAME)) )
						{
							if(current_dev_num < pcu_number)
							{
								heart_dst_num = ( current_dev_num + 1 );
								memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
								current_dev_num++;
								while(pcu_dev_info_point != NULL)
								{
									if(pcu_dev_info_point->dev_number == heart_dst_num)
									{
										if(0 == pcu_dev_info_point->set_online)
										{
											pcu_dev_info_point->pcu_info.online = 0 ;
										}
										else
										{
											pcu_dev_info_point->set_online = ( pcu_dev_info_point->set_online - 1 ) ;
										}
										pcu_dev_info_point->reply = 0 ;
//										pcu_dev_info_point->pcu_info.online = 0 ;
										break;
									}
									else
									{
										pcu_dev_info_point = pcu_dev_info_point->next ;
									}
								}
								pcu_dev_info_point = dev_status_info.pcu_dev_info_head ;
							}
							else
							{
								current_dev_num = 0 ;
								heart_dst_num = 0 ;
								bzero(current_dev_name,10);
								memcpy(current_dev_name,cmu_name,sizeof(cmu_name));
							}
						}
						else if( 0 == (strcmp(current_dev_name,CMU_NAME)) )
						{
							if(current_dev_num < cmu_number)
							{
								heart_dst_num = ( current_dev_num + 1 );
								memcpy(heart_dst_name,current_dev_name,sizeof(current_dev_name));
								current_dev_num++;
								while(cmu_dev_info_point != NULL)
								{
									if(cmu_dev_info_point->dev_number == heart_dst_num)
									{
										if(0 == cmu_dev_info_point->set_online)
										{
											cmu_dev_info_point->cmu_info.online = 0 ;
										}
										else
										{
											cmu_dev_info_point->set_online = ( cmu_dev_info_point->set_online - 1 ) ;
										}
										cmu_dev_info_point->reply = 0 ;
//										cmu_dev_info_point->cmu_info.online = 0 ;
										break;
									}
									else
									{
										cmu_dev_info_point = cmu_dev_info_point->next ;
									}
								}
								cmu_dev_info_point = dev_status_info.cmu_dev_info_head ;
							}
							else
							{
								current_dev_num = 0 ;
								heart_dst_num = 0 ;
								bzero(current_dev_name,10);
								memcpy(current_dev_name,bcu_name,sizeof(bcu_name));
							}
						}
						else
						{
							net_debug_printf(("the dst dev name of heart package is error\n"));
							current_dev_num = 0 ;
							heart_dst_num = 0 ;
							bzero(current_dev_name,10);
							memcpy(current_dev_name,bcu_name,sizeof(bcu_name));
						}
					}

					if(NULL != current_dev_name)
					{
						if(heart_dst_num != 0)
						{
							while(ccu_dev_info_point != NULL)
							{
								if(ccu_dev_info_point->dev_number == 1)
								{
									ccu_dev_info_point->reply = 0 ;
									ccu_dev_info_point->ccu_info.online = 0 ;
									break;
								}
								else
								{
									ccu_dev_info_point = ccu_dev_info_point->next ;
								}
							}
							ccu_dev_info_point = dev_status_info.ccu_dev_info_head ;
							curr_pkg_number = pkg_counte;

//							diag_printf("heart_dst_name = %s,heart_dst_num = %d\n",heart_dst_name,heart_dst_num);

							ZhwServiceSendBufferInit(heart_dst_name,heart_dst_num,dev_info.dev_name,dev_info.dev_number,1,1,pkg_counte);
							if(BlockBufferWrite( send_buffer, (void *)&heart_send_buffer, sizeof(heart_send_buffer) ) < 0)
							{
								net_debug_printf(("apply send heart request package to network is error !!!\n"));
							}
							else
							{
								net_debug_printf(("send a heart package the package number is %d\n",pkg_counte));
							}
						}
					}
			}
		}

		if(BlockBufferRead(recv_buffer,(void *)&heart_recv_buffer,sizeof(heart_recv_buffer)) > 0)
		{
			net_debug_printf(("%s--%d--%d--%d\n"
					,heart_recv_buffer.src_dev_name
					,heart_recv_buffer.src_dev_number
					,heart_recv_buffer.heart_package.package_type
					,heart_recv_buffer.heart_package.package_number));
			if( 1 )
			{
				if(heart_recv_buffer.heart_package.package_type == 1)
				{
#ifdef CCU_DEV
					  if(ccu_state.ccu_info.devices_no == 2 && strcmp(heart_recv_buffer.dst_dev_name,"CCU") == 0)
					  {//wilson-zw-2015-5-28-心跳包带TCMS状态
						 other_ccu_tcms_flag = heart_recv_buffer.heart_package.dev_info_u.ccu_info.tcms_effective_flag;
					  }
#endif
					primary_server_down = 5;
					ZhwSetPrimaryServerNS(1);

					memcpy(&g_time_info,&heart_recv_buffer.heart_package.current_time,sizeof(heart_recv_buffer.heart_package.current_time));
					ZhwClientSendBufferInit(heart_recv_buffer.src_dev_name,heart_recv_buffer.src_dev_number,dev_info.dev_name,dev_info.dev_number,1,2,heart_recv_buffer.heart_package.package_number);

					if(BlockBufferWrite( send_buffer, (void *)&heart_send_buffer, sizeof(heart_send_buffer) ) < 0)
					{
						net_debug_printf(("heart apply send reply package to network is error !!!\n"));
					}
					else
					{
						net_debug_printf(("heart apply send reply package to network is ok!!!\n"));
					}
				}
				else if(heart_recv_buffer.heart_package.package_type == 2)
				{
					if(0 == primary_server_down)
					{
						if((heart_recv_buffer.heart_package.package_number == curr_pkg_number) )
						{
							if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
							{
								net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
							}
						}
						else
						{
							dev_number = heart_recv_buffer.src_dev_number;
							if(strcmp(heart_recv_buffer.src_dev_name,"EAMP") == 0)
							{
								if(dev_status_info.eamp_dev_info_head->reply != 1)
								{
									if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
									{
										net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
									}
								}
							}
							if(strcmp(heart_recv_buffer.src_dev_name,"BCU") == 0)
							{
								if(dev_status_info.bcu_dev_info_head->reply != 1)
								{
									if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
									{
										net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
									}
								}
							}
							if(strcmp(heart_recv_buffer.src_dev_name,"CCU") == 0)
							{
								if(dev_status_info.ccu_dev_info_head->reply != 1)
								{
									if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
									{
										net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
									}
								}
							}
							if(strcmp(heart_recv_buffer.src_dev_name,"PCU") == 0)
							{
								if(dev_status_info.pcu_dev_info_head->reply != 1)
								{
									if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
									{
										net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
									}
								}
							}
							if(strcmp(heart_recv_buffer.src_dev_name,"CMU") == 0)
							{
								if(dev_status_info.cmu_dev_info_head->reply != 1)
								{
									if((err_ret = ZhwDevStatusUpdate(heart_recv_buffer,dev_status_info)) < 0 )
									{
										net_debug_printf(("ZhwDevStatusUpdate() function run fail , the fail code is :%d !!!\n",err_ret));
									}
								}
							}
						}
					}
				}
			}
		}

		SendHeartBetaToPeripheral();//
		if(heart_beat_dev_status == 1)
		{
			heart_beat_dev_status = 0;
			if(primary_server_down == 0)
			{
				if(0 == get_dev_status)
				{
					get_dev_status = 2 ;
#ifdef CCU_DEV
					CCUGetAllPerDevFailureInfo();
#endif
				}
				else
				{
					get_dev_status--;
				}
			}
		}

		if(send_to_bcu == 0)
		{
			send_to_bcu = SEND_TO_BCU ;
			if( (primary_server_down) == 0 )
			{
//				ZhwSendDevStatusToBCU(dev_fault_send_buffer_id,dev_info);
				ZhwSendDevStatusToBCUOrCCU(dev_fault_send_buffer_id,dev_info,"BCU");
				ZhwSendDevStatusToBCUOrCCU(dev_fault_send_buffer_id,dev_info,"CCU");
			}
		}

		sched_yield();
	}
}

void ZhwHeartClient(int recv_buffer,int send_buffer,struct global_info dev_info)
{
	int set_ns = (MAX_TYPE) * 2 ;
	ZhwAlarmInit();

	net_debug_printf(("running into heart client ZhwHeartClient() function!!!\n"));

	while(1)
	{
		if(heart_thread_loop_number == 10000)
		{
			heart_thread_loop_number = 0;
			net_debug_printf(("Client Dev--Selftest module run OK !!!\n"));
		}
		else
		{
			heart_thread_loop_number = heart_thread_loop_number + 1 ;
		}

		if(heart_beat == 1)
		{
			heart_beat = 0;
			if(0 == set_ns)
			{
				set_ns = (MAX_TYPE) * 2 ;
				ZhwSetClientNS(0);
			}
			else
			{
				set_ns--;
			}
		}

#ifdef EAMP_DEV
		EampnetworkHeartBetaHandle();
#endif
		SendHeartBetaToPeripheral();

		if(BlockBufferRead(recv_buffer,(void *)&heart_recv_buffer,sizeof(heart_recv_buffer)) > 0)
		{
			net_debug_printf(("receive :%s-%d-%d-%d\n"
					,heart_recv_buffer.src_dev_name
					,heart_recv_buffer.src_dev_number
					,heart_recv_buffer.heart_package.package_type
					,heart_recv_buffer.heart_package.package_number));

			ZhwSetClientNS(1);

			if( (heart_recv_buffer.heart_package.package_type == 1) )
			{
				memcpy(&g_time_info,&heart_recv_buffer.heart_package.current_time,sizeof(heart_recv_buffer.heart_package.current_time));
				ZhwClientSendBufferInit(heart_recv_buffer.src_dev_name,heart_recv_buffer.src_dev_number,dev_info.dev_name,dev_info.dev_number,1,2,heart_recv_buffer.heart_package.package_number);
				if(BlockBufferWrite( send_buffer, (void *)&heart_send_buffer, sizeof(heart_send_buffer) ) < 0)
				{
					net_debug_printf(("heart apply function send package to network buffer is error!!!\n"));
				}
				else
				{
					net_debug_printf(("replay :%s-%d-%d-%d\n"
										,heart_send_buffer.dst_dev_name
										,heart_send_buffer.dst_dev_number
										,heart_send_buffer.heart_package.package_type
										,heart_send_buffer.heart_package.package_number));
				}
			}
			else if( (heart_recv_buffer.heart_package.package_type == 3) )
			{
				net_debug_printf(("heart apply client have received the data send from itself!!!\n"));
			}
		}
		sched_yield();
	}

}

void ZhwHeartModule(struct global_info dev_info)
{
	switch(dev_info.is_server)
	{
	case 0:
		ZhwHeartClient(heart_recv_buffer_id,heart_send_buffer_id,dev_info);
		break;
	case 1:
		ZhwHeartPrimaryServer(heart_recv_buffer_id,heart_send_buffer_id,dev_info);
		break;
	case 2:
		ZhwHeartBackupServer(heart_recv_buffer_id,heart_send_buffer_id, dev_info);
		break;
	default:
		net_debug_printf(("the number of server flag is error!!!\n"));
		break;
	}
}

int ZhwDevTestModule(udp_heart_socket_buffer_t udp_heart_buffer)
{
	net_debug_printf(("I am success entry ZhwDevTestModule() function!!!\n"));

	struct dev_type_and_no local_dev_type_num;
	struct global_info local_dev_info;

	local_dev_type_num = ReadFromIO();

	local_dev_info = ReadGlobalArray(local_dev_type_num.dev_type,NULL,local_dev_type_num.dev_number,1);

	heart_send_buffer_id = udp_heart_buffer.udp_heart_socket_send_buffer;
	heart_recv_buffer_id = udp_heart_buffer.udp_heart_socket_recv_buffer;

	ZhwHeartModule(local_dev_info);

	return 0;
}
