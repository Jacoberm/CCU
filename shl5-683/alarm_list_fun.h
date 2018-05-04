/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ecos_alarm_list_fun.h
 * File Identification: 
 * Abstract:  
 *
 * Current Version:	V2013_0415_0.0.1
 * Author:	AlvinChang
 * Completion Date:	2013-06-28
 *
 * Last Version:
 * Last Author:
 * Finishing Date:
 */

#ifndef ECOS_ALARM_LIST_FUN_H_
#define ECOS_ALARM_LIST_FUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cyg/kernel/kapi.h> //for eCos alarm
//#include "data_struct.h"

//#define CONFIG_ALARM_LIST_DEBUG
#ifdef CONFIG_ALARM_LIST_DEBUG
#define debug_alarm(x)  printf x
#else
#define debug_alarm(x)
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

typedef void (*alarm_handle_func_t)(cyg_handle_t, cyg_addrword_t);

#define ALARM_NAME_LEN    64

typedef struct AlarmParamType
{
     cyg_handle_t   counter_handle; ///< counter attached to alarm
     cyg_handle_t   alarm_handle;   ///< alarm handle
     cyg_alarm       alarm_object;     ///<
     alarm_handle_func_t  alarm_handle_function; ///< 
     unsigned int abs_trigger_time; ///<
     unsigned int interval; ///<  
     char timeout;  ///< [0]: alarm running;  [1]: alarm time-out;  [-1]: alarm stop 
}alarm_param_type_t;

struct AlarmListNodeDefinition
{
      char alarm_name[ALARM_NAME_LEN];
      alarm_param_type_t  *alarm_param;
      struct AlarmListNodeDefinition *next;
      unsigned char used;
};
typedef struct AlarmListNodeDefinition alarm_list_node_t;

 
/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmIdIsValid( alarm_param_type_t *alarm );

/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmCreate(char *alarm_name, unsigned int abs_trigger_time, unsigned int interval, alarm_handle_func_t cb_func );

/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmEnable(int alarm_id);

/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmEnableWithNewTrigger(int alarm_id, unsigned int new_trigger_time, unsigned int new_interval );

/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmDisable(int alarm_id);

/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmCheckTimeOut(int alarm_id);

/*********************************************************
 ** @brief 
 ** @param  
 ** @return  0
 ** @return  1
 ** @see 
 ** @note                  
 ********************************************************/
int AlarmTimeOutProcess(void *data);


#endif /* ECOS_ALARM_LIST_FUN_H_ */
