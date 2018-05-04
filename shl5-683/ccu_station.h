/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_station.h
 * 文件标识：
 * 摘    要： TCMS相关功能处理函数
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-11-18
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef CCU_STATION_H_
#define CCU_STATION_H_

#include "data_struct.h"
#include "ccu_function.h"

extern station_information_t station_information[2][MAX_STATION_NUMBER];

extern region_station_t *p_line3_region_info;
extern region_station_t *p_line4_region_info;

/** CreateCCURegionSpace function description
 *  创建CCU区间信息空间
 *
 *  @param   线路号
 *  @param   区间总数
 *  @return  null
 */
void CreateCCURegionSpace(int param_line_no,int param_total_items);

/** InitCCURegionInfo function description
 *  初始化CCU区间信息
 *
 *  @param   null
 *  @return  null
 */
void InitCCURegionInfo();

/** InitStationInformation function description
 *  初始化站代码信息
 *
 *  @param   null
 *  @return  null
 */
void InitStationInformation();

/** ClearSkipStationInfo function description
 *  清除越站信息
 *
 *  @param   线路号
 *  @return  null
 */
void ClearSkipStationInfo(int line_number_of_skip_station);

/** GetDownDistanceInfo function description
 *  获取下行距离、开么侧等信息
 *
 *  @param   null
 *  @return  null
 */
void GetDownDistanceInfo();

/** GetAnnStationInfo function description
 *  获取站代码信息，包括报站音频等内容
 *
 *  @param   null
 *  @return  null
 */
void GetAnnStationInfo();

/** GetStationDistance function description
 *  获取站台之间距离信息
 *
 *  @param   null
 *  @return  null
 */
void GetStationDistance();

/** GetTwoStationDistance function description
 *  获取两个站台之间的距离
 *
 *  @param   线路号
 *  @param   开始站
 *  @param   结束站
 *  @param   上下行
 *  @return  两个站台之间的距离
 */
unsigned long GetTwoStationDistance(unsigned char line_no,unsigned char first_station,unsigned char secode_station,unsigned char up_or_down);

/** CalculateNextStationNo function description
 *  计算下一站站代码
 *
 *  @param   线路号
 *  @param   当前站
 *  @return  下一站站代码
 */
int CalculateNextStationNo(int param_line_no,int param_cur_station_no);

/** ShowDistanceInfo function description
 *  显示站台之间的距离信息
 *
 *  @param   null
 *  @return  null
 */
void ShowDistanceInfo();

#endif /* CCU_STATION_H_ */
