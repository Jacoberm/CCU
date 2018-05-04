/*
 * sqlite_api_fun.h
 *
 *  Created on: 2013-5-21
 *      Author: benty
 */

#ifndef SQLITE_API_FUN_H_
#define SQLITE_API_FUN_H_

#include "sqlite_middle_layer.h"
#include "sqlite_convert_fill_fun.h"

//#define CONFIG_DISPLAY_TABLE_CONTEXT ///< ach add, 2013

#define NullVersionValue "0.00.000"	//对应数据库为空的值。

#ifndef MIDDLE_NEW_FILE_NAME
	#define MIDDLE_NEW_FILE_NAME "update"
#else
	#error MIDDLE_NEW_FILE_NAME was defined
#endif

//读取音频数据库版本号
const char *ReadAnnVersion(void);
//读取配置数据库版本号
const char *ReadCfgVersion(void);
//设置音频数据库版本号
int SetAnnVersion(const char *ann_version_string);
//设置配置数据库版本号
int SetCfgVersion(const char *cfg_version_string);

int AllDataDBState(void); //检测音频数据库是否 可以查询。
//返回0，数据库正常运行，可以查询数据。
//返回非0，数据库运行有误，不可以查询数据。

int AllConfigDBState(void); //检测配置数据库是否 可以查询。
//返回0，数据库正常运行，可以查询数据。
//返回非0，数据库运行有误，不可以查询数据。



//extern char update_flage;
char QueryUpdateFlage(void );//查询更新标志
void SetUpdateFlage(char set_value );


//extern char tftp_restart_flag;
char QueryTftpRestartFlage(void );//查询设备是否更新完成需要重启
void SetTftpRestartFlage(char set_value );


int SqlTestMain(void);//roben测试的函数
int SqliteOpen(void);//打开音频、配置数据库。

int AnnDbopen(char *save_file_name);//打开音频数据库的接口。参数为数据库文件路径
int AnnDbClose(void);//关闭音频数据库的接口

int StationNoFill(ann_station_report_t *pToResult,int nrow);//保证查询出来的站序号是有序的
int StationNoFill2(sqlite3_stmt *stmt, int nrow_num, int clom_num, station_code_of_line_t *p_data_place);//StationNoFill中模块函数

//查找默认线路号
int JudgeDefaultLine(void *p_to_table, int row);

//查找默认区间
int JudgeDefaultSecion(void *p_data, int row );


//说明：判断传入的线路号和区间 是不是环线。
//参数：line、section，要判断的线路区间
//		p_data，函数LineSectionOfEntry的查询结果
//		row，	函数LineSectionOfEntry的返回的行数
//结果:  0，该线路区间不是环线，
//		1，该线路区间是环线
//	    -1，错误返回
int JudgeSectionIsLoop(int line, int section, void *p_data, int row);

//站代码表查询入口
int StationCodeOfEntry(int line, int *row, int *column,void **p_data);

//线路号表查询入口
int LineSectionOfEntry(int line, int *row, int *column,void **p_data);

//拼接音频表（报站表）表查询入口
int AnnReportOfEntry(int line, int section, int *row, int *column,void **p_data);

//紧急广播表查询入口
int EmergencyOfEntry(int line, int *row, int *column,void **p_data);

//模拟设置信息获取
int SimulateVariableOfEntry(int *row, int *column,void **p_data);

//数据库关闭接口（音频、配置）
int SqliteClose(void);

//数据库打开接口（音频、配置）
int SqliteOpen(void);

//具体的配置数据库打开接口
int OpenConfigDB(void);

//具体的音频数据库打开接口
int OpenDataDB(void);

//模糊数据（共用的拼接部分，如开门侧）解析接口
int ConnectInit(void);


//主备数据库版本比较接口。如果读不出版本号，则认为数据库损坏
int CompareDBVersion(int db_type, sqlite3 **database_test, sqlite3 *db_save,
				sqlite3 *db_backup, char *file_path_save, char *file_path_backup);





/*
typedef struct
{
	unsigned char begin_volume;
	unsigned char end_volume;
	float gradient;
}noise_check_t;
void* TNoiseCheckMiddleLayer(void **p_result,int *row, int *column);
 */
 //噪检信息表的查询接口
int SqlOfTNoiseCheck(int *row, int *column,void **p_data);


/*
typedef struct
{
	unsigned char add_del;	//【1】，删除文件，【0】，添加或者更新文件
	char audio_path[MAX_STRING_LEN];
}refresh_audio_path_t;
void* TRefreshAudioPathMiddleLayer(void **p_result,int *row, int *column);
*/
//音频更新文件路径统计表的查询接口
int SqlOfTRefreshAudioPath(int *row, int *column,void **p_data);


/*
typedef struct
{
	unsigned char priority_id;
	char event_type[MAX_STRING_LEN];
}priority_id_t;
void* TPriorityMiddleLayer(void **p_result,int *row, int *column);
 */
 //事件优先级表的查询接口
int SqlOfTPriority(int *row, int *column,void **p_data);


/*
typedef struct
{
	char path_name[MAX_STRING_LEN];
}record_save_path_t;
void* TRecordSavePathMiddleLayer(void **p_result,int *row, int *column);
 */
//int SqlOfTRecordSavePath(int *row, int *column,void **p_data);


/*
typedef struct
{
	char path_name[MAX_STRING_LEN];
}log_path_name_t;
void* TLogSavePathMiddleLayer(void **p_result,int *row, int *column);
 */
//int SqlOfTableTLogSavePath(int *row, int *column,void **p_data);
//录音,log保存路径表的查询接口
int SqlOfTableTFilePath(int *row, int *column,void **p_data);
/*
typedef struct
{
	unsigned char device_type;
	int device_number_count;
	char device_name[MAX_STRING_LEN];
}device_number_t;
void* TDeviceNumberMiddleLayer(void **p_result,int *row, int *column);
 */
 //设备数量统计表的查询接口
int SqlOfTableTDeviceNumber(int *row, int *column,void **p_data);


/*
typedef struct
{
	int version_id;
	int current_version;
	char short_name[MAX_STRING_LEN];
	char time[MAX_STRING_LEN];
}db_version_t;
void* TDBVersionMiddleLayer(void **p_result,int *row, int *column);
 */
int SqlOfTableTDatabaseVersion(int *row, int *column,void **p_data);//音频数据库.
int SqlOfTConfigDBVersion(int *row, int *column,void **p_data);		//配置数据库.


/*
typedef struct
{
	unsigned char device_type;
	unsigned char device_no;
	unsigned char server_whether;
	char device_name[MAX_STRING_LEN];
	char ip_address[MAX_STRING_LEN];
	char mac_address[MAX_STRING_LEN];
}ip_mac_config_t;
void* TIpMacMiddleLayer(void **p_result,int *row, int *column);
 */
 //IP、MAC地址等配置数据表的查询接口
int SqlOfTableTIpMac(int *row, int *column,void **p_data);


/*
typedef struct
{
	unsigned char line_no;
	unsigned char default_line;
}line_table_t;
void* TlineMiddleLayer(void **p_result,int *row, int *column);
 */
 //线路号表的查询接口
int SqlOfTableTline(int *row, int *column,void **p_data);

//监听音源表的查询接口
int SqlOfTMonitor(int *row, int *column,void **p_data);

//PTT默认持续时间表的查询接口
int SqlOfTPTTLastTime(int *row, int *column,void **p_data);

//时间段音量调节表的查询接口
int SqlOfTVolumeControlByTime(int *row, int *column,void **p_data);

//提示音表的查询接口
int SqlOfTPromptAudio(int *row, int *column,void **p_data);

//语言与字体表的查询接口
int SqlOfTLanguageAndFont(int *row, int *column,void **p_data);
#endif /* SQLITE_API_FUN_H_ */
