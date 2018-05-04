/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_log_info.h
 * 文件标识：
 * 摘    要： CCUlog日志操作
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-04-04
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef CCU_LOG_INFO_H_
#define CCU_LOG_INFO_H_

#include "ccu_function.h"

extern char recording_file_path[30];

/** CCULogInfoEntry() description
 *  Log日志线程处理函数
 *
 *  @param   void *arg
 *  @return  null
 */
void *CCULogInfoEntry(void *arg);

/** SetRecordingFileName() description
 *  设置录音文件名
 *
 *  @param   文件类型，1:d2d 2:d2p
 *  @param   源设备
 *  @param   目标设备
 *  @return  null
 */
void SetRecordingFileName(unsigned char file_type,unsigned char src_no,unsigned char dst_no);

#endif /* CCU_LOG_INFO_H_ */
