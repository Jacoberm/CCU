
#include "tftp_client_fun.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>


//修改配置文件中音频文件更新标志的状态。
int ModifyConfigfileState(char *config_file_name, int want_chang_file, int want_change_state)
{
	if (ConfigFileModify(config_file_name, want_chang_file, want_change_state) < 0)
	{
		tftp_print( ("error on ConfigFileModify\n") );
		return -1;
	}
	return 0;
}

//接收,存储标准tftp包.
int TftpControlFunction(struct sockaddr_in *udp_PTT_recv_addr,
							FILE *write_stream,
							char *down_file_name)
{
	int ret = 0;
	int err = 0;

	//要下载的文件目录，和下载后所要创建的文件目录是“一样的”（这不正确，roben）
	//现在已经分开，必须在参数中填写好。
	//小于512就默认为是结束了.
	//ret = tftp_get(down_file_name, (const struct sockaddr_in *)&udp_PTT_recv_addr, buf, kReadBufLen, TFTP_OCTET, &err);
	ret = tftp_get_analog(down_file_name, (const struct sockaddr_in *)udp_PTT_recv_addr, write_stream, TFTP_OCTET, &err);
	if (ret < 0)//下载错误
	{
		tftp_print(("roben_ye:errno:%d,%s\n",errno,strerror(errno)));
		tftp_print( ("roben_ye:ret:%d\n",ret) );
		 tftp_print( ("res = %d, err = %d\n", ret, err) );
		 return -1;
	}

	return 0;
}


//tftp下载、存储接口。
int DownloadFileFromTFTP(char *down_file_name, char *tftp_server_ip, char *save_file_name)
{
	int ret = 0;

	//打开本地保存的文件
	FILE *write_stream = fopen(save_file_name, "w+");
	if (write_stream == NULL)
	{
		fclose(write_stream);
		tftp_print( ("erro on fopen a file--DownloadFileFromTFTP \n") );
		return -1;
	}

	//创建tftp服务器端套接字地址信息.
	struct sockaddr_in udp_PTT_recv_addr;
	InitRecvAddr(&udp_PTT_recv_addr, tftp_server_ip);

	//接收,存储动作.
	ret = TftpControlFunction(&udp_PTT_recv_addr, write_stream, down_file_name);
	if (ret < 0)
	{
		tftp_print( ("error on TftpControlFunction---111\n") );
		fclose(write_stream);
		write_stream = NULL;
		return -1;
	}

	fclose(write_stream);
	write_stream = NULL;

	tftp_print( ("DownloadFileFromTFTP---end \n") );
	return 0;
}

//tftp服务器端地址打包
int InitRecvAddr(struct sockaddr_in *udp_PTT_recv_addr, char *tftp_server_ip)
{
	memset(udp_PTT_recv_addr, 0, sizeof(struct sockaddr_in));

	udp_PTT_recv_addr->sin_family = AF_INET;
	udp_PTT_recv_addr->sin_port = 0;
	udp_PTT_recv_addr->sin_addr.s_addr = inet_addr(tftp_server_ip);

	return 0;
}

//int DownloadFileFromTFTP_test(char *down_file_name, char *tftp_server_ip, char *save_file_name)
//{
//	int ret = 0;
//
//	FILE *write_stream = fopen(save_file_name, "w");
//	if (write_stream == NULL)
//	{
//		tftp_print( ("erro on fopen a file \n") );
//		//return -1;
//	}
//
//
//	//创建tftp服务器端套接字地址信息.
//	struct sockaddr_in udp_PTT_recv_addr;
//	InitRecvAddr(&udp_PTT_recv_addr, tftp_server_ip);
//
//	//接收,存储动作.
//	ret = TftpControlFunction(&udp_PTT_recv_addr, write_stream, down_file_name);
//	if (ret < 0)
//	{
//		tftp_print( ("error on TftpControlFunction\n") );
//		fclose(write_stream);
//		return -1;
//	}
//
//	fclose(write_stream);
//    tftp_print( ("DownloadFileFromTFTP_test: exit\n") );
//
//	return 0;
//}

////将下载来的数据更新到本地文件
//int RefreshDatabaseFile(FILE *writw_file_stream, char *buf, int len)
//{
//	if (WriteDataToFile(writw_file_stream, buf, len) < 0)
//	{
//		tftp_print( ("error on WriteDataToFile\n") );
//		return -1;
//	}
//
//   return 0;
//}
//
////将buf中的数据用while循环写，直到全部写进文件
//int WriteDataToFile(FILE *writw_file_stream, char *buf, int len)
//{
//	int ret = 0;
//
//	if ((ret = fwrite(buf, len, 1, writw_file_stream)) != 1)//fwrite返回值为第三个参数的值.
//	{
//		tftp_print( ("Error writing data\n") );
//		tftp_print( ("ret:%d\n", ret) );
//		return -1;
//	}
//
//	return len;
//}


//打包。
//static int InitPackage(tftp_network_cmd_t *tftp_message, char local_dev_num, int type)
//{
//
//    //将字符型IP转换为 int型IP。
////	ret = inet_addr(local_ip);
////	if (ret == INADDR_NONE)
////	{
////		tftp_print( ("error on the inet_addr\n") );
////		return -1;
////	}
//
//	//    //将int型IP转化为 字符型IP
//	//    struct in_addr test_ret;
//	//    test_ret.s_addr = local_ip_int;
//	//    char *p = inet_ntoa(test_ret);
//	//    tftp_print( ("the ip %s\n", p) );
//
//	bzero(tftp_message, sizeof(tftp_network_cmd_t));
////	tftp_message->device_ip = ret;
//	tftp_message->device_num = local_dev_num;
////	tftp_send_message->block_available = 0;
//	tftp_message->device_state = type;
//
//	return 0;
//}
//将文件内存中的缓冲区，强制回写到硬盘。
//int ForceDataFromBufToDisk(FILE *file_stream)
//{
//	int fp_id = 0;
//	//将缓存区内容同步内核缓存区。
//	if (fflush(file_stream) == EOF )//看ecos源代码.支持
//	{
//		tftp_print( ("error on fflush fun \n") );
//		return -1;
//	}
//
//	//FILE *文件指针转换为int类型。
//	if ((fp_id = fileno(file_stream)) < 0)//支持
//	{
//		tftp_print( ("error on the fileno fun\n") );
//		return -1;
//	}
//
//	//将内核缓存区写道磁盘.
//	if (fsync(fp_id) < 0)//支持
//	{
//		tftp_print( ("error on the fsync fun\n") );
//		return -1;
//	}
//	return 0;
//}

//打包数据和发送数据。
//int InitAndSendPackage(int local_sock_send, int local_dev_num, int type)
//{
//	tftp_network_cmd_t tftp_message;
//
////	tftp_print( ("connectin server,send package-----222\n") );
//
//	if ( InitPackage(&tftp_message, local_dev_num, type) < 0 )
//	{
//		tftp_print( ("error on the InitPackage\n") );
//		return -1;
//	}
//	if ( TCPsend(local_sock_send, &tftp_message, sizeof(tftp_network_cmd_t), 0) < 0)
//	{
//		tftp_print( (" error on the TCPsend\n") );
//		return -1;
//	}
//	return 0;
//}
