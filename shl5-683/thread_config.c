/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：thread_config.c
 * 文件标识：
 * 摘    要： 线程配置函数源文件
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2012-8-14
 *
 * 取代版本： V0.0.0
 * 原作者  ： wilson
 * 完成日期：
 */

#include "thread_config.h"
#include <stdlib.h>

//线程属性值配置函数
//配置线程信息，优先级、调度方式、堆栈大小、继承方式等。
//入口参数：线程属性名，优先级大小，堆栈基址，堆栈大小
void ThreadAttributionConfigure(pthread_attr_t *attr,int priority,void *addr,int stacksize )
{
	int res = 0;//接受函数操作到返回值
	//设定优先级参数
	struct sched_param param;
	param.sched_priority = priority;

	//线程属性初始化
	res = pthread_attr_init(attr);
    if (res != 0)
    {
        perror("Attribute initialization failed");
        exit(EXIT_FAILURE);
    }

    //配置线程的继承性
    res = pthread_attr_setinheritsched(attr,PTHREAD_EXPLICIT_SCHED);
    if (res != 0)
    {
        perror("Setting inherisched  failed");
        exit(EXIT_FAILURE);
    }

	//配置线程的调度参数，如优先级等
    res = pthread_attr_setschedparam(attr,&param);
    if (res != 0)
	{
		perror("Setting scheduling priority failed");
		exit(EXIT_FAILURE);
	}

	//配置线程的调度策略
    res = pthread_attr_setschedpolicy(attr,SCHED_RR);
    if (res != 0)
	{
		perror("Setting scheduling policy failed");
		exit(EXIT_FAILURE);
	}

	//设置堆栈基址和大小
    res = pthread_attr_setstackaddr( attr, addr);
    if (res != 0)
	{
		perror("Setting stack address failed");
		exit(EXIT_FAILURE);
	}
    res = pthread_attr_setstacksize( attr, stacksize);
    if (res != 0)
	{
		perror("Setting stack size failed");
		exit(EXIT_FAILURE);
	}
}
