/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_read_from_db_test.h
 * 文件标识：
 * 摘    要： 全局配置表头文件
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2012-09-17
 *
 * 取代版本： v0.0.0
 * 原作者  ： 周魏
 * 完成日期：2012-08-22
 */

#ifndef ZHW_READ_FROM_DB_TEST_H_
#define ZHW_READ_FROM_DB_TEST_H_
#include "zhw_network_config.h"
#include "bcu_get_data_from_ccu_type.h"
#include <string.h>

#define TEST_COLUMN 6

extern ip_mac_table_item_t  device_info[];///<modify by wilson 2013-06-28
extern unsigned int  device_info_items;///< add, 2013

//typedef struct
//{
//	unsigned char device_type;
//	char device_name[50];
//	unsigned char device_no;
//	char ip_address[50];
//	char mac_address[6];
//	unsigned char server_whether;
//}ip_mac_config_t;

/*
 *   	@brief	填充用户的结构。
 *     @param row 行指针
 *     @param column 	列指针
 *     @param point  数据头指针
 *     @return   1：成功。
 */
int ZhwReadFromTestDB(int *row, int *column,void **point);


#endif /* ZHW_READ_FROM_DB_TEST_H_ */
