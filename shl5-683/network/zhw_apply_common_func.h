/*
*Copyright (c) 2012,
*All right reserved.
*
*文件名称： zhw_apply_common_func.h
*文件标识：应用层共用的函数
*摘  要：将一些应用层读需要用的函数统一放置。
*接受参数：无
*返回数据：
*
*当前版本：00.00.01
*作  者：周魏
*完成日期：
*
*取代版本：原版本号
*原作者 ：
*完成时间：
*/
#ifndef ZHW_APPLY_COMMON_FUNC_H_
#define ZHW_APPLY_COMMON_FUNC_H_
#include "zhw_network_main.h"

int ZhwApplyBufferCreate(int buffer_type,int buffer_number,int buffer_size,char *buffer_name);


#endif /* ZHW_APPLY_COMMON_FUNC_H_ */
