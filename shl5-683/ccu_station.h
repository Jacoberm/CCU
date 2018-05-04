/*
 * Copyright (c) 2012��MIT
 * All right reserved.
 *
 * �ļ����ƣ� ccu_station.h
 * �ļ���ʶ��
 * ժ    Ҫ�� TCMS��ع��ܴ�����
 *
 * ��ǰ�汾�� V1.0.0
 * ��    �ߣ� wilson
 * ������ڣ�2013-11-18
 *
 * ȡ���汾��
 * ԭ����  ��
 * ������ڣ�
 */

#ifndef CCU_STATION_H_
#define CCU_STATION_H_

#include "data_struct.h"
#include "ccu_function.h"

extern station_information_t station_information[2][MAX_STATION_NUMBER];

extern region_station_t *p_line3_region_info;
extern region_station_t *p_line4_region_info;

/** CreateCCURegionSpace function description
 *  ����CCU������Ϣ�ռ�
 *
 *  @param   ��·��
 *  @param   ��������
 *  @return  null
 */
void CreateCCURegionSpace(int param_line_no,int param_total_items);

/** InitCCURegionInfo function description
 *  ��ʼ��CCU������Ϣ
 *
 *  @param   null
 *  @return  null
 */
void InitCCURegionInfo();

/** InitStationInformation function description
 *  ��ʼ��վ������Ϣ
 *
 *  @param   null
 *  @return  null
 */
void InitStationInformation();

/** ClearSkipStationInfo function description
 *  ���Խվ��Ϣ
 *
 *  @param   ��·��
 *  @return  null
 */
void ClearSkipStationInfo(int line_number_of_skip_station);

/** GetDownDistanceInfo function description
 *  ��ȡ���о��롢��ô�����Ϣ
 *
 *  @param   null
 *  @return  null
 */
void GetDownDistanceInfo();

/** GetAnnStationInfo function description
 *  ��ȡվ������Ϣ��������վ��Ƶ������
 *
 *  @param   null
 *  @return  null
 */
void GetAnnStationInfo();

/** GetStationDistance function description
 *  ��ȡվ̨֮�������Ϣ
 *
 *  @param   null
 *  @return  null
 */
void GetStationDistance();

/** GetTwoStationDistance function description
 *  ��ȡ����վ̨֮��ľ���
 *
 *  @param   ��·��
 *  @param   ��ʼվ
 *  @param   ����վ
 *  @param   ������
 *  @return  ����վ̨֮��ľ���
 */
unsigned long GetTwoStationDistance(unsigned char line_no,unsigned char first_station,unsigned char secode_station,unsigned char up_or_down);

/** CalculateNextStationNo function description
 *  ������һվվ����
 *
 *  @param   ��·��
 *  @param   ��ǰվ
 *  @return  ��һվվ����
 */
int CalculateNextStationNo(int param_line_no,int param_cur_station_no);

/** ShowDistanceInfo function description
 *  ��ʾվ̨֮��ľ�����Ϣ
 *
 *  @param   null
 *  @return  null
 */
void ShowDistanceInfo();

#endif /* CCU_STATION_H_ */
