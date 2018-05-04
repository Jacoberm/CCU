/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：ccu_database_update.h
 * 文件标识：
 * 摘    要：CCU database update function declare
 * 			 CCU 数据库更新函数声明和数据结构声明
 *
 * 当前版本：V1.0.0
 * 作    者：roben
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */


#ifndef TFTP_CLIENT_APPLY_H_
#define TFTP_CLIENT_APPLY_H_

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

#include "tftp_client_fun.h"
#include "sqlite_api_fun.h"

//音频文件的整个更新路径和保护机制。
//音频文件更新的entry。
int AudiofileTftpclientApi(char *tftp_server_ip);

//通过查询最新的数据库的音频文件更新表，看是否有音频文件更新了。
int AudioFileUpdate(char *tftp_server_ip, char *save_file_name);

//int ChangToOldName(char *save_file_name, char *new_save_name);
//将tftp save file覆盖backup file，tftp save file作备份
int ProtectDbBackup(char* save_file_name,char *backup_file_name);

//配置文件标志改为激活：有音频数据需要更新
int AudioConfigModifyToActive(void);

//配置文件标志改为非激活：没有音频数据需要更新，或者音频数据已经更新完了
int AudioConfigModifyToNegative(void);

//查询是有MP3文件没有更新完，每次上电时查看，防止每次上电都更新
int AudioConfigStateSelect(void);


//数据库的更新函数entry
typedef enum
{
	start_db_event_type = '0',
	DB_ann_type, 			//数据库 版本号
	DB_Config_type,	//配置数据库版本号
	end_db_event_type
}db_event_type;

//数据库更新文件和配置文件和报复机制，整个流程。
int DBfileTftpclientApi(char *tftp_server_ip, int db_type);

//int DBFileModifyState(char *config_file_name, int refreashed_file);

//int DBFileFindSavePath(char *config_file_name, char *save_file_name, int *refreashed_file);

//tftp循环尝试接口
int TftpClientFun(char *tftp_server_ip, char *down_file_name, char *save_file_name);

//音频数据库更新
int DataDbFileUpdate(char *tftp_server_ip);
//配置数据库更新
int ConfigDbFileUpdate(char *tftp_server_ip);

#endif /* TEST_DATABASE_UPDATE_H_ */
