/*
 * sqlite_convert_fill_fun.h
 *
 *  Created on: 2014-1-8
 *      Author: benty
 */

#ifndef SQLITE_CONVERT_FILL_FUN_H_
#define SQLITE_CONVERT_FILL_FUN_H_

#include "sqlite_middle_layer.h"

typedef struct
{
	unsigned char display_content[100];	//保存显示类容
	unsigned char audio_path[100];	//保存音频路径
}audio_and_display_t;

typedef enum
{
	no_attribute = 0,	//无属性,默认无属性

	end_station_att = 1,//终点站属性
	next_station_att = 2,//下一站属性
	current_station_att = 3,//当前站属性
	end_station_eng_att = 4,//终点在-英文属性
	up_down_att = 5,	//上下行，内外圈属性
	loop_cycle_att = 6,//环线属性
	open_door_side_att = 7,//开门侧属性
	next_station_eng_att = 8,//下一站-英文属性
	open_door_side_eng_att = 9,//开门侧-英文属性

	end_of_attribute
}fill_attribute_t;

typedef enum
{
	false_loop_att = 0,	//不是环线，播放终点站

	true_loop_att = 1,//环线,不播放终点站

}loop_cycle_t;


typedef struct
{
	audio_and_display_t up_down[2];	//【1】,外圈下行（Outer Down），
										//【0】，内圈上行（Inner Up）
}line_direction_t;

typedef struct
{
	audio_and_display_t next_station_ch[2];	//【1】,下一站终点站，
												//【0】，下一站
}next_station_chinese_t;

typedef struct
{
	audio_and_display_t next_station_eng[2];	//【1】,Next stop is the terminal station:
													//【0】，next staion
}next_station_eng_t;

typedef struct
{
	audio_and_display_t get_station_eng[2];	//【1】,We’re now at the terminal station
													//【0】，We’re now at
}get_station_eng_t;


typedef struct
{
	audio_and_display_t door_side_ch[3];	//【2】,两侧开门
												//【1】,左侧开门
												//【0】，右侧开门
}door_side_ch_t;

typedef struct
{
	audio_and_display_t door_side_eng[3];	//【2】,both
												//【1】,left
												//【0】，right
}door_side_eng_t;

//将数据库中的模糊数据（共用部分数据），分解填充到全局结构体中，以便后续快速使用
int DataConvertStruct(ann_station_report_t *p_to_sql);

//打印测试
void StructPrintTest(void);

//共用结构体检测，判断共用结构体中是否有空，有空则不正确。
void CommonStructTest(void);

//line:当前线路号
//up_down:	//【1】,外圈下行（Outer Down），【0】，内圈上行（Inner Up）
//end_staton_display:终点站显示
//end_staton_audio:终点站音频
//end_station_no，终点站站代码号
//ret_loop：//【0】不是环线，【1】环线（判断是否是环线的接口已经编写好了JudgeSectionIsLoop）
//open_door：开门侧，//【2】,两侧开门，【1】,左侧开门，【0】，右侧开门。无tms时，以预报站中信息为准
//p_ann_report当前要填充的站，其结构是ann_station_report_t
//字幕的属性和音频的属性都相同
//先替换预报站字幕、音频信息。在替换本站的报站字幕、音频信息。
//根据具体信息，实现每一站属性（如开门侧）等替换
int FileAttributeInit(int line, int up_down, char* end_staton_display, char* end_staton_audio, int end_station_no, int ret_loop, int open_door, void *p_ann_report);

//更具传入参数确定“终点站，XXX”两端是否播放，显示
int LoopCycleFill(unsigned int loop_cycle_type, ann_station_information_t *prt_data_pre);

//【1】,外圈下行（Outer Down），
//【0】，内圈上行（Inner Up）
//3、4号线上下行填充，仅仅4号线（环线）有。
int FillUpDown(int up_down, ann_station_information_t *prt_data_pre);

//音频、字幕终点站填充、替换接口
int EndStationFill2(char* end_staton_display, char* end_staton_audio, ann_station_information_t *prt_data_pre);

//【1】,下一站终点，
//【0】，下一站
int NextStionFill(int next_station_type, ann_station_information_t *prt_data_pre);

//实现开门替换
int OpenDoorSideFill(int open_door_side, ann_station_information_t *prt_data_pre);

//终点站到站“终点站” 和 “请全体乘客下车”信息播放
int GetEndStationFill(unsigned int loop_cycle_type, ann_station_information_t *prt_data_get);

//【1】,We’re now at the terminal station
//【0】，We’re now at
int GetStationFill(int next_station_type, ann_station_information_t *prt_data_get);


#endif /* SQLITE_CONVERT_FILL_FUN_H_ */
