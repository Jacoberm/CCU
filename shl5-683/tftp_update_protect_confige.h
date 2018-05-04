/*
 * tftp_update_protect_confige.h
 *
 *  Created on: 2013-4-17
 *      Author: benty
 */

#ifndef TFTP_UPDATE_PROTECT_CONFIGE_H_
#define TFTP_UPDATE_PROTECT_CONFIGE_H_


#include "tftp_network_base.h"

//配置文件中每行最大长度
#ifndef MAX_LINE_SIZE
	#define MAX_LINE_SIZE 200
#else
	#error MAX_LINE_SIZE was defined
#endif

//配置文件中寻找标志的字符串
#ifndef FIND_STATE_STRING
	#define FIND_STATE_STRING "(mti)"
#else
	#error FIND_STATE_STRING was defined
#endif

//现在没有使用
#ifndef FIND_END_STRING
	#define FIND_END_STRING "#"
#else
	#error FIND_END_STRING was defined
#endif


//文件名和路径的总和的最大长度。
#ifndef MAX_FILE_NAME_SIZE
	#define MAX_FILE_NAME_SIZE 100
#else
	#error MAX_FILE_NAME_SIZE was defined
#endif


//#ifndef ADD_NEW_FILE_NAME
//	#define ADD_NEW_FILE_NAME "new"
//#else
//	#error ADD_NEW_FILE_NAME was defined
//#endif

//音频数据库
#ifndef SAVE_TFTP_ANN_DB_FILE
	#define SAVE_TFTP_ANN_DB_FILE  "/nand/dbfile/pa_ann_sqlit3.db"
#else
	#error SAVE_TFTP_ANN_DB_FILE was defined
#endif

#ifndef DOWN_TFTP_ANN_DB_FILE
	#define DOWN_TFTP_ANN_DB_FILE SAVE_TFTP_ANN_DB_FILE
#else
	#error DOWN_TFTP_ANN_DB_FILE was defined
#endif

//音频备份数据库
#ifndef BACKUP_ANN_DB_FILE
	#define BACKUP_ANN_DB_FILE  "/nand/dbfile/pa_ann_sqlit3_cp.db"
#else
	#error BACKUP_ANN_DB_FILE was defined
#endif

//roben_ye,数据库crc文件存储位置(2014-07-31)
#ifndef CRC_ANN_BACK_FILE
	#define CRC_ANN_BACK_FILE  "/nand/dbfile/pa_ann_sqlit3_cp_crc.txt"
#else
	#error CRC_ANN_BACK_FILE was defined
#endif

//roben_ye,数据库crc文件存储位置(2014-10-29)
#ifndef CRC_ANN_TFTP_FILE
	#define CRC_ANN_TFTP_FILE  "/nand/dbfile/pa_ann_sqlit3_crc.txt"
#else
	#error CRC_ANN_TFTP_FILE was defined
#endif

//roben_ye,数据库crc文件存储位置(2014-07-31)
#ifndef CRC_CFG_BACK_FILE
	#define CRC_CFG_BACK_FILE  "/nand/dbfile/pa_config_sqlit3_cp_crc.txt"
#else
	#error CRC_CFG_BACK_FILE was defined
#endif


//roben_ye,数据库crc文件存储位置(2014-10-29)
#ifndef CRC_CFG_TFTP_FILE
	#define CRC_CFG_TFTP_FILE  "/nand/dbfile/pa_config_sqlit3_crc.txt"
#else
	#error CRC_CFG_TFTP_FILE was defined
#endif

//#define CRC_BUFFRE_LEN 32*1024

//配置数据库
#ifndef SAVE_TFTP_CONFIG_DB_FILE
	#define SAVE_TFTP_CONFIG_DB_FILE "/nand/dbfile/pa_config_sqlit3.db"
#else
	#error SAVE_TFTP_CONFIG_DB_FILE was defined
#endif

#ifndef DOWN_TFTP_CONFIG_DB_FILE
	#define DOWN_TFTP_CONFIG_DB_FILE SAVE_TFTP_CONFIG_DB_FILE
#else
	#error DOWN_TFTP_CONFIG_DB_FILE was defined
#endif

//配置备份数据库
#ifndef BACKUP_CONFIG_DB_FILE
	#define BACKUP_CONFIG_DB_FILE "/nand/dbfile/pa_config_sqlit3_cp.db"
#else
	#error BACKUP_CONFIG_DB_FILE was defined
#endif

//配置文件
#ifndef AUDIO_CONFIGURE_FILE
	#define AUDIO_CONFIGURE_FILE "/nand/dbfile/audio_update_config.txt"
#else
	#error AUDIO_CONFIGURE_FILE was defined
#endif


typedef enum
{
	start_modify_file = 0,
	file_one = 1, //文件1
	file_two, 		//文件2
	end_modify_file
}modify_file;

//roben_ye,两个文件的状态为其中的一个，且不能相同。
typedef enum
{
	start_modify_state = '0',//error mode
	active = '1', 		//激活状态，表示有音频文件需要更新
	freashing,			//正在跟新数据库。现在没有使用
	negative,			//非激活状态，无音频文件更新
	end_modify_state	// error mode
}modify_state;

//将数据写向硬件中的存储位子.
int WriteToFileToDisk(FILE *stream , char *ptoline );
//具体的读,写动作.
int ModifyFileActionEntry(FILE *stream, int want_chang_file, char want_chang_mode);
//主要还是在做初始化的工作.修改配置文件标志状态
int ConfigFileModify(char *config_file_path, int want_chang_file, int want_chang_mode);
//读取想要读取行的状态内容.
int ReadConfigFileState(char *config_file_path, int want_read_file);
//查询文件状态信息。
int FindFileState(FILE *stream, int want_read_file, char *find_string);
//读取需要修改行的信息。以行为单位,
int GetFileLine(FILE *stream, void *ptoline, int get_len, int flage);
//移动文件指针,主要用于动作的文件指针与前面的动作的文件指针不相同.
//特殊的一行为单位,并且每行字数相同.
int FseekFileFP(FILE *stream, char *ptoline, int flage);

#endif /* TFTP_UPDATE_PROTECT_CONFIGE_H_ */
