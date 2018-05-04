/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_state_machine.c
 * File Identification: 
 * Abstract:  
 *
 * Current Version:	V2013_0306_0.0.1
 * Author:	AlvinChang
 * Completion Date:	2013-03-06
 *
 * Last Version:
 * Last Author:
 * Finishing Date:
 */ 
#include "ccu_state_machine.h"

//now, begin to define params
event_option_table_t  ccu_ann_option_table[] = 
{
      	{"OCC", OCC_EVENT},
      	{"LIVE", LIVE_ANN_EVENT},
      	{"EMERG", EMERG_ANN_EVENT},
      	{"MANUAL", MANUAL_ANN_EVENT},
      	{"TMS-MANUAL", TMS_MANUAL_ANN_EVENT},
      	{"TMS-AUTO", TMS_AUTO_ANN_EVENT},
      	{"D2D", D2D_INTERCOMM_EVENT},
      	{"D2P", D2P_INTERCOMM_EVENT},
      	{"D2DXD2P", D2D_HANGUP_D2P_EVENT},
      	{"D2PPEND", D2P_INTERCOM_PENDING},
      	{"LINENO", LINE_NUMBER_CHANGE_EVENT}
};
#define CCU_ANN_OPTION_ITEMS ((sizeof(ccu_ann_option_table))/(sizeof(ccu_ann_option_table[0])))

static state_priority_list_t  ccu_ann_priority_list_node_idle = 
{
      ANN_IDLE,
       0xFFFF, //this is always the lowest priority for idle state (65535)
       NULL,
       NULL  //when power up, there is only idle node in the list
};

event_priority_table_t ccu_ann_priority_default_table[] =
{
      	{"OCC", 1},
      	{"EMERG", 3},			
      	{"LIVE", 2},
      	{"TMS-AUTO", 5},      	
      	{"MANUAL", 4},
      	{"TMS-MANUAL", 6}
};
#define CCU_ANN_PRIORITY_DEFAULT_TABLE_ITEMS  ((sizeof(ccu_ann_priority_default_table))/(sizeof(ccu_ann_priority_default_table[0])))

//simulation for new priority table
event_priority_table_t ccu_ann_priority_new_table[] = 
{
      	{"EMERG", 1},
      	{"TMS-AUTO", 2},
      	{"LIVE", 3},      	
      	{"OCC", 4},
      	{"MANUAL", 5},
      	{"TMS-MANUAL", 6}
};
#define CCU_ANN_PRIORITY_NEW_TABLE_ITEMS  ((sizeof(ccu_ann_priority_new_table))/(sizeof(ccu_ann_priority_new_table[0])))


int CcuTestSelectNextEvent(void)
{
       static unsigned int count_next_event = 0;
       unsigned int ret = 0;

        if( count_next_event >= CCU_ANN_OPTION_ITEMS )
        {
             count_next_event = 0; //reset
             debug_print(("CcuTestSelectNextEvent: end\n"));
             return -1;
        }
        ret = ccu_ann_option_table[count_next_event++].unique_id;		
		
       return (int)ret;
}

int CcuDynamicStateSort(event_priority_table_t  *priority_table_from_database, unsigned int table_items)
{
      //int ret = -1;
      int i = 0, j = 0;
      event_priority_table_t  tmp_table_item = {"", 0};
	  
      if( NULL==priority_table_from_database )
      {
    	  debug_print(("CcuDynamicStateSort"));
             return -1;
      }

      #if 1 //debug
      {
	      for(j=0; j<table_items; j++)
	      {
	    	  debug_print(("before sort:  %s-%d\n", priority_table_from_database[j].event_name,  priority_table_from_database[j].event_priority));
	      }
      }
      #endif	  
	  
      for(j=0; j<table_items-1; j++)   // loot time is table_items-1
      {
              for(i=0; i<table_items-j-1; i++)
              {
                      if( priority_table_from_database[i].event_priority > priority_table_from_database[i+1].event_priority )
                      { //priority value is larger, the priority is smaller
                            tmp_table_item = priority_table_from_database[i];
                            priority_table_from_database[i] = priority_table_from_database[i+1];
                            priority_table_from_database[i+1] = tmp_table_item;
                      }
             }
      }

      #if 1 //debug
      {
	      for(j=0; j<table_items; j++)
	      {
	    	  debug_print(("after sort:  %s, %d\n", priority_table_from_database[j].event_name,  priority_table_from_database[j].event_priority));
	      }
      }
      #endif

       return 0;
}
static state_priority_list_t *CcuAnnStateAddNewNode(event_priority_table_t *new_priority_item )
{
        state_priority_list_t *tmp_node = NULL;
         int min_len = 0, s1_len = 0, s2_len = 0;
         int j = 0;

         if( NULL == new_priority_item )
        {
            debug_print(("CcuAnnStateAddNewNode:  null ptr\n"));
            return NULL;
        }		 	
		
        tmp_node = (state_priority_list_t *)malloc( sizeof(state_priority_list_t));
        if( NULL==tmp_node )
        {
            debug_print(("CcuAnnStateAddNewNode: malloc fail\n"));
            return NULL;
        }
        memset( (void *)tmp_node, 0x00, sizeof(*tmp_node) );
        
        //No1.			
        s2_len = strlen( new_priority_item->event_name);
        for(j=0; j<CCU_ANN_OPTION_ITEMS; j++)
        {
              min_len = 0;
              s1_len = 0;					
              s1_len = strlen(ccu_ann_option_table[j].name);
              min_len = (s1_len>=s2_len) ? (s2_len) : (s1_len);
              if( 0==strncmp(ccu_ann_option_table[j].name, new_priority_item->event_name,  min_len) )
              { 
                    //name matched
                    tmp_node->unique_id = ccu_ann_option_table[j].unique_id;
                    break;
              }
        }
		
        if( j==CCU_ANN_OPTION_ITEMS ) //not found in option table, so id is 0xFFFF-FFFF
        {
             tmp_node->unique_id = 0xFFFFFFFF;
             debug_print(("CcuAnnStateAddNewNode:  event id not found\n"));
        }
        
        //No2.
        tmp_node->priority = new_priority_item->event_priority;
        
        //No3.
        tmp_node->this_state_process = NULL;	
		
        return tmp_node;
}
int CcuAnnStateCreatePriorityNode(event_priority_table_t *db_priority_table )
{
		state_priority_list_t *tmp_node = NULL;

		state_priority_list_t  *current_node = (state_priority_list_t  *)&ccu_ann_priority_list_node_idle;

		if( NULL==db_priority_table )
		{
			return -1;
		}

		//find the last node
		while( current_node->next != NULL )
		{
			debug_print(("CcuAnnStateCreatePriorityNode:  %d,%d\n", (int)current_node->unique_id, (int)current_node->priority));
			debug_print(("CcuAnnStateCreatePriorityNode:  %d,%d\n", (int)current_node->this_state_process, (int)current_node->next));
			current_node = current_node->next;
		}

		//create new node as new last node
		tmp_node = CcuAnnStateAddNewNode( db_priority_table );
		if( NULL == tmp_node )
		{
			debug_print(("CcuAnnStateCreatePriorityNode: new node fail\n"));
			return -1;
		}

		current_node->next = tmp_node;
		//No4.
		current_node = tmp_node; //current node updated
		current_node->next = NULL; //new end node

		return 0;
}
int CcuAnnStateCreatePriorityList(event_priority_table_t *db_priority_table, unsigned int table_items)
{
	int i = 0, j = 0;
	state_priority_list_t *tmp_node = NULL;
	int min_len = 0, s1_len = 0, s2_len = 0;
	state_priority_list_t  *current_node = (state_priority_list_t  *)&ccu_ann_priority_list_node_idle;

	if( NULL==db_priority_table )
	{
		   return -1;
	}

	//CcuDynamicStateSort(db_priority_table, table_items);

	for(i=table_items; i>0; i--)
	{
		tmp_node = (state_priority_list_t *)malloc( sizeof(state_priority_list_t) );
		if( NULL==tmp_node )
		{
			debug_print(("CcuAnnStateCreatePriorityList: malloc fail\n"));
			return -1;
		}
		memset( (void *)tmp_node, 0x00, sizeof(*tmp_node) );
  
		//No1.
		s2_len = strlen(db_priority_table[i-1].event_name);
		for(j=0; j<CCU_ANN_OPTION_ITEMS; j++)
		{
			min_len = 0;
			s1_len = 0;
			s1_len = strlen(ccu_ann_option_table[j].name);
			min_len = (s1_len>=s2_len) ? (s2_len) : (s1_len);
			if( 0==strncmp(ccu_ann_option_table[j].name, db_priority_table[i-1].event_name,  min_len) )
			{
				//name matched
				tmp_node->unique_id = ccu_ann_option_table[j].unique_id;
				break;
			}
		}
		if( j==CCU_ANN_OPTION_ITEMS ) //not found in option table, so id is 0xFFFF-FFFF
		{
			tmp_node->unique_id = 0xFFFFFFFF;
			debug_print(("CcuAnnStateCreatePriorityList:  event id not found\n"));
		}
  
	  	//No2.
		tmp_node->priority = db_priority_table[i-1].event_priority;
	  
	        //No3.
                tmp_node->this_state_process =  NULL;
	           
		current_node->next = tmp_node;
		//No4.
		current_node = tmp_node; //current node updated
		tmp_node = NULL; //for next loop
	}

         #if 1
         {
               state_priority_list_t  *curr_node = (state_priority_list_t  *)&ccu_ann_priority_list_node_idle;
               while( curr_node != NULL )
               {
                   debug_print(("new list: %d,%d, 0x%08x \n", curr_node->unique_id, curr_node->priority, (unsigned int)(curr_node->this_state_process)));	
                   curr_node = curr_node->next; //pointer to next which priority is larger
               }
         }
         #endif

	  return 0;
}
int CcuAnnStateListUpdate(event_priority_table_t *db_priority_table, unsigned int table_items)
{
	int ret = -1;
	  
	if( NULL==db_priority_table )
	{
		debug_print(("CcuAnnStateListUpdate: use default priority,total=%d\n", CCU_ANN_PRIORITY_DEFAULT_TABLE_ITEMS));
		if ( 0 == CcuAnnStateCreatePriorityList(ccu_ann_priority_default_table, CCU_ANN_PRIORITY_DEFAULT_TABLE_ITEMS) )
		{
			debug_print(("CcuAnnStateListUpdate: default list ok\n"));
			ret = 0;
		}
	}
	else
	{
		debug_print(("CcuAnnStateListUpdate: use new priority,total=%d\n", table_items));
		if ( 0 == CcuAnnStateCreatePriorityList(db_priority_table, table_items) )
		{
			debug_print(("CcuAnnStateListUpdate: new list ok\n"));
			ret = 0;
		}
	}
  return ret;
}
int CcuAnnStateNodeUpdate(event_priority_table_t *db_priority_table)
{
	int ret = -1;

	if( NULL==db_priority_table )
	{
		debug_print(("CcuAnnStateNodeUpdate: add new priority node\n"));
		if ( 0 == CcuAnnStateCreatePriorityNode(db_priority_table) )
		{
			debug_print(("CcuAnnStateNodeUpdate: new list ok\n"));
			ret = 0;
		}
	}

	return ret;
}
int CcuStateEventIdJudge(int event_id )
{
    int ret = -1;

	switch( event_id )
	{
		 case OCC_EVENT:
		 case LIVE_ANN_EVENT:
		 case EMERG_ANN_EVENT:
		 case MANUAL_ANN_EVENT:
		 case TMS_MANUAL_ANN_EVENT:
		 case TMS_AUTO_ANN_EVENT:
		 	  ret = 0;
			  break;
			  
		 case D2D_INTERCOMM_EVENT:
		 case D2P_INTERCOMM_EVENT:
		 case D2D_HANGUP_D2P_EVENT:
		 case D2P_INTERCOM_PENDING:
		 	ret = 1;
			break;
			
		 case LINE_NUMBER_CHANGE_EVENT:
		 	ret =2;
		        break;

		case ANN_IDLE:
		case INTERCOM_IDLE:
			ret = -1;
			break;

		default:
			ret = -1;
			break;               
	}

	return ret;
}
state_priority_list_t  *CcuAnnStatePriorityListSearch(unsigned int event_id)
{
	state_priority_list_t *ret_ptr = NULL;
	state_priority_list_t  *current_node = (state_priority_list_t  *)&ccu_ann_priority_list_node_idle; //now, currrent node is idle

	while( current_node != NULL )
	{
		debug_print(("CcuAnnStatePriorityListSearch: %d,%d, %d\n", current_node->unique_id, current_node->priority, event_id));
		if( event_id == current_node->unique_id )
		{
			ret_ptr = current_node;
			break;
		}
		current_node = current_node->next; //pointer to next which priority is larger
	}
	return ret_ptr;
}

unsigned int CcuGetAnnStatePriority(unsigned int event_id)///<add by wilson 2013-06-28
{
	unsigned int ret = 0;
	#if 0 ///< shouldn't free
	state_priority_list_t *temp_state_priority_list;
	temp_state_priority_list = (state_priority_list_t *)malloc(sizeof(state_priority_list_t));
	temp_state_priority_list = CcuAnnStatePriorityListSearch(event_id);
	ret = temp_state_priority_list->priority;
	free(temp_state_priority_list);
        #else
	state_priority_list_t *temp_state_priority_list;
	temp_state_priority_list = CcuAnnStatePriorityListSearch(event_id);
	ret = temp_state_priority_list->priority;	
	#endif
	return ret;
}
//add end, ach, 2013

