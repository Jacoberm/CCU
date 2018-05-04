/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_main.h
 * 文件标识：
 * 摘    要： 广播控制盒主函数
 *
 * 当前版本： V1.0.0
 * 作    者： Alvin
 * 完成日期：2013-04-20
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */

#ifndef CCU_MAIN_H_
#define CCU_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/io/io.h>
#include <cyg/io/serialio.h>
#include <cyg/io/devtab.h>
#include <cyg/io/canio.h>
#include <cyg/io/spi.h>
//#include <cyg/io/spi_stm32.h>
#include <cyg/io/audio/audio.h>
#include <cyg/io/audio/audio_dev.h>

#include <cyg/kernel/kapi.h>       /* All the kernel specific stuff */
#include <cyg/kernel/kapi.h>

#include <cyg/hal/hal_arch.h>      /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/hal/hal_io.h>
#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>

#include <pkgconf/hal.h>
#include <pkgconf/io_audio.h>
#include <pkgconf/io.h>
#include <pkgconf/system.h>
//#include <pkgconf/devs_spi_cortexm_stm32.h>
#include <pkgconf/devs_snd_vs1063a.h>

#include "thread_config.h"
#include "data_struct.h"
#include "buffer_read_write_fun.h"
#include "udp_type.h"
#include "thread_comm.h"
#include "tms_module.h" ///<add, 2013
#include "ccu_function.h"
#include "ccu_ann_fun.h"

#include "media_and_monitor.h"
#include "tftp_server_apply.h"
#include "sqlite_api_fun.h"

#include "manual_ann.h"

#ifndef CCU_DEV
#define CCU_DEV
#endif


/** CcuNetworkThreadEntry  function description
 *  The entry of CCU network thread
 *
 *  @param   void *arg
 *  @return  null
 */
void *CcuNetworkThreadEntry(void *arg);

extern void *CcuSyncThreadEntry(void *arg);
extern int CcuGdIpTableIfHasRead(void);
extern int CcuGdReadServerIp(char *out_ip, int out_len);
extern bool ccu_read_occ_status(void);
extern int CcuPriorListInit(void);

extern int zhw_dev_weather_server(int a); ///< add, 1226
extern int GetCurrentDeviceNetworkState();
extern int GetServerCcuNetworkState(void);
extern int ReadNum(void);
extern int ReadNumWilson(void);

#endif /* CCU_MAIN_H_ */
