/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	alarm_list_fun.c
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

#include "alarm_list_fun.h"

//this is the head node of alarm list
static alarm_list_node_t   alarm_list_head = 
{
      "", ///<
      NULL,
      NULL,
      0
};

 static void *LocalMalloc( unsigned int wantedSize );
 static void LocalFree( void *pv );
 static int AlarmPointerSave(alarm_param_type_t  *new_alarm_param, char *alarm_name);
 

static void *LocalMalloc( unsigned int wantedSize )
{
    void *pv_return;

	pv_return = malloc( wantedSize );

	return pv_return;
}
static void LocalFree( void *pv )
{
	if( pv )
	{
            free( pv );
	}
}
static int AlarmPointerSave(alarm_param_type_t  *new_alarm_param, char *alarm_name)
 {
       int min_len = 0, ret = 0;
       unsigned char found = 0;
       alarm_list_node_t *curr = (alarm_list_node_t *)&alarm_list_head;
       alarm_list_node_t *ptr = curr;
       
       if ( new_alarm_param==NULL ||alarm_name==NULL )
       {
             return -1;
       }
	   
       //check if new buffer name is same to someone
       while( ptr != NULL )
       {
            if ( ptr->used==1 )
            {
                  if ( strcmp(ptr->alarm_name, alarm_name)==0 ) //alarm exist, so don't save this new alarm
                  {                         
       	                return -1;
                  }
         }
         curr = ptr; //save old
         ptr = ptr->next; //pointer to next
       }	   
       
       //now, buffer name is uniq
       curr = (alarm_list_node_t *)&alarm_list_head;
       ptr = curr;
       while( ptr != NULL )
       {
            if ( !ptr->used )
            {
                  ptr->used = 1;
	          if ( strlen(alarm_name) >= sizeof(ptr->alarm_name) )
	          {
	                 min_len = sizeof(ptr->alarm_name);
	          }
	          else
	          {
	                min_len = strlen(alarm_name)+1;
	          }
                  strncpy( ptr->alarm_name, alarm_name, min_len-1 );
                  ptr->alarm_name[min_len] = 0x00;
		  ptr->alarm_param = new_alarm_param;
		  found = 1;
                  debug_alarm(("save:  %s, %d, %x, %x\n", ptr->alarm_name, ptr->used, (unsigned int)(ptr->alarm_param), (unsigned int)(ptr->next) ));
                  break;
         }
         curr = ptr; //save old
         ptr = ptr->next; //pointer to next
       }
       
       if ( found==0 ) //all nodes are used ,so we need a new node, now, curr is the tail node
       {
             alarm_list_node_t *tmp_node = NULL;
             tmp_node = (alarm_list_node_t *) LocalMalloc( sizeof( alarm_list_node_t ) );
             if( tmp_node==NULL )
             {
                   ret = -1;
             }
            else
             {
                   //save new pointer into new node
                   memset( (void *)tmp_node, 0x00, sizeof(*tmp_node) );
                   tmp_node->used = 1;
                   if ( strlen(alarm_name) >= sizeof(tmp_node->alarm_name) )
                   {
                          min_len = sizeof(tmp_node->alarm_name);
                   }
                   else
                   {
                          min_len = strlen(alarm_name)+1;
                   }		   
                   strncpy( tmp_node->alarm_name, alarm_name, min_len-1 );
                   tmp_node->alarm_name[min_len] = 0x00;
                   tmp_node->alarm_param = new_alarm_param;		     
                   curr->next = tmp_node; //tmp_node as tail node
                   debug_alarm(("save tmp:  %s, %d, %x, %x\n", tmp_node->alarm_name, tmp_node->used, 
				   	(unsigned int)(tmp_node->alarm_param), (unsigned int)(tmp_node->next) ));
               }
       }
       
       return ret;
}

int AlarmIdIsValid( alarm_param_type_t *alarm )
{
       alarm_list_node_t *curr = (alarm_list_node_t *)&alarm_list_head;
       alarm_list_node_t *ptr = curr;
       int ret = 0;

       if ( alarm==NULL )
	   return 0;

       while( ptr != NULL )
       {
            if ( ptr->used==1 )
             {
                   if ( ptr->alarm_param == alarm ) //alarm is valid
                   {
                        ret = 1;
                        break;
                   }			   
             }
	      curr = ptr; //save current
	      ptr = ptr->next; //pointer to next
	}	   

        return ret;
}

int AlarmCreate(char *alarm_name, unsigned int abs_trigger_time, unsigned int interval, alarm_handle_func_t cb_func )
{
	alarm_param_type_t  *px_new_alarm = NULL;	

	if ( alarm_name==NULL )
	    return (int)NULL;

	px_new_alarm = ( alarm_param_type_t * ) LocalMalloc( sizeof( alarm_param_type_t ) );
	if( NULL != px_new_alarm )
	{
                   cyg_alarm_create( cyg_real_time_clock(),  ///< cyg_handle_t  counter; //Attached to this counter
		                                cb_func,  ///< cyg_alarm_t  *alarmfn;  //Call-back function
		                                (cyg_addrword_t)px_new_alarm,         ///< cyg_addrword_t  data;  //Call-back data
		                                &px_new_alarm->alarm_handle, ///< cyg_handle_t  *handle;  //alarm object Returned to operate alarm
                                                &px_new_alarm->alarm_object  ///< cyg_alarm  *alarm;  //put alarm here
                                             ); 

                    cyg_alarm_initialize(  px_new_alarm->alarm_handle,    ///< cyg_handle_t alarm;  //which is "cyg_handle_t  *handle" in create() 
	                                            cyg_current_time()+abs_trigger_time, ///< cyg_tick_count_t  trigger; //Absolute trigger time
	                                            interval ///< cyg_tick_count_t  interval ;  //Relative retrigger interval
	                                    );
                    cyg_alarm_disable(px_new_alarm->alarm_handle); ///< cyg_handle_t alarm
                    px_new_alarm->alarm_handle_function = cb_func;
                    px_new_alarm->abs_trigger_time = abs_trigger_time;
                    px_new_alarm->interval = interval;

		if ( AlarmPointerSave(px_new_alarm, alarm_name) == 0 ) //save ok
		{
		     debug_alarm(("AlarmCreate: ptr=%x, name=%s\n", (unsigned int)(px_new_alarm), alarm_name));
		     return  (int)px_new_alarm;
		}
		else
		{
                          LocalFree( px_new_alarm );
		}
	}
	else
	{
		LocalFree( px_new_alarm );
	}


	return (int)NULL;
}

int AlarmEnable(int alarm_id)
{
       alarm_param_type_t  *alarm_ptr = (alarm_param_type_t  *)alarm_id;

       if( AlarmIdIsValid( alarm_ptr ) == 0 )
        {
              return -1;
        }

       //cyg_alarm_disable( alarm_ptr->alarm_handle); ///< cyg_handle_t alarm
       //alarm_ptr->timeout = -1; ///< reset
       
	cyg_alarm_initialize( alarm_ptr->alarm_handle, 
		                        cyg_current_time()+alarm_ptr->abs_trigger_time, 
		                        alarm_ptr->interval
		                     );
       
       cyg_alarm_enable( alarm_ptr->alarm_handle); ///< cyg_handle_t alarm
       alarm_ptr->timeout = 0; 

       return 0;
}

int AlarmEnableWithNewTrigger(int alarm_id, unsigned int new_trigger_time, unsigned int new_interval )
{
       alarm_param_type_t  *alarm_ptr = (alarm_param_type_t  *)alarm_id;

       if( AlarmIdIsValid( alarm_ptr ) == 0 )
        {
              return -1;
        }

       //cyg_alarm_disable( alarm_ptr->alarm_handle); ///< cyg_handle_t alarm
       //alarm_ptr->timeout = -1; ///< reset
       
        alarm_ptr->abs_trigger_time = new_trigger_time;
	alarm_ptr->interval = new_interval;
	cyg_alarm_initialize( alarm_ptr->alarm_handle, 
		                        cyg_current_time()+alarm_ptr->abs_trigger_time, 
		                        alarm_ptr->interval
		                     );
       
       cyg_alarm_enable( alarm_ptr->alarm_handle); ///< cyg_handle_t alarm
       alarm_ptr->timeout = 0; 

       return 0;
}

int AlarmDisable(int alarm_id)
{
       alarm_param_type_t  *alarm_ptr = (alarm_param_type_t  *)alarm_id;

       if( AlarmIdIsValid( alarm_ptr ) == 0 )
        {
              return -1;
        }	   
   
       cyg_alarm_disable( alarm_ptr->alarm_handle); ///< cyg_handle_t alarm
       alarm_ptr->timeout = -1; ///< reset

       return 0;
}

int AlarmResetTimeOut(int alarm_id)
{
       alarm_param_type_t  *alarm_ptr = (alarm_param_type_t  *)alarm_id;

       if( AlarmIdIsValid( alarm_ptr ) == 0 )
        {
              return -1;
        }

       alarm_ptr->timeout = -1; ///< reset

       return 0;
}
int AlarmCheckTimeOut(int alarm_id)
{
       alarm_param_type_t  *alarm_ptr = (alarm_param_type_t  *)alarm_id;

       if( AlarmIdIsValid( alarm_ptr ) == 0 )
        {
              return -1;
        }   

       return (int)(alarm_ptr->timeout);
}

int AlarmTimeOutProcess(void *data)
{
       alarm_param_type_t *palarm_param = (alarm_param_type_t *)data;
       if( AlarmIdIsValid( palarm_param ) == 0 )
        {
              return -1;
        }
        palarm_param->timeout = 1;

        return 0;
}

