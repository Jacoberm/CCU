/*
 * sqlite_select.h
 *
 *  Created on: 2013-4-30
 *      Author: benty
 */

#ifndef SQLITE_SELECT_H_
#define SQLITE_SELECT_H_

#include "sqlite3.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <limits.h>
#include <unistd.h> // sleep()
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "tftp_update_protect_confige.h"

//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
//任何结构体变了，查询部分的代码都要改变。
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

extern ip_mac_old;

//数据表所存放的数据库的位置。
extern sqlite3 *database_db;//音频数据库打开的handle
extern sqlite3 *config_db;//配置数据库打开的handle

#ifndef MAX_STRING_LEN
	#define MAX_STRING_LEN 100
#else
	#error MAX_STRING_LEN defined
#endif


#define Roben_Print //diag_printf
//#define Roben_Print //

//#define SQLITE_PRINT
#ifndef SQLITE_PRINT
   #define sqlite_print(s) diag_printf s
#else
   #define sqlite_print(s) //diag_printf s
#endif


typedef enum
{
	start_select_event = '0',
	ip_mac_config = '1',//设备配置表
	DB_version, 			//数据库 版本号
	device_count,			//设备数量

	//合并为路径表
//	log_path,				//log 日志保存的文件路径
//	record_path,			//录音文件保存路径
	file_path,				//路径表
	prompt_audio,			//提示音表
	monitor,				//监听音源表
	PTT_last_time,			//PTT默认持续时间
	volume_control_Bytime,	//时间段音量调节
	language_and_font,		//语言与字体

	priority, 				//优先级表
	audio_refreesh_path,	//记录所有音频文件跟新的路径的表
	config_DB_version,	//配置数据库版本号
	noise_check,			//噪音检测数据信息
	line_ID,					//查询所有线路。
	TSimulationTimeInterval ,//模拟设置信息表
	end_select_event
}db_select_event;

typedef enum
{
	pre_station = 0,//预报站
	report_station = 1,//报站
	end_report_pre
}report_pre_event;


//紧急广播查询结构体。
typedef struct
{
       unsigned int emerg_ann_id;		//紧急广播号
       unsigned int line_no;				//线路号。“0”表示该条紧急广播是所有线路共用的
       	   	   	   	   	   	   	   	   	    // 非0，表示该条线路号所特有的紧急广播
       unsigned int repeat_times;		//重复次数
       unsigned char audio_path[MAX_STRING_LEN];		//紧急广播的音频文件长度
       unsigned char display_message[MAX_STRING_LEN*4];//紧急广播的显示可能比较多//2015-4-23 modify
       unsigned char emerg_ann_name[MAX_STRING_LEN];		//紧急广播名
}emerg_table_item_t;

//线路号区间查询结构体。
typedef struct
{
       unsigned int line_no;				//线路号号
       unsigned char line_direction; // [1]: 内圈,上行;  [2]: 外圈, 下行
       unsigned char spec_cycle;		//【！0】,是特殊圈号；【0】，不是。暂时没用
       unsigned char short_name[MAX_STRING_LEN];	//描述列
       unsigned char start_time[10];				//开始时间
       unsigned char end_time[10];					//结束时间。暂时没有用到
       unsigned char default_section; //[1]: yes;  [0]:no.【1】,是当前线路的默认区间，【0】不是默认区间。
       unsigned int section_no;			//区间顺序号
       unsigned int start_station;		//起始站站代码号.
       unsigned int end_station;		//终点站站代码号。
}line_section_table_t;


//查询线路号下的站代码表，结构体。
typedef struct
{
		unsigned int station_no;				//车站顺序号
		unsigned char fix_across_station;//【0】,不越战；【1】，越战。
		unsigned char spec_open_door;//【0】,不是特殊开门测；【1】，是特殊开门侧。
		unsigned char station_name[MAX_STRING_LEN];	//车站名
		unsigned char station_audio[MAX_STRING_LEN];	//车站名对应的音频
		unsigned char station_audio_eng[MAX_STRING_LEN];	//车站名对应的英文音频
		unsigned char station_display[MAX_STRING_LEN];		//车站名对应的字幕
		unsigned char station_display_eng[MAX_STRING_LEN];  //车站名对应的英文字幕
}station_code_of_line_t;



typedef struct
{
		unsigned char audio_display[MAX_STRING_LEN];//音频文件保存路径。显示部分保存显示内容/路径。
				//对应于以前版本的“audio_text【100】”

		unsigned int file_attr;	//上面的路径、内容所对应的属性。
		 	 	 	 	 	 //如果没有添加属性，这里会被赋值为0.
				//对应于以前版本的“Attribute”

		unsigned int color_attr;	//字幕的颜色，仅仅字幕在使用。
				//新增

		unsigned char available_or_not;	//新区间中，字幕、音频是否需要使用
											//1：表示该段字幕、音频不需要播放
											//0：该段音频、字幕正常播放
				//新增
}audio_display_list;
//报站信息
typedef struct
{
		unsigned char pre_arrive;		//【0】，预报站 ,【1】，报站。如有需要，还可以继续添加。
		unsigned int station_next_no;	//下一站，station表中对应的line下面的第几站

       unsigned char repeat_times;	//重复次数
       unsigned char open_door;		//开门侧，0左边，1右边
       unsigned int before_after;	//触发距离的区间范围
       unsigned int triger_distance;	//触发距离
       unsigned int station_distance;	//到下一站距离
       unsigned int pre_triger_distance;	//预报站触发距离。0表示出站就报音频

       //现在是判断，结构体的首地址是否为空来确定是否是到尾部了。
       //       int audio_len;			//告诉上面应用层，有几个链接在一起的音频文件，
       audio_display_list *audio;	//该指针指向所有拼接的数据和属性

       //现在是判断，结构体的首地址是否为空来确定是否是到尾部了。
       //       int display_len;			//告诉上面应用层，有几个链接在一起的显示文件，
       audio_display_list *display;		//该指针指向所有的显示内容和属性

}ann_station_information_t;

typedef struct
{
	unsigned int station_no;					//站代码号
	ann_station_information_t station_type[2];//【0】,预报站；【1】，报站。
}ann_station_report_t;								//音频拼接表（报站表）总的结构体


//返回查询符合条件的行数
int TableRowSelect(sqlite3 *db, char *sql, int line_num);

//将查询数据转换为指定的类型。
int EmergDataConvert(	sqlite3_stmt *stmt, int clom_num, emerg_table_item_t *p_data_place);
//从数据库查询该线路号的所有紧急广播信息。
int EmergSelectFromDB(sqlite3 *db,int line_num,emerg_table_item_t *pToResult);
//紧急广播搜索入口参数。
void* EmergenceSelectEntry(int line, int *row, int *column);


//查询符合条件的线路号区间下报站（音频拼接）行数。
int AnnReportRowSelect(sqlite3 *db, int line_num, int section_num);
//报站表（音频拼接表）行数查询
int RowSelectReport(sqlite3 *db, int line_num, int section_num, int report_pre);
//报站表（音频拼接表）填充查询结果到申请空间。
int AnnReportDataConvert(sqlite3 *db, sqlite3_stmt *stmt, int nrow_num, int clom_num, int pre_get, ann_station_report_t *p_buf);
//查询具体的、所有的拼接字幕文件
audio_display_list * QueryDetailText(sqlite3 *db, int coarse_id);
//查询具体的、所有的拼接音频文件
audio_display_list * QueryDetailAudio(sqlite3 *db, int coarse_id);
//将申请空间的站代码号，有序填充
int ReportStationNoFill(ann_station_report_t *pToResult,int nrow);
//从数据库查询报站表（音频拼接表）的信息
int AnnReportSelectFromDB(sqlite3 *db, int nrow, int var_line,int var_section, ann_station_report_t *pToResult);
//报站预报站音频、显示查询的总入口。
void *AnnReportEntry(int line, int section_no, int *row, int *column);


//填充站码表查询结果到申请空间。
int StationDataConvert(sqlite3_stmt *stmt, int nrow_num, int clom_num, station_code_of_line_t *p_data_place);
//从数据库查询站代码的信息
int StationSelectFromDB(sqlite3 *db, int nrow, int var, station_code_of_line_t *pToResult);
//线路站代码查询的入口。
void *StationCodeEntry(int line, int *row, int *column);
//int ChangeToChineseAndEnglish(station_code_of_line_t*p_data_place, const char *PtoResult);


//填充区间表查询结果到申请空间。
//必须按照数据结构填充，所以每个查询函数的填充都不一样。
int LineSectionDataConvert(sqlite3_stmt *stmt, int clom_num, line_section_table_t *p_data_place);
//从数据库查询区间号的信息
int SectionSelectFromDB(sqlite3 *db, int var, line_section_table_t *pToResult);
//线路的区间查询的入口。
void *LineSectionSelectEntry(int line, int *row, int *column);



//释放查询结果所占用空间。为配套使用。
int SqliteAllTableFree(char **SqlResult);
//组合成为真正的sql查询语句。整个表查询才使用
char *SqlStringConnect(char *table_name);
//查询结果，并将行、列、查询结果返回。
char **SqlSelectFromDB(sqlite3 *db, char *table_name, int *row, int *column);

//SQL 查询想要的数据表。
//数据库句柄为全局变量。
char **AllTableSelectDB(int event, int *row, int *column);

//释放查询的空间
void SqlTableBufferFree(void **pv);
//申请指定长度的空间，并初始化为0
void *SqlLocalBufferMalloc( unsigned int wantedSize );
//释放空间
void SqlLocalBufferFree( void *pv );
//void AnnReportBufferFree(void *pv,int row);
//报站音频释放空间
void AnnReportBufferFree(void **pv,int row);

#endif /* SQLITE_SELECT_H_ */

