/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： udp_send_fun.h
 * 文件标识：
 *
 * 当前版本： V1.0.1
 * 作    者： roben_ye
 * 摘    要： 4.添加新的条件，解决包号从最大值（255）到包号最小（0-3）丢包的问题。
 * 完成日期： 2012-09-22
 *
 * 取代版本： V1.0.0
 * 原作者  ： roben_ye
 * 摘    要： 1. 接受控制和音频 数据包
 *			  2. 对音频数据包排序、丢包处理
 *			  3. 将接受到的控制命令包和处理后的音频数据包move到青春创建的buffer中
 * 完成日期：2012-08-20
 */
#include "udp_recv_fun.h"

#define _LED
#ifndef _LED
   #define udp_print(s) //diag_printf s
#else
   #define udp_print(s)
#endif


unsigned int byte_into_buffer = 0; //debug,2012
unsigned int buffer_over_times = 0; //debug,2012
unsigned int net_recv_bytes = 0;// debug,2012
unsigned int net_recv_total_bytes = 0; //debug,2012

int UdpRecvBytesDebug(void)
{
     udp_print(("UdpRecvBytesDebug:  %d,%d,%d,%d\n", byte_into_buffer, buffer_over_times, net_recv_bytes, net_recv_total_bytes ));
     return 0;
}

struct sockaddr_in* InitSocketAdd(int socket_port_num, char SocketIPNum[16],
												struct sockaddr_in* InitAddress)
{
	int ret = 0;
	bzero((void*)InitAddress,sizeof(*InitAddress));

	(*InitAddress).sin_family = AF_INET;
	(*InitAddress).sin_port = htons(socket_port_num);
	if ( (ret = inet_pton(AF_INET, SocketIPNum, (char *)&(*InitAddress).sin_addr.s_addr)) <= 0)
	{
		udp_print((" ERROR in  transforming the IP add\n"));
	 	return NULL;
	}
	return InitAddress;
}

int SocketBinding(int socket_num, struct sockaddr_in SockAddr,
		 char BindingIP[16], int bind_port_num)
{
	struct sockaddr_in* BindAddress = NULL;
	BindAddress = InitSocketAdd(bind_port_num, BindingIP, &SockAddr);
	if (NULL == BindAddress)
	{
		udp_print(("wrong on the BindAddress after InitSocketAdd\n"));
		return -1;
	}
	if (bind(socket_num, (const struct sockaddr *)BindAddress, sizeof(*BindAddress)) < 0)
	{
		udp_print(("ERROR on bind\n"));
		return(-1);
	}

	return 0;
}

buffer_pointer_t CacheCreate(void)
{
	int ret =0;
	unsigned int cache_size = Cache_PACK_NUM*sizeof(data_package_node_t);
	unsigned int command_size = sizeof(network_send_package_t);
	buffer_pointer_t px_new_buffer = {0};
	px_new_buffer.pToCommand = (network_send_package_t *)malloc(command_size);

	px_new_buffer.pToHead = (data_package_node_t*) malloc(cache_size);

	px_new_buffer.pToData = px_new_buffer.pToHead;
	for(ret = 0; ret < Cache_PACK_NUM; ret++)
	{
		px_new_buffer.ArrayToHandle[ret] = (px_new_buffer.pToHead)++;
	}
	px_new_buffer.pToHead = px_new_buffer.pToData;

	return  px_new_buffer;
}
int CacheFree(buffer_pointer_t px_new_buffer)
{
	free(px_new_buffer.pToCommand);
	free(px_new_buffer.pToHead);
	return 0;
}
int clean_force_len = 0;

int ForceClearDataCache(data_package_node_t* ArrayToHandle[], data_package_node_t** pToData)
{
	udp_print(("we entry the ForceClearDataCache\n"));
	udp_print(("ArrayToHandle:%d\n",ArrayToHandle[0]->package_counter));
	udp_print(("*pToData:%d\n",(*pToData)->package_counter));
	if ( (NULL == ArrayToHandle) && (NULL == *pToData))
	{
		udp_print(("the ForceClearDataCache's input parameter\n"));
	}
	else
	{
		*pToData = ArrayToHandle[0];
		clean_force_len = 1;
	}
	udp_print(("ArrayToHandle:%d\n",ArrayToHandle[0]->package_counter));
	udp_print(("*pToData:%d\n",(*pToData)->package_counter));
	return 0;
}

int ForcePushDataCacheToBuffer(data_package_node_t *ArrayToHandle[], data_package_node_t **pToData, int  buffer_rw_id)
{
//	udp_print(("ArrayToHandle:%d\n",ArrayToHandle[0]->package_counter));
//	udp_print(("*pToData:%d\n",(*pToData)->package_counter));
//	udp_print(("############we entry the ForcePushDataCacheToBuffer\n"));

	int ret_ye = 0;

	if (NULL == *pToData)
	{
		udp_print(("the ForcePushDataCacheToBuffer's input parameter\n"));
	}

	while(ret_ye < Cache_PACK_NUM)
	{
		if (*pToData != ArrayToHandle[ret_ye])
		{
			udp_print(("we want to push num:%d\n",ArrayToHandle[ret_ye]->package_counter));
			PushToBuffer(buffer_rw_id,(const void*)(ArrayToHandle[ret_ye]->data_buffer),  (ArrayToHandle[ret_ye]->send_len));
		}
		else if (*pToData == ArrayToHandle[ret_ye])
		{
			*pToData = ArrayToHandle[0];
			clean_force_len = 1;
			break;
		}
		else
		{
			udp_print(("ForcePushDataCacheToBuffer: has wrong\n"));
		}
		ret_ye++;
	}
	udp_print(("ArrayToHandle:%d\n",ArrayToHandle[0]->package_counter));
	udp_print(("*pToData:%d\n",(*pToData)->package_counter));
	return 0;
}

int UdpRecvFun(const int socket, int flage,buffer_pointer_t *BufferPoint, int buffer_rw_id)
{
	int ret_ye = 0;
	static int write_data_buffer_id = 0;//控制命令时，将buffer全部拷贝到青春的buffer使用

	if (flage == 1)//command
	{
		ret_ye = UdpRecvDataPart(socket, flage, BufferPoint, buffer_rw_id);

//		ForcePushDataCacheToBuffer(BufferPoint->ArrayToHandle,&(BufferPoint->pToData),write_data_buffer_id);

		if (ret_ye < 0)
		{
			udp_print(("UdpRecvDataPart < 0\n"));
			return(-1);
		}
	}
	else if (flage == 2)//data
	{
		write_data_buffer_id = buffer_rw_id;
//		if (UdpRecvDataPart(socket, flage, BufferPoint, buffer_rw_id) < 0)
//		{
//			udp_print(("UdpRecvDataPart < 0\n"));
//			return(-1);
//		}

		if (UdpRecvDataPart2(socket, flage, BufferPoint, buffer_rw_id) < 0)
		{
			udp_print(("UdpRecvDataPart < 0\n"));
			return(-1);
		}

	}
	else
	{
	    udp_print(("wrong flag, not command or data\n"));
		return(-1);
	}
	return 0;
}


int UdpRecvDataPart2( int socket, int flage, buffer_pointer_t *BufferPoint, int buffer_rw_id)
{
	if(DataRecvAndHandle2(socket, buffer_rw_id, &(BufferPoint->pToData), BufferPoint->ArrayToHandle) < 0)
	{
		 udp_print(("DataRecvAndHandle return value <0\n"));
	 	 return -1;
	}

	return 0;
}

char network_buffer_num = 0;//记录当前网络排序buffer中的包数
unsigned int desired_value = 0;	//希望接收到的包的需要
unsigned int push_value = 0;		//刚才copy到应用层buffer的包的包号

#define MAX_SORT_NUFFER_NUM Cache_PACK_NUM/2	//当大于等于3时，必须强制复制到应用层buffer。
#define MAX_PRE_ARRIVE_NUM 12	//网络包最大可以提前到达数量

int DataRecvAndHandle2(const int sockfd, int buffer_rw_id,data_package_node_t** pToData,
								data_package_node_t* ArrayToHandle[])
{
	int loop_ye = 0;

	if ((sockfd < 0) || (NULL == pToData) || (NULL == *pToData))
	{
		udp_print(("wrong parameter of DataRecvAndHandle\n"));
		return -1;
	}

	for (loop_ye = 0; loop_ye < Cache_PACK_NUM; loop_ye++)
	{
		if (!ArrayToHandle[loop_ye]  )
		{
			udp_print(("wrong parameter ArrayToHandle of DataRecvAndHandle\n"));
			return -1;
		}
	}

	if (GetNetData2(sockfd, (void*)*pToData, sizeof(**pToData)) < 0)
	{
		udp_print(("DataRecvAndHandle of GetNetData return is NULL\n"));
		return -1;
	}

	SortAlgorithm2(pToData, ArrayToHandle,buffer_rw_id);

	 return 0;
}


//数据排序、丢弃的主要函数功能
//排序仅仅为音频包
int SortAlgorithm2(data_package_node_t **pToData, data_package_node_t *ArrayToHandle[], int buffer_rw_id)
{
	//正常情况，network buffer应该是空的。
	//以及d2d结束，又进行d2d
	if (0 == network_buffer_num)
	{
//		//期望包号和接收包号相等，直接copy该包到应用层buffer
//		if ((*pToData)->package_counter == desired_value)
//		{
//			desired_value++;
//			//copy到应用层
//			int copy_size = 0;
//			if ((copy_size = PushToDataBuffer2(ArrayToHandle, buffer_rw_id)) < 0)
//			{
//				udp_print(("PushToDataBuffer copy_size：%d\n", copy_size));
//			}
//		}
//		//设备用来举例
//		//d2p结束后（音频流正常结束），d2d开始
//		//此时，如果出现2，1，3，4.。。。；则“1”丢弃
//		else
//		{
//			AddPakageToCache2(pToData, ArrayToHandle);
//			desired_value = pToData->package_counter + 1;
//		}

		//不能出现：network_buffer_num ==0， 但是ArrayToHandle[0] ！= (*pToData)


		//正常情况，network buffer应该是空的。
		//如果network buffer是空，收到任何包都copy到应用层buffer中去。
		int copy_size = 0;
		if ((copy_size = PushToDataBuffer2(*pToData, buffer_rw_id)) < 0)
		{
			udp_print(("PushToDataBuffer copy_size：%d\n", copy_size));
		}

	}
	else	//network buffer存在包，可能要排序、copy到应用层buffer、丢弃包
	{
//		//期望包号和接收包号相等，copy到应用层buffer
//		if ((*pToData)->package_counter == desired_value)
//		{
//			//现将包添加进network buffer
//			AddPakageToCache2(pToData, ArrayToHandle);
//			desired_value += 1;
//
//			//copy包到应用层buffer
//			//1，3，4，5 仅仅拷贝1，下一次可能拷贝2，3，4，5；或者345，或者3456
//		}
//
//		AddAndSortFun2(pToData, ArrayToHandle,buffer_rw_id);

		//将包丢掉，并且将copy一个包到应用层buffer
		if(((*pToData)->package_counter <= push_value)|| ((*pToData)->package_counter > (desired_value + MAX_PRE_ARRIVE_NUM)))
		{
			int copy_size = 0;
			//discard this package
			//拷贝所有能拷贝的包。
			//期望51，network buffer：52，53，现在来了1。丢弃1，拷贝52，53，期望54
			//期望51，network buffer：52，54，现在来了1。丢弃1，拷贝52，期望53

			CopyToAppBuffer2(ArrayToHandle, buffer_rw_id);
		}
		else//到这里的包丢不应该被丢弃
		{
			AddAndSortFun2(pToData, ArrayToHandle, buffer_rw_id);
			//比较期望值和A【0】的包号是否相等。
			if(desired_value == ArrayToHandle[0]->package_counter)
			{
				CopyToAppBuffer2(ArrayToHandle, buffer_rw_id);
			}
			if(MAX_SORT_NUFFER_NUM < network_buffer_num)
			{
				CopyToAppBuffer2(ArrayToHandle, buffer_rw_id);
			}
		}

	}


//	if ((*counter != 0) && (*counter<kCacheCriticalValue))
//	{
//		AddOrAbandonFun(counter, push_flage,kCacheCriticalValue, pToData, ArrayToHandle,buffer_rw_id);
//	}
//	else if ((*counter != 0) && (*counter >=kCacheCriticalValue))
//	{
//		*push_flage = 1;
//		AddOrAbandonFun(counter, push_flage,kCacheCriticalValue, pToData, ArrayToHandle,buffer_rw_id);
//	}//end of??? "else if ((*counter != 0) && (*counter >=kCacheCriticalValue))"
//	else if (*counter == 0) //the first time we receive data.
//	{
//		AddPakageToCache(counter, pToData, ArrayToHandle);
//	}
	return 0;
}
//通过ArrayToHandle[]->package_counter成员的大小比较来实现排序。
int ArrayMoveSort2( data_package_node_t **pToData, data_package_node_t *ArrayToHandle[])
{
	int local_var = network_buffer_num;

	while(local_var >= 0)
	{
		if((*pToData)->package_counter > ArrayToHandle[local_var-1]->package_counter)
		{
			ArrayToHandle[local_var] = ArrayToHandle[local_var-1];
			local_var--;
		}
		else 		//“==”情况不会出现在这个函数内
		{
			ArrayToHandle[local_var] = *pToData;
			break;
		}
	}
	return 0;
}


int AddAndSortFun2(data_package_node_t **pToData, data_package_node_t *ArrayToHandle[], int buffer_rw_id)
{
	if ( ((*pToData)->package_counter == (ArrayToHandle[network_buffer_num-1]->package_counter+1))
		||  ( (ArrayToHandle[network_buffer_num-1] == 0) && (network_buffer_num < MAX_SORT_NUFFER_NUM) )
		)
	{
		AddPakageToCache2(pToData, ArrayToHandle);
	}
	else //if (pToData->package_counter != ArrayToHandle[*counter-1]->package_counter+1 )
	{
		if (((*pToData)->package_counter < (ArrayToHandle[network_buffer_num-1]->package_counter)+ MAX_SORT_NUFFER_NUM *3)//roben_ye,2013-09-05,"*3",data lost test.
																															//network is not very good
				&& (((*pToData)->package_counter > ArrayToHandle[network_buffer_num-1]->package_counter)))
		{
			AddPakageToCache2( pToData, ArrayToHandle);
		}
		else if (  ((*pToData)->package_counter < ArrayToHandle[network_buffer_num-1]->package_counter)
					&& (	((*pToData)->package_counter >= desired_value)
								|| ((*pToData)->package_counter < MAX_SORT_NUFFER_NUM*4)
						)
				 )
		{
			ArrayMoveSort2(pToData, ArrayToHandle);
			network_buffer_num = network_buffer_num+1;
			*pToData = ArrayToHandle[network_buffer_num];
		}
		else //abandon the package
		{
			udp_print(("we want to abandon the package:%d,%d,%d\n", (*pToData)->end_flags,
																	(*pToData)->package_counter,
																	(*pToData)->send_len));
			int roben_ye = 0;

			for (;roben_ye < 6;roben_ye++)
			{
				udp_print(("ArrayToHandle[:%d]:%d\n",roben_ye,ArrayToHandle[roben_ye]->package_counter));
			}

		}
	}

	return 0;
}


//ArrayToHandle[network_buffer_num]指向所接收到的数据所存放的位置，
//并让*pToData指向一个新的位置，以便用于接收数据。
int AddPakageToCache2(data_package_node_t **pToData, data_package_node_t *ArrayToHandle[])
{
	//这种情况什么时候会出现？？？
	if ( ((*pToData)->package_counter <= push_value) && (network_buffer_num != 0))
	{
		udp_print(("we real want to abandon the package\n"));
//		WriteNetworkErrorToLog("we real want to abandon the package\n");
		return -1;
	}
	else
	{
		ArrayToHandle[network_buffer_num] = *pToData;
		network_buffer_num += 1;
		*pToData = ArrayToHandle[network_buffer_num];
	}

	return 0;
}

int CopyToAppBuffer2(data_package_node_t *ArrayToHandle[], int buffer_rw_id)
{
	//拷贝所有能拷贝的包。
	//期望51，network buffer：52，53，现在来了1。丢弃1，拷贝52，53，期望54
	//期望51，network buffer：52，54，现在来了1。丢弃1，拷贝52，期望53

	while(1)
	{
		//至少copy 1个包
		if(network_buffer_num > 0)
		{
			WriteTobuffer2(ArrayToHandle, buffer_rw_id);
			if (ArrayToHandle[0]->package_counter == push_value +1)
			{
				//还需要拷贝
				continue;
			}
			else
			{
				//不需要拷贝了
				break;
			}
		}
		else
		{
			//network buffer为空，拷贝完了
			break;
		}
	}
	return 0;
}


//始终将ArrayToHandle[0]数据推向音频缓存buffer；然后移动指针，使其指向下一指针成员所指向的位置。
//返回值始终为0.
int WriteTobuffer2(data_package_node_t *ArrayToHandle[], int buffer_rw_id)
{
	udp_print(("PushToDataBuffer---be\n"));
	int copy_size = 0;
	if ((copy_size = PushToDataBuffer2(ArrayToHandle[0], buffer_rw_id)) < 0)
	{
		udp_print(("PushToDataBuffer\n"));
		int loop_arry = 0;
		for(; loop_arry< 5; loop_arry++)
		{
			udp_print(("ArrayToHandle[%d].num:%d\n", loop_arry, ArrayToHandle[loop_arry]->package_counter));
		}
		//如果“return(-1);”打开，那么当buffer填满的时候，会引起死机，buffer越界。
		//		return(-1);
	}
	if (copy_size < ArrayToHandle[0]->send_len)
	{
		udp_print(("the add len is %d\n",copy_size));
		udp_print(("sizeof(*ArrayToHandle[0])%d\n",sizeof(*ArrayToHandle[0])));
		udp_print(("(*ArrayToHandle[0]).send_len:%d\n",ArrayToHandle[0]->send_len));
		udp_print(("the data buffer is full, we fail to add the package to buffer\n"));
	}

	network_buffer_num--;
	ArraySort(ArrayToHandle);

	return 0;
}
//获取AddDataToBuffer函数接口的参数
int PushToDataBuffer2(data_package_node_t *pushed_package, int buffer_rw_id)
{
	int len = 0;

	//copy到应用层buffer的包号
	push_value = pushed_package->package_counter;

	//期望值必须 大于copy过的包号
	if(desired_value <= push_value)
	{
		//为下一次接收包做准备
		desired_value = push_value + 1;
	}

//	if ((len = PushToBuffer2(buffer_rw_id,(const void*)(ArrayToHandle[0]->data_buffer),  (ArrayToHandle[0]->send_len))) < 0)
	if ((len = PushToBuffer2(buffer_rw_id, pushed_package->data_buffer, pushed_package->send_len)) < 0)
	{
		udp_print(("PushToDataBuffer: error, len=%d, buff_ptr=%x\n", len, buffer_rw_id));
		return -1;
	}

   udp_print(("PushToDataBuffer:num=%d\n", pushed_package->package_counter));

	return len;
}
//调用AddDataToBuffer函数接口，实现将排序cache中的数据推向音频缓存buffer中
int PushToBuffer2(int  buffer_rw_id, const void *PushBuffer, int len)
{
	int ret = 0;

	if ((ret = CharBufferWrite(buffer_rw_id, PushBuffer, len)) < 0)
	{
        buffer_over_times++;
        udp_print(("we stop at PushToBuffer\n"));
		udp_print(("PushToBuffer: CharBufferWrite()<0, ret=%d, buff_ptr=%x\n", ret, buffer_rw_id ));
		WriteNetworkErrorToLog("CharBufferWrite(buffer_rw_id, PushBuffer, len) < 0\n");
		return(-1);
	}

	byte_into_buffer += ret;
	udp_print(("byte_into_buffer:%d\n",byte_into_buffer));

	return ret;
}


//函数接收网络数据
int GetNetData2(const int sockfd,void *pToData, int len_num)
{
	int ret = 0;
	struct sockaddr_in recv_add = {0};
	int *pToPackLen = &len_num;

	if ((ret = recvfrom(sockfd, pToData, len_num, 0,(struct sockaddr *)&recv_add, (socklen_t*)pToPackLen)) < 0)
	{
		udp_print(("recvfrom error!\n"));
		return -1;
	}
//	udp_print(("recvfrom---ret:%d\n",ret));
//	PrintPortAndIp(recv_add);

	return 0;
}




int UdpRecvDataPart( int socket, int flage, buffer_pointer_t *BufferPoint, int buffer_rw_id)
{
	if (UdpRecv(socket, flage, &(BufferPoint->pToData),
			BufferPoint->pToCommand,BufferPoint->ArrayToHandle, buffer_rw_id)< 0 )
	{
		udp_print(("wrong UdpRecv\n"));
		WriteNetworkErrorToLog("wrong UdpRecv\n");
		return(-1);
	}

	udp_print(("network recv end\n"));
	return 0;
}

int UdpRecv(const int sockfd, const int flage, data_package_node_t** pToData,
		network_send_package_t* pToCommand, data_package_node_t* ArrayToHandle[], int  buffer_rw_id)
{

	if (flage == 1)	//receive command data from the network
	{
		if ((CmdRecvAndHandle(sockfd,pToCommand, buffer_rw_id)) < 0)
		{
			 udp_print(("CmdRecvAndHandle return value <0\n"));
		 	 return -1;
		}
	}
	else if (flage == 2)	//receive audio data from the network
	{
		if(DataRecvAndHandle(sockfd, buffer_rw_id, pToData, ArrayToHandle) < 0 )
		{
			 udp_print(("DataRecvAndHandle return value <0\n"));
		 	 return -1;
		}
	}
	else	//not data,not command
	{
		udp_print(("This is not the type(not data and command) we realize \n"));
	}

	 return 0;
}
int CmdRecvAndHandle(int sockfd, network_send_package_t* pToCommand, int  buffer_rw_id)
{
	int ret_ye = 0;

	if ((NULL == pToCommand) ||(sockfd < 0) ||(buffer_rw_id < 0))
	{
		udp_print(("wrong parameter of UdpRecv\n"));
		return -1;
	}

	if (NULL == (pToCommand = (network_send_package_t*)GetNetData(sockfd, (void*)pToCommand, sizeof(*pToCommand))))
	{
		udp_print(("GetNetData cmd return is NULL\n"));
		return -1;
	}
	if ((ret_ye = PushToCommandBuffer((const network_send_package_t*)pToCommand, buffer_rw_id)) < 0)
	{
		udp_print(("CmdRecvAndHandle:  BlockBufferWrite()<0\n"));
		return(-1);
	}
	if(ret_ye < sizeof(*pToCommand))
	{
		//udp_print(("the add len is %d\n",ret));
		//udp_print(("the data buffer is full, we fail to add the package to buffer\n"));
	}
	return 0;
}

int DataRecvAndHandle(const int sockfd, int buffer_rw_id,data_package_node_t** pToData,
								data_package_node_t* ArrayToHandle[])
{
	int end_flage = 0;
	int loop_ye = 0;
	static int counter_packag = 0;
	static int AddDataToBuff_flage = 0;

	if ((sockfd < 0) || (NULL == pToData) || (NULL == *pToData))
	{
		udp_print(("wrong parameter of DataRecvAndHandle\n"));
		return -1;
	}

	for (loop_ye = 0; loop_ye < Cache_PACK_NUM; loop_ye++)
	{
		if (!ArrayToHandle[loop_ye]  )
		{
			udp_print(("wrong parameter ArrayToHandle of DataRecvAndHandle\n"));
			return -1;
		}
	}

//	if ( (*pToData) == (ArrayToHandle[0]) )//条件问题
	if ( (*pToData) == (ArrayToHandle[0]) && (clean_force_len != 0)  )//条件问题
	{

		diag_printf("*pToData) == (ArrayToHandle[0])\n");
		counter_packag = 0;
		AddDataToBuff_flage = 0;
		clean_force_len = 0;
	}

	if (NULL == (*pToData = (data_package_node_t*)GetNetData(sockfd, (void*)*pToData, sizeof(**pToData))))
	{
		udp_print(("DataRecvAndHandle of GetNetData return is NULL\n"));
		return -1;
	}
	end_flage = (*pToData)->end_flags;

	SortAlgorithm(&counter_packag, &AddDataToBuff_flage, pToData, ArrayToHandle,buffer_rw_id);

	//“WriteTobuffer”执行的标志都要考“SortAlgorithm”设定，所以没办法让排序、Cache复制函数这两模块完全独立。
	if((end_flage == 1)||(AddDataToBuff_flage != 0))
	{
		if (end_flage == 1)//现在这个结构体成员没有使用，但是应该留着
		{
			while(counter_packag)
			{
				WriteTobuffer(&counter_packag, &AddDataToBuff_flage, ArrayToHandle, buffer_rw_id);
			}
		}
		else
		{
			//》》》》》》这个主要是让“SortAlgorithm”中没有调用“WriteTobuffer”函数。//
			//同时，让少于排序的包数积蓄足够的包来排序。//
			if (counter_packag <= Cache_PACK_NUM/2)
			{
				AddDataToBuff_flage = 0;
			}
			//这个“counter_packag < Cache_PACK_NUM/2”的标志可能失效。《《《《《《//
			WriteTobuffer(&counter_packag, &AddDataToBuff_flage, ArrayToHandle, buffer_rw_id);
		}
	}

	 return 0;
}


//获取AddDataToBlockBuffer函数接口的参数
int PushToCommandBuffer(const network_send_package_t *pToCommand, int buffer_rw_id)
{
	int len = 0;
        // void *pToCommandBuffer = NULL;

        //pToCommandBuffer = (void *)GetDataBlockBufferPointer(NETWORK_RESPONSE_BUFFER_ID_A);
       if ((len=PushToBlockBuffer(buffer_rw_id,(const void*)pToCommand, sizeof(*pToCommand))) < 0)
	{
		udp_print(("we stop at PushToBuffer\n"));
               udp_print(("PushToCommandBuffer:  BlockBufferWrite()<0\n"));
				
		return(-1);
	}

	return len;
}
//调用AddDataToBlockBuffer函数接口，向控制命令缓存buffer写数据
int PushToBlockBuffer( int buffer_rw_id, const void *PushBuffer, int len)
{
	int ret = 0;

	if ( (ret =BlockBufferWrite(buffer_rw_id, PushBuffer, len)) < 0 )
	{
		udp_print(("we stop at PushToBlockBuffer\n"));
		udp_print(("PushToBlockBuffer:  BlockBufferWrite()<0, ret=%d, buff_ptr=%x\n", ret, buffer_rw_id ));
		return (-1);
	}

	return ret;
}
//始终将ArrayToHandle[0]数据推向音频缓存buffer；然后移动指针，使其指向下一指针成员所指向的位置。
//返回值始终为0.
int WriteTobuffer(int *counter, int *push_flage, data_package_node_t *ArrayToHandle[], int buffer_rw_id)
{
	udp_print(("PushToDataBuffer---be\n"));
	int copy_size = 0;
	if ((copy_size = PushToDataBuffer(ArrayToHandle, buffer_rw_id)) < 0)
	{
		udp_print(("PushToDataBuffer\n"));
		int i_ye = 0;
		for(;i_ye< 5;i_ye++)
		{
			udp_print(("ArrayToHandle[%d].num:%d\n",i_ye,ArrayToHandle[i_ye]->package_counter));
		}
			//如果“return(-1);”打开，那么当buffer填满的时候，会引起死机，buffer越界。
		//		return(-1);
	}
	if (copy_size < (*ArrayToHandle[0]).send_len)
	{
		udp_print(("the add len is %d\n",copy_size));
		udp_print(("sizeof(*ArrayToHandle[0])%d\n",sizeof(*ArrayToHandle[0])));
		udp_print(("(*ArrayToHandle[0]).send_len:%d\n",(*ArrayToHandle[0]).send_len));
		udp_print(("the data buffer is full, we fail to add the package to buffer\n"));
	}

	(*counter)--;
	ArraySort(ArrayToHandle);
	if (*counter == 0)
	{
		*push_flage = 0;
	}
	return 0;
}
//获取AddDataToBuffer函数接口的参数
int PushToDataBuffer(data_package_node_t *ArrayToHandle[], int buffer_rw_id)
{

	int len = 0;
	//BufferPointer pToDataBuffer = NULL;

       net_recv_bytes += (ArrayToHandle[0]->send_len);

//	pToDataBuffer = GetDataBufferPointer(DATA_BUFFER_ID_NETWORK_RECEIVE);
	if ((len = PushToBuffer(buffer_rw_id,(const void*)(ArrayToHandle[0]->data_buffer),  (ArrayToHandle[0]->send_len))) < 0)
	{
		udp_print(("we stop at AddDataToBuffer\n"));
		udp_print(("PushToDataBuffer: error, len=%d, buff_ptr=%x\n", len, buffer_rw_id));
		return(-1);
	}
   udp_print(("PushToDataBuffer: data into buffer, ok, so exit: len=%d\n", len));
		
	return len;
}

//调用AddDataToBuffer函数接口，实现将排序cache中的数据推向音频缓存buffer中
int PushToBuffer(int  buffer_rw_id, const void *PushBuffer, int len)
{
	int ret = 0;

	if ((ret = CharBufferWrite(buffer_rw_id, PushBuffer, len)) < 0)
	{
        buffer_over_times++;
        udp_print(("we stop at PushToBuffer\n"));
		udp_print(("PushToBuffer: CharBufferWrite()<0, ret=%d, buff_ptr=%x\n", ret, buffer_rw_id ));
		return(-1);
	}

        byte_into_buffer += ret;	
        udp_print(("byte_into_buffer:%d\n",byte_into_buffer));

	return ret;
}

//成员指针指向其下一个成员所指向的位置，ArrayToHandle【7】指向ArrayToHandle【0】
int ArraySort(data_package_node_t *ArrayToHandle[])
{
	int loop_arry= 0;

	data_package_node_t *pTransform = ArrayToHandle[0];
	for(; loop_arry< Cache_PACK_NUM; loop_arry++)	//"Cache_PACK_NUM" is the size of the ArrayToHandle
	{
		ArrayToHandle[loop_arry] = ArrayToHandle[loop_arry+1];
	}
	ArrayToHandle[Cache_PACK_NUM-1] = pTransform;

	return 0;
}

//函数接收网络数据
void* GetNetData(const int sockfd,void *pToData, int len_num)
{
	int ret = 0;
	struct sockaddr_in recv_add = {0};
	int *pToPackLen = &len_num;

	if ((ret=recvfrom(sockfd, pToData, len_num, 0,(struct sockaddr *)&recv_add, (socklen_t*)pToPackLen)) < 0)
	{
		udp_print(("recvfrom error!\n"));
		exit(-1);
	}
	udp_print(("recvfrom---ret:%d\n",ret));
	PrintPortAndIp(recv_add);

	return pToData;
}

//print the port and IP of the device which we receive information from.
int PrintPortAndIp(struct sockaddr_in recv_add)
{
	 char IP_p[16] = {0};

//	 udp_print(("the address is%s\n",inet_ntop(AF_INET, (const void *)&recv_add.sin_addr.s_addr, IP_p, sizeof(IP_p))));
//	 udp_print(("And the port is %d\n ",ntohs(recv_add.sin_port)));

	 return 0;
}

//数据排序、丢弃的主要函数功能
int SortAlgorithm(int *counter,int *push_flage, data_package_node_t **pToData,
							data_package_node_t *ArrayToHandle[],int buffer_rw_id)
{
	const unsigned char kCacheCriticalValue = Cache_PACK_NUM/2;

	if ((*counter != 0) && (*counter<kCacheCriticalValue))
	{
		AddOrAbandonFun(counter, push_flage,kCacheCriticalValue, pToData, ArrayToHandle,buffer_rw_id);
	}
	else if ((*counter != 0) && (*counter >=kCacheCriticalValue))
	{
		*push_flage = 1;
		AddOrAbandonFun(counter, push_flage,kCacheCriticalValue, pToData, ArrayToHandle,buffer_rw_id);
	}//end of??? "else if ((*counter != 0) && (*counter >=kCacheCriticalValue))"
	else if (*counter == 0) //the first time we receive data.
	{
		AddPakageToCache(counter, pToData, ArrayToHandle);
	}
	return 0;
}

int AddOrAbandonFun(int *counter,int *push_flage, const unsigned char kCacheCriticalValue,
				data_package_node_t **pToData,data_package_node_t *ArrayToHandle[],int buffer_rw_id)
{
	if ( ((*pToData)->package_counter == (ArrayToHandle[*counter-1]->package_counter+1))
		||  ( (ArrayToHandle[*counter-1] == 0) && (*counter < kCacheCriticalValue) )
		)
	{
		AddPakageToCache(counter, pToData, ArrayToHandle);
	}
	else //if (pToData->package_counter != ArrayToHandle[*counter-1]->package_counter+1 )
	{
		if (((*pToData)->package_counter < (ArrayToHandle[*counter-1]->package_counter)+kCacheCriticalValue *3)//roben_ye,2013-09-05,"*3",data lost test.
																															//network is not very good
				&& (((*pToData)->package_counter > ArrayToHandle[*counter-1]->package_counter)))
		{
//			diag_printf("");
			AddPakageToCache(counter, pToData, ArrayToHandle);
		}
		else if (  ((*pToData)->package_counter < ArrayToHandle[*counter-1]->package_counter)
					&& (	((*pToData)->package_counter > ArrayToHandle[0]->package_counter)
								|| ((*pToData)->package_counter < kCacheCriticalValue*4)
						)
				 )
		{
			ArrayMoveSort(counter, pToData, ArrayToHandle);
			*counter = *counter+1;
			*pToData = ArrayToHandle[*counter];
		}
		else //abandon the package
		{
//			diag_printf("AddOrAbandonFun --111111\n");
			udp_print(("we want to abandon the package:%d,%d,%d\n", (*pToData)->end_flags,
																	(*pToData)->package_counter,
																	(*pToData)->send_len));
			int roben_ye = 0;

			for (;roben_ye < 6;roben_ye++)
			{
				udp_print(("ArrayToHandle[:%d]:%d\n",roben_ye,ArrayToHandle[roben_ye]->package_counter));
			}

			if (*counter < kCacheCriticalValue)
			{
				AddPakageToCache(counter, pToData, ArrayToHandle);
//				//这里其实最好还是将数据包丢掉。
//				WriteTobuffer(counter, push_flage, ArrayToHandle, buffer_rw_id);
				*push_flage = 1;
			}
		}
	}

	return 0;
}

//通过ArrayToHandle[]->package_counter成员的大小比较来实现排序。
int ArrayMoveSort(int *counter, data_package_node_t **pToData, data_package_node_t *ArrayToHandle[])
{
	int local_var = *counter;

	while(local_var >= 0)
	{
		if((*pToData)->package_counter > ArrayToHandle[local_var-1]->package_counter)
		{
			ArrayToHandle[local_var]=ArrayToHandle[local_var-1];
			local_var--;
		}
		else 		//“==”情况不会出现在这个函数内
		{
			ArrayToHandle[local_var] = *pToData;
			break;
		}
	}
	return 0;
}
//ArrayToHandle[*counter]指向所接收到的数据所存放的位置，
//并让*pToData指向一个新的位置，以便用于接收数据。
int AddPakageToCache(int *counter, data_package_node_t **pToData, data_package_node_t *ArrayToHandle[])
{
	if ( ((*pToData)->package_counter <= ArrayToHandle[0]->package_counter) && (*counter != 0))
	{
//		diag_printf(("we real want to abandon the package\n"));
		WriteNetworkErrorToLog("we real want to abandon the package\n"); 
		return -1;
	}
	else
	{
		ArrayToHandle[*counter] = *pToData;
		*counter = *counter + 1;
		*pToData = ArrayToHandle[*counter];
		return 0;
	}
}





