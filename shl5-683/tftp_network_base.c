/*
 * tftp_network_base.c
 *
 *  Created on: 2013-5-21
 *      Author: benty
 */

#include "tftp_network_base.h"
#include <errno.h>

//端口复用。
//int SocketSet(int socket_fd)
//{
//	int on = 1;
//
//	if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
//	{
//		tftp_print( ("ERROR on setsockopt \n") );
//		return(-1);
//	}
//
//	return 0;
//}


//tftp and sqlite 可能都要使用。
//copy file from one to anther
int CopyFile(char *copied_file,char * covered_file)
{

	tftp_print( ("CopyFile,copied_file %s,covered_file %s\n",copied_file,covered_file) );
    int file_copied = 0;
    int file_covered = 0;

    if((file_copied = open(copied_file, O_RDONLY)) < 0)
    {
    	tftp_print( ("CopyFile is error open%s\n",copied_file) );
    	return -1;
    }
    if((file_covered = open(covered_file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0)
    {
    	tftp_print( ("CopyFile is error open%s\n",covered_file) );
    	return -1;
    }

    const int len = 256;
    char buf[len];
    memset(buf, 0, len);
    int read_size = 0;
    int ret = 0;
    int write_len = 0;

    read_size = read(file_copied, buf, len);
    while(read_size != 0)//保证文件的所有数据都读出来了。
    {
    	ret = read_size;
    	while(ret)//保证每次读出来的都写到新的文件中了。
    	{
    		write_len = write(file_covered, buf, ret);
    		if (write_len < 0)
    		{
    			tftp_print( ("CopyFile is error write fun\n") );
    			return -1;
    		}
    		ret  -= write_len;
    	}

    	read_size = read(file_copied, buf, len);
    	if (read_size < 0)
    	{
    		tftp_print( ("CopyFile is error read fun\n") );
    		return -1;
    	}
    }

    close(file_copied);
    close(file_covered);

	return 0;
}


//通过比较文件生成校验和与保存的校验和，查看文件是否被改动。
int TestFileCRC(const char* back_db_file, const char* crc_txt_file)
{
	return 0; // add by zhw 2015-12-9
	tftp_print( ("TestFileCRC file fun\n") );
	const int CRC_BUFFRE_LEN = 32*1024;

	if ((back_db_file == NULL) || (crc_txt_file == NULL))
	{
		tftp_print( ("GenerateFileCRC  fun parameter error\n") );
		return -1;
	}

	int crc_file = open(back_db_file, O_RDONLY);
	if (crc_file < 0)
	{
		tftp_print(("the crc file open error\n"));
		return -1;
	}

	char *bufffer = (char *)malloc(CRC_BUFFRE_LEN);
	if (bufffer == NULL)
	{
		tftp_print(("the crc  malloc----error\n"));
		return -1;
	}
	bzero(bufffer,CRC_BUFFRE_LEN);

	unsigned  int crc_return_value = 0;

	int read_size = read(crc_file, bufffer, CRC_BUFFRE_LEN);

	 while(read_size != 0)//保证文件的所有数据都读出来了。
	 {
		crc_return_value = cyg_crc32_accumulate(crc_return_value, bufffer, read_size);

		bzero(bufffer,read_size);
		read_size = read(crc_file, bufffer, CRC_BUFFRE_LEN);
		if (read_size < 0)
		{
			tftp_print(("CopyFile is error read fun\n"));
			if (bufffer != NULL)
			{
				free(bufffer);
				bufffer = NULL;
			}
			return -1;
		}
	 }
	 close(crc_file);
	if (bufffer != NULL)
	{
		free(bufffer);
		bufffer = NULL;
	}
	 tftp_print(("the  back_db_file %s\n",back_db_file));
	 tftp_print(("the crc file  %s\n",crc_txt_file));
	 tftp_print(("the  back_db_file crc :%x\n",crc_return_value));

	 //下面读取文件中的校验和，并与生成的校验和比较
	 char crc_string[10];//crc 32 位校验的结果是一个4 byte 数字，采用16进制保存
	 bzero(crc_string,10);
	 sprintf(crc_string, "%x",crc_return_value);//转换为字符串

	 char crc_txt[10];//crc 32 位校验的结果是一个4 byte 数字，采用16进制保存
	 bzero(crc_txt,10);
	 int crc_file_txt = open(crc_txt_file, O_RDONLY);
	 if (crc_txt_file < 0)
	 {
		 tftp_print(("the crc save file open error\n"));
		 return -1;
	 }
	 if (read(crc_file_txt, crc_txt,10) <= 0)
	 {
		 tftp_print(("the txt file is not good\n"));
	 }
	 close(crc_file_txt);

	 if (strcmp(crc_txt,crc_string) == 0)
	 {
		 tftp_print(("the file crc is the same\n"));
	 }
	 else
	 {
		 tftp_print(("crc_txt:%s\n",crc_txt));
		 tftp_print(("crc_string:%s\n",crc_string));
		 return 1;
	 }

	return 0;
}

////通过比较文件生成校验和与保存的校验和，查看文件是否被改动。
//int TestFileCRC(const char* back_db_file, const char* crc_txt_file, const char* tftp_db_file)
//{
//	tftp_print( ("TestFileCRC file fun\n") );
//	const int CRC_BUFFRE_LEN = 32*1024;
//
//	if ((back_db_file == NULL) || (crc_txt_file == NULL))
//	{
//		tftp_print( ("GenerateFileCRC  fun parameter error\n") );
//		return -1;
//	}
//
//	int crc_file = open(back_db_file, O_RDONLY);
//	if (crc_file < 0)
//	{
//		tftp_print(("the crc file open error\n"));
//		return -1;
//	}
//
//	char *bufffer = (char *)malloc(CRC_BUFFRE_LEN);
//	if (bufffer == NULL)
//	{
//		tftp_print(("the crc  malloc----error\n"));
//		return -1;
//	}
//	bzero(bufffer,CRC_BUFFRE_LEN);
//
//	unsigned  int crc_return_value = 0;
//
//	int read_size = read(crc_file, bufffer, CRC_BUFFRE_LEN);
//
//	 while(read_size != 0)//保证文件的所有数据都读出来了。
//	 {
//		crc_return_value = cyg_crc32_accumulate(crc_return_value, bufffer, read_size);
//
//		bzero(bufffer,read_size);
//		read_size = read(crc_file, bufffer, CRC_BUFFRE_LEN);
//		if (read_size < 0)
//		{
//			tftp_print(("CopyFile is error read fun\n"));
//			if (bufffer != NULL)
//			{
//				free(bufffer);
//				bufffer = NULL;
//			}
//			return -1;
//		}
//	 }
//	 close(crc_file);
//	if (bufffer != NULL)
//	{
//		free(bufffer);
//		bufffer = NULL;
//	}
//	 tftp_print(("the  back_db_file %s\n",back_db_file));
//	 tftp_print(("the  back_db_file crc :%x\n",crc_return_value));
//
//	 //下面读取文件中的校验和，并与生成的校验和比较
//	 char crc_string[10];//crc 32 位校验的结果是一个4 byte 数字，采用16进制保存
//	 bzero(crc_string,10);
//	 sprintf(crc_string, "%x",crc_return_value);//转换为字符串
//
//	 char crc_txt[10];//crc 32 位校验的结果是一个4 byte 数字，采用16进制保存
//	 bzero(crc_txt,10);
//	 int crc_file_txt = open(crc_txt_file, O_RDONLY);
//	 if (crc_txt_file < 0)
//	 {
//		 tftp_print(("the crc save file open error\n"));
//		 return -1;
//	 }
//	 if (read(crc_file_txt, crc_txt,10) <= 0)
//	 {
//		 tftp_print(("the txt file is not good\n"));
//	 }
//	 close(crc_file_txt);
//
//	 if (strcmp(crc_txt,crc_string) == 0)
//	 {
//		 tftp_print(("the file crc is the same\n"));
//	 }
//	 else
//	 {
//		 tftp_print(("the crc_txt:%s\n",crc_txt));
//		 tftp_print(("the crc_string:%s\n",crc_string));
//		 //校验和不对，文件应该被覆盖。
//		 if (CopyFile(tftp_db_file, back_db_file) < 0)
//		 {
//			 tftp_print(("the crc file covery error\n"));
//			 return -1;
//		 }
//
//		 //并添加新的校验和
//		 if (GenerateFileCRC(back_db_file,crc_txt_file) < 0)
//		 {
//			 tftp_print(("the new crc Generate error\n"));
//			 return -1;
//		 }
//	 }
//
//	return 0;
//}

//对指定文件做crc 32 位校验和，并存储到指定文件。
int GenerateFileCRC(const char* file_name, const char* crc_save_file )
{
	tftp_print( ("GenerateFileCRC file fun\n") );
	const int CRC_BUFFRE_LEN = 32*1024;

	if ((file_name == NULL) || (crc_save_file == NULL))
	{
		tftp_print( ("GenerateFileCRC  fun parameter error\n") );
		return -1;
	}

	int crc_file = open(file_name, O_RDONLY);
	if (crc_file < 0)
	{
		tftp_print(("the crc file open error\n"));
		return -1;
	}

	char *bufffer = (char *)malloc(CRC_BUFFRE_LEN + 1);
	if (bufffer == NULL)
	{
		tftp_print(("the crc  malloc----error\n"));
		return -1;
	}
	bzero(bufffer,CRC_BUFFRE_LEN);

	unsigned  int crc_return_value = 0;

	int read_size = read(crc_file, bufffer, CRC_BUFFRE_LEN);

	 while(read_size != 0)//保证文件的所有数据都读出来了。
	 {
		crc_return_value = cyg_crc32_accumulate(crc_return_value, bufffer, read_size);

		bzero(bufffer,read_size);
		read_size = read(crc_file, bufffer, CRC_BUFFRE_LEN);
		if (read_size < 0)
		{
			tftp_print(("CopyFile is error read fun\n"));
			return -1;
		}
	 }
	 close(crc_file);
	 tftp_print(("the crc of this file is :%x\n",crc_return_value));

	 //下面是存储生成的校验和
	 char crc_string[10];//crc 32 位校验的结果是一个4 byte 数字，采用16进制保存
	 bzero(crc_string,10);
	 sprintf(crc_string, "%x",crc_return_value);//转换为字符串

	 int crc_file_save = open(crc_save_file, O_CREAT|O_WRONLY|O_TRUNC, 0777);
	 if (crc_file_save < 0)
	 {
		 tftp_print(("the crc save file open error\n"));
		 return -1;
	 }

	 if (write(crc_file_save, crc_string, 10) <= 0)
	 {
		 tftp_print(("the crc save file write is wrong\n"));
		 return -1;
	 }

	 //roben_ye,确认是否需要使用该函数
	 if (fsync(crc_file_save) < 0)
	 {
		 tftp_print(("the crc save file fsync is wrong\n"));
		 return -1;
	 }
	 close(crc_file_save);

	return 0;
}

////tcp 接收数据
//int TCPrecv(int sockfd,void *buf,int len,int flags)
//{
//	int nRev = 0, recvCount = 0;
//	int length = len;
//
//	void *ptorecv = buf;
//
//	 if (ptorecv == NULL)
//	 {
//		 tftp_print( ("TCPrecv error on the input buffer\n") );
//		 return 0;
//	 }
//	 // 循环接收数据
//	 while(length > 0)
//	 {
//		if ( (nRev = recv(sockfd, ptorecv + recvCount, length, flags)) < 0)//网络出现异常
//		{
//			tftp_print( ("Failed recv() because of \n") );
//		 	return -1;
//		}
//
//		else if(nRev==0)//链接异常。
//		{
//			tftp_print( ("Server disconnect 111111\n") );
//			return -1;
//		}
//
//		else
//		{
//			length-=nRev;
//			recvCount+=nRev;
//		}
//
//	 }
//
//	 return recvCount; //返回接收到的字节数
//}
////tcp发送数据。
//int TCPsend(int sockfd, void *buf, int len, int flags)
//{
//	 int ret = 0, sendCount = 0;
//	 int length = len;
//	 void *pToSend = buf;
//
//	 if(pToSend==NULL)
//	 {
//		 tftp_print( ("TCPsend error on input error\n") );
//		 return -1;
//	 }
//	 while(length > 0)
//	 {
//		  if ( (ret = send(sockfd, pToSend + sendCount, length, flags)) < 0) //网络出现异常
//		  {
//			   tftp_print( ("Failed in the send\n") );
//			   return -1;
//		  }
//		  else
//		  {
//			  length -= ret;
//			  sendCount += ret;
//		  }
//	 }
//
//	 return sendCount; // 返回已发送的字节数
//}
//将端口号和IP地址填充到套接字地址中。
//struct addrinfo * SocketAddressFill(char *tftp_server_ip, int port, int socket_type)
//{
//	int ret = 0;
//	char server_port[10];
//	struct addrinfo hints;
//	struct addrinfo * server_add = NULL;
//
//	memset(&hints,0,sizeof(hints));
//	hints.ai_family = PF_UNSPEC;
//	hints.ai_protocol = 0;
//	hints.ai_socktype = socket_type;//roben_ye，例子中有的第二个参数填写为“tftp”，通过可以解析到为UDP。
//							//如果不填写，默认设置为“tcp”."SOCK_STREAM"也是UDP.
//							//如果填写"SOCK_DGRAM"表示UDP.
//
//	//roben_ye,这里的第一个参数必须是指向了确定的地址的指针。
//	//将INT端口号转换为字符串型端口号.
//	sprintf(server_port, "%d", port);
//
//	//将IP和和套接字类型填充到server_add中。后面用的就是本函数填充的字段。
//	//返回0，success。否则，失败。
//	ret = getaddrinfo(tftp_server_ip, server_port, &hints, &server_add);
//	if (ret)
//	{
//		tftp_print( ("the getaddrinfo return error") );
//		return NULL;
//	}
//
//	return server_add;
//}

//申请空间
void *LocalTftpMalloc( unsigned int wantedSize )
{
    void *pv_return = NULL;

	pv_return = malloc(wantedSize);

	if (pv_return == NULL)
	{
		tftp_print( ("error on malloc\n") );
		exit(-1);
	}

	bzero(pv_return, wantedSize);

	return pv_return;
}

//释放空间
void LocalTftpFree( void *pv )
{
	if( pv )
	{
       free( pv );
       pv = NULL;
	}
}


//==========================================================================
//
//      lib/tftp_client.c
//
//      TFTP client support
//
//==========================================================================

// TFTP client support
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
                    ) {

	 tftp_print( ("roben: entry the tftp_client_get_inner_analog\n") );

    int blksize=SEGSIZE;
    int result = 0;
    int s=-1;
    int actual_len, data_len;
    socklen_t from_len, recv_len;
    static int get_port = 7700;
    struct addrinfo * addrinfo;
    struct addrinfo * res;
    struct addrinfo hints;
    int error;

    struct sockaddr local_addr, from_addr;
    struct tftphdr *hdr = (struct tftphdr *)data;
    const char *fp;
    char *cp, *bp;
    struct timeval timeout;
    unsigned short last_good_block = 0;//	roben_ye,这里的包号记录变量是从0开始的，
    										//	从实际抓包来看，server的包标号是从1开始的。
    fd_set fds;
    int total_timeouts = 0;

    *err = 0;  // Just in case

    // Create initial request
    hdr->th_opcode = htons(RRQ);  // Read file
    cp = (char *)&hdr->th_stuff;//roben_ye,好像没有这个成员。
    fp = filename;
    while (*fp) *cp++ = *fp++;
    *cp++ = '\0';
    if (mode == TFTP_NETASCII) {
        fp = "NETASCII";
    } else if (mode == TFTP_OCTET) {
        fp = "OCTET";
    } else {
        *err = TFTP_INVALID;
        return -1;
    }
    while (*fp) *cp++ = *fp++;
    *cp++ = '\0';
#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
    if (negotiate) {
        fp="blksize";
        while (*fp)
          *cp++ = *fp++;
        *cp++ = '\0';
        cp+=sprintf(cp, "%d", CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET_SIZE);
        *cp++ = '\0';
    }
#endif

    memset(&hints,0,sizeof(hints));
    hints.ai_family = PF_UNSPEC;

    //roben_ye,	客户端调用getaddrinfo时，ai_flags一般不设置AI_PASSIVE，
    //roben_ye,	但是主机名nodename和服务名servname（更愿意称之为端口）则应该不为空。

    error = getaddrinfo(server, "tftp", &hints, &res);//roben_ye,没看懂含义。
    									//将IP和和套接字类型填充到res中。后面用的就是本函数填充的字段。
    if (error) {
      *err = TFTP_NETERR;
      return -1;
    }
    addrinfo = res;
    while (addrinfo) {
      s = socket(addrinfo->ai_family, addrinfo->ai_socktype,
                 addrinfo->ai_protocol);
      if (s >= 0) {
        memcpy(&local_addr,addrinfo->ai_addr,addrinfo->ai_addrlen);
        switch(addrinfo->ai_addr->sa_family) {
        case AF_INET: {
        	//roben_ye,为什么要创建两个地址？？？？
          struct sockaddr_in * saddr =
            (struct sockaddr_in *) addrinfo->ai_addr;
          struct sockaddr_in * laddr =
            (struct sockaddr_in *) &local_addr;
          if (port) {
            saddr->sin_port = htons(port);
          }
          laddr->sin_port = htons(get_port++);	//	roben_ye, 将通用结构体赋值为特殊使用的结构体。
          laddr->sin_addr.s_addr = INADDR_ANY;
          break;
        }

        default:
          *err = TFTP_NETERR;
          goto out;
        }
        	//roben_ye,???local_addr这里的IP是server 的 IP 呀？？？
        	//“laddr->sin_addr.s_addr = INADDR_ANY;” 这里已经修改了。
        if (bind(s,&local_addr,addrinfo->ai_addrlen) < 0) {
          *err = TFTP_NETERR;
          goto out;
        }

        // Send request
        if (sendto(s, data, (int)(cp-data), 0,
                   addrinfo->ai_addr,
                   addrinfo->ai_addrlen) < 0) {
          // Problem sending request
          *err = TFTP_NETERR;
          goto nextaddr;
        }

        // Read data
//        bp = buf;
        while (true) {
          timeout.tv_sec = TFTP_TIMEOUT_PERIOD;
          timeout.tv_usec = 0;
          FD_ZERO(&fds);
          FD_SET(s, &fds);
          if (select(s+1, &fds, 0, 0, &timeout) <= 0) {
            total_timeouts++;
            if ((last_good_block == 0) && (total_timeouts > TFTP_RETRIES_MAX)) {
              // Timeout - no data received. Probably no server.
              *err = TFTP_TIMEOUT;
              goto nextaddr;
            }
            if (total_timeouts > TFTP_TIMEOUT_MAX) {
              // Timeout - have received data. Network problem?
              *err = TFTP_TIMEOUT;
              goto out;
            }

            if (last_good_block == 0 ) {
              // Send request
              if (sendto(s, data, (int)(cp-data), 0,
                         addrinfo->ai_addr,
                         addrinfo->ai_addrlen) < 0) {
                // Problem sending request
                *err = TFTP_NETERR;
                goto nextaddr;
              }
            } else {
              // Try resending last ACK
              hdr->th_opcode = htons(ACK);
              hdr->th_block = htons(last_good_block);
              if (sendto(s, data, 4 /* FIXME */, 0,
                         &from_addr, from_len) < 0) {
                // Problem sending request
                *err = TFTP_NETERR;
                goto out;
              }
            }
          } //select <= 0
          else {
            recv_len = blksize+sizeof(struct tftphdr);
            from_len = sizeof(from_addr);
            if ((data_len = recvfrom(s, data, recv_len, 0,
                                     &from_addr, &from_len)) < 0) {
              // What happened?
              *err = TFTP_NETERR;
              goto out;
            }
#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
            if (ntohs(hdr->th_opcode) == OACK) {
              // We can have only *one* option, the one we sent..
              if (strncmp(data+2, "blksize", data_len)==0) {
                blksize=atol(data+2+strlen("blksize")+1);
              } else {
                // option ignored, use default.
              }
              // Send out the ACK
              hdr->th_opcode = htons(ACK);
              hdr->th_block = htons(last_good_block);
              if (sendto(s, data, 4 /* FIXME */, 0, 
                         &from_addr, from_len) < 0) {
                // Problem sending request
                *err = TFTP_NETERR;
                goto out;
              }
            } else
#endif
            if (ntohs(hdr->th_opcode) == DATA)
			 {
					actual_len = 0;
					if (ntohs(hdr->th_block) == (last_good_block+1))
					  {
						  // Consume this data
						  cp = hdr->th_data;
						  data_len -= 4;  /* Sizeof TFTP header */
						  actual_len = data_len;
						  result += actual_len;

	//					  if (len<data_len)
	//						{
	//						  // Buffer overflow
	//						  *err = TFTP_TOOLARGE;
	//						  goto out;
	//						}

	//	                  memcpy(bp, cp, data_len);
	//	                  bp+=data_len;
	//	                  len-=data_len;

						  	 //roben_ye,just for demol
							//这里直接写到文件就可以，没有足够的空间来将整个文件保存起来。
						  if (data_len > 0)
						  {
								if (RefreshDataToFile(write_stream, cp, data_len) < 0)
								{
									tftp_print( ("error on RefreshDataToFile\n") );
									 tftp_print( ("roben_ye:cp:%d\n",data_len) );
									close(s);
									return -1;
								}
						  }

//						  tftp_print( (":%s\n",cp) );

						  last_good_block++;
					}
					else
					{
					  // To prevent an out-of-sequence packet from
					  // terminating transmission prematurely, set
					  // actual_len to a full size packet.
					  actual_len = blksize;
					}
					// Send out the ACK
					hdr->th_opcode = htons(ACK);
					hdr->th_block = htons(last_good_block);
					if (sendto(s, data, 4 /* FIXME */, 0, &from_addr, from_len) < 0)
					{
					  // Problem sending request
					  *err = TFTP_NETERR;
					  goto out;
					}
					// A short packet marks the end of the file.
					/* 4 = Sizeof TFTP header */
					if ((actual_len >= 0) && (actual_len < blksize))
					  {
					  // End of data
					  close(s);
					  freeaddrinfo(res);

					  //roben_ye,强制将缓存区数据同步到磁盘上去.
						if (ForceDataFromBufToDisk(write_stream) < 0)
						{
							tftp_print( ("error on ForceDataFromBufToDisk\n") );
							return -1;
						}

					  return result;
					}
				  }
            	else if (ntohs(hdr->th_opcode) == ERROR)
            	{
                  *err = ntohs(hdr->th_code);
                  goto out;
            	}
            	else
            	{
                  // What kind of packet is this?
                  *err = TFTP_PROTOCOL;
                  goto out;
                }
          }
        }
      }
      else
      {
    	  tftp_print( ("error !!!!!!!!!!!!!!!!!!!!!!!!!!!!!s:%d",s) );
    	  tftp_print((":%d,%s\n",errno,strerror(errno)));
//    	  close(s);
      }
      // If we got here, it means there was a problem connecting to the
      // server. Try the next address returned by getaddrinfo
    nextaddr:
      if (-1 != s) {
        close(s);
      }
      addrinfo=addrinfo->ai_next;
    }
    // We ran into problems. Cleanup
 out:
    if (-1 != s) {
      close (s);
    }
    freeaddrinfo(res);
    return -1;
}


int tftp_client_get_analog(const char * const filename,
                    const char * const server,
                    const int port,
                    FILE *write_stream,
                    const int mode,
                    int * const err) {
        int result;
#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
        char *data = malloc(CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET_SIZE+
                            sizeof(struct tftphdr));
        if (data==NULL) {
          *err=TFTP_ENOSPACE;
          return -1;
        }
#else
        char data[SEGSIZE+sizeof(struct tftphdr)];
#endif
        result= tftp_client_get_inner_analog(data, filename, server,
                                     port, write_stream, mode, err
#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
                                     ,1
#endif
                                     );
        if (result<0)
          {
#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
            // try without negotiating packet size. The serves that do
            // not support options negotiation may or may not ignore the
            // options. If they return an error in the case of options
            // this code path will try without packet size negotiation.
            result=tftp_client_get_inner_analog(data, filename, server,
                                         port, write_stream, mode, err,
                                         0);
#endif
          }

#ifdef CYGPKG_NET_TFTPD_CLIENT_BIG_PACKET
        free(data);
#endif

        return result;
}

//
// Read a file from a host into a local buffer.  Returns the
// number of bytes actually read, or (-1) if an error occurs.
// On error, *err will hold the reason.
//
// Depreciated. Use tftp_client_get instead.
int tftp_get_analog(const char * const filename,
         const struct sockaddr_in * const server,
         FILE *write_stream,
         const int mode,
         int * const err)
{
  char server_name[20];
  char *ret;
  int port;

  ret = inet_ntop(AF_INET, (void *)&server->sin_addr,
                  server_name, sizeof(server_name));
  if (NULL == ret) {
      *err = TFTP_NETERR;
      return -1;
  }
  /*roben_ye,server 暂时的port是0，ip（server_name）号为填充的IP号*/
  port = server->sin_port;

  return tftp_client_get_analog(filename, server_name, port, write_stream, mode, err);
}

//将下载来的数据保存到本地文件
int RefreshDataToFile(FILE *writw_file_stream, char *buf, int len)
{
	int ret = 0;
	ret = WriteDataToFile(writw_file_stream, buf, len);
	if (ret < 0)
	{
		tftp_print( ("error on WriteDataToFile\n") );
		return -1;
	}
//	tftp_print( ("roben_ye:cp:%d\n",ret) );
   return 0;
}

//将buf中的数据用while循环写，直到全部写进文件
int WriteDataToFile(FILE *writw_file_stream, char *buf, int len)
{
	int ret = 0;

//	tftp_print( ("len:%d\n", len) );

	//	if ((ret = fwrite(buf, 1, len, writw_file_stream)) != 1)//fwrite返回值为第三个参数的值.
	if ((ret = fwrite(buf, len, 1, writw_file_stream)) != 1)//fwrite返回值为第三个参数的值.
	{
		tftp_print( ("Error writing data-111\n") );
		tftp_print( ("ret:%d,len:%d\n", ret,len) );

		return -1;
	}


	return len;
}

//强制将内存中的数据写到flash中，
int ForceDataFromBufToDisk(FILE *file_stream)
{
	tftp_print( ("ForceDataFromBufToDisk--1111 \n") );
	int fp_id = 0;
	//将缓存区内容同步内核缓存区。
	if (fflush(file_stream) == EOF )//看ecos源代码.支持
	{
		tftp_print( ("error on fflush fun \n") );
		return -1;
	}

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
//		tftp_print( ("error on the fsync fun--111\n") );
//		perror("error on the fsync fun--111\n") ;
//		return -1;
//	}
	return 0;
}
