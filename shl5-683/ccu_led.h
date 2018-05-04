/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_led.h
 * 文件标识：
 * 摘    要： CCU LED头文件
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-07-23
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#ifndef CCU_LED_H_
#define CCU_LED_H_

#include <stdio.h>                      /* printf */
#include <string.h>                     /* strlen */
#include <pkgconf/io.h>
#include <pkgconf/system.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serialio.h>
#include <cyg/kernel/kapi.h>            /* All the kernel specific stuff */
#include <cyg/io/io.h>                  /* I/O functions */
#include <cyg/io/canio.h>
#include <cyg/hal/hal_arch.h>           /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
//#include "ser_test_protocol.inl"
#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
//#include <cyg/io/canio.h>
#include "data_struct.h"

typedef struct
{
	char chinese_info[30];
	char english_info[40];
}tip_info_t;

extern cyg_io_handle_t ser0_handle;
extern cyg_io_handle_t ser1_handle;
extern cyg_io_handle_t ser_led;

extern cyg_io_handle_t    hCAN_Tbl[2];
extern cyg_io_handle_t    hLoopCAN_Tbl[2];

void CCUSerialInit();

/** SerialInit function description
 *  This function is used to initialize the serial port
 *
 *  @param   serial port
 *  @param   serial device file
 *  @return  null
 */
void SerialInit(cyg_io_handle_t *param_io,char *param_dev_string,int param_baud);

/** TransmitShowCommand_wilson function description
 *  通过RS232将数据发送给FDU
 *
 *  @param   serial device address
 *  @return  null
 */
void TransmitShowCommand_wilson(char address);

/** TransmitShowCommand_wilson_can function description
 *  通过can将数据发送给FDU
 *
 *  @param   设备地址
 *  @param   can号
 *  @param   设备标识
 *  @return  null
 */
void TransmitShowCommand_wilson_can(char address,cyg_uint8 can_num ,  cyg_uint8 device_add);

/** CCUFduAnnouncement function description
 *  This function is used to
 *
 *  @param  line no
 *  @param  terminal station no
 *  @param  whether the cycle line
 *  @param  language
 *  @return  null
 */
void CCUFduAnnouncement(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, int param_color);

/** CCUFduAnnouncementOnlyUp24 function description
 *  This function is used to show led,which is located at head(use up 24 lien)
 *
 *  @param  line no
 *  @param  terminal station no
 *  @param  whether the cycle line
 *  @param  language
 *  @return  null
 */
void CCUFduAnnouncementOnlyUp16(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, int param_color,unsigned char param_language);
/** TransmitFduData_wilson function description
 *  This function is used to
 *
 *  @param  device address
 *  @param  terminal station no
 *  @param  whether the cycle line
 *  @param  language
 *  @return  null
 */
void TransmitFduData_wilson(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char up_or_down, int color);

/** TransmitClearCommand_wilson function description
 *  清除FDU目前显示内容
 *
 *  @param   设备地址
 *  @return  null
 */
void TransmitClearCommand_wilson(char address);

/** SendFDUHeartBetaCan function description
 *  通过can发送心跳包给FDU
 *
 *  @param   null
 *  @return  null
 */
void SendFDUHeartBetaCan();

/** TransmitClearCommand_wilson function description
 *  清除FDU目前显示内容（can）
 *
 *  @param   设备地址
 *  @param   can号
 *  @param   设备标识
 *
 *  @return  null
 */
void TransmitClearCommand_wilson_can(char address,cyg_uint8 can_num ,  cyg_uint8 device_add);

/** IduClear function description
 *  清除IDU显示内容
 *
 *  @param   null
 *  @return  null
 */
void IduClear();

/** FduClear function description
 *  清除FDU显示内容
 *
 *  @param   null
 *  @return  null
 */
void FduClear();

/** CCUFduAnnouncement_line4_terminal_station function description
 *  发送4号线终点站信息给FDU
 *
 *  @param  线路号
 *  @param  终点站
 *  @param  是否环线
 *  @param  上下行
 *  @param  语言
 *  @return  null
 */
void CCUFduAnnouncement_line4_terminal_station(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, char param_language);

/** CCUFduAnnouncementLine4TerminalStationUp16 function description
 *  发送4号线终点站信息给FDU（FDU总共24行，该函数只显示上面16行）
 *
 *  @param  线路号
 *  @param  终点站
 *  @param  是否环线
 *  @param  上下行
 *  @param  语言
 *  @return  null
 */
void CCUFduAnnouncementLine4TerminalStationUp16(unsigned char param_line_no, unsigned char param_terminal_no, unsigned char param_whether_the_cycle_line,unsigned char up_or_down, char param_language);

/** TransmitFduData_line4_terminal_station function description
 *  发送4号线终点站信息给FDU（can）
 *
 *  @param  设备地址
 *  @param  线路号
 *  @param  终点站
 *  @param  是否环线
 *  @param  上下行
 *  @param  语言
 *  @return  null
 */
void TransmitFduData_line4_terminal_station(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, char chinese);

/** CCUFduAnnouncementLine4TerminalStationUp16 function description
 *  发送4号线终点站信息给FDU（FDU总共24行，该函数只显示上面16行）（can）
 *
 *  @param  设备地址
 *  @param  线路号
 *  @param  终点站
 *  @param  是否环线
 *  @param  上下行
 *  @param  语言
 *  @return  null
 */
void TransmitFduDataLine4TerminalStationUp16(char address, unsigned char line_no, unsigned char terminal_no,unsigned char whether_cycle_line,char param_up_or_down, char chinese);

/** sendCanData function description
 *  发送数据
 *
 *  @param  需要发送的数据
 *  @param  需要发送数据的长度
 *  @param  设备ID
 *  @param  哪一路can
 *  @return  null
 */
void sendCanData(cyg_uint8 *program_data, cyg_uint32 count, cyg_uint16	std_id , cyg_uint8 can_num);

/** alarm_func_can function description
 *  FDU显示定时器处理函数
 *
 *  @param   定时器句柄
 *  @param   定时器私有数据
 *  @return  null
 */
void alarm_func_can(cyg_handle_t alarm_handle, cyg_addrword_t data);

/** CreatCANTimer function description
 *  创建FDU显示定时器
 *
 *  @param   null
 *  @return  null
 */
void CreatCANTimer();
#endif /* EAMP_LED_H_ */
