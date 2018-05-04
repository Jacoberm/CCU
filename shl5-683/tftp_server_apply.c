/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：ccu_database_update.c
 * 文件标识：
 * 摘    要：CCU database update function define
 *
 * 当前版本：V1.0.0
 * 作    者：roben
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */



#include "tftp_server_apply.h"

//现在只创建了一个tftp服务器。
static int arry_socket_fd[TCP_MAX_CONNECT];


//int main(void)
//{
//	char *tftp_server_ip = "192.168.7.21";
//	tftp_print( ("strlen server ip:%d\n",strlen(tftp_server_ip)) );
//
//	TftpServerAPI(tftp_server_ip);
//    return 0;
//}

//一直运行,不退出.除非出错.
//tftp 服务器entry。
int TftpServerAPI(char *tftp_server_ip)
{
	tftp_print( ("the tftp_server_ip:%s\n ",tftp_server_ip) );
	int tftp_server_create_num = 0;

	int tftp_server_id = 0;

	while( tftp_server_create_num < TCP_MAX_CONNECT)
	{
       tftp_print( ("tftp_server_create\n ") );
//		CreateTFTPThread();
		while(1)
		{
			//如果返回 《 0，表示打开tftp server 失败，继续打开操作，直到成功
			tftp_server_id = tftp_test_server();
			if (tftp_server_id < 0)
			{
				tftp_print( ("error on the tftp server\n") );
	   //			return -1;
			}
			else
			{
				   tftp_print( ("Ccu, tftp server-%d, start\n", tftp_server_id) );
				   break;
			}

		}
		tftp_print( ("the tftp_server_create_num:%d\n ",tftp_server_create_num) );
		tftp_server_create_num++;
	}

		//TCP连接没有使用，没什么实际用途。
    //TftpServerCmdFun(tftp_server_ip);

    return 0;
}

//接受新套接字，并将新套接字置为数组中不为0的成员。
//并查询是否大于最大套接字。
//int AcceptConnectSocket(int server_socket, int *max_sock_num)
//{
//	int ret = 0;
//	int connect_socket = 0;
//    struct sockaddr from_addr;
//    int from_len = sizeof(from_addr);
//
//    tftp_print( ("TftpServerCmdFun--bb--3333\n") );
//	if( (connect_socket = accept(server_socket, &from_addr, (socklen_t *)&from_len)) == -1)
//	{
//		tftp_print( ("error on the accept fun\n") );
//		return -1;
//	}
//
//	while(1)
//	{
//		tftp_print( ("TftpServerCmdFun--bb--4444\n") );
//		if(arry_socket_fd[ret] == 0)
//		{
//			arry_socket_fd[ret] = connect_socket;
//			if (connect_socket > *max_sock_num)
//			{
//				*max_sock_num = connect_socket;
//			}
//			break;
//		}
//		ret++;
//	}
//
//	return 0;
//}


////如果没有参数“ret”，没办法定位到底是那个套接字有数据需要接收。
//int ConnectSockRecv(int ret, int total_connect_num)
//{
//	int connect_num = total_connect_num;
//	tftp_network_cmd_t data_place;//接收数据包。
//
//	//接受数据，并解包。
//	//如果长度小于0，表示链接有错，或者链接已经断开
//	if (TCPrecv(arry_socket_fd[ret], &data_place, sizeof(data_place), 0) < 0)
//	{
//		close(arry_socket_fd[ret]);
//		arry_socket_fd[ret] = 0;
//		connect_num --;
//	}
//	else
//	{
//		//解包，“data_place.device_state == 6”表示链接结束。
//		if (data_place.device_state == ACK_end_refresh)
//		{
//			tftp_print( ("entry the close---111\n") );
//			close(arry_socket_fd[ret]);
//			arry_socket_fd[ret] = 0;
//			connect_num --;
//		}
//	}
//
//	return connect_num;
//}


////tftp 服务器控制函数，控制链接数量。
//int TftpServerCmdFun(char *tftp_server_ip)
//{
//	struct addrinfo *addrinfo = NULL;
//	struct addrinfo *server_add = NULL;
//
//	int ret = 0;
//	fd_set readfds;
//	struct timeval timeout;
//
//	int server_socket = 0;
//	int max_sock_num = 0;
//
//	int total_connect_num = 0;
//
//	tftp_print( ("TftpServerCmdFun-----111111\n") );
//
//        ///< 1，tcp服务器套接字链接达到过最大链接数量。
//	//如果长度小于0，表示链接有错，或者链接已经断开
//	int open_socket_flage = 0;
//
//	//填充套接字地址，绑定时等使用。
//    if ( (server_add = SocketAddressFill(tftp_server_ip, TFTP_SERVER_RECV_PORT, SOCK_STREAM)) == NULL)
//    {
//    	tftp_print( ("TftpServerCmdFun-----222\n") );
//    	tftp_print( ("error on SocketAddressFill\n") );
//    	return -1;
//    }
//    else
//	{
//    	tftp_print( ("TftpServerCmdFun-----33333\n") );
//		//返回得到的是服务器相关信息。
//		addrinfo = server_add;
//
//		tftp_print( ("entry the  ServerSockCreateAndListen\n") );
//		server_socket = ServerSockCreateAndListen(addrinfo);
//
//		while(1)
//		{
//			//给max_sock_num一个初始值，不用每次都从0开始，还要和server_socket比较。
//			max_sock_num = server_socket;
//
//			//可以提炼，但这样更清楚。
//			if ( 		( (total_connect_num < TCP_MAX_CONNECT) && (server_socket == 0) )
//					||  ( (open_socket_flage != 0) && (total_connect_num < TCP_MAX_CONNECT) )
//				)
//			{
//				tftp_print( ("TftpServerCmdFun-----44444\n") );
//				tftp_print( ("we entry the ServerSockCreateAndListen\n") );
//				server_socket = ServerSockCreateAndListen(addrinfo);
//				if (server_socket < 0)
//				{
//
//					server_socket = 0;
//					return -1;
//				}
//				open_socket_flage = 0;
//			}
//
//			timeout.tv_sec = 10;
//			timeout.tv_usec = 0;
//			FD_ZERO(&readfds);
//
//			//找最大的套接字。
//			for(ret = 0; ret < TCP_MAX_CONNECT; ret++)
//			{
//				tftp_print( ("TftpServerCmdFun-----55555\n") );
//				if (arry_socket_fd[ret] != 0)
//				{
//					tftp_print( ("FD_SET-----1111\n") );
//					FD_SET(arry_socket_fd[ret], &readfds);
//
//					if (arry_socket_fd[ret] > max_sock_num)
//					{
//						max_sock_num = arry_socket_fd[ret];
//					}
//				}
//			}
//
//			//如果tcp服务器套接字存在。则，服务器套接字绑定到select上。
//			if (server_socket)
//			{
//				FD_SET(server_socket, &readfds);
//				tftp_print( ("TftpServerCmdFun-----66666\n") );
//			}
//
//			if ( (ret = select(max_sock_num + 1, &readfds, NULL, NULL, &timeout)) < 0)
//			{
//				tftp_print( ("TftpServerCmdFun-----77777\n") );
//				tftp_print( ("the select return：%d\n",ret) );
//				tftp_print( ("the select is error!\n") );
//				return -1;
//			}
//			else
//			{
//				tftp_print( ("TftpServerCmdFun-----88888\n") );
//				//服务器套接字非0，表示套接字没有被关掉，还在运行，准备接收新的链接申请，表示未达到最大链接数量。
//				if(server_socket)
//				{
//					tftp_print( ("TftpServerCmdFun--bb--11\n") );
//					if(FD_ISSET(server_socket,&readfds))//检测是否有套接字需要接收。
//					{
//						tftp_print( ("TftpServerCmdFun--bb--222\n") );
//						if (AcceptConnectSocket(server_socket, &max_sock_num) < 0)
//						{
//							tftp_print( ("error on the AcceptConnectSocket\n") );
//							return -1;
//						}
//
//						total_connect_num ++;
//					} //end of FD_ISSET
//				}
//				//链接数已满，关闭服务器套接字。
//				//roben_ye,这个是否需要移动到上面的if语句中？？？？
//				//roben_ye,close两次，会出现线程死掉
//				if ((total_connect_num >= TCP_MAX_CONNECT) && (server_socket != 0))
//				{
//					tftp_print( ("TftpServerCmdFun-----9999\n") );
//					//roben_ye,连着关了两次
//					close(server_socket);
//					server_socket = 0;
//					open_socket_flage = 1;
//				}
//				//查找绑定select的套接字是否有数据需要读取。
//				for(ret = 0; ret < TCP_MAX_CONNECT; ret++)
//				{
//					tftp_print( ("TftpServerCmdFun--bb--6666\n") );
//					if( FD_ISSET(arry_socket_fd[ret],&readfds) )
//					{
//						total_connect_num = ConnectSockRecv(ret, total_connect_num);
//
//						if (arry_socket_fd[ret] == 0)
//						{
//							tftp_print( ("entry the close---222\n") );
//						}
//
//					}
//				}
//
//				if (total_connect_num == 0)
//				{
//					open_socket_flage = 0;
//				}
//
//			}//end of else
//
//		}//end of while
//		sched_yield();
//	}
//
//	freeaddrinfo(server_add);
//	return 0;
//}

////创建tftp服务器线程
//int CreateTFTPThread(void)
//{
//	pthread_t thread_of_tftp;
//	pthread_attr_t attr_of_tftp;
//
//	TftpThreadConfigure(&attr_of_tftp);//设置触摸屏线程属性值
//
//	int return_value_of_thread_create = 0;
//
//	//创建TFTP 服务器线程
//	return_value_of_thread_create = pthread_create(&thread_of_tftp,
//														&attr_of_tftp,
//														TftpTthreeadEntry,
//														NULL
//														);
//	return 0;
//}
