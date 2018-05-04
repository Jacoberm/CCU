/*
 * All right reserved.
 *
 * 文件名称：network_include_extern_head.h
 * 文件标识：
 * 摘    要：网络模块引用应用层头文件
 *
 * 当前版本：V1.0.0
 * 作    者：wilson
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */

#ifndef NETWORK_INCLUDE_EXTERN_HEAD_H_
#define NETWORK_INCLUDE_EXTERN_HEAD_H_

#include "bcu_get_data_from_ccu_type.h"
//////if PCU_DEV
//#include "../pcu_certificate/thread_comm.h"
//#include "../pcu_certificate/buffer_read_write_fun.h"
//#include "../pcu_certificate/log_record.h"
//#include "../pcu_certificate/function_for_network.h"

///////if EAMP_DEV
//#include "../eamp_certificate/thread_comm.h"
//#include "../eamp_certificate/buffer_read_write_fun.h"
//#include "../eamp_certificate/log_record.h"
//#include "../eamp_certificate/function_for_network.h"

///////if CCU_DEV
//#include "../ccu_certificate/thread_comm.h"
//#include "../ccu_certificate/buffer_read_write_fun.h"
//#include "../ccu_certificate/log_record.h"
//#include "../ccu_certificate/function_for_network.h"

////////if CMU_DEV
#include "../buffer_read_write_fun.h"
#include "../function_for_network.h"

#endif /* NETWORK_INCLUDE_EXTERN_HEAD_H_ */
