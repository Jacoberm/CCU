/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_device_volume.h
 * 文件标识：
 * 摘    要： PIS音量信息文件操作
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2014-08-01
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef CCU_DEVICE_VOLUME_H_
#define CCU_DEVICE_VOLUME_H_

#include "file_operate.h"
#include "ccu_function.h"

//默认音量大小文件名
#define pis_pa_volume_file_name "/nand/volume/mingxiao_pis_pa_volume.dat"

extern FILE *fp_pis_pa_volume;

/** OpenPISPACDevVolumeFile function description
 *  打开PIS音量信息文件
 *
 *  @param   null
 *  @return  null
 */
void OpenPISPACDevVolumeFile();

/** WritePISPADevVolumeInfoToFile function description
 *  将当前PIS音量信息写入到音量文件中
 *
 *  @param   null
 *  @return  null
 */
void WritePISPADevVolumeInfoToFile();

/** UpdatePISPADevVolume function description
 *  更新当前PIS音量信息
 *
 *  @param   null
 *  @return  null
 */
void UpdatePISPADevVolume();
void UpdatePISPADevVolume2();

/** ReadLocalDeviceVolumeInfo function description
 *  从本地文件中读取音量信息
 *
 *  @param   null
 *  @return  null
 */
void ReadLocalDeviceVolumeInfo();

/** WriteDefaultDeviceVolumeToFile function description
 *  系统首次运行时设置默认音量信息
 *
 *  @param   null
 *  @return  null
 */
void WriteDefaultDeviceVolumeToFile();



#endif /* CCU_DEVICE_VOLUME_H_ */
