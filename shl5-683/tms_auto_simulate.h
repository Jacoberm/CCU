/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： tms_auto_simulate.h
 * 文件标识：
 * 摘    要：The head file of current bcu to simulate auto broadcast
 *
 * 当前版本： V1.1.0
 * 作    者： wilson
 * 完成日期：2013-01-09
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef TMS_AUTO_SIMULATE_H_
#define TMS_AUTO_SIMULATE_H_

#include <stdio.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/plf_io.h>
#include "data_struct.h"
#include "bcu_config.h"
#include "state_machine.h"
#include "bcu_function.h"

extern int temp_start_stop_station[5][2];

/*This timer is used to send heart beat information to keep all decive connect*/
extern cyg_handle_t  counter_handle_auto_simulate;//计数器句柄
extern cyg_handle_t alarm_handle_auto_simulate;//告警器句柄extern
extern cyg_alarm alarm_object_auto_simulate;//告警器

/** CreateAutoSimulateTimer function description
 *  It belongs to BCU
 *	  Function:Create the auto_simulate timer
 *
 *  @param   null
 *  @return  if create successfully return 0
 */
int CreateAutoSimulateTimer();

/** DeleteAutoSimulateTimer function description
 *  It belongs to BCU
 *	  Function:Delete the auto simulate timer
 *
 *  @param   null
 *  @return  if delete successfully return 0
 */
int DeleteAutoSimulateTimer();

/** alarm_func function description
 *  It belongs to BCU
 *	  Function:auto simulate timer function
 *
 *  @param   timer handle
 *  @param   private data
 *  @return  null
 */
void alarm_func_handle_auto_simulate(cyg_handle_t counter_handle, cyg_addrword_t data);

/** SetHeartBetaTimeInterval function description
 *  It belongs to BCU
 *	  Function:Set the auto simulate timer time interval
 *
 *  @param   time interval
 *  @return  if set successfully return 0
 */
int SetAutoSimulateTimeInterval(int time_interval);

/** OpenHeartBetaTimer function description
 *  It belongs to BCU
 *	  Function:Open the auto simulate timer
 *
 *  @param   null
 *  @return  if open successfully return 0
 */
int OpenAutoSimulateTimer();

/** CloseHeartBetaTimer function description
 *  It belongs to BCU
 *	  Function:Close the auto simulate timer
 *
 *  @param   null
 *  @return  if close successfully return 0
 */
int CloseAutoSimulateTimer();

/** StartAutoSimulateTimer function description
 *  It belongs to BCU
 *	  Function:Start the auto simulate timer
 *
 *  @param   null
 *  @return  if close successfully return 0
 */
void StartAutoSimulateTimer();

/** BcuUpdateAutoAnnouncement function description
 *	Auto change line no
 *
 *  @param   null
 *  @return  null
 */
void BcuUpdateAutoAnnouncement();


#endif /* TMS_AUTO_SIMULATE_H_ */

