/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_dev_config_info.h
 * 文件标识：
 * 摘    要： 设备配置信息主函数头文件
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2013-03-26
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */

#ifndef ZHW_DEV_CONFIG_INFO_H_
#define ZHW_DEV_CONFIG_INFO_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>

//#include "sqlite_middle_layer.h"
//#include "sqlite_api_fun.h"

#include "zhw_read_from_db_test.h"
#include "network_include_extern_head.h"

//#ifndef SQL_TEST
//#define SQL_TEST 1
//#endif

#ifndef LOW_TEST
#define LOW_TEST 1
#endif

struct dev_type_and_no
{
	int dev_type;
	int dev_number;
};


struct global_info
{
	char *dev_name;
	int dev_type;
	int dev_number;
	char *dev_ip;
	char dev_mac[6];
	int is_server;
};

struct global_array_info
{
	struct global_info global_config_info;
	struct global_array_info *next;
};
typedef struct global_array_info global_array_t;

extern int eamp_number;
extern int bcu_number;
extern int ccu_number;
extern int pcu_number;



/*
 *   	@brief	本函数的功能是通过调用底层I/O口驱动函数获得设备类型
 *   			和设备号；
 *     @param void 无传入参数
 *     @return   dev_type_and_no  本接口对应的设备类型和设备号
 */
struct dev_type_and_no ReadFromIO();

/*
 *   	@brief	本函数的功能是读取数据库中设备信息并对全局数组进行初始化；
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @return   device_ip  相应设备的IP地址
 */
void InitGlobalArray();

/*
 *   	@brief	本函数是用于IP解析的主函数，功能是根据所给的设备类型或者设备名（只需给出其中的一个即可，另一个可用0或NULL表示）
 *   	和设备号解析出设备的IP地址，用于向相应设备发送数据和解析本地设备；
 *  	@param device_type 从IO接口读取出的本地设备类型，从驱动中直接读取过来的是整数；
 *  			111（整数7）：表示EAMP；
 *  			110（整数6）：表示BCU；
 *  			100（整数4）：表示PCU；
 *  			   ：表示CCU（未定）
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @param resolve_type 表示给出的用于解析的是设备名还是设备类型；
 *     				1：表示本地设备(表示给出的是设备类型)；
 *     				2：表示全局设备（表示给出的是设备名）；
 *     @return   device_ip  相应设备的IP地址
 */
struct global_info ReadGlobalArray(int ,char *,int ,int );

/*
 *   	@brief	本函数的功能是根据所给的设备类型和设备号责解析出本地设备的IP地址；
 *     @param device_type 需要解析的设备的接口类型
 *     @param device_number 	需要解析的设备号
 *     @return  local_device_ip  本地设备的IP地址
 *     				-1:表示传入参数有问题，表中查询不到所要设备信息。
 */
struct global_info ResolveUseType(int device_type,int device_number);

/*
 *   	@brief	本函数的功能是根据所给的设备名和设备号责解析出全局设备的IP地址；
 *     @param device_name 需要解析的设备名
 *     @param device_number 	需要解析的设备号
 *     @return  global_device_ip  所需查询的全局设备的IP地址
 *     				-1:表示传入参数有问题，表中查询不到所要设备信息。
 */
struct global_info ResolveUseName(char *device_name,int device_number);

/*
 *   	@brief	本函数的功能是根据所给的设备IP解析出设备信息
 *     @param device_ip 需要解析的设备ip
 *     @return  ret_dev_info  所需查询的设备的信息。
 *     				-1:表示传入参数有问题，表中查询不到所要设备信息。
 */
struct global_info ResolveUseIp(char *device_ip);

/*
 *   	@brief	本函数的功能是根据所给的设备名称返回设备数量
 *     @param char *dev_name 需要查询的设备名
 *     @return  dev_number  所需查询的设备的数量。
 *     				-1:表示表中查询不到所要设备信息，或所得数量为0。
 */
int ZhwGetDevNumber(char *dev_name);

int ReadType(void);

int ReadNum(void);

int ZhwUpdateBCUIpArray(ip_mac_table_row_t *pip_mac_table_row);

/*
 *   	@brief	本函数的功能是根据所给服务器类型返回设备信息
 *     @param int a 需要查询的服务器：1表示主服务器，2表示备份服务器；
 *     @return  struct global_info  所查询设备的信息。
 *     				
 */
struct global_info ResolveUseServerNumber(int a); 
#endif /* ZHW_DEV_CONFIG_INFO_H_ */
