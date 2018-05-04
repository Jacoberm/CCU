/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： udp_send_fun.h
 * 文件标识：
 *
 * 当前版本： V1.0.1
 * 作    者： roben_ye
 * 摘    要： 3.添加while循环，将传送下来的数据全部发送完
 * 完成日期： 2012-09-19
 *
 * 取代版本： V1.0.0
 * 原作者  ： roben_ye
 * 摘    要： 1. 发送控制命令函数
 *			  2. 打包音频数据、发送音频等函数
 * 完成日期：2012-08-20
 */
#ifndef UDP_SEND_FUN_H_
#define UDP_SEND_FUN_H_

////roben_ye_0001>>>>>>>>>
#include <network.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#include "udp_recv_fun.h"
#include "udp_type.h"

/*********************************************************
 ** ������ƣ� UdpSendFunDATA
 ** �����ܣ� ����һ����Ƶ��ݺ���Ľӿ�
 ** ����˵����  1��          sockfd�� ��Ƶ��ݷ��͵�UDP�׽���
 **           2��    		buffer�� ����������Ϣָ��
 **           3��		buffer_len�� ���͵Ŀ�����Ϣ�ĳ���
 **           4��    SEND_IP[16]�� UDPĿ�ĵ�ַ��IP��
 **           5��	 send_port_num�� UDPĿ�ĵ�ַ�Ķ˿ں�
 ** ����أ�  1��    			ret�� ʵ�ʷ��͵ĳ���
 **            2��				  -1�� ������Ϣ
 ** ����˵����
 ********************************************************/
int UdpSendFunDATA(int sockfd, void *buffer, int buffer_len, char SEND_IP[16],int send_port_num);

/*********************************************************
 ** ������ƣ� UdpSendFunCMD
 ** �����ܣ� ����һ����������ṹ����Ϣ��ͬʱҲ�����ϲ�Ľӿں���
 ** ����˵����  1��          sockfd�� �����Ŀ�������͵�UDP�׽���
 **           2��    		buffer�� ����������Ϣָ��
 **           3��		buffer_len�� ���͵Ŀ�����Ϣ�ĳ���
 **           4��    SEND_IP[16]�� UDPĿ�ĵ�ַ��IP��
 **           5��	 send_port_num�� UDPĿ�ĵ�ַ�Ķ˿ں�
 ** ����أ�  1��    			ret�� ʵ�ʷ��͵ĳ���
 **            2��			 	 -1�� ������Ϣ
 ** ����˵����
 ********************************************************/
int UdpSendFunCMD(int sockfd, void *buffer, int buffer_len, char SEND_IP[16],int send_port_num);

/*********************************************************
 ** ������ƣ� UdpSendBuffer
 ** �����ܣ� ������͵���Ƶ��ݵĳ��ȵĲ�ͬ���Դ�����������÷��ͺ�����ݷ���Ŀ�ĵ�ַ
 ** ����˵����  1��          sockfd�� ��Ƶ��ݷ��͵�UDP�׽���
 **           2��    		buffer�� ����������Ϣָ��
 **           3��		buffer_len�� ���͵Ŀ�����Ϣ�ĳ���
 **           4��      SendtoAdd�� ���͵�ַ�����˿ں�IP��ַ�����һ�������ġ�
 ** ����أ�  1��    			ret�� ʵ�ʷ��͵ĳ���
 **            2��				  -1�� ������Ϣ
 ** ����˵����
 ********************************************************/
int UdpSendBuffer(int sockfd, void *buffer, int buffer_len,struct sockaddr_in *SendtoAdd);

/*********************************************************
 ** ������ƣ� InitSendPackage
 ** �����ܣ� ��malloc����Ŀռ������ݣ���Щ��ݽ�����Ŀ�ĵء���һ������Ϊ��ݴ��
 ** ����˵����  1��      pToSendBuf�� ָ��malloc����Ŀռ�
 **           2��    		buffer�� �ϲ㴫��������Ƶ��ݣ���Щ�����Ҫ����
 **           3��		buffer_len�� ���͵���Ƶ��ݵĳ���

 ** ����أ�  1��    			  0�� �ɹ����
 ** ����˵����
 ********************************************************/
int InitSendPackage(data_package_node_t *pToSendBuf, void *buffer, int buffer_len);

#endif /* UDP_DUOBO_SERVER_H_ */
