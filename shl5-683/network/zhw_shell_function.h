/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：  zhw_shell_function.h
 * 文件标识：
 * 摘    要： 网络主程序头文件
 *
 * 当前版本： V1.0.0
 * 作    者： 周魏
 * 完成日期：2012-09-17
 *
 * 取代版本： v0.0.0
 * 原作者  ： 周魏
 * 完成日期：2012-08-22
 */
#ifndef ZHW_SHELL_FUNCTION_H_
#define ZHW_SHELL_FUNCTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include <limits.h>
#include <string.h>
#include <unistd.h> // sleep()
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

#include <sys/socket.h>

#include "zhw_dev_config_info.h"
#include "zhw_socket_init.h"
#include "data_struct.h"

//#include <cyg/infra/testcase.h>
//#include <cyg/hal/hal_io.h>
//#include <cyg/kernel/kapi.h>
//#include <cyg/hal/plf_io.h>

void ZhwProTS(network_shell_package_t t);

void ZhwCqcModule(network_shell_package_t t);

void NetShellReboot(network_shell_package_t t);

void DFUProc(network_shell_package_t t);
 

void ZhwBakFunc4(network_shell_package_t t);

void ZhwBakFunc5(network_shell_package_t t);

void ZhwBakFunc6(network_shell_package_t t);

void ZhwBakFunc7(network_shell_package_t t);

void ZhwBakFunc8(network_shell_package_t t);

void ZhwBakFunc9(network_shell_package_t t);


extern void NetShellReboot(network_shell_package_t t); ///<add,2016

#endif /* ZHW_SHELL_FUNCTION_H_ */
