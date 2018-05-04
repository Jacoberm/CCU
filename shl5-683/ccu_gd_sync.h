/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_gd_sync.h
 * File Identification: 
 * Abstract:  
 *
 * Current Version:	V2013_0415_0.0.1
 * Author:	AlvinChang
 * Completion Date:	2013-04-16
 *
 * Last Version:
 * Last Author:
 * Finishing Date:
 */
#ifndef _ACH_GD_SYNC_H_
#define _ACH_GD_SYNC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <cyg/kernel/kapi.h> //for alarm

#include "data_struct.h"
#include "buffer_read_write_fun.h"
#include "alarm_list_fun.h"
#include "sqlite_api_fun.h"
#include "tftp_client_apply.h"
#include "thread_comm.h"
#include "ccu_function.h"
#include "ccu_state_machine.h"
#include "network/zhw_dev_config_info.h" ///< add, 0701

#define CONFIG_BCU_VOL_FILE  "/nand/common/bcu_vol"

#define CONFIG_CCU_WAIT_SYNC_RESPONSE ///< add, 0701
#define CCU_STEP_SYNC_WITH_SM

//mask for field updating
#define GD_SYNC_FIELD_DEV_NAME                                   0x01
#define GD_SYNC_FIELD_DEV_TYPE                                      0x02
#define GD_SYNC_FIELD_DEV_NO                                          0x04
#define GD_SYNC_FIELD_DEV_ANN_STATE                         0x08
#define GD_SYNC_FIELD_DEV_INTERCOMM_STATE        0x10
#define GD_SYNC_FIELD_CONFIG_DB_VERSION                0x20
#define GD_SYNC_FIELD_ANN_DB_VERSION                       0x40
#define GD_SYNC_FIELD_DEV_CURRENT_STATE                0x80
#define GD_SYNC_FIELD_SERVER_FLAG                                0x100
#define GD_SYNC_FIELD_DKEY_FLAG                                  0x200

#if 0 //self-test
/****************  struct for database, begin, 2013 ****************************/
/********************************************************************/
#define STATION_NAME_MAX_BYTES  32

//#1:  when power up, we can read all items from station table,2013
//For example, there are 32 stations, so there are  32 * 45 bytes = 1440
typedef struct StationTableItemType
{
       unsigned int station_id;
       unsigned int line_no;	   
       unsigned int station_sequeue; // 1,2,3,
       unsigned char skip_this_station; // [1]: skip it forever
       unsigned char station_name[STATION_NAME_MAX_BYTES];
}station_table_item_t;

//#2:  section talbe
typedef struct SectionTableItemType
{
       unsigned int section_id;
       unsigned int line_no;
       unsigned char line_direction; // [1]: up;  [2]: down
       unsigned char setion_no;
       unsigned int start_station_id;
       unsigned int end_station_id;	   
}section_table_item_t;

//#3:  announce talbe
typedef struct AnnServiceTableItemType
{
       unsigned int section_id;
       unsigned int current_station_id;
       unsigned int service_type;	
       unsigned int next_station_id;
       unsigned int audio_file_id;	
       unsigned int led_file_id;
       unsigned int repeat_times;
       unsigned int distance_delta;
       unsigned int distance_arrive_trigger;
       unsigned int distance_to_next_station;
       unsigned int distance_forecast_trigger;
}ann_service_table_item_t;

//#4:  emergency talbe
typedef struct EmergServiceTableItemType
{
       unsigned int emerg_ann_id;
       unsigned int line_no;
       unsigned int repeat_times;
       unsigned int audio_file_id;	
       unsigned int led_file_id;	   
}emerg_service_table_item_t;

//#5: audio file path talbe
#define AUDIO_FILE_PATH_MAX_LEN  32
typedef struct AudioFilePathTableItemType
{
       unsigned int audio_file_id;	
       char audio_file_path[AUDIO_FILE_PATH_MAX_LEN];
}audio_file_path_table_item_t;

//#6:  display chars talbe for LED/LCD
#define LED_DISPLAY_MAX_BYTES  256   // 128 chinese if GB2312
typedef struct LedCharTableItemType
{
       unsigned int led_char_id;
       char led_char[LED_DISPLAY_MAX_BYTES];
}led_char_table_item_t;

//following struct is for EAMP
typedef struct AnnServiceTableMemoryType
{
       unsigned int current_station_id;
       unsigned int service_type;	
       unsigned int next_station_id;
       char audio_file_path[AUDIO_FILE_PATH_MAX_LEN];
       char led_char[LED_DISPLAY_MAX_BYTES];
       unsigned int repeat_times;
       unsigned int distance_delta;
       unsigned int distance_arrive_trigger;
       unsigned int distance_to_next_station;
       unsigned int distance_forecast_trigger;
}ann_service_table_memory_t;
#define  TOTAL_OF_ALL_STATIONS  32
//ann_service_table_memory_t  eamp_current_ann_table[TOTAL_OF_ALL_STATIONS]; //[320]x32 = 10KiB for one section line

/********************************************************************/
/****************  struct for database, end, 2013 ****************************/
#endif

typedef struct DevConfigTableItemType
{
       unsigned int dev_type; ///< 设备类型码
       unsigned int dev_no; ///< 设备号
       unsigned int db_server_dev_no; ///< 服务器设备号
       unsigned int config_db_version; ///< config版本号,目前未用该类型
       unsigned int ann_db_version; ///< ann版本号,目前未用该类型
       char config_db_version_string[CONFIG_CCU_VERSION_STRING_LEN]; ///< config版本字符串,目前在用
       char ann_db_version_string[CONFIG_CCU_VERSION_STRING_LEN]; ///< ann版本字符串,目前在用
       char dev_name[DEVICE_NAME_LENGTH]; ///< 本地设备名称
       char db_server_dev_name[DEVICE_NAME_LENGTH]; ///< 服务器设备名称
       char db_server_ip[IP_NAME_LENGTH]; ///< 服务器IP
       char dev_ip[IP_NAME_LENGTH]; ///< 本地设备IP
       unsigned char is_db_server; ///< 本地设备是不是服务器
       unsigned char is_main_ccu; ///< 本地设备是不是主CCU(用于自动报站),目前未用
       unsigned char driver_key; ///< 是不是有司机钥匙插入,目前未用
       unsigned char server_flag_from_iptable; ///< 服务器标识: <1>表示主服务器; <2>表示次服务器
} dev_config_table_item_t;

///< dev_ann_table_item_t暂时未使用
typedef struct DevAnnTableItemType
{
       unsigned int ann_db_version;	   
       unsigned int line_no;
       unsigned int start_station;	
       unsigned int end_station;
       //ann_station_info_t *plist_head;
} dev_ann_table_item_t;

///< dev_info_table_update_t暂时未使用
typedef struct DevInfoTableUpdateType
{
       unsigned int dev_type;
       unsigned int dev_no;
       unsigned int config_db_version;
       unsigned int ann_db_version;	   
       unsigned int gd_mask;	   
       char dev_name[DEVICE_NAME_LENGTH];	   
       unsigned char is_db_server;
       unsigned char driver_key;	   
       unsigned char dev_stat;
       unsigned char stat_context[GD_DEV_INFO_MAX_LEN];
} dev_info_table_update_t;

///< db_server_update_t暂时未使用
typedef struct
{
     unsigned int server_dev_no;
     char server_dev_name[DEVICE_NAME_LENGTH];
     char server_dev_ip[IP_NAME_LENGTH];	  
}db_server_update_t;

typedef struct {
        int next_phae; ///< 下一个表格类型
        int curr_row; ///< 当前第几行(从1开始计)
        int  row; ///< 总的行数
        int column; ///< 总的列数
        int repeat_times; ///< 超时次数
        void *p_table; ///< 指向读取的表格数据(指向的是整个表格数据)
        unsigned char read_db_once; ///< <0>表示未同类型读一个表; <1>表示已读同类型第一个表; <2>表示已发同类型第一个表
        unsigned char read_db2_once; ///< <0>表示未读同类型第二个表; <1>表示已读同类型第二个表; <2>表示已发同类型第二个表
        void *door_side_array; ///< med,1229
}db_table_row_sync_opt_t;

//global data sync struct, begin
#define GD_SYNC_STATE_IDLE  0x100
#define GD_SYNC_STATE_CCU_DB_SYNC  0x101
#define GD_SYNC_STATE_BEP_DB_SYNC  0x102
#define GD_SYNC_STATE_CCU_DEV_SYNC  0x103 //dev from bcu ---> ccu

#define GD_SYNC_STATE_TRIGGER_IDLE  0x200
#define GD_SYNC_STATE_TRIGGER_PU_READY 0x201
#define GD_SYNC_STATE_TRIGGER_CCU_DB_SYNC  0x202
#define GD_SYNC_STATE_TRIGGER_BEP_DB_SYNC  0x202
#define GD_SYNC_STATE_TRIGGER_CCU_DEV_SYNC  0x203

typedef struct
{
	void (*enter)(void *); ///< 指向当前状态的enter接口
	void (*process)(void *); ///< 指向当前状态的process接口
	void (*exit)(void *); ///< 指向当前状态的exit接口
	int state_id; ///< 当前状态ID
}gd_state_process_t;

typedef struct
{
	gd_state_process_t *psrc_gd_state; ///< 源状态结构体(内含多个函数接口)
	int trigger_event; ///< 触发事件类型
	gd_state_process_t *pdest_gd_state; ///< 目标状态结构体(内含多个函数接口)
}gd_state_rule_table_t;

typedef struct
{
     dev_config_table_item_t   *plocal_dev_config_info; ///< 保存本地设备信息,详见dev_config_table_item_t
     gd_state_process_t  *plocal_curr_gd_state; ///< 保存当前状态结构,详见gd_state_process_t
     network_send_package_t  *precv_net_cmd; ///< 保存发送命令包
     common_big_package_t  *precv_common_net_cmd; ///< 保存接收命令包
     db_table_row_sync_opt_t *ptable_row_opt; ///< 保存表格数据行信息
     unsigned int send_net_cmd_buffer_id; ///< 发送命令缓冲区ID
     unsigned int recv_net_cmd_buffer_id; ///< 接收命令缓冲区ID
     unsigned int soft_count; ///< 软计数器
     unsigned int alarm_sync_id; ///< 本地定时器ID
     unsigned int default_line_no; ///< 默认线路号
     unsigned int default_section_no;	///< 默认区间号
     unsigned char enable_sync_bcu; ///< 是否正在更新BCU,<0>表示未更新; <1>表示正在更新
     unsigned char bcu[16]; ///< 记录BCU请求次数,用来统计对哪个BCU发送表格
     unsigned char other_ccu_offline; ///< for server-CCU offline    
}gd_state_param_t;


///< gd_dev_state_count_t暂时未用
typedef struct
{
      unsigned char bcu_state_count[CONFIG_BCU_NUM];
      unsigned char ccu_state_count[CONFIG_CCU_NUM];
      unsigned char eamp_state_count[CONFIG_EAMP_NUM];
      unsigned char pcu_state_count[CONFIG_PECU_NUM];	  
}gd_dev_state_count_t;

#ifdef CONFIG_BCU_SYNC_FROM_CCU_NET
typedef struct {
     int line_no;
     int total_row;
     int curr_row;
     void *p_table;
     gd_state_param_t  *gd_state_param;
} net_sync_bcu_param_t;

typedef struct {
     int net_cmd_tx_buffer;
     int sync_mask;
     void *sync_row;
     int sync_type_size;
     gd_state_param_t  *gd_state_param;
} net_setup_package_t;
#define MAX_DOOR_SIDE_ARRAY_ITEMS   ( (GD_DEV_INFO_MAX_LEN-sizeof(line_station_door_side_t)-4) / (sizeof(line_station_door_side_array_t)))
#define MAX_DOOR_SIDE_ARRAY_BYTES  ( MAX_DOOR_SIDE_ARRAY_ITEMS * (sizeof(line_station_door_side_array_t)) )
#endif
//global data sync, end

typedef struct GbDevTypeAndName
{
      int device_type;
      char device_name[DEVICE_NAME_LENGTH];
}dev_type_name_t;


static gd_state_process_t  ccu_gd_sync_idle;
static gd_state_process_t  ccu_gd_sync_ccu_db;
static gd_state_process_t  ccu_gd_sync_bep_db;
static gd_state_process_t  ccu_gd_sync_ccu_dev;
static gd_state_rule_table_t  ccu_gd_rule_table[];
static unsigned int ccu_gd_rule_table_items;

global_device_info_state_t  *CcuGdGetLocalGdTable(void);
int CcuGdSyncStateSwitch(gd_state_process_t **gd_src_state_proc, int trigger_signal, void *sync_info);

extern int zhw_dev_weather_server(int); ///< add, 1226

extern struct global_info ReadGlobalArray(int device_type,char *device_name_or_ip,int device_number,int resolve_type);
extern struct global_info ResolveUseServerNumber(int a);

#endif /* _CCU_GD_SYNC_H_ */
