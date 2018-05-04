/*
 * sqlite_middle_layer.h
 *
 *  Created on: 2013-5-20
 *      Author: benty
 */

#ifndef SQLITE_MIDDLE_LAYER_H_
#define SQLITE_MIDDLE_LAYER_H_

#include "sqlite_select.h"

typedef struct
{
	unsigned char line_no;		//线路号
	unsigned char default_line;	//[1]: yes;  [0]:no.【1】,是当前线路的默认区间，【0】不是默认区间。
	unsigned char short_name[50];	//说明列
}line_table_t;
//“线路号”表中间层替换
void* TlineMiddleLayer(void **p_result,int *row, int *column);


typedef struct
{
	unsigned char type[50];
	unsigned char value[50];
	unsigned char explanation[100];
}simulate_variable_info_t;
void* TSimulateVariableMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	unsigned char device_name[10]; //EAMP、PECU、BCU、CCU
	unsigned char device_type_no;//<<,add,2013-12-30. 1:PCU; 5:CCU; 3:BCU; 7:EAMP
	unsigned char device_no;			//功放几，PCU几
	unsigned char ip_address[20];//“192.168.111.222”
	unsigned char mac_address[50];//mac地址
	unsigned char server_whether;//是否是服务器。0，不是服务器。1，服务器；2，此服务器
}ip_mac_config_t;
//“IP-mac”表中间层替换
void* TIpMacMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	int device_number_count;		//对应的设备数据量
	unsigned char device_name[10];//EAMP、PECU、BCU、CCU
}device_number_t;
//int SqlOfTableTDeviceNumber(int *row, int *column,void **p_data);
//“设备数量”表中间层替换
void* TDeviceNumberMiddleLayer(void **p_result,int *row, int *column);


typedef struct
{
	unsigned int version_id;//说明项
	unsigned char short_name[MAX_STRING_LEN];//说明项
	unsigned char time[12];	//说明项。"2013-11-12",精确到天
	unsigned char current_version[MAX_STRING_LEN];//具体版本号。比较这一项，来确定是否跟新
}db_version_t;
//“版本号”表中间层替换
void* TDBVersionMiddleLayer(void **p_result,int *row, int *column);



typedef struct
{
	unsigned char even_type[20];		//事件名：“log_event”、“record_event”
	unsigned char path_name[MAX_STRING_LEN];//路径名
}file_path_t;
//“路径”表中间层替换
void* TFilePathMiddleLayer(void **p_result,int *row, int *column);


//roben_ye,这个必须写一个小的查询语句
typedef struct
{
	unsigned char even_type[20];		//事件名（有的暂时没有用到）
			//LIVE、Open_Door、D2D_INTERCOMM、D2P_INTERCOMM、Arrive_Station、Pre_Station、Last_cycle
	char path_name[MAX_STRING_LEN];	//事件对应的提示音路径。不能拼接
}prompt_audio_t;
//“提示音”表中间层替换
void* TPromptAudioMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	unsigned char event_name[20];	//事件名：Media、OCC、LIVE、EMERG、MANUAL、
										//TMS-AUTO、TMS-MANUAL、D2D_INTERCOMM、D2P_INTERCOMM
	unsigned char monitor_or_not;	//1”（Yes）要监听；“0”（No）
	unsigned char record_or_not;		//1”（Yes）要录音、“0”（No）
}monitor_t;
//“监听音源”表中间层替换
void* TMonitorMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	unsigned char event_name[20];	//事件名：LIVE、D2D_INTERCOMM、D2P_INTERCOMM
	unsigned char short_name[MAX_STRING_LEN];//说明列
	unsigned int last_time;			//时间
}PTT_last_time_t;
//“PTT默认持续时间”表中间层替换
void* TPTTLastTimeMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	unsigned char start_time[20];	//起始时间
	unsigned char end_time[20];		//终止时间
	unsigned char volume;				//该时间段音量
}volume_control_Bytime_t;
//“时间段音量调节”表中间层替换
void* TVolumeControlByTimeMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	unsigned char language[20];			//语言
	unsigned char font[20];				//字体
	unsigned char active_font_or_not;//“1”（Yes）默认字体、“0”（No）
}language_and_font_t;
//“语言与字体”表中间层替换
void* TLanguageAndFontMiddleLayer(void **p_result,int *row, int *column);



typedef struct
{
	char event_type[MAX_STRING_LEN];	//事件名。OCC、LIVE、EMERG、MANUAL、TMS-AUTO、
											//TMS-MANUAL、D2D_INTERCOMM、D2P_INTERCOMM
	char short_name[MAX_STRING_LEN];	//说明列
	unsigned char priority_id;
}priority_id_t;
//“优先级”表中间层替换
void* TPriorityMiddleLayer(void **p_result,int *row, int *column);

typedef struct
{
	char audio_path[MAX_STRING_LEN];	//更新文件名
}refresh_audio_path_t;
//“自动更新（音频文件更新表）”表中间层替换
void* TRefreshAudioPathMiddleLayer(void **p_result,int *row, int *column);


typedef struct
{
	int min_volume;	//数据库的"BeginVolume" line，表示该档位的起始值。
	int max_volume;	//"EndVolume" line，表示该档位的最大值
	int added_value;	//增值，表示 档位值（名称以后会修改）
	float gradient;	//斜率，暂时不使用,以后会删除
}noise_check_t;
//“自动更新（音频文件更新表）”表中间层替换
void* TNoiseCheckMiddleLayer(void **p_result,int *row, int *column);

//int NewFileNameMake(char *old_file_name, char *new_file_name, char *find_str, char*add_string);

//将主备数据库打开。
int OpenDbfile(sqlite3 **old_db_save, sqlite3 **old_db_backup,
						char *file_path_save, char *file_path_backup);


#endif /* SQLITE_MIDDLE_LAYER_H_ */
