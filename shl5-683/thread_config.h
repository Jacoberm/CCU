/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：thread_config.h
 * 文件标识：
 * 摘    要： 线程配置函数头文件
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2012-8-11
 *
 * 取代版本： V0.0.0
 * 原作者  ： wilson
 * 完成日期：
 */

#ifndef THREAD_CONFIG_H_
#define THREAD_CONFIG_H_

#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <cyg/infra/testcase.h>


#define CCU_PRIORIT  (10)
#define CCU_APP_THREAD_MAX_PRIOR   (CCU_PRIORIT+1)

//线程属性值配置函数
//配置线程信息，优先级、调度方式、堆栈大小、继承方式等。
//入口参数：线程名，优先级大小，堆栈基址，堆栈大小
/** ThreadAttributionConfigure函数说明
 *  本函数主要操作是设置线程属性值
 *
 *  @param   线程属性
 *  @param   线程优先级
 *  @param   线程堆栈地址
 *  @param   线程堆栈大小
 *  @return  null
 */
void ThreadAttributionConfigure(pthread_attr_t *attr,int priority,void *addr,int stacksize);

#endif /*THREAD_HANDLE_H_*/
