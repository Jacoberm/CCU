/*
 * sqlite_convert_fill_fun.c
 *
 *  Created on: 2014-1-8
 *      Author: benty
 */
#include "sqlite_convert_fill_fun.h"

//#define sqlite_convert_file_print
#ifndef sqlite_convert_file_print
   #define sqlite_convert_file_print(s) diag_printf s
#else
   #define sqlite_convert_file_print(s)
#endif


//保存列车内外圈、上下行运行方向
//【1】,外圈下行（Outer Down），
//【0】，内圈上行（Inner Up）
line_direction_t line_direction;

//下一站、下一站终点站
//【1】,下一站终点，
//【0】，下一站
next_station_chinese_t next_station_chinese;

//开门侧
//【2】,两侧开门
//【1】,右侧开门
//【0】，左侧开门
door_side_ch_t door_side_ch;

//next station
//【1】,Next stop is the terminal station:
//【0】，next staion
next_station_eng_t next_station_eng;

//open door side
//【2】,both
//【1】, right
//【0】，left
door_side_eng_t door_side_eng;


//到站信息
//【1】,We’re now at the terminal station
//【0】，We’re now at
get_station_eng_t get_station_eng;


//将线路号0，区间0的查询结果分解到填充结构体中

//查询结果有3条数据，（排序是按照 车站信息表中 的 站序号 来排列）

//1，右侧-外圈-下一站终点站--终点站到站（eng）
#define LineOne 1

//2，左侧-内圈-下一站--非终点站到站（eng）
#define LineTwo 0

//3，两侧开门，现在没有，为空

//将数据库中的模糊数据（共用部分数据），分解填充到全局结构体中，以便后续快速使用
int DataConvertStruct(ann_station_report_t *p_to_sql)
{
	if (NULL == p_to_sql)
	{
		sqlite_convert_file_print( ("the fuzzy data is NULL\n") );
		return -1;
	}

	//解析数据行1
	int ret_count = 0;
	{
		//数据第一行为运行方向， 指向"内外圈、上下行运行"
		//保存列车内外圈、上下行运行方向----外圈下行
		strncpy(line_direction.up_down[LineOne].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(line_direction.up_down[LineOne].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count += 3;//数据指向"下一站"

		//"下一站、下一站终点站"----下一站终点站
		strncpy(next_station_chinese.next_station_ch[LineOne].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(next_station_chinese.next_station_ch[LineOne].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"开门侧"

		//开门侧---右侧开门
		strncpy(door_side_ch.door_side_ch[LineOne].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(door_side_ch.door_side_ch[LineOne].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"next station"

		//next station----Next stop is the terminal station:
		strncpy(next_station_eng.next_station_eng[LineOne].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(next_station_eng.next_station_eng[LineOne].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"open door side"

		//open door side-----right
		strncpy(door_side_eng.door_side_eng[LineOne].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(door_side_eng.door_side_eng[LineOne].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"到站信息（eng）"

		//到站信息（eng）-----“We’re now at the terminal station”
		strncpy(get_station_eng.get_station_eng[LineOne].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(get_station_eng.get_station_eng[LineOne].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
	}


	//解析数据行2
	p_to_sql++;
	ret_count = 0;//复为0
	{
		//数据第一行为运行方向， 指向"内外圈、上下行运行"
		//保存列车内外圈、上下行运行方向----内圈上行
		strncpy(line_direction.up_down[LineTwo].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(line_direction.up_down[LineTwo].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count += 3;//数据指向"下一站"

		//下一站、下一站终点站----下一站
		strncpy(next_station_chinese.next_station_ch[LineTwo].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(next_station_chinese.next_station_ch[LineTwo].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"开门侧"

		//开门侧---左侧开门
		strncpy(door_side_ch.door_side_ch[LineTwo].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(door_side_ch.door_side_ch[LineTwo].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"next station"

		//next station----next staion
		strncpy(next_station_eng.next_station_eng[LineTwo].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(next_station_eng.next_station_eng[LineTwo].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"open door side"

		//open door side-----left
		strncpy(door_side_eng.door_side_eng[LineTwo].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(door_side_eng.door_side_eng[LineTwo].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);
		ret_count++;//指向"到站信息（eng）"

		//到站信息（eng）-----“We’re now at”
		strncpy(get_station_eng.get_station_eng[LineTwo].audio_path, (p_to_sql->station_type[report_station].audio + ret_count)->audio_display, MAX_STRING_LEN);
		strncpy(get_station_eng.get_station_eng[LineTwo].display_content, (p_to_sql->station_type[report_station].display + ret_count)->audio_display, MAX_STRING_LEN);

	}

	//解析数据行3，数据不需要（上海3、4号线不需要）,暂时都没有两侧开门的数据，
	p_to_sql++;
	ret_count = 0;//复为0

	//打印测试
	StructPrintTest();

	//共用结构体检测，判断共用结构体中是否有空，有空则不正确。
	CommonStructTest();

	return 0;
}

//某站的路径打印测试，
int PrintTheAudioPath(void *ptr)
{
	ann_station_information_t *prt_data_pre = &(((ann_station_report_t*)ptr)->station_type[pre_station]);

	if ((NULL != prt_data_pre->audio) && (NULL != prt_data_pre->display))
	{
		int count_len = 0;
		int num_audio_path = 0;

		while(0 != (prt_data_pre->display + count_len)->audio_display[0])
		{
			sqlite_convert_file_print(("----display_color--%x-\n",(prt_data_pre->display + count_len)->color_attr));
			if (0 == (prt_data_pre->display + count_len)->available_or_not )
			{
				num_audio_path++;
			}
			count_len++;
		}
		sqlite_convert_file_print(("----the num_display--%d-\n", num_audio_path));
	}

	ann_station_information_t *prt_data_get = &(((ann_station_report_t*)ptr)->station_type[report_station]);

	if ((NULL != prt_data_get->audio) && (NULL != prt_data_get->display))
	{
		int count_len = 0;
		int num_audio_path = 0;


		while(0 != (prt_data_get->audio + count_len)->audio_display[0])
		{
			sqlite_convert_file_print(("----display_color--%x-\n",(prt_data_get->display + count_len)->color_attr));
			if (0 == (prt_data_get->display + count_len)->available_or_not )
			{
				num_audio_path++;
			}
			count_len++;
		}
		sqlite_convert_file_print(("----the num_display--%d-\n", num_audio_path));
	}


	return 0;
}

//字幕的属性和音频的属性都相同
//先替换预报站字幕、音频信息。在替换本站的报站字幕、音频信息。
//根据具体信息，实现每一站属性（如开门侧）等替换
int FileAttributeInit(int line, int up_down, char* end_staton_display, char* end_staton_audio, int end_station_no, int ret_loop, int open_door, void *p_ann_report)
{
	//	pre_station = 0,//预报站
	//	report_station = 1,//报站
	sqlite_convert_file_print(("\n----------------------\n"));
	sqlite_convert_file_print(("\n--end_staton_display = %s-----------------\n",end_staton_display));
	sqlite_convert_file_print(("\n--end_staton_audio = %s-----------------\n",end_staton_audio));
	sqlite_convert_file_print(("\n--end_station_no = %d-----------------\n",end_station_no));
	sqlite_convert_file_print(("\n--line = %d-----------------\n",line));
	sqlite_convert_file_print(("\n--up_down = %d-----------------\n",up_down));
	sqlite_convert_file_print(("\n--ret_loop = %d-----------------\n",ret_loop));
	sqlite_convert_file_print(("\n--open_door = %d-----------------\n",open_door));

	if ( (NULL == p_ann_report) || (NULL == end_staton_display)
			|| (0 == end_staton_display[0]) )
	{
		sqlite_convert_file_print(("\n-----the report buffer is null-: %d,%d, %d\n", (unsigned int)(end_staton_display), end_staton_display[0], (NULL == p_ann_report)));
		return -1;
	}

	//检测标准：以前定义的接口选项
	if (	(up_down < 0) || (up_down > 1)
			|| (open_door < 0) || (open_door > 2)
	)
	{
		sqlite_convert_file_print(("\n-----the rup_down or open_door is error-\n"));
		return -1;
	}

	ann_station_information_t *prt_data_pre = &(((ann_station_report_t*)p_ann_report)->station_type[pre_station]);
	ann_station_information_t *prt_data_get = &(((ann_station_report_t*)p_ann_report)->station_type[report_station]);

//预到站部分---填充>>>>>>>>>>>
	if ((NULL != prt_data_pre->audio) && (NULL != prt_data_pre->display))
	{
		//更具传入参数确定“终点站，XXX”两端是否播放，显示
		if (ret_loop == 1)//环线,不播放终点站
		{
			LoopCycleFill(true_loop_att, prt_data_pre);
		}
		else//非环线，播放终点站
		{
			LoopCycleFill(false_loop_att, prt_data_pre);
		}

		//3、4预到站上下行填充，仅有4号线环线有该选项。
		if (4 == line)
		{
			//3、4预到站上下行填充，仅有4号线环线有该选项。
			FillUpDown(up_down, prt_data_pre);
		}

		//音频、字幕终点站填充接口
              if( NULL != end_staton_audio  && 0 == end_staton_audio[0] ) ///< ??
		EndStationFill2(end_staton_display, end_staton_audio, prt_data_pre);

		//【1】,下一站终点，
		//【0】，下一站
		if (((ann_station_report_t*)p_ann_report)->station_no == end_station_no)
			//表示当前站为终点站
		{
			if (ret_loop == 1)//环线
			{
				NextStionFill(LineTwo, prt_data_pre);
			}
			else
			{
				//中英文一起修改，避免只调用了其中一个接口
				NextStionFill(LineOne, prt_data_pre);
			}
		}
		else//当前站为非终点站
		{
			NextStionFill(LineTwo, prt_data_pre);
		}

		//开门侧-填充,中英文一起修改，避免只调用了其中一个接口
		//【2】,两侧开门
		//【1】,右侧开门
		//【0】，左侧开门
		OpenDoorSideFill(open_door, prt_data_pre);
	}
//预到站部分---填充<<<<<


//到站部分---填充>>>>>>>>>>>
//中英文、字幕和音频 一起修改，避免只调用了其中一个接口

	if ((NULL != prt_data_get->audio) && (NULL != prt_data_get->display))
	{
		//【1】,We’re now at the terminal station
		//【0】，We’re now at
		if (((ann_station_report_t*)p_ann_report)->station_no == end_station_no)
		{
			if (ret_loop == 1)//环线-- 不播放“终点站” 和 “请全体乘客下车。”
			{
				GetEndStationFill(true_loop_att, prt_data_get);
				GetStationFill(LineTwo, prt_data_get);
			}
			else			//表示当前站为终点站--要播放“终点站” 和 “请全体乘客下车。”
			{
				GetEndStationFill(false_loop_att, prt_data_get);
				GetStationFill(LineOne, prt_data_get);
			}
		}
		else			//表示当前站不是终点站-- 不 播放“终点站” 和 “请全体乘客下车。”
		{
			GetEndStationFill(true_loop_att, prt_data_get);
			GetStationFill(LineTwo, prt_data_get);
		}

		//开门侧-填充,中英文一起修改，避免只调用了其中一个接口
		//【2】,两侧开门
		//【1】,右侧开门
		//【0】，左侧开门
		OpenDoorSideFill(open_door, prt_data_get);
	}
//到站部分---填充<<<<<<<<<<

	return 0;
}

//【1】,We’re now at the terminal station
//【0】，We’re now at
int GetStationFill(int next_station_type, ann_station_information_t *prt_data_get)
{
	int count_len = 0;
	int fill_type = next_station_type;

	//音频替换
	while(0 != (prt_data_get->audio + count_len)->audio_display[0])
	{
		if((prt_data_get->audio + count_len)->file_attr == next_station_eng_att)
		{
			bzero((prt_data_get->audio + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_get->audio + count_len)->audio_display, get_station_eng.get_station_eng[fill_type].audio_path);
		}

		count_len++;
	}

	 count_len = 0;
	//字幕替换
	while(0 != (prt_data_get->display + count_len)->audio_display[0])
	{
		if((prt_data_get->display + count_len)->file_attr == next_station_eng_att)
		{
			bzero((prt_data_get->display + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_get->display + count_len)->audio_display, get_station_eng.get_station_eng[fill_type].display_content);
		}

		count_len++;
	}

	return 0;
}


//终点站到站“终点站” 和 “请全体乘客下车”信息播放
int GetEndStationFill(unsigned int loop_cycle_type, ann_station_information_t *prt_data_get)
{
	int count_len = 0;
	unsigned int fill_type = loop_cycle_type;

	//音频替换
	while(0 != (prt_data_get->audio + count_len)->audio_display[0])
	{
		if((prt_data_get->audio + count_len)->file_attr == loop_cycle_att)
		{
			(prt_data_get->audio + count_len)->available_or_not = fill_type;
			//不能要break，到站信息中有两个相通属性的字段
		}

		count_len++;
	}

	 count_len = 0;
	//字幕替换
	while(0 != (prt_data_get->display + count_len)->audio_display[0])
	{
		if((prt_data_get->display + count_len)->file_attr == loop_cycle_att)
		{
			(prt_data_get->display + count_len)->available_or_not = fill_type;
		}

		count_len++;
	}

	return 0;
}




//开门侧-填充,中英文一起修改，避免只调用了其中一个接口
////实现开门替换
int OpenDoorSideFill(int open_door_side, ann_station_information_t *prt_data_pre)
{
	int count_len = 0;
	unsigned char fill_type = open_door_side;

	while(0 != (prt_data_pre->audio + count_len)->audio_display[0])
	{
		if((prt_data_pre->audio + count_len)->file_attr == open_door_side_att)
		{
			bzero((prt_data_pre->audio + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->audio + count_len)->audio_display, door_side_ch.door_side_ch[fill_type].audio_path);
		}
		else if((prt_data_pre->audio + count_len)->file_attr == open_door_side_eng_att)
		{
			bzero((prt_data_pre->audio + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->audio + count_len)->audio_display, door_side_eng.door_side_eng[fill_type].audio_path);
//			sqlite_convert_file_print( ("display_content1111:%s\n",door_side_eng.door_side_eng[fill_type].audio_path) );
		}

		count_len++;
	}

	 count_len = 0;
	//字幕替换
	while(0 != (prt_data_pre->display + count_len)->audio_display[0])
	{
		if((prt_data_pre->display + count_len)->file_attr == open_door_side_att)
		{
			bzero((prt_data_pre->display + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->display + count_len)->audio_display, door_side_ch.door_side_ch[fill_type].display_content);
		}
		else if((prt_data_pre->display + count_len)->file_attr == open_door_side_eng_att)
		{
			bzero((prt_data_pre->display + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->display + count_len)->audio_display, door_side_eng.door_side_eng[fill_type].display_content);
		}

		count_len++;
	}

	return 0;
}

//音频、字幕终点站填充、替换接口
int EndStationFill2(char* end_staton_display, char* end_staton_audio, ann_station_information_t *prt_data_pre)
{
	int count_len = 0;

	//音频替换---中文
	while(0 != (prt_data_pre->audio + count_len)->audio_display[0])
	{
		if((prt_data_pre->audio + count_len)->file_attr == end_station_att)
		{
			bzero((prt_data_pre->audio + count_len)->audio_display, MAX_STRING_LEN);
			strcpy((prt_data_pre->audio + count_len)->audio_display, end_staton_audio);
		}
		count_len++;
	}

	 count_len = 0;
	//字幕替换----中文
	while(0 != (prt_data_pre->display + count_len)->audio_display[0])
	{
		if((prt_data_pre->display + count_len)->file_attr == end_station_att)
		{
			bzero((prt_data_pre->display + count_len)->audio_display, MAX_STRING_LEN);
			strcpy((prt_data_pre->display + count_len)->audio_display, end_staton_display);
		}

		count_len++;
	}



	//	 count_len = 0;
	//	//字幕替换
	//	while(0 != (prt_data_pre->display + count_len)->audio_display[0])
	//	{
	//		if((prt_data_pre->display + count_len)->file_attr == next_station_att)
	//		{
	//			(prt_data_pre->display + count_len)->available_or_not = fill_type;
	//		}
	//		else if((prt_data_pre->display + count_len)->file_attr == next_station_eng_att)
	//		{
	//			(prt_data_pre->display + count_len)->available_or_not = fill_type;
	//		}
	//
	//		count_len++;
	//	}


	return 0;
}


//更具传入参数确定“终点站，XXX”两端是否播放，显示
int LoopCycleFill(unsigned int loop_cycle_type, ann_station_information_t *prt_data_pre)
{
	int count_len = 0;
	unsigned int fill_type = loop_cycle_type;

	//音频替换---中文
	while(0 != (prt_data_pre->audio + count_len)->audio_display[0])
	{
		if((prt_data_pre->audio + count_len)->file_attr == loop_cycle_att)
		{
			(prt_data_pre->audio + count_len)->available_or_not = fill_type;
		}
		else if((prt_data_pre->audio + count_len)->file_attr == end_station_att)
		{
			(prt_data_pre->audio + count_len)->available_or_not = fill_type;
		}

		count_len++;
	}

	 count_len = 0;
	//字幕替换---中文
	while(0 != (prt_data_pre->display + count_len)->audio_display[0])
	{
		if((prt_data_pre->display + count_len)->file_attr == loop_cycle_att)
		{
			(prt_data_pre->display + count_len)->available_or_not = fill_type;
		}
		else if((prt_data_pre->display + count_len)->file_attr == end_station_att)
		{
			(prt_data_pre->display + count_len)->available_or_not = fill_type;
		}

		count_len++;
	}

	return 0;
}

//【1】,外圈下行（Outer Down），
//【0】，内圈上行（Inner Up）
//3、4号线上下行填充，仅仅4号线（环线）有。
int FillUpDown(int up_down, ann_station_information_t *prt_data_pre)
{
	int count_len = 0;
	int fill_type = up_down;

	//音频替换
	while(0 != (prt_data_pre->audio + count_len)->audio_display[0])
	{
		if((prt_data_pre->audio + count_len)->file_attr == up_down_att)
		{
			bzero((prt_data_pre->audio + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->audio + count_len)->audio_display, line_direction.up_down[fill_type].audio_path);
		}
		count_len++;
	}

	 count_len = 0;
	//字幕替换
	while(0 != (prt_data_pre->display + count_len)->audio_display[0])
	{
		if((prt_data_pre->display + count_len)->file_attr == up_down_att)
		{
			bzero((prt_data_pre->display + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->display + count_len)->audio_display, line_direction.up_down[fill_type].display_content);
		}

		count_len++;
	}

	return 0;
}

//【1】,下一站终点，
//【0】，下一站
int NextStionFill(int next_station_type, ann_station_information_t *prt_data_pre)
{
	int count_len = 0;
	int fill_type = next_station_type;

	//音频替换
	while(0 != (prt_data_pre->audio + count_len)->audio_display[0])
	{
		if((prt_data_pre->audio + count_len)->file_attr == next_station_att)
		{
			bzero((prt_data_pre->audio + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->audio + count_len)->audio_display, next_station_chinese.next_station_ch[fill_type].audio_path);
		}
		else if((prt_data_pre->audio + count_len)->file_attr == next_station_eng_att)
		{
			bzero((prt_data_pre->audio + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->audio + count_len)->audio_display, next_station_eng.next_station_eng[fill_type].audio_path);
		}

		count_len++;
	}

	 count_len = 0;
	//字幕替换
	while(0 != (prt_data_pre->display + count_len)->audio_display[0])
	{
		if((prt_data_pre->display + count_len)->file_attr == next_station_att)
		{
			bzero((prt_data_pre->display + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->display + count_len)->audio_display, next_station_chinese.next_station_ch[fill_type].display_content);
		}
		else if((prt_data_pre->display + count_len)->file_attr == next_station_eng_att)
		{
			bzero((prt_data_pre->display + count_len)->audio_display,MAX_STRING_LEN);
			strcpy((prt_data_pre->display + count_len)->audio_display, next_station_eng.next_station_eng[fill_type].display_content);
		}

		count_len++;
	}

	return 0;
}

//测试分解的数据是否填充到结构体中了
void StructPrintTest(void)
{
	sqlite_convert_file_print( ("#############----StructPrintTest---################\n") );

	sqlite_convert_file_print( ("audio_path:%s\n",line_direction.up_down[LineOne].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",line_direction.up_down[LineOne].display_content) );
	sqlite_convert_file_print( ("audio_path:%s\n",line_direction.up_down[LineTwo].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",line_direction.up_down[LineTwo].display_content) );

	sqlite_convert_file_print( ("audio_path:%s\n",next_station_chinese.next_station_ch[LineOne].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",next_station_chinese.next_station_ch[LineOne].display_content) );
	sqlite_convert_file_print( ("audio_path:%s\n",next_station_chinese.next_station_ch[LineTwo].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",next_station_chinese.next_station_ch[LineTwo].display_content) );

	sqlite_convert_file_print( ("audio_path:%s\n",door_side_ch.door_side_ch[LineOne].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",door_side_ch.door_side_ch[LineOne].display_content) );
	sqlite_convert_file_print( ("audio_path:%s\n",door_side_ch.door_side_ch[LineTwo].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",door_side_ch.door_side_ch[LineTwo].display_content) );

	sqlite_convert_file_print( ("audio_path:%s\n",next_station_eng.next_station_eng[LineOne].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",next_station_eng.next_station_eng[LineOne].display_content) );
	sqlite_convert_file_print( ("audio_path:%s\n",next_station_eng.next_station_eng[LineTwo].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",next_station_eng.next_station_eng[LineTwo].display_content) );

	sqlite_convert_file_print( ("audio_path:%s\n",door_side_eng.door_side_eng[LineOne].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",door_side_eng.door_side_eng[LineOne].display_content) );
	sqlite_convert_file_print( ("audio_path:%s\n",door_side_eng.door_side_eng[LineTwo].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",door_side_eng.door_side_eng[LineTwo].display_content) );


	sqlite_convert_file_print( ("audio_path:%s\n",get_station_eng.get_station_eng[LineOne].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",get_station_eng.get_station_eng[LineOne].display_content) );
	sqlite_convert_file_print( ("audio_path:%s\n",get_station_eng.get_station_eng[LineTwo].audio_path) );
	sqlite_convert_file_print( ("display_content:%s\n",get_station_eng.get_station_eng[LineTwo].display_content) );
}

//判断共用结构体中是否有空
void CommonStructTest(void)
{
	sqlite_convert_file_print( ("************  CommonStructTest *************\n") );
	if ( (0 == line_direction.up_down[LineOne].audio_path[0])
		|| (0 == line_direction.up_down[LineOne].display_content[0])
		|| (0 == line_direction.up_down[LineTwo].audio_path[0])
		|| (0 == line_direction.up_down[LineTwo].display_content[0])
	)
	{
		sqlite_convert_file_print( ("*********** line_direction is error  ***********\n") );
	}

	if ( (0 == next_station_chinese.next_station_ch[LineOne].audio_path[0])
		|| (0 == next_station_chinese.next_station_ch[LineOne].display_content[0])
		|| (0 == next_station_chinese.next_station_ch[LineTwo].audio_path[0])
		|| (0 == next_station_chinese.next_station_ch[LineTwo].display_content[0])
	)
	{
		sqlite_convert_file_print( ("*********** next_station_chinese is error  ***********\n") );
	}

	if ( (0 == door_side_ch.door_side_ch[LineOne].audio_path[0])
		|| (0 == door_side_ch.door_side_ch[LineOne].display_content[0])
		|| (0 == door_side_ch.door_side_ch[LineTwo].audio_path[0])
		|| (0 == door_side_ch.door_side_ch[LineTwo].display_content[0])
	)
	{
		sqlite_convert_file_print( ("*********** door_side_ch is error  ***********\n") );
	}

	if ( (0 == next_station_eng.next_station_eng[LineOne].audio_path[0])
		|| (0 == next_station_eng.next_station_eng[LineOne].display_content[0])
		|| (0 == next_station_eng.next_station_eng[LineTwo].audio_path[0])
		|| (0 == next_station_eng.next_station_eng[LineTwo].display_content[0])
	)
	{
		sqlite_convert_file_print( ("*********** door_side_ch is error  ***********\n") );
	}

	if ( (0 == door_side_eng.door_side_eng[LineOne].audio_path[0])
		|| (0 == door_side_eng.door_side_eng[LineOne].display_content[0])
		|| (0 == door_side_eng.door_side_eng[LineTwo].audio_path[0])
		|| (0 == door_side_eng.door_side_eng[LineTwo].display_content[0])
	)
	{
		sqlite_convert_file_print( ("*********** door_side_eng is error  ***********\n") );
	}

	if ( (0 == get_station_eng.get_station_eng[LineOne].audio_path[0])
		|| (0 == get_station_eng.get_station_eng[LineOne].display_content[0])
		|| (0 == get_station_eng.get_station_eng[LineTwo].audio_path[0])
		|| (0 == get_station_eng.get_station_eng[LineTwo].display_content[0])
	)
	{
		sqlite_convert_file_print( ("*********** door_side_eng is error  ***********\n") );
	}

}
