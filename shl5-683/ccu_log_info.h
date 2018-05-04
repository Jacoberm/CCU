/*
 * Copyright (c) 2012��MIT
 * All right reserved.
 *
 * �ļ����ƣ� ccu_log_info.h
 * �ļ���ʶ��
 * ժ    Ҫ�� CCUlog��־����
 *
 * ��ǰ�汾�� V1.0.0
 * ��    �ߣ� wilson
 * ������ڣ�2014-04-04
 *
 * ȡ���汾��
 * ԭ����  ��
 * ������ڣ�
 */

#ifndef CCU_LOG_INFO_H_
#define CCU_LOG_INFO_H_

#include "ccu_function.h"

extern char recording_file_path[30];

/** CCULogInfoEntry() description
 *  Log��־�̴߳�����
 *
 *  @param   void *arg
 *  @return  null
 */
void *CCULogInfoEntry(void *arg);

/** SetRecordingFileName() description
 *  ����¼���ļ���
 *
 *  @param   �ļ����ͣ�1:d2d 2:d2p
 *  @param   Դ�豸
 *  @param   Ŀ���豸
 *  @return  null
 */
void SetRecordingFileName(unsigned char file_type,unsigned char src_no,unsigned char dst_no);

#endif /* CCU_LOG_INFO_H_ */
