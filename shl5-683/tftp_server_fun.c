/*
 * tftp_server_fun.c
 *
 *  Created on: 2013-1-26
 *      Author: benty
 */


#include "tftp_server_fun.h"
#include <errno.h>


struct tftp_test_fileops tftp_server_fileops =
{
    tftp_server_open, tftp_server_close, tftp_server_write, tftp_server_read
};

#if  1///< roben
//tftp 打开文件
int tftp_server_open(const char *file_name, int flags)
{
   int res = -1;


//   char file_name[512];
//   sprintf(file_name,"/nand%s",fn);
//
//   tftp_print(("the open file name is :%s\n",file_name) );

   if (O_CREAT&flags)//主要用于上传文件到服务器
   {
	   tftp_print( ("O_CREAT&flags ---open\n") );
	   res = open(file_name, O_RDWR|O_CREAT|O_TRUNC,0777);
	   if (res < 0)
	   {
		   tftp_print( ("error ont the O_CREAT&flags:%d,%d\n",res,errno) );
	   }
   }
   else
   {
	   if (O_RDONLY&flags)//主要用于从服务器下载文件
	   {
		   tftp_print( ("O_RDONLY&flags ---open\n") );
		   res = open(file_name, O_RDONLY);
		   if (res < 0)
		   {
			   tftp_print( ("error-111- ont the open(fn, O_RDWR)\n") );
		   }
	   }
	   else if (O_WRONLY&flags)//roben_ye,这种模式没有使用，write 的时候是上面的模式
	   {
		   tftp_print( ("O_WRONLY&flags ---open\n") );
		   res = open(file_name, O_WRONLY|O_TRUNC);
		   if (res < 0)
		   {
			   tftp_print( ("error --2222--ont the open(fn, O_RDWR)\n") );
		   }
	   }
	   else
	   {
		   tftp_print(("the open file ---error\n") );
	   }


//	   res = open(file_name, O_RDWR|O_TRUNC);
//	   if (res < 0)
//	   {
//		   tftp_print( ("error ont the open(fn, O_RDWR)\n") );
//	   }
   }

   return res;
}

//tftp 关闭
int tftp_server_close(int fd)
{

	tftp_print( ("tftp_server_close --fun\n") );
//		lseek(fd, 0, SEEK_SET);
//	   char *buf2 = (char *)malloc(100);
//
//
//	   while(1)
//	   {
//		   bzero(buf2,100);
//		   int read_len = read(fd, buf2, 80);//100,不行，11，不行
//
//		   if (read_len < 0)
//		   {
//			   tftp_print( ("buf-file_len-1111: %d\n", read_len) );
//		   }
//		   else
//		   {
//			   tftp_print( ("buf:-file_len-222 %d\n", read_len) );
//			   tftp_print( ("buf-222: %s\n", buf2) );
//			   if (read_len < 80)
//			   {
//				   break;
//			   }
//		   }
//
//	   }
//
//	   free(buf2);

	close(fd);
	tftp_print( ("tftp_server_close --end\n") );
	return 0;
}
//tftp 更新文件
int tftp_server_write(int fd, const void *buf, int len)
{
	 tftp_print(("######################\n"));
	 tftp_print(("the tftp_server_write len is :%d\n",len) );
	int res = -1;
    if ((NULL == buf) || (fd < 0))
    {
    	tftp_print( ("parameter is  wrong\n") );
    	return -1;
    }

    res = write(fd, buf, len);
    if (res < 0)
    {
    	tftp_print( ("error on the write file\n") );
    	return -1;
    }
    return res;
}
//tftp 读取文件
int tftp_server_read(int fd, void *buf, int len)
{
	int res = -1;
	 tftp_print(("******************\n"));
	 tftp_print(("tftp_server_read len is :%d\n",len) );

    if ((NULL == buf) || (fd < 0))
    {
    	tftp_print( ("parameter is  wrong\n") );
    	return -1;
    }

    res = read(fd, buf, len);
    if (res < 0)
    {
    	tftp_print( ("error on the read file\n") );
    	return -1;
    }
    return res;
}
#else ///< just debug
unsigned char *sim_file = NULL;
unsigned int sim_file_pos = 0;
int read_sim(int fd, void *buf, int len)
{
     int ret = -1;
     int i = 0;
     int sim_len = 768;
     unsigned char *in_buff = (unsigned char *)buf;
     int last_start_pos = 0;	 

     if(sim_file_pos>=sim_len)
     {
          ret = 0;
	  return ret;
     }
	 
     if(NULL!=sim_file)
     {
             last_start_pos = sim_file_pos;     
	     for(i=0; i<sim_len&&i<len; i++)
	     {
	           in_buff[i] = sim_file[i+last_start_pos];
	           ret = i+1;
                   sim_file_pos++;
		     if(sim_file_pos>=sim_len)
		     {
		          break;
		     }				   
	     } 
     }

     tftp_print( ("tftp, read_sim:  read_bytes=%d\n", ret) );
     return ret;
}
int write_sim(int fd, void *buf, int len)
{
     int ret = -1;
     int i = 0;
     int sim_len = 768;
     unsigned char *in_buff = (unsigned char *)buf;	 
	 
     for(i=0; i<sim_len&&i<len; i++)
     {
           sim_file[i] = in_buff[i];
           ret = i+1;		   
     }

     return ret;
}
//tftp 打开文件
int tftp_server_open(const char *fn, int flags)
{
     int res = -1;
     int sim_len = 768;
     int i = 0;

     tftp_print( ("tftp_server_open: enter\n") );	 
     sim_file = (unsigned char *)malloc(sim_len);
     if(NULL==sim_file)
     {
            tftp_print( ("tftp_server_open: null ptr\n") );
	    return -1;
     }
     for(i=0; i<sim_len; i++)
     {
           sim_file[i] = i%256;
     }	 
	 
     res = 2;
     return res;
}

//tftp 关闭
int tftp_server_close(int fd)
{
        tftp_print( ("tftp_server_close: enter\n") );
	//close(fd);
	free(sim_file);
	sim_file = NULL;
	return 0;
}
//tftp 更新文件
int tftp_server_write(int fd, const void *buf, int len)
{
    int res = -1;
tftp_print( ("tftp_server_write\n") );	
    if ((NULL == buf) || (fd < 0))
    {
    	tftp_print( ("parameter is  wrong\n") );
    	return -1;
    }

    //res = write(fd, buf, len);
    res = write_sim(fd, (void *)buf, len);
    if (res < 0)
    {
    	tftp_print( ("error on the write file\n") );
    	return -1;
    }
	
    return res;
}
//tftp 读取文件
int tftp_server_read(int fd, void *buf, int len)
{
	int res = -1;
tftp_print( ("tftp_server_read\n") );
    if ((NULL == buf) || (fd < 0))
    {
    	tftp_print( ("parameter is  wrong\n") );
    	return -1;
    }

    //res = read(fd, buf, len);
    res = read_sim(fd, buf, len);	
    if (res < 0)
    {
    	tftp_print( ("error on the read file\n") );
    	return -1;
    }
	
    return res;
}
#endif


//tftp 服务器打开接口
int tftp_test_server(void)
{
	int server_id = 0;
	if ((server_id = tftp_server_start()) < 0)
	{
		tftp_print( ("tftp open error\n") );
		return -1;
	}
	//tftp_server_end(server_id);
	return server_id;
}

//int read_test()
//{
//	   char *buf2 = (char *)malloc(20);
//	   bzero(buf2,20);
//
//	   int res = -1;
//	   res = open("/nand/test.c", O_RDWR,0777);
//	   if (res < 0)
//	   {
//		   tftp_print( ("error ont the O_CREAT&flags:%d,%d\n",res,errno) );
//	   }
//
//	   //roben_ye,文件指针被移动到尾部了
////	   lseek(res, 0, SEEK_SET);
//	   int read_len = read(res, buf2, 9);//100,不行，11，不行
//
//	   if (read_len < 0)
//	   {
//		   tftp_print( ("buf-file_len-1111: %d\n", read_len) );
//	   }
//	   else
//	   {
//		   tftp_print( ("buf:-file_len-222 %d\n", read_len) );
//		   tftp_print( ("buf-222: %s\n", buf2) );
//	   }
//	   free(buf2);
//}


//拆成两个函数，tftp启动函数
int tftp_server_start(void)
{
    int server_id = 0;

//    tftp_print( ("the tftp local file system\n ") );
//
//    	int res = -1;
//	   res = open("/nand/test.c", O_RDWR|O_CREAT,0777);
//	   if (res < 0)
//	   {
//		   tftp_print( ("error ont the O_CREAT&flags:%d,%d\n",res,errno) );
//	   }
//
//	   char *buf = (char *)malloc(20);
//	   bzero(buf,20);
//	   strcpy(buf,"hello world");
//	   tftp_print( ("buf-1111: %s,strlen:%d\n", buf,strlen(buf)) );
//
//	   int ret_value = 0;
//	   ret_value = write(res, buf, strlen(buf));
//	   if (ret_value < 0)
//	   {
//		   tftp_print( ("error on write buf:%d\n",errno) );
//	   }
//	   tftp_print( ("write buf len:%d\n",ret_value) );//11
//
//	   free(buf);
//
//	   read_test();

    tftp_print(("hello tftp start \n"));
    //与内核中定义的结构体不一样，如果到时候跑步起来，将自己定义的结构体改为内核所定义的结构体。
    server_id = tftpd_start(0, (struct tftpd_fileops*)&tftp_server_fileops);
    if (server_id > 0)
    {
      tftp_print( ("TFTP server created - id: %x\n", server_id) );
    }
    else
    {
      tftp_print( ("Couldn't create first server!\n") );
      return -1;
    }

//    cyg_thread_delay(2*100);
//    TNR_ON();
//
//    cyg_thread_delay(60*60*100);
    tftp_print( ("TFTP server created - id-99\n") );
    return server_id;
}

//拆成两个函数，tftp关闭函数，现在没有使用
int tftp_server_end(int server_id)
{
	int ret = 0;
//    cyg_thread_delay(60*100);//这个必须关掉，一定要想办法解决。
    TNR_OFF();

    if (server_id > 0)
    {
    	 tftp_print( ("TFTP tftpd_stop--111\n") );
    	 ret = tftpd_stop(server_id);
    	 tftp_print( ("TFTP server - id: %x stopped - res: %d\n", server_id, ret) );
    }

    return 0;
}

//TFTP服务器线程处理函数
//即使没有参数，也不能填“void”
//void *TftpTthreeadEntry(void *arg)
//{
//	int tftp_server_id = 0;
//
//	while(1)
//	{
//		//如果返回 《 0，表示打开tftp server 失败，继续打开操作，直到成功
//		tftp_server_id = tftp_test_server();
//		if (tftp_server_id < 0)
//		{
//			tftp_print( ("error on the tftp server\n") );
////			return -1;
//		}
//		else
//		{
//			   tftp_print( ("Ccu, tftp server-%d, start\n", tftp_server_id) );
//			   break;
//		}
//
//		sched_yield();
//	}
//
//	return NULL;
//}

//线程属性值配置函数
//入口参数：线程属性名
//void TftpThreadConfigure(pthread_attr_t *attr)
//{
//	int res = 0;//接受函数操作到返回值
//
//	//线程属性初始化
//	res = pthread_attr_init(attr);
//    if (res != 0)
//    {
//        tftp_print( ("Attribute initialization failed\n ") );
//        exit(-1);
//    }
//
//    //配置线程的继承性
//    res = pthread_attr_setinheritsched(attr, PTHREAD_INHERIT_SCHED);
//    if (res != 0)
//    {
//        tftp_print( ("Setting pthread_attr_setinheritsched failed\n") );
//        exit(-1);
//    }
//}


////创建非0的套接字，并作相应设定和绑定等。
//int ServerSockCreateAndListen(struct addrinfo * addrinfo)
//{
//	int server_socket = 0;
//	//roben_ye,要求不能使用套接字为0的套接字。
//
//	while(server_socket == 0)
//	{
//		tftp_print( ("tftp,create sock: %d, %d, %d\n",
//			        addrinfo->ai_family,
//				addrinfo->ai_socktype,
//				addrinfo->ai_protocol) );
//		tftp_print( ("server --type\n") );
//		if ( (server_socket = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol)) < 0)
//		{
//			tftp_print( ("AF_INET:%d,SOCK_DGRAM:%d,protocol:%d\n", addrinfo->ai_family,
//					addrinfo->ai_socktype, addrinfo->ai_protocol) );
//			tftp_print( ("error on the socket create\n") );
//		}
//
//                tftp_print( ("tftp,create sock-111:  %d\n", server_socket) );
//		if (server_socket == 0)
//		{
//			//如果创建的套接字是0，必须调用dup将其套接字给为非0
//			server_socket = dup(server_socket);
//		}
//	}
//	//add,添加套接字设定。
//	if ( SocketSet(server_socket) < 0)
//	{
//		tftp_print( ("error on SocketSet\n") );
//		return -1;
//	}
//	//绑定。
//	if (bind(server_socket, addrinfo->ai_addr, addrinfo->ai_addrlen) < 0)
//	{
//	   tftp_print( ("error on bind fun\n") );
//	   return -1;
//	}
//	//监听，服务器。
//	if(listen(server_socket, TCP_MAX_CONNECT) < 0)
//	{
//		tftp_print( ("ERROR on listen") );
//		close(server_socket);
//		return -1;
//	}
//
//	return server_socket;
//}

