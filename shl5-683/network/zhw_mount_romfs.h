/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_mount_romfs.h
 * 文件标识：
 * 摘    要： 用于挂载romfs文件系统的函数头文件
 *
 * 当前版本： V1.0.0
 * 作    者： zhouwei
 * 完成日期：2012-11-08
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef ZHW_MOUNT_ROMFS_H_
#define ZHW_MOUNT_ROMFS_H_

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <unistd.h>
#include <cyg/fileio/fileio.h>
#include "network_include_extern_head.h"

//#define FILENAME "state_line_3/1_1.mp3"


/** MountRomfs function description
 *  This function belongs to eamp,which is used to load the file system,which is Romfs
 *
 *  @param   null
 *  @return  null
 */
int MountRomfs(void);
int MountYaffs2(void);
int ZhwMkdir(char *newdir,int amode);
int DirCreateOfNandflash();


#endif /* ZHW_MOUNT_ROMFS_H_ */
