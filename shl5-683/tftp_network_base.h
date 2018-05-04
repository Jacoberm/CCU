/*
 * tftp_network_base.h
 *
 *  Created on: 2013-5-21
 *      Author: benty
 */

#ifndef TFTP_NETWORK_BASE_H_
#define TFTP_NETWORK_BASE_H_

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

#include <network.h>
#include <arpa/tftp.h>
#include <tftp_support.h>


//1（客服端发送），请求。
//2（服务器端发送），busy（##现在不用了####）
// 3（服务器端发送），同意更新。
//4（客服端发送），确认更新包。
//5（客服端发送），结束跟新包。
//6（服务器端发送），更新结束。
typedef enum
{
	start_device_state = '0',
	request = '1',
	busy,
	agree_request,
	ACK_agree_request,
	end_refresh,
	ACK_end_refresh,
	end_device_state
}device_state;				//现在不使用

//typedef struct
//{
//	char device_state;//当前设备，关于tftp跟新的状态。1（客服端发送），请求。
//						//2（服务器端发送），busy（##现在不用了####）
//						// 3（服务器端发送），同意更新。4（客服端发送），确认更新包。5（客服端发送），结束跟新包。
//						//6（服务器端发送），更新结束。
//	char device_num;	//暂时现不用。
//
//	unsigned int device_ip;
//}tftp_network_cmd_t;

//#ifndef TFTP_SERVER_RECV_PORT
//	#define TFTP_SERVER_RECV_PORT 33337
//#else
//	#error TFTP_SERVER_RECV_PORT defined
//#endif

//#define TFTP_PRINT
#ifndef TFTP_PRINT
   #define tftp_print(s) //diag_printf s
//   #define tftp_print(s) diag_printf  s
#else
   #define tftp_print(s)
#endif


//int SocketSet(int socket_fd);

//tcp 接收数据
//int TCPrecv(int sockfd,void *buf,int len,int flags);
////tcp发送数据。
//int TCPsend(int sockfd, void *buf, int len, int flags);
//将端口号和IP地址填充到套接字地址中。
//struct addrinfo * SocketAddressFill(char *tftp_server_ip, int port, int socket_type);
//申请空间
void *LocalTftpMalloc( unsigned int wantedSize );
//释放空间
void LocalTftpFree( void *pv );
//将下载来的数据保存到本地文件
int RefreshDataToFile(FILE *writw_file_stream, char *buf, int len);
//强制将内存中的数据写到flash中，
int ForceDataFromBufToDisk(FILE *file_stream);
//将buf中的数据用while循环写，直到全部写进文件
int WriteDataToFile(FILE *writw_file_stream, char *buf, int len);

int tftp_get_analog(const char * const filename,
         const struct sockaddr_in * const server,
         FILE *write_stream,
         const int mode,
         int * const err);

int tftp_client_get_analog(const char * const filename,
                    const char * const server,
                    const int port,
                    FILE *write_stream,
                    const int mode,
                    int * const err);

int tftp_client_get_inner_analog(char *data,
                   const char * const filename,
                   const char * const server,//TFTP server ip号
                   const int port,
                   FILE *write_stream,
                   const int mode,
                   int * const err
#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
                   ,int negotiate
#endif
                   );

//tftp and sqlite 可能都要使用。
//copy file from one to anther
int CopyFile(char *copied_file,char * covered_file);

//通过比较文件生成校验和与保存的校验和，查看文件是否被改动。
//int TestFileCRC(const char* crc_file_name, const char* crc_txt_file, const char* db_back_up);
int TestFileCRC(const char* back_db_file, const char* crc_txt_file);

//对指定文件做crc 32 位校验和，并存储到指定文件。
int GenerateFileCRC(const char* crc_file_name, const char* crc_save_file );
#endif /* TFTP_NETWORK_BASE_H_ */
