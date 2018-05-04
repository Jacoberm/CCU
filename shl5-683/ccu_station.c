/*
 * ccu_station.c
 *
 *  Created on: 2013-11-18
 *      Author: benty
 */

#include "ccu_station.h"

region_station_t *p_line3_region_info;
region_station_t *p_line4_region_info;

station_information_t station_information[2][MAX_STATION_NUMBER] =
                                             {
												 {
														 {0,0,0,"",""},
														 {3,1,1,"上海南站", "Shanghai South Railway Station"},
														 {3,2,1,"石龙路", "Shilong Road"},
														 {3,3,1,"龙漕路", "Longcao Road"},
														 {3,4,0,"漕溪路", "Caoxi Road"},
														 {3,5,1,"宜山路", "Yishan Road"},
														 {3,6,1,"虹桥路", "Hongqiao Road"},
														 {3,7,0,"延安西路", "West Yan'an Road"},
														 {3,8,2,"中山公园", "Zhongshan Park"},
														 {3,9,1,"金沙江路", "Jinshajiang Road"},
														 {3,10,0,"曹杨路", "Caoyang Road"},
														 {3,11,1,"镇坪路", "Zhenping Road"},
														 {3,12,1,"中潭路", "Zhongtan Road"},
														 {3,13,1,"上海火车站", "Shanghai Railway Station"},
														 {3,14,1,"宝山路", "Baoshan Road"},
														 {3,15,1,"东宝兴路", "Dongbaoxing Road"},
														 {3,16,1,"虹口足球场", "Hongkou Football Stadium"},
														 {3,17,1,"赤峰路", "Chifeng Road"},
														 {3,18,1,"大柏树", "Dabaishu"},
														 {3,19,1,"江湾镇", "Jiangwan Town"},
														 {3,20,1,"殷高西路", "West Yingao Road"},
														 {3,21,1,"长江南路", "South Changjiang Road"},
												 	 	 {3,22,1,"淞发路", "Songfa Road"},
												 	 	 {3,23,1,"张华浜", "Zhanghuabang"},
												 	 	 {3,24,1,"淞滨路", "Songbin Road"},
												 	 	 {3,25,0,"水产路", "Shuichan Road"},
												 	 	 {3,26,1,"宝杨路", "Baoyang Road"},
												 	 	 {3,27,1,"友谊路", "Youyi Road"},
												 	 	 {3,28,1,"铁力路", "Tieli Road"},
												 	 	 {3,29,1,"江杨北路", "North Jiangyang Road"}
												 },
												 {
														 {0,0,0,"",""},
														 {4,1,1,"宝山路", "Baoshan Road"},
														 {4,2,1,"海伦路", "Hailun Road"},
														 {4,3,1,"临平路", "Linping Road"},
														 {4,4,1,"大连路", "Dalian Road"},
														 {4,5,1,"杨树浦路", "Yangshupu Road"},
														 {4,6,1,"浦东大道", "Pudong Avenue"},
														 {4,7,1,"世纪大道", "Century Avenue"},
														 {4,8,1,"浦电路", "Pudian Road"},
														 {4,9,1,"蓝村路", "Lancun Road"},
														 {4,10,1,"塘桥", "Tangqiao"},
														 {4,11,1,"南浦大桥", "Nanpu Bridge"},
														 {4,12,1,"西藏南路", "South Xizang Road"},
														 {4,13,1,"鲁班路", "Luban Road"},
														 {4,14,1,"大木桥路", "Damuqiao Road"},
														 {4,15,1,"东安路", "Dong'an Road"},
														 {4,16,1,"上海体育场", "Shanghai Stadium"},
														 {4,17,1,"上海体育馆", "Shanghai Indoor Stadium"},
														 {4,18,1,"宜山路", "Yishan Road"},
												 		 {4,19,1,"虹桥路", "Hongqiao Road"},
												 	 	 {4,20,1,"延安西路", "West Yan'an Road"},
												 	 	 {4,21,1,"中山公园", "Zhongshan Park"},
												 	 	 {4,22,1,"金沙江路", "Jinshajiang Road"},
												 	 	 {4,23,1,"曹杨路", "Caoyang Road"},
												 	 	 {4,24,1,"镇坪路", "Zhenping Road"},
												 		 {4,25,1,"中潭路", "Zhongtan Road"},
												 		 {4,26,1,"上海火车站", "Shanghai Railway Station"}
												 }
                                             };


void CreateCCURegionSpace(int param_line_no,int param_total_items)
{///<创建CCU区间空间
	if(param_line_no == 3)
	{
		if(p_line3_region_info != NULL)
		{
			free(p_line3_region_info);
		}
		p_line3_region_info = (region_station_t *)malloc(param_total_items * sizeof(region_station_t));

		ccu_state.ccu_region_info[0].total_regions_number = param_total_items;
		ccu_state.ccu_region_info[0].p_regin_info = p_line3_region_info;
		ccu_state.ccu_region_info[0].default_region_no = 1;
		ccu_state.ccu_region_info[0].current_region_no = ccu_state.ccu_region_info[0].default_region_no;
		ccu_state.ccu_region_info[0].total_stations_number = 29;
	}
	else if(param_line_no == 4)
	{
		if(p_line4_region_info != NULL)
		{
			free(p_line4_region_info);
		}
		p_line4_region_info = (region_station_t *)malloc(param_total_items * sizeof(region_station_t));

		ccu_state.ccu_region_info[1].total_regions_number = param_total_items;
		ccu_state.ccu_region_info[1].p_regin_info = p_line4_region_info;
		ccu_state.ccu_region_info[1].default_region_no = 1;
		ccu_state.ccu_region_info[1].current_region_no = ccu_state.ccu_region_info[1].default_region_no;
		ccu_state.ccu_region_info[1].total_stations_number = 26;
	}
}

void InitCCURegionInfo()
{///<初始化CCU区间信息
	ccu_state.ccu_region_info[0].default_region_no = 1;
	ccu_state.ccu_region_info[1].default_region_no = 1;

	///<line 3
	ccu_state.ccu_region_info[0].p_regin_info->line_no = 3;
	ccu_state.ccu_region_info[0].p_regin_info->over_station_no = 29;
	ccu_state.ccu_region_info[0].p_regin_info->region_no = 1;
	ccu_state.ccu_region_info[0].p_regin_info->running_direction = 1;
	ccu_state.ccu_region_info[0].p_regin_info->start_station_no = 1;
	ccu_state.ccu_region_info[0].p_regin_info->whether_the_cycle_bus = 0;
	ccu_state.ccu_region_info[0].p_regin_info->current_region_flag = 1;

	(ccu_state.ccu_region_info[0].p_regin_info + 1)->line_no = 3;
	(ccu_state.ccu_region_info[0].p_regin_info+ 1)->over_station_no = 21;
	(ccu_state.ccu_region_info[0].p_regin_info+ 1)->region_no = 2;
	(ccu_state.ccu_region_info[0].p_regin_info+ 1)->running_direction = 1;
	(ccu_state.ccu_region_info[0].p_regin_info+ 1)->start_station_no = 1;
	(ccu_state.ccu_region_info[0].p_regin_info+ 1)->whether_the_cycle_bus = 0;
	(ccu_state.ccu_region_info[0].p_regin_info+ 1)->current_region_flag = 2;

	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->line_no = 3;
	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->over_station_no = 1;
	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->region_no = 3;
	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->running_direction = 2;
	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->start_station_no = 19;
	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->whether_the_cycle_bus = 0;
	(ccu_state.ccu_region_info[0].p_regin_info+ 2)->current_region_flag = 3;

	///<line 4
	(ccu_state.ccu_region_info[1].p_regin_info)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info)->over_station_no = 1;
	(ccu_state.ccu_region_info[1].p_regin_info)->region_no = 1;
	(ccu_state.ccu_region_info[1].p_regin_info)->running_direction = 1;
	(ccu_state.ccu_region_info[1].p_regin_info)->start_station_no = 1;
	(ccu_state.ccu_region_info[1].p_regin_info)->whether_the_cycle_bus = 1;
	(ccu_state.ccu_region_info[1].p_regin_info)->current_region_flag = 1;

	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->over_station_no = 5;
	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->region_no = 2;
	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->running_direction = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->start_station_no = 5;
	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->whether_the_cycle_bus = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 1)->current_region_flag = 2;


	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->over_station_no = 2;
	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->region_no = 3;
	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->running_direction = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->start_station_no = 2;
	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->whether_the_cycle_bus = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 2)->current_region_flag = 3;

	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->over_station_no = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->region_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->running_direction = 2;
	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->start_station_no = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->whether_the_cycle_bus = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 3)->current_region_flag = 4;

	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->over_station_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->region_no = 5;
	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->running_direction = 2;
	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->start_station_no = 5;
	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->whether_the_cycle_bus = 0;
	(ccu_state.ccu_region_info[1].p_regin_info+ 4)->current_region_flag = 5;

	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->over_station_no = 13;
	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->region_no = 6;
	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->running_direction = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->start_station_no = 1;
	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->whether_the_cycle_bus = 0;
	(ccu_state.ccu_region_info[1].p_regin_info+ 5)->current_region_flag = 6;

	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->line_no = 4;
	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->over_station_no = 14;
	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->region_no = 7;
	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->running_direction = 2;
	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->start_station_no = 19;
	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->whether_the_cycle_bus = 0;
	(ccu_state.ccu_region_info[1].p_regin_info+ 6)->current_region_flag = 7;
}
void InitStationInformation()
{///<初始化站台信息
	int i = 0;
	int j = 0;
	for(i = 0 ; i < 2;i ++)
	{
		for(j = 0; j < MAX_STATION_NUMBER; j++)
		{
			station_information[i][j].effective = 1;
		}
	}
}

void ClearSkipStationInfo(int line_number_of_skip_station)
{///<清除越站信息
	int i = 0;
	for(i = 0; i < MAX_STATION_NUMBER; i++)
	{
		if(station_information[line_number_of_skip_station - 3][i].effective != 2)
		{
			station_information[line_number_of_skip_station - 3][i].effective = 1;
		}
	}
		ccu_state.pa_failure_info[15] &= 0x1F;
		ccu_state.pa_failure_info[16] = 0;
		ccu_state.pa_failure_info[17] = 0;
		ccu_state.pa_failure_info[18] = 0;
		ccu_state.pa_failure_info[19] = 0;
		ccu_state.pa_failure_info[20] = 0;
		ccu_state.pa_failure_info[21] = 0;
		ccu_state.pa_failure_info[22] &= 0xF0;
//		skip_station_setting_is_effective = 8;
}

void GetDownDistanceInfo()
{///<获取下行距离信息
	int column = 0;
	int row;
	void *p_ann_db_3 = NULL;
	void *p_ann_db_4 = NULL;
	ann_station_report_t  *p_ann_station;
	int l = 0;
	int temp_region_no = -1;
	for(l = 0;l < ccu_state.ccu_region_info[0].total_regions_number;l++)
	{
		if((ccu_state.ccu_region_info[0].p_regin_info + l)->start_station_no == ccu_state.ccu_region_info[0].current_region_info.over_station_no &&
			(ccu_state.ccu_region_info[0].p_regin_info + l)->over_station_no == ccu_state.ccu_region_info[0].current_region_info.start_station_no	)
		{
			temp_region_no = (ccu_state.ccu_region_info[0].p_regin_info + l)->region_no;
			break;
		}
	}

	AnnReportOfEntry(3, temp_region_no, &row, &column, &p_ann_db_3);
	p_ann_station = (ann_station_report_t *)p_ann_db_3;
	for(l = 0;l < row && l < MAX_STATION_NUMBER;l ++)
	{
		if((p_ann_station + l)->station_no < MAX_STATION_NUMBER)
		{
			ccu_state.ccu_tms_station_diatance[0][(p_ann_station + l)->station_no].pre_triger_distance[1] =
					(p_ann_station + l)->station_type[0].pre_triger_distance;
			ccu_state.ccu_tms_station_diatance[0][(p_ann_station + l)->station_no].triger_distance[1] =
								(p_ann_station + l)->station_type[0].triger_distance;
			ccu_state.ccu_tms_station_diatance[0][(p_ann_station + l)->station_no].triger_dia_distance[1] =
								(p_ann_station + l)->station_type[0].before_after;
			ccu_state.pp_station_info[0][(p_ann_station + l)->station_no].open_door_side[1] =
					(p_ann_station + l)->station_type[0].open_door+1;
			//ccu_state.pp_station_info[0][(p_ann_station + l)->station_no].open_door_side[1] =
				//	((ann_station_report_t *)station_information[0][(p_ann_station + l)->station_no].p_station_info)->station_type[0].open_door+1;
		}
	}

	temp_region_no = -1;
	int temp_default_region_position = 0;
	for(l = 0;l < ccu_state.ccu_region_info[1].total_regions_number;l++)
	{
		if((ccu_state.ccu_region_info[1].p_regin_info + l)->region_no == ccu_state.ccu_region_info[1].default_region_no)
		{
			temp_default_region_position = l;
			break;
		}
	}
	for(l = 0;l < ccu_state.ccu_region_info[1].total_regions_number;l++)
	{
		if((ccu_state.ccu_region_info[1].p_regin_info + l)->region_no != ccu_state.ccu_region_info[1].default_region_no &&
			(ccu_state.ccu_region_info[1].p_regin_info + l)->start_station_no == (ccu_state.ccu_region_info[1].p_regin_info + temp_default_region_position)->over_station_no &&
			(ccu_state.ccu_region_info[1].p_regin_info + l)->over_station_no == (ccu_state.ccu_region_info[1].p_regin_info + temp_default_region_position)->start_station_no &&
			(ccu_state.ccu_region_info[1].p_regin_info + l)->whether_the_cycle_bus == (ccu_state.ccu_region_info[1].p_regin_info + temp_default_region_position)->whether_the_cycle_bus)
		{
			temp_region_no = (ccu_state.ccu_region_info[1].p_regin_info + l)->region_no;
			break;
		}
	}
	return ;
	diag_printf("temp_region_no = %d\n",temp_region_no);
	AnnReportOfEntry(4, temp_region_no, &row, &column, &p_ann_db_4);
	p_ann_station = (ann_station_report_t *)p_ann_db_4;
	for(l = 0;l < row && l < MAX_STATION_NUMBER;l ++)
	{
		if((p_ann_station + l)->station_no < MAX_STATION_NUMBER)
		{
			ccu_state.ccu_tms_station_diatance[1][(p_ann_station + l)->station_no].pre_triger_distance[1] =
					(p_ann_station + l)->station_type[0].pre_triger_distance;
			ccu_state.ccu_tms_station_diatance[1][(p_ann_station + l)->station_no].triger_distance[1] =
								(p_ann_station + l)->station_type[0].triger_distance;
			ccu_state.ccu_tms_station_diatance[1][(p_ann_station + l)->station_no].triger_dia_distance[1] =
								(p_ann_station + l)->station_type[0].before_after;
			ccu_state.pp_station_info[1][(p_ann_station + l)->station_no].open_door_side[1] =
					(p_ann_station + l)->station_type[0].open_door+1;
			diag_printf("4-AA-%d,%d\n",(p_ann_station + l)->station_no,(p_ann_station + l)->station_type[0].open_door);
		}
	}
	SqlTableBufferFree(&p_ann_db_3);
	SqlTableBufferFree(&p_ann_db_4);
}
void GetAnnStationInfo()
{///<获取站台详细信息
	int column = 0;
	int row;
	void *p_ann_db_3 = NULL;
	void *p_ann_db_4 = NULL;
	ann_station_report_t  *p_ann_station;
	int l = 0;
	debug_print(("@@:%d-%d\n",ccu_state.ccu_region_info[0].default_region_no,ccu_state.ccu_region_info[1].default_region_no));

	///<line 3
	AnnReportOfEntry(3, ccu_state.ccu_region_info[0].default_region_no, &row, &column, &p_ann_db_3);
	ccu_state.ccu_region_info[0].current_audio_file_number = row;
	p_ann_station = (ann_station_report_t *)p_ann_db_3;
	ccu_state.p_line_ann_station_info[0] = (ann_station_report_t *)p_ann_db_3;
	for(l = 0;l < row && l < MAX_STATION_NUMBER;l ++)
	{
		diag_printf("3:station id = %d\n",(p_ann_station + l)->station_no);
		if((p_ann_station + l)->station_no < MAX_STATION_NUMBER)
		{
			ccu_state.pp_station_info[0][(p_ann_station + l)->station_no].p_station_info =
					(void *)(p_ann_station + l);
		}
	}
return;
	///<line 4
	AnnReportOfEntry(4, ccu_state.ccu_region_info[1].default_region_no, &row, &column, &p_ann_db_4);
	p_ann_station = (ann_station_report_t *)p_ann_db_4;
	ccu_state.p_line_ann_station_info[1] = (ann_station_report_t *)p_ann_db_4;
	ccu_state.ccu_region_info[1].current_audio_file_number = row;
	for(l = 0;l < row && l < MAX_STATION_NUMBER;l ++)
	{
		debug_print(("4:station id = %d\n",(p_ann_station + l)->station_no));
		if((p_ann_station + l)->station_no < MAX_STATION_NUMBER)
		{
			ccu_state.pp_station_info[1][(p_ann_station + l)->station_no].p_station_info =
					(void *)(p_ann_station + l);
		}

	}
	
//	SqlTableBufferFree(&p_ann_db);
}

void GetStationDistance()
{///<获取站台距离信息
	int i = 0;
	unsigned long station_distance = 0;
	ccu_state.ccu_tms_station_diatance[0][1].distance = station_distance;
	for(i = 2;i <= ccu_state.ccu_region_info[0].total_stations_number;i++)
	{
		station_distance += ((ann_station_report_t *)station_information[0][i].p_station_info)->station_type[0].station_distance;
		ccu_state.ccu_tms_station_diatance[0][i].distance = station_distance;
		ccu_state.ccu_tms_station_diatance[0][i].triger_distance[0] = ((ann_station_report_t *)station_information[0][i].p_station_info)->station_type[0].triger_distance;
		ccu_state.ccu_tms_station_diatance[0][i].triger_dia_distance[0] = ((ann_station_report_t *)station_information[0][i].p_station_info)->station_type[0].before_after;
		ccu_state.ccu_tms_station_diatance[0][i].pre_triger_distance[0] = ((ann_station_report_t *)station_information[0][i].p_station_info)->station_type[0].pre_triger_distance;
		ccu_state.pp_station_info[0][i].open_door_side[0] = ((ann_station_report_t *)station_information[0][i].p_station_info)->station_type[0].open_door+1;
	}
	ccu_state.pp_station_info[0][1].open_door_side[0] = ((ann_station_report_t *)station_information[0][1].p_station_info)->station_type[0].open_door+1;
return ;
	station_distance = 0;
	ccu_state.ccu_tms_station_diatance[1][1].distance = station_distance;
	for(i = 2;i <= ccu_state.ccu_region_info[1].total_stations_number;i++)
	{
		station_distance += ((ann_station_report_t *)station_information[1][i].p_station_info)->station_type[0].station_distance;
		ccu_state.ccu_tms_station_diatance[1][i].distance = station_distance;
		ccu_state.ccu_tms_station_diatance[1][i].triger_distance[0] = ((ann_station_report_t *)station_information[1][i].p_station_info)->station_type[0].triger_distance;
		ccu_state.ccu_tms_station_diatance[1][i].triger_dia_distance[0] = ((ann_station_report_t *)station_information[1][i].p_station_info)->station_type[0].before_after;
		ccu_state.ccu_tms_station_diatance[1][i].pre_triger_distance[0] = ((ann_station_report_t *)station_information[1][i].p_station_info)->station_type[0].pre_triger_distance;
		ccu_state.pp_station_info[1][i].open_door_side[0] = ((ann_station_report_t *)station_information[1][i].p_station_info)->station_type[0].open_door+1;
	}
	station_distance = ((ann_station_report_t *)station_information[1][1].p_station_info)->station_type[0].station_distance;
	ccu_state.ccu_tms_station_diatance[1][0].distance = station_distance;
	ccu_state.ccu_tms_station_diatance[1][0].triger_distance[0] = ((ann_station_report_t *)station_information[1][1].p_station_info)->station_type[0].triger_distance;
	ccu_state.ccu_tms_station_diatance[1][0].triger_dia_distance[0] = ((ann_station_report_t *)station_information[1][1].p_station_info)->station_type[0].before_after;
	ccu_state.ccu_tms_station_diatance[1][0].pre_triger_distance[0] = ((ann_station_report_t *)station_information[1][1].p_station_info)->station_type[0].pre_triger_distance;
	ccu_state.pp_station_info[1][1].open_door_side[0] = ((ann_station_report_t *)station_information[1][1].p_station_info)->station_type[0].open_door+1;
}

unsigned long GetTwoStationDistance(unsigned char line_no,unsigned char first_station,unsigned char secode_station,unsigned char up_or_down)
{///<获取两站之间的站台距离
	unsigned int ret = 0;
	if(line_no == 3)
	{
		ret = abs(ccu_state.ccu_tms_station_diatance[0][secode_station].distance - ccu_state.ccu_tms_station_diatance[0][first_station].distance);
	}
	else if(line_no == 4)
	{
		unsigned char temp_total_stations = ccu_state.ccu_region_info[1].total_stations_number;
		if(up_or_down == 1)
		{
			if(first_station >= secode_station)
			{
				ret = abs(ccu_state.ccu_tms_station_diatance[1][temp_total_stations].distance - ccu_state.ccu_tms_station_diatance[1][first_station].distance);

				ret += ccu_state.ccu_tms_station_diatance[1][0].distance;

				ret += ccu_state.ccu_tms_station_diatance[1][secode_station].distance;

			}
			else
			{
				ret = abs(ccu_state.ccu_tms_station_diatance[1][secode_station].distance - ccu_state.ccu_tms_station_diatance[1][first_station].distance);
			}
		}
		else if(up_or_down == 2)
		{
			if(first_station <= secode_station)
			{
				ret = abs(ccu_state.ccu_tms_station_diatance[1][temp_total_stations].distance - ccu_state.ccu_tms_station_diatance[1][secode_station].distance);

				ret += ccu_state.ccu_tms_station_diatance[1][0].distance;

				ret += ccu_state.ccu_tms_station_diatance[1][first_station].distance;
			}
			else
			{
				ret = abs(ccu_state.ccu_tms_station_diatance[1][secode_station].distance - ccu_state.ccu_tms_station_diatance[1][first_station].distance);
			}
		}
	}
	return ret;
}

void ShowDistanceInfo()
{///<显示站台距离信息
	int k = 0;

	for(k = 1;k<= 15;k++)
	{
		diag_printf("Line3:station:%d,%d,%d\n",k,ccu_state.pp_station_info[0][k].open_door_side[0],ccu_state.pp_station_info[0][k].open_door_side[1]);
	}
//	for(k = 1;k<= 26;k++)
//	{
//		diag_printf("Line4:station:%d,%d,%d\n",k,ccu_state.pp_station_info[1][k].open_door_side[0],ccu_state.pp_station_info[1][k].open_door_side[1]);
//	}

	for(k = 1;k <= 15;k++)
	{
		diag_printf("3:%d:distance:%ld  pre:%ld,%ld    dia:%ld,%ld    tri:%ld,%ld\n",k,ccu_state.ccu_tms_station_diatance[0][k].distance-ccu_state.ccu_tms_station_diatance[0][k-1].distance,
				ccu_state.ccu_tms_station_diatance[0][k].pre_triger_distance[0],ccu_state.ccu_tms_station_diatance[0][k].pre_triger_distance[1],
				ccu_state.ccu_tms_station_diatance[0][k].triger_dia_distance[0],ccu_state.ccu_tms_station_diatance[0][k].triger_dia_distance[1],
				ccu_state.ccu_tms_station_diatance[0][k].triger_distance[0],ccu_state.ccu_tms_station_diatance[0][k].triger_distance[1]);
	}
//	for(k = 1;k <= 26;k++)
//	{
//		diag_printf("4:%d:distance:%ld  pre:%ld,%ld    dia:%ld,%ld    tri:%ld,%ld\n",k,ccu_state.ccu_tms_station_diatance[1][k].distance-ccu_state.ccu_tms_station_diatance[1][k-1].distance,
//				ccu_state.ccu_tms_station_diatance[1][k].pre_triger_distance[0],ccu_state.ccu_tms_station_diatance[1][k].pre_triger_distance[1],
//				ccu_state.ccu_tms_station_diatance[1][k].triger_dia_distance[0],ccu_state.ccu_tms_station_diatance[1][k].triger_dia_distance[1],
//				ccu_state.ccu_tms_station_diatance[1][k].triger_distance[0],ccu_state.ccu_tms_station_diatance[1][k].triger_distance[1]);
//	}
}


