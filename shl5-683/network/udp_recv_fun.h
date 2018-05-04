/*
 * Copyright (c) 2012��MIT
 * All right reserved.
 *
 * �ļ���ƣ� udp_send_fun.h
 * �ļ���ʶ��
 *
 * ��ǰ�汾�� V1.0.1
 * ��    �ߣ� roben_ye
 * ժ    Ҫ�� 4.����µ������������Ŵ����ֵ��255���������С��0-3����������⡣
 * ������ڣ� 2012-09-22
 *
 * ȡ��汾�� V1.0.0
 * ԭ����  �� roben_ye
 * ժ    Ҫ�� 1. ���ܿ��ƺ���Ƶ ��ݰ�
 *			  2. ����Ƶ��ݰ����򡢶�����
 *			  3. �����ܵ��Ŀ��������ʹ�������Ƶ��ݰ�move���ഺ������buffer��
 * ������ڣ�2012-08-20
 */
#ifndef UDP_RECV_FUN_H_
#define UDP_RECV_FUN_H_

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
#include "udp_type.h"
#include "network_include_extern_head.h"

int UdpRecvDataPart2( int socket, int flage, buffer_pointer_t *BufferPoint, int buffer_rw_id);
int DataRecvAndHandle2(const int sockfd, int buffer_rw_id,data_package_node_t** pToData,
								data_package_node_t* ArrayToHandle[]);
int GetNetData2(const int sockfd,void *pToData, int len_num);
int SortAlgorithm2(data_package_node_t **pToData, data_package_node_t *ArrayToHandle[], int buffer_rw_id);
int PushToDataBuffer2(data_package_node_t *pushed_package, int buffer_rw_id);
int PushToBuffer2(int  buffer_rw_id, const void *PushBuffer, int len);
int CopyToAppBuffer2(data_package_node_t *ArrayToHandle[], int buffer_rw_id);
int WriteTobuffer2(data_package_node_t *ArrayToHandle[], int buffer_rw_id);
int AddAndSortFun2(data_package_node_t **pToData, data_package_node_t *ArrayToHandle[], int buffer_rw_id);
int AddPakageToCache2(data_package_node_t **pToData, data_package_node_t *ArrayToHandle[]);


/*********************************************************
 ** ������ƣ� InitSocketAdd
 ** �����ܣ� ��ʼ���ṹ��InitAddress��ʹ�����IP�Ͷ˿ڸ�ֵ�����Ա
 ** ����˵����  1��socket_port_num�� �˿ں�
 **           2��    SocketIPNum�� IP��
 **           3��	   InitAddress�� ��Ҫ��ʼ���Ľṹ��
 ** ����أ�  1��     InitAddress�� �ṹ���ʼ���ɹ�
 **           2��		   exit(-1)�� �ṹ���ʼ�����ɹ���IP��ַ��ֵ��������
 ** ����˵����
 ********************************************************/
struct sockaddr_in* InitSocketAdd(int socket_port_num, char SocketIPNum[16],
									   struct sockaddr_in* InitAddress);

/*********************************************************
 ** ������ƣ� UdpRecv
 ** �����ܣ� ���ܺ���������ƺ���ݣ������庯�����ģ�麯�������Ӧ����
 ** ����˵����  1��			 sockfd�� ���ν�����Ϣ��UDP�׽���
 **           2��	   		  flage�� �����ǽ��տ����������Ƶ���
 **           3��    	    pToData�� ��λָ�룬ָ��ǰ������ʱ����������Ƶ��ݵĿ�ʼλ��
 **           4��	   	 pToCommand�� ָ��ǰ������ʱ�����������Ŀ�ʼλ��
 **           5��	  ArrayToHandle�� ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������
 **           	6��	   	 buffer_rw_id�� ָ����Ҫ����ݴ����buffer�������buffer��ָӦ�ò�ģ��������Լ������Cache).
 **
 ** ����أ�  1��     				0�� �ɹ�����
 **           2��		  		   -1�� ��̳��?��ǰ����
 ** ����˵����Cache�� recv�������Ҫ��������Ƶ�������
 ** 		Buffer�� �ഺ���������ڴ洢���
 **   BlockBuffer�� �ഺ���������ڴ洢��������
 ********************************************************/
int UdpRecv(const int sockfd, const int flage, data_package_node_t **pToData,
		network_send_package_t *pToCommand, data_package_node_t *ArrayToHandle[], int  buffer_rw_id);

/*********************************************************
 ** ������ƣ� PushToCommandBuffer
 ** �����ܣ� ��ȡAddDataToBlockBuffer����ӿڵĲ���
 ** ����˵����  1��		  pToCommand�� �����˿��������ָ�룬����ָ��Ŀ������Ҫ��move��BlockBuffer
 ** ����أ�  1��     				0�� �ɹ�����
 **           2��		  		   -1�� ��̳��?��ǰ����
 ** ����˵����
 ********************************************************/
int PushToCommandBuffer(const network_send_package_t *pToCommand, int buffer_rw_id);

/*********************************************************
 ** ������ƣ� PushToBlockBuffer
 ** �����ܣ� ����AddDataToBlockBuffer����ӿڣ�����������(BlockBuffer)д���
 ** ����˵����  1��		 pToBuffer�� PushToCommandBuffer��ȡ�Ĳ���
 **           2��     PushBuffer�� ָ��Ҫ���ƵĿ���������׵�ַ
 **           3��	   			len�� ��Ҫ���ƵĿ�������ĳ���
 ** ����أ�  1��     			ret�� ʵ�ʴ�PushBufferָ��ĵ�ַ���Ƶ�BlockBuffer�ĳ���
 **            2��		  		 -1�� ��̳��?��ǰ����
 ** ����˵����
 ********************************************************/
int PushToBlockBuffer(int buffer_rw_id, const void *PushBuffer, int len);

/*********************************************************
 ** ������ƣ� WriteTobuffer
 ** �����ܣ� ��ArrayToHandle[0]ָ�����ݸ��Ƶ���Ƶbuffer��Buffer�������ƶ�ָ��
 ** 			��ʹArrayToHandle[0]ָ����һ������ArrayToHandle[0]����Ҫ�����Ƶ���Ƶbuffer�Ŀ�ʼλ��λ�á�
 ** 			��������Ǹ�����ݵ���Ƶbuffer��Buffer���Ľӿ�
 ** ����˵����  1��		    counter�� ����ǰ��Cache���ݴ����Ƶ�����
 **           2��     push_flage�� �Ƿ���Ҫ��ʼ���ƣ��ʼ��4���������ʱ�����Ƶģ������㹻�����ݡ�����󣬲ſ�ʼ���ƣ�
 **           3��	 ArrayToHandle��ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������Ҫ���ƵĿ�������ĳ���
 ** ����أ�  1��     			  0�� �ɹ�����
 ** 			2��		  		 -1�� ��̳��?��ǰ����
 ** ����˵����push_flage��0���յ�����ݻ������������ڻ����Ὺʼ������ݵ���Ƶbuffer
 ** 					   1���յ�������Ѿ������ˣ����Ը�����ݵ���Ƶbuffer
 ********************************************************/
int WriteTobuffer(int *counter, int *push_flage, data_package_node_t *ArrayToHandle[], int buffer_rw_id);

/*********************************************************
 ** ������ƣ� PushToDataBuffer
 ** �����ܣ���ȡAddDataToBuffer����ӿڵĲ�����ú�������Ƶbufferд���
 ** ����˵����  1��	 ArrayToHandle��ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������Ҫ���ƵĿ�������ĳ���
 ** ����أ�  1��     			  0�� �ɹ�����
 **			   2��		  		 -1�� ��̳��?��ǰ����
 ** ����˵����
 ********************************************************/
int PushToDataBuffer(data_package_node_t *ArrayToHandle[], int buffer_rw_id);


/*********************************************************
 ** ������ƣ� PushToBuffer
 ** �����ܣ� ����AddDataToBuffer����ӿڣ���cache��ʵ�����������ݸ�����Ƶ����buffer��
 ** ����˵����  1��		 pToBuffer�� PushToDataBuffer�л�ȡ�Ĳ���
 **           2��     PushBuffer�� ָ��Ҫ���Ƶ���Ƶ��ݵ��׵�ַ
 **           3��	   			len�� ��Ҫ���ƵĿ�������ĳ���
 ** ����أ�  1��     			ret�� ʵ�ʴ�PushBufferָ��ĵ�ַ���Ƶ�BlockBuffer�ĳ���
 **            2��		  		 -1�� ��̳��?��ǰ����
 ** ����˵����
 ********************************************************/
int PushToBuffer(int buffer_rw_id, const void *PushBuffer, int len);

/*********************************************************
 ** ������ƣ� ArraySort
 ** �����ܣ� ʵ�ֳ�Աָ�����ת��ʹ��ָ������һ����Ա��ָ���λ�ã�ArrayToHandle��7��ָ��ArrayToHandle��0��
 ** ����˵����  1��	 ArrayToHandle��ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������Ҫ���ƵĿ�������ĳ���
 ** ����أ�  1��     			  0�� �ɹ�����
 ** ����˵����
 ********************************************************/
int ArraySort(data_package_node_t *ArrayToHandle[]);

/*********************************************************
 ** ������ƣ� GetNetData
 ** �����ܣ� �������������ݣ������������Ƶ��ݶ�������
 ** ����˵����  1��		 	 sockfd�� ������ݵ�UDP�׽���
 **           2��        pToData�� ָ��Ҫ�����ݴ�������ݵ�buffer���׵�ַ
 **           3��	   		len_num�� ��Ҫ��������յ�������ݵĳ���
 ** ����أ�  1��     	   exit(-1)�� recvfrom�������
 **           2��		  	pToData�� �ɹ����ܣ�������ָ������յ�����ݵ��׵�ַ
 ** ����˵���� �������������ָ����recvfrom����Ҫ���յ����
 ********************************************************/
void* GetNetData(const int sockfd,void *pToData, int len_num);

/*********************************************************
 ** ������ƣ� PrintPortAndIp
 ** �����ܣ� ��ӡ����struct sockaddr_in recv_add ��Я��Ķ˿ں�IP��Ϣ
 ** ����˵����  1��	     recv_add��struct sockaddr_in�ṹ����������Ա�����˿ں�IP��
 ** ����أ�  1��     			  0�� �ɹ�����
 ** ����˵����
 ********************************************************/
int PrintPortAndIp(struct sockaddr_in recv_add);

/*********************************************************
 ** ������ƣ� SortAlgorithm
 ** �����ܣ� ������򡢶�������庯�����ģ�麯�������Ӧ����
 ** ����˵����  1��			counter�� ����ǰ��Cache���ݴ����Ƶ�����
 **           2��     push_flage�� �Ƿ���Ҫ��ʼ���ƣ��ʼ��4���������ʱ�����Ƶģ������㹻�����ݡ�����󣬲ſ�ʼ���ƣ�
 **           3��    	    pToData�� ��λָ�룬ָ��ǰ������ʱ����������Ƶ��ݵĿ�ʼλ��
 **           4��	  ArrayToHandle�� ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������
 ** ����أ�  1��     				0�� �ɹ�����
 ** ����˵����
 ********************************************************/
//int SortAlgorithm(int *counter,int *push_flage, data_package_node_t **pToData,
//						data_package_node_t *ArrayToHandle[]);
int AddOrAbandonFun(int *counter,int *push_flage, const unsigned char kCacheCriticalValue,
				data_package_node_t **pToData,data_package_node_t *ArrayToHandle[],int buffer_rw_id);

int SortAlgorithm(int *counter,int *push_flage, data_package_node_t **pToData,
							data_package_node_t *ArrayToHandle[],int buffer_rw_id);

/*********************************************************
 ** ������ƣ� ArrayMoveSort
 ** �����ܣ� �������.ͨ��Ƚ�(*pToData)->package_counter��ArrayToHandle[]->package_counter�Ĵ�С��ʵ������
 ** ����˵����  1��			counter�� ��ǰCache���ݴ����Ƶ�����
 **           2��    	    pToData�� ��λָ�룬ָ��ǰ������ʱ����������Ƶ��ݵĿ�ʼλ��
 **           3��	  ArrayToHandle�� ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������
 ** ����أ�  1��     				0�� �ɹ�����
 ** ����˵����
 ********************************************************/
int ArrayMoveSort(int *counter, data_package_node_t **pToData, data_package_node_t *ArrayToHandle[]);

/*********************************************************
 ** ������ƣ� AddPakageToCache
 ** �����ܣ� ArrayToHandle[*counter]ָ������յ���������ŵ��׵�ַ��
 ** 			���ã�*pToData��ָ��һ���µ�λ�ã��Ա��´����ڽ�����ݡ�
 ** ����˵����  1��			counter�� ��ǰCache���ݴ����Ƶ�����
 **           2��    	    pToData�� ��λָ�룬ָ��ǰ������ʱ����������Ƶ��ݵĿ�ʼλ��
 **           3��	  ArrayToHandle�� ָ�����飬ÿ����Ա��һ����Ƶ��ݵĽṹ��ָ�룬ָ������ָ����������cache
 **           							����ͨ��ϵ��ƶ���ʵ��cache��ѭ������
 ** ����أ�  1��     				0�� �ɹ�����
 ** ����˵����
 ********************************************************/
int AddPakageToCache(int *counter, data_package_node_t **pToData, data_package_node_t *ArrayToHandle[]);

/*********************************************************
 ** ������ƣ� CacheCreate
 ** �����ܣ� ������������Ľ��պ���Ƶ��ݵĽ��ա�������Ҫ�õ���buffer
 ** ����˵����
 ** ����أ�  1��     	px_new_buffer�� �ɹ����ء�һ����ʼ�����buffer_pointer_t�ṹ�塣
 ** 								��Ա����ָ������������Ƶ��ݿ�ʼλ�õ�buffer
 ** ����˵����
 ********************************************************/
buffer_pointer_t CacheCreate(void);
/*********************************************************
 ** ������ƣ� CacheFree
 ** �����ܣ� �ͷ�CacheCreate��������buffer
 ** ����˵����			px_new_buffer��buffer_pointer_t�ṹ�壬��Ա��CacheCreate�г�ʼ��
 ** ����أ�  1��     		0�� �ɹ����ء�
 ** ����˵����
 ********************************************************/
int CacheFree(buffer_pointer_t px_new_buffer);

/*********************************************************
 ** ������ƣ� UdpRecvFun
 ** �����ܣ� UDP���ܲ��ֵġ�main������Ҳ�����ϲ�Ľӿں���
 ** ����˵����  1��			 socket�����ν�����Ϣ��UDP�׽���
 **           2��    	     flage�� �жϱ��ε��ý��ܺ�������Ϊ�˽��ܿ����������Ƶ���
 **           3��	   BufferPoint�� buffer_pointer_t�ṹ��ָ�룬ָ��CacheCreate�ķ���ֵ��
 **           4�� 	    buffer_rw_id �� ָ����Ҫ����ݴ����buffer�������buffer��ָӦ�ò�ģ��������Լ������Cache��
 ** ����أ�  1��     				0�� �ɹ�����
 ** 							   -1�� ��̳��?��;����
 ** ����˵����	flage==1 : ����������ܵ���UdpRecvFun
 ** 			flage==2 : ��Ƶ��ݽ��ܵ���UdpRecvFun
 ********************************************************/
int UdpRecvFun(const int socket, int flage,buffer_pointer_t *BufferPoint, int buffer_rw_id);

/*********************************************************
 ** ������ƣ� UdpRecvDataPart
 ** �����ܣ� ��������Ľ��ܺ���UdpRecv����������buffer_pointer_t�ĳ�Ա����
 ** ����˵����  1��	        socket�� ���ν�����Ϣ��UDP�׽���
 **           2��	   		  flage�� �жϱ��ε��ý��ܺ�������Ϊ�˽��ܿ����������Ƶ���
 **           3��	   BufferPoint�� buffer_pointer_t�ṹ��ָ�룬ָ��CacheCreate�ķ���ֵ��
 **           4��	   buffer_rw_id�� ָ����Ҫ����ݴ����buffer�������buffer��ָӦ�ò�ģ��������Լ������Cache).
 ** ����أ�  1��     				0�� �ɹ�����
 ** 							   -1�� ��̳��?��;����
 ** ����˵����
 ********************************************************/
int UdpRecvDataPart( int socket, int flage, buffer_pointer_t *BufferPoint, int buffer_rw_id);

/*********************************************************
 ** ������ƣ� SocketBinding
 ** �����ܣ� �׽��ְ󶨶˿ڲ����IP����
 ** ����˵����  1��	    socket_num�� ����Ҫ�󶨵��׽���
 **           2��	   BindAddress�� bind�����һ��������������ں����ڲ�����
 **           3��	   	  BindingIP�� �׽�����Ҫ�󶨵�IP��ַ
 **           4��	 bind_port_num�� �׽�����Ҫ�󶨵Ķ˿ں�
 ** ����أ�  1��     			   0�� �ɹ�����
 ** 							  -1�� bind����������?��;����
 ** ����˵����
 ********************************************************/
int SocketBinding(int socket_num, struct sockaddr_in BindAddress,
		 char BindingIP[16], int bind_port_num);


int CmdRecvAndHandle(int sockfd, network_send_package_t* pToCommand, int  buffer_rw_id);
int DataRecvAndHandle(const int sockfd,int buffer_rw_id,data_package_node_t** pToData,
								data_package_node_t* ArrayToHandle[]);
int ForceClearDataCache(data_package_node_t* ArrayToHandle[], data_package_node_t** pToData);///<2013-10-31
int ForcePushDataCacheToBuffer(data_package_node_t *ArrayToHandle[], data_package_node_t **pToData, int  buffer_rw_id);
#endif /* UDP_RECV_FUN_H_ */
