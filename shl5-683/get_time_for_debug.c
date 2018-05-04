/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_main.c
 * File Identification: 
 * Abstract:  
 *
 * Current Version:	V2013_0415_0.0.1
 * Author:	 H.product Department
 * Completion Date:	2013-04-16
 *
 * Last Version:
 * Last Author:
 * Finishing Date:
 */
 
#include <stdio.h> 
#include <string.h>
#include <sys/time.h>

#define MAX_ITEMS_DEBUG  128

static int store_time_index = 0;
static unsigned long int array[MAX_ITEMS_DEBUG][2];
static unsigned long int sample_len[MAX_ITEMS_DEBUG/2];
static int sam_len_index = 0;
static int sam_len_output_index = 0;

int DebugTimeDisplay(void)
{     
      int i = 0;
	  
      debug_print(("DebugTimeDisplay\n"));
      for( i=0; i<MAX_ITEMS_DEBUG; i++ )
      {
    	  debug_print(("sec=%ld, usec=%ld, ", array[i][0], array[i][1]));
          if(i%2==1) ///< 1,3,5,7,....
          {
               if( sam_len_output_index < MAX_ITEMS_DEBUG/2 )
               {
            	   debug_print(("sample_bytes=%ld\n", sample_len[sam_len_output_index++]));
	       }        
	  }
      }
      
      return 0;
}

int DebugTimeReset(void)
{     
      int i = 0;
	  
      debug_print(("DebugTimeReset\n"));
      store_time_index = 0;
      memset((void *)array, 0x00, sizeof(array));
      
      return 0;
}

int DebugStoreSampleValue(int len)
{
      if( NULL==sample_len||sam_len_index>=MAX_ITEMS_DEBUG/2 )
      {
            return -1;
      }
	  
      sample_len[sam_len_index] = len;
      (sam_len_index)++;
	  
      return 0;
}

int DebugStoreTimeFromGetTimeOfDay(void)
{
      struct timeval tp;

      if( NULL==array ||store_time_index>=MAX_ITEMS_DEBUG )
      {
            return -1;
      }
	  
      gettimeofday(&tp, NULL);
      array[store_time_index][0] = tp.tv_sec;
      array[store_time_index][1] = tp.tv_usec;
      (store_time_index)++;
	  
      return 0;
}

