/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_buffer_init.h
 * 文件标识：
 * 摘    要： 设备配置信息主函数头文件
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

#ifndef ZHW_BUFFER_INIT_H_
#define ZHW_BUFFER_INIT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include "network_include_extern_head.h"

#include "zhw_network_config.h"

typedef struct buffer_info
{
	int common_type; //1--common;2--big_common.
	int pkg_type;
	int buffer_type;
	int buffer_id;
	struct buffer_info *next;
}buffer_info_t;

//int ZhwBufferInit(int common_type,unsigned int pkg_type,unsigned int buffer_type, int buffer_id);
int ZhwBufferRegister(int common_type,unsigned int pkg_type,unsigned int buffer_type, int buffer_id);

int ZhwBufferInq(int common_type,int pkg_type,int buffer_type);
#endif /* ZHW_BUFFER_INIT_H_ */
