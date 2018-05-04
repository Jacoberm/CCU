/*
 * Copyright (c) 2012,MIT
 * All right reserved.
 *
 * 文件名称： buffer_read_write_fun.c
 * 文件标识：
 * 摘    要：1. 字节缓冲区接口：创建、读、写、销毁等函数
 *			 2. 块缓冲区接口：创建、读、写、销毁等函数 *
 *
 * 当前版本： V1.0.1
 * 作    者： AlvinChang
 * 完成日期：2012-09-28
 *
 * 取代版本： V1.0.0
 * 原作者  ： AlvinChang
 * 完成日期：2012-08-20
 */
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "buffer_read_write_fun.h"

//#define CONFIG_BUFFER_DEBUG
#ifdef CONFIG_BUFFER_DEBUG
//#define BUFFER_DEBUG(x)  printf x
#define BUFFER_DEBUG(x)  //diag_printf x
#else
#define BUFFER_DEBUG(x)
#endif

//this is the head node of data buffer list
static char_buffer_id_t   char_buffer_head = 
{
   "",
   NULL,
   0,
   NULL
};

//this is the head node of block buffer list
static block_buffer_id_t  block_buffer_head =
{
   "",
   NULL,
   0,
   NULL
};

static void *LocalPortMalloc( unsigned int wantedSize );
static void LocalPortFree( void *pv );
static int prvCopyQueueData( block_buffer_t  *pblock_buffer, const void *pitem_to_buffer, unsigned int pitem_size );
static void CharBufferIdReset(char_buffer_t *pdata_buffer);
static int CharBufferPointerSave(char_buffer_t  *px_new_buffer, char *buffer_name);
static void BlockBufferIdReset(block_buffer_t *pblock_buffer);
static int BlockBufferPointerSave(block_buffer_t  *px_new_buffer, char *buffer_name);


static void *LocalPortMalloc( unsigned int wantedSize )
{
    void *pv_return;

	pv_return = malloc( wantedSize );

	return pv_return;
}
static void LocalPortFree( void *pv )
{
	if( pv )
	{
            free( pv );
	}
}

void CharBufferIDNodesDisplay(void)
{
       char_buffer_id_t *curr = (char_buffer_id_t *)&char_buffer_head;
       char_buffer_id_t *ptr = curr;

	while( ptr != NULL )
	{
	      BUFFER_DEBUG(("DisplayDataBufferIDNodes: %s, %d, curr_buffer=%x, next_id=%x\n", ptr->buffer_name, ptr->used, (int)(ptr->pdata_buffer), (unsigned int)(ptr->next)));
	      BUFFER_DEBUG(("DisplayDataBufferIDNodes: %x, %x, %x\n", (int)(ptr->pdata_buffer->pb_head), (int)(ptr->pdata_buffer->pb_tail), (int)(ptr->pdata_buffer->buffer_size)));
	      ptr = ptr->next; //pointer to next
	}

       return ;
}
static int CharBufferIsValid( char_buffer_t *pdata_buffer )
{
       char_buffer_id_t *curr = (char_buffer_id_t *)&char_buffer_head;
       char_buffer_id_t *ptr = curr;
       int ret = 0;

       if ( pdata_buffer==NULL )
	   return 0;

       while( ptr != NULL )
       {
            if ( ptr->used==1 )
             {
                   if ( ptr->pdata_buffer == pdata_buffer ) //this block buffer is valid
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
static void CharBufferIdReset(char_buffer_t *pdata_buffer)
{
       char_buffer_id_t *curr = (char_buffer_id_t *)&char_buffer_head;
       char_buffer_id_t *ptr = curr;

       if ( pdata_buffer==NULL )
    	   return ;
	   
		while( ptr != NULL )
		{
             if ( ptr->used==1 )
             {
                   if ( pdata_buffer==ptr->pdata_buffer ) //buffer matched
                   {
                         //delete this node which is not head
                         if ( ptr != (char_buffer_id_t *)&char_buffer_head  )
                         	 {
                                curr->next = ptr->next;
                                LocalPortFree(ptr);                                
			 }
			 else //you try to delete head, but we just let used=0
			 {
                                  ptr->used = 0;
			 }
                         break;			 
		   }
	      }
	      curr = ptr; //save current
	      ptr = ptr->next; //pointer to next
	}
}
int CharBufferOpen( char *buffer_name )
{
       int ret = -1, min_len = 0;
       char_buffer_id_t *curr = (char_buffer_id_t *)&char_buffer_head; //list head
       char_buffer_id_t *ptr = curr;

       if ( buffer_name==NULL )
	   return -1;

	while( ptr != NULL )
	{
             min_len = 0;
             if ( ptr->used==1 )
             {
                   BUFFER_DEBUG(("CharBufferOpen:   %s, %x, %s\n", ptr->buffer_name, (unsigned int)(ptr->pdata_buffer), buffer_name ));
                   if ( strlen(buffer_name) >= sizeof(ptr->buffer_name) )
                   {
                         min_len = sizeof(ptr->buffer_name)-1;
		   }
		   else if ( strlen(buffer_name)==strlen(ptr->buffer_name) )
		   {
                          min_len = strlen(ptr->buffer_name);
		   }
		   
                   if ( min_len>0 && strncmp(ptr->buffer_name, buffer_name, min_len)==0 ) //buffer name matched
                   {
                        ret = (int)(ptr->pdata_buffer);
                        break;
                   }		   
	      }
	      curr = ptr; //save current
	      ptr = ptr->next; //pointer to next
	}

	return ret;
}
static int CharBufferPointerSave(char_buffer_t  *px_new_buffer, char *buffer_name)
{
       int min_len = 0, ret = 0;
       unsigned char found = 0;
       char_buffer_id_t *curr = (char_buffer_id_t *)&char_buffer_head;
       char_buffer_id_t *ptr = curr;

        if ( px_new_buffer==NULL ||buffer_name==NULL  )
	   return -1;

        //check if new buffer name is same to someone
	while( ptr != NULL )
	{
             if ( ptr->used==1 )
             {
                   if ( strcmp(ptr->buffer_name, buffer_name)==0 ) //buffer exist, so don't save this new buffer
                   {                         
			 return -1;
		   }
	      }
	      curr = ptr; //save old
	      ptr = ptr->next; //pointer to next
	}	   

        //now, buffer name is uniq
	curr = (char_buffer_id_t *)&char_buffer_head;
        ptr = curr;
	while( ptr != NULL )
	{
             if ( !ptr->used )
             {
                   ptr->used = 1;
		   if ( strlen(buffer_name) >= sizeof(ptr->buffer_name) )
		   {
                         min_len = sizeof(ptr->buffer_name);
		   }
		   else
		   {
                        min_len = strlen(buffer_name)+1;
		   }
                   strncpy( ptr->buffer_name, buffer_name, min_len-1 );
                   ptr->buffer_name[min_len] = 0x00;
		   ptr->pdata_buffer = px_new_buffer;
		   found = 1;
		   BUFFER_DEBUG(("save:  %s, %d, %x, %x\n", ptr->buffer_name, ptr->used, (unsigned int)(ptr->pdata_buffer), (unsigned int)(ptr->next)));
		   break;
	      }
	      curr = ptr; //save old
	      ptr = ptr->next; //pointer to next
	}

	if ( found==0 ) //all nodes are used ,so we need a new node, now, curr is the tail node
	{
	      char_buffer_id_t *tmp_node = NULL;
              tmp_node = (char_buffer_id_t *) LocalPortMalloc( sizeof( char_buffer_id_t ) );
	      if( tmp_node==NULL )
	      {
                  ret = -1;
	      }
	      else
		{
		     //save new pointer into new node
                   memset( (void *)tmp_node, 0x00, sizeof(*tmp_node) );
                   tmp_node->used = 1;
		   if ( strlen(buffer_name) >= sizeof(tmp_node->buffer_name) )
		   {
                         min_len = sizeof(tmp_node->buffer_name);
		   }
		   else
		   {
                        min_len = strlen(buffer_name)+1;
		   }		   
                   strncpy( tmp_node->buffer_name, buffer_name, min_len-1 );
                   tmp_node->buffer_name[min_len] = 0x00;
		   tmp_node->pdata_buffer = px_new_buffer;		     
		   curr->next = tmp_node; //tmp_node as tail node
		   BUFFER_DEBUG(("save tmp:  %s, %d, %x, %x\n", tmp_node->buffer_name, tmp_node->used, (unsigned int)(tmp_node->pdata_buffer), (unsigned int)(tmp_node->next)));
		}
	}

	return ret;
}

int CharBufferCreate( unsigned int buffer_size, char *buffer_name )
{
	char_buffer_t  *px_new_buffer = NULL;

	if ( buffer_name==NULL )
	    return (int)NULL;

	/* Allocate the new queue structure. */
	if( buffer_size > ( unsigned int ) 0 )
	{
		px_new_buffer = (char_buffer_t *) LocalPortMalloc( sizeof( char_buffer_t ) );
		if( NULL != px_new_buffer )
		{
			px_new_buffer->pb_head = ( unsigned char * ) LocalPortMalloc( buffer_size );
			if( NULL != px_new_buffer->pb_head )
			{
				px_new_buffer->pb_tail = px_new_buffer->pb_head + buffer_size;
				px_new_buffer->pb_writeto = px_new_buffer->pb_head;
				px_new_buffer->pb_readfrom = px_new_buffer->pb_head;
				px_new_buffer->buffer_size = buffer_size;
				if ( CharBufferPointerSave(px_new_buffer, buffer_name) == 0 ) //save ok
				{
				     BUFFER_DEBUG(("CharBufferCreate: ptr=%x,buff_name=%s\n", (unsigned int)(px_new_buffer), buffer_name));
				     return  (int)px_new_buffer;
				}
				else
				{
				      LocalPortFree(px_new_buffer->pb_head);
                                      LocalPortFree( px_new_buffer );
				}
			}
			else
			{
				LocalPortFree( px_new_buffer );
			}
		}
	}

	/* Will only reach here if we could not allocate enough memory or no memory was required. */
	return (int)NULL;
}

int CharBufferWrite( int pbuffer_v, const void *psrc_buffer, unsigned int data_real_size )
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *src_ptr = (unsigned char *)psrc_buffer;
       //unsigned char *writeto_backup = pbuffer->pb_writeto;
       int copy_bytes = 0;

       if ( CharBufferIsValid(pbuffer)==0 || NULL==psrc_buffer )
       {
             BUFFER_DEBUG(("CharBufferWrite:  pointer error,don't write\n"));
             return errBUFFER_POINTER_ERROR;
       }

       //pthread_mutex_lock(&pqueue->wr_lock);
       {
             while( data_real_size )
             {
                 unsigned char *pwriteto_test = pbuffer->pb_writeto;
                 pwriteto_test++;
                 if( pwriteto_test >= pbuffer->pb_tail )
                 {
                     pwriteto_test = pbuffer->pb_head;
                 }
                 if( pwriteto_test == pbuffer->pb_readfrom )
                 {
                     //pbuffer->pb_writeto = writeto_backup;
                     BUFFER_DEBUG(("char buffer full, copy_bytes= %d,but source have %d bytes left\n",copy_bytes ,data_real_size ));
                     if ( copy_bytes==0 )
                         xreturn = errBUFFER_FULL;
                     break;
                 }

                 *(pbuffer->pb_writeto) = *src_ptr;
                 src_ptr++;
		 pbuffer->pb_writeto++;
                 copy_bytes++;
                 data_real_size--;
                 xreturn = copy_bytes;
                 if( pbuffer->pb_writeto >= pbuffer->pb_tail )
                 {
                 	pbuffer->pb_writeto = pbuffer->pb_head;
                 }
             	}

		BUFFER_DEBUG(("CharBufferWrite, exit: wr=%d, rd=%d, copied=%d\n", (int)(pbuffer->pb_writeto), (int)(pbuffer->pb_readfrom), copy_bytes));
	}
       //pthread_mutex_unlock(&pqueue->wr_lock);

	return xreturn;
}
int CharBufferRead( int pbuffer_v, void *pdest_buffer, unsigned int dest_buffer_size )
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *dest_ptr = (unsigned char *)pdest_buffer;
       unsigned int copy_bytes = 0;

       if ( CharBufferIsValid(pbuffer)==0 || NULL==pdest_buffer )
       {
             return errBUFFER_POINTER_ERROR;
       }	

       //pthread_mutex_lock(&pqueue->wr_lock);
       {
             while(1)
             {
                 if( pbuffer->pb_writeto == pbuffer->pb_readfrom )
                 {
                     if ( copy_bytes==0 )
			     xreturn = errBUFFER_EMPTY;
                     break;
                 }
		   if( copy_bytes >= dest_buffer_size )
		   {
		        BUFFER_DEBUG(("CharBufferRead: dest buffer not enough\n"));
                        break;
		   }

                 *dest_ptr = *(pbuffer->pb_readfrom);
                 pbuffer->pb_readfrom++;
                 dest_ptr++;				 
                 copy_bytes++;
                 xreturn = copy_bytes;
                 if( pbuffer->pb_readfrom >= pbuffer->pb_tail )
                 {
                 	pbuffer->pb_readfrom = pbuffer->pb_head;
                 }

            }

 	     BUFFER_DEBUG(("CharBufferRead, exit: wr=%d, rd=%d, copied=%d\n", (int)(pbuffer->pb_writeto), (int)(pbuffer->pb_readfrom), copy_bytes));
	}
       //pthread_mutex_unlock(&pqueue->wr_lock);

	return xreturn;
}
//尚未测试
int CharBufferIsFull( int pbuffer_v )
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *current_rd = NULL;
       unsigned char *current_wr = NULL;	   

       if ( CharBufferIsValid(pbuffer)==0)
       {
            BUFFER_DEBUG(("CharBufferIsFull:  pointer error\n"));
             return errBUFFER_POINTER_ERROR;
       }

       //pthread_mutex_lock(&pqueue->wr_lock);
       {
             unsigned char *pwriteto_test = pbuffer->pb_writeto;
             pwriteto_test++;
             if( pwriteto_test >= pbuffer->pb_tail )
             {
                    pwriteto_test = pbuffer->pb_head;
             }
             if( pwriteto_test == pbuffer->pb_readfrom )
             {
                  xreturn = errBUFFER_FULL;
		  return xreturn;
             }

             current_rd = pbuffer->pb_readfrom;
             current_wr = pbuffer->pb_writeto;
	     if( current_rd <= current_wr )
	     {
                    xreturn = pbuffer->buffer_size - (current_wr-current_rd) - 1;
	     }
	     else
	     {
                    xreturn = current_rd - current_wr -1;
	     }
	}
       //pthread_mutex_unlock(&pqueue->wr_lock);

	return xreturn;
}

int CharBufferCurrentBytes( int pbuffer_v )
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *current_rd = NULL;
       unsigned char *current_wr = NULL;	   

       if ( CharBufferIsValid(pbuffer)==0)
       {
             BUFFER_DEBUG(("CharBufferIsFull:  pointer error\n"));
             return errBUFFER_POINTER_ERROR;
       }

       //pthread_mutex_lock(&pqueue->wr_lock);
       {
             unsigned char *pwriteto_test = pbuffer->pb_writeto;
             pwriteto_test++;
             if( pwriteto_test >= pbuffer->pb_tail )
             {
                    pwriteto_test = pbuffer->pb_head;
             }
             if( pwriteto_test == pbuffer->pb_readfrom )
             {
                  xreturn = errBUFFER_FULL;
		  return xreturn;
             }

             current_rd = pbuffer->pb_readfrom;
             current_wr = pbuffer->pb_writeto;
	     if( current_rd <= current_wr )
	     {
                    xreturn = pbuffer->buffer_size - (current_wr-current_rd) - 1;
	     }
	     else
	     {
                    xreturn = current_rd - current_wr -1;
	     }
	}
       //pthread_mutex_unlock(&pqueue->wr_lock);
       
	return (pbuffer->buffer_size-xreturn);
}
void *CharBufferCurrentReadPointer_temp( int pbuffer_v, unsigned int *pcurrent_total_bytes,unsigned int *pcurrent_right_delta)
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *current_rd = NULL;
       unsigned char *current_wr = NULL;

       if ( CharBufferIsValid(pbuffer)==0)
       {
             BUFFER_DEBUG(("CharBufferCurrentReadPointer:  pointer error\n"));
             return NULL;
       }

       //pthread_mutex_lock(&pqueue->wr_lock);
       {
             current_rd = pbuffer->pb_readfrom;
             current_wr = pbuffer->pb_writeto;
	     if( current_rd <= current_wr )
	     {
                    xreturn = current_wr-current_rd;
                    *pcurrent_right_delta = xreturn;
	     }
	     else
	     {
                   xreturn = (pbuffer->pb_tail - pbuffer->pb_readfrom ) + (pbuffer->pb_writeto - pbuffer->pb_head);
                   *pcurrent_right_delta = pbuffer->pb_tail - pbuffer->pb_readfrom;
	     }

	     if ( NULL != pcurrent_total_bytes )
	     {
		    *pcurrent_total_bytes = xreturn;
	     }

	}
       //pthread_mutex_unlock(&pqueue->wr_lock);

	return (void *)(pbuffer->pb_readfrom);
}
void *CharBufferCurrentReadPointer( int pbuffer_v, unsigned int *pcurrent_total_bytes,unsigned int *pcurrent_right_delta )
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *current_rd = NULL;
       unsigned char *current_wr = NULL;

       if ( CharBufferIsValid(pbuffer)==0)
       {
             BUFFER_DEBUG(("CharBufferCurrentReadPointer:  pointer error\n"));
             return NULL;
       }

       //pthread_mutex_lock(&pqueue->wr_lock);
       {
             current_rd = pbuffer->pb_readfrom;
             current_wr = pbuffer->pb_writeto;
	     if( current_rd <= current_wr )
	     {
                    xreturn = current_wr-current_rd;
					if( NULL != pcurrent_right_delta )
		            {
                         *pcurrent_right_delta = xreturn;
					}
	     }
	     else
	     {
                   xreturn = (pbuffer->pb_tail - pbuffer->pb_readfrom ) + (pbuffer->pb_writeto - pbuffer->pb_head);
				   if( NULL != pcurrent_right_delta )
		           {
                       *pcurrent_right_delta = pbuffer->pb_tail - pbuffer->pb_readfrom;
		           }
	     }

	     if ( NULL != pcurrent_total_bytes )
	     {
		    *pcurrent_total_bytes = xreturn;
	     }

	}
       //pthread_mutex_unlock(&pqueue->wr_lock);

	return (void *)(pbuffer->pb_readfrom);
}

void CharBufferUpdateCurrentReadPointer( int pbuffer_v, unsigned int origin_len, unsigned int left_len )
{
               // int xreturn = 0;
	       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
	       unsigned int delta_1 = 0, delta_2 = 0, delta_3 = 0;
	       unsigned char *tmp_rd = (unsigned char *)pbuffer->pb_readfrom;

	       if ( CharBufferIsValid(pbuffer)==0)
	       {
	             BUFFER_DEBUG(("CharBufferUpdateCurrentReadPointer:  pointer error\n"));
                     // return errBUFFER_POINTER_ERROR;
	             return ;
	       }

	       //pthread_mutex_lock(&pbuffer->wr_lock);
	       {
	            BUFFER_DEBUG(("qqwwww:   %x, %x, %d,%d\n", (unsigned int)(pbuffer->pb_readfrom), (unsigned int)(pbuffer->pb_writeto), origin_len, left_len));
	            BUFFER_DEBUG(("qqwwww:   %x, %x\n", (unsigned int)(pbuffer->pb_head), (unsigned int)(pbuffer->pb_tail) ));
	            if( pbuffer->pb_readfrom == pbuffer->pb_writeto )
			 return ;
		    else if ( pbuffer->pb_readfrom < pbuffer->pb_writeto )
		    {
		            tmp_rd += (origin_len-left_len);
			    if( tmp_rd >= pbuffer->pb_writeto )
			    {
	                            pbuffer->pb_readfrom = pbuffer->pb_writeto;
			    }
			    else
			    {
	                          pbuffer->pb_readfrom =  tmp_rd;
			    }
		    }
		    else //wr is left, rd is right
		    {
		            tmp_rd += (origin_len-left_len);
	                    if( tmp_rd  >=  pbuffer->pb_tail )
	                    {
	                         delta_1 = pbuffer->pb_tail - pbuffer->pb_readfrom;
	                         delta_2 = pbuffer->pb_writeto -  pbuffer->pb_head;
				 delta_3 = (origin_len-left_len) - delta_1;
				 if ( delta_3 >= delta_2 ) //always,  delta_2 >= delta_3
				 {
	                                 pbuffer->pb_readfrom =  pbuffer->pb_head + delta_2;
				 }
				 else
				 {
	                                 pbuffer->pb_readfrom =  pbuffer->pb_head + delta_3;
				 }
	                    }
			    else
			    {
	                          pbuffer->pb_readfrom =  tmp_rd;
			    }
			}
			
			BUFFER_DEBUG(("qqwwww-02:   %x, %x, %d,%d\n", (unsigned int)pbuffer->pb_readfrom, (unsigned int)pbuffer->pb_writeto, origin_len, left_len));
		}
	       //pthread_mutex_unlock(&pbuffer->wr_lock);

		return ;
}

void CharBufferUpdateCurrentReadPointer2( int pbuffer_v, unsigned int origin_len, unsigned int left_len, unsigned char last_clear_flag )
{
//	  int xreturn = 0;
	       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
	       unsigned int delta_1 = 0, delta_2 = 0, delta_3 = 0;
	       unsigned char *tmp_rd = (unsigned char *)pbuffer->pb_readfrom;
	       unsigned char *buffer_rd_cp = (unsigned char *)pbuffer->pb_readfrom; ///<ach add, 2013
	       unsigned char *buffer_wr_cp = (unsigned char *)pbuffer->pb_writeto; ///<ach add, 2013

	       if ( CharBufferIsValid(pbuffer)==0)
	       {
	             BUFFER_DEBUG(("CharBufferUpdateCurrentReadPointer:  pointer error\n"));
                     //return errBUFFER_POINTER_ERROR;
	             return ;
	       }

	       //pthread_mutex_lock(&pbuffer->wr_lock);
	       {
	            BUFFER_DEBUG(("qqwwww:   %x, %x, %d,%d\n", (unsigned int)pbuffer->pb_readfrom, (unsigned int)pbuffer->pb_writeto, origin_len, left_len));
	            BUFFER_DEBUG(("qqwwww:   %x, %x\n", (unsigned int)pbuffer->pb_head, (unsigned int)pbuffer->pb_tail ));
	            if( pbuffer->pb_readfrom == pbuffer->pb_writeto )
			 return ;
		    else if ( pbuffer->pb_readfrom < pbuffer->pb_writeto )
		    {
		            tmp_rd += (origin_len-left_len);
			    if( tmp_rd >= pbuffer->pb_writeto )
			    {
	                            pbuffer->pb_readfrom = pbuffer->pb_writeto;
			    }
			    else
			    {
	                          pbuffer->pb_readfrom =  tmp_rd;
			    }
		    }
		    else //wr is left, rd is right
		    {
		            tmp_rd += (origin_len-left_len);
	                    if( tmp_rd  >=  pbuffer->pb_tail )
	                    {
	                         delta_1 = pbuffer->pb_tail - pbuffer->pb_readfrom;
	                         delta_2 = pbuffer->pb_writeto -  pbuffer->pb_head;
				 delta_3 = (origin_len-left_len) - delta_1;
				 if ( delta_3 >= delta_2 ) //always,  delta_2 >= delta_3
				 {
	                                 pbuffer->pb_readfrom =  pbuffer->pb_head + delta_2;
				 }
				 else
				 {
	                                 pbuffer->pb_readfrom =  pbuffer->pb_head + delta_3;
				 }
	                    }
			    else
			    {
	                          pbuffer->pb_readfrom =  tmp_rd;
			    }
			}
			
	                BUFFER_DEBUG(("qqwwww-02:   %x, %x, %d,%d,%d\n", (unsigned int)pbuffer->pb_readfrom, (unsigned int)pbuffer->pb_writeto, origin_len, left_len, ( last_clear_flag != pbuffer->clear_count )));

			if( last_clear_flag != pbuffer->clear_count ) ///<ach add, 2013
			{
                              //restore old position
                             pbuffer->pb_readfrom = buffer_rd_cp;
                             pbuffer->pb_writeto = buffer_wr_cp;
			}
		}
	       //pthread_mutex_unlock(&pbuffer->wr_lock);

		return ;
}

/**
 *  SingleThreadCharBufferBackWrite
 *
 *  @param :  pbuffer_v
 *  @return:  back_len
 *  @
 *  @note:   This function is just used in the single thread after CharBufferRead called and before CharBufferWrite
 *  @note:    This funciton is just called when device is busy.
 */
int SingleThreadCharBufferBackWrite( int pbuffer_v, int back_len )
{
       int xreturn = 0;
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       unsigned char *current_rd = NULL;
       unsigned char *current_wr = NULL;

       if ( CharBufferIsValid(pbuffer)==0)
       {
             BUFFER_DEBUG(("CharBufferIsFull:  pointer error\n"));
             return errBUFFER_POINTER_ERROR;
       }

       current_rd = pbuffer->pb_readfrom;
       current_wr = pbuffer->pb_writeto;
       if( current_rd <= current_wr )
       {
             if( current_rd - pbuffer->pb_head >= back_len )
             {
                     pbuffer->pb_readfrom -= back_len;
	     }
	     else if ( (current_rd - pbuffer->pb_head) + (pbuffer->pb_tail - current_wr -1 ) >= back_len )
	     {
	            int tmp_len = 0;
	            tmp_len = back_len - (current_rd - pbuffer->pb_head);
                    pbuffer->pb_readfrom = pbuffer->pb_tail - tmp_len;
	     }
	     else
	     {
                     BUFFER_DEBUG(("CharBufferBackWrite:  must error 1\n"));                   
	     }
       }
       else
       	{
               if( current_rd - current_wr - 1 >= back_len )
               {
                      pbuffer->pb_readfrom -= back_len;
	       }
	       else
	       {
                     BUFFER_DEBUG(("CharBufferBackWrite:  must error 2\n"));
	       }
        }
  
        return xreturn;
}

void CharBufferClear( int pbuffer_v )
{
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;    

       if ( CharBufferIsValid(pbuffer)==0 )
       {
             return ;
       }
			 
      pbuffer->pb_writeto = pbuffer->pb_head;
      pbuffer->pb_readfrom = pbuffer->pb_head;
      #ifdef CONFIG_CHAR_BUFFER_CLEAR_FLAG
      pbuffer->clear_count++;
      #endif
}

void CharBufferBrush( int pbuffer_v )
{
       char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;

       if ( CharBufferIsValid(pbuffer)==0 )
       {
             return ;
       }

      //pbuffer->pb_writeto = pbuffer->pb_head;
      pbuffer->pb_readfrom = pbuffer->pb_head;
}
void CharBufferDelete( int pbuffer_v )
{
        char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       
       if ( CharBufferIsValid(pbuffer)==0 )
       {
            BUFFER_DEBUG(("CharBufferDelete:  buffer pointer error\n" ));
            return ;
       }
		
       CharBufferIdReset(pbuffer);
       BUFFER_DEBUG(("release %d bytes\n", pbuffer->buffer_size ));
       LocalPortFree( pbuffer->pb_head );
       LocalPortFree( pbuffer );
}
#ifdef CONFIG_CHAR_BUFFER_CLEAR_FLAG
int CharBufferCheckClearFlag( int pbuffer_v ) //add, 2013
{
        char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
       
       if ( CharBufferIsValid(pbuffer)==0 )
       {
            BUFFER_DEBUG(("CharBufferCheckClearFlag:  buffer pointer error\n" ));
            return -1;
       }
		
       return (int)(pbuffer->clear_count);
}
#else
int CharBufferCheckClearFlag( int pbuffer_v ) //add, 2013
{
     return 0;
}
#endif

int CharBufferDisplayParam( int pbuffer_v )
{
        char_buffer_t  *pbuffer = (char_buffer_t  *)pbuffer_v;
		
       if ( CharBufferIsValid(pbuffer)==0 )
       {
            BUFFER_DEBUG(("CharBufferDisplayParam:  buffer pointer error\n" ));
            return -1;
       }
	   
        BUFFER_DEBUG(("CharBufferDisplayParam:  %08x, %08x, %08x, %08x\n", (unsigned int)(pbuffer->pb_head), (unsigned int)(pbuffer->pb_tail),
        (unsigned int)(pbuffer->pb_readfrom), (unsigned int)(pbuffer->pb_writeto)));

        return 0;
}

///<block buffer operation
static int BlockBufferPtrIsValid(block_buffer_ptr_t *buffer_ptr_pos )
{
      int ret = -1;
      int loop_max = 256, i = 0;
      unsigned int tmp_pos = 0;	  
      unsigned char rd_valid = 0, wr_valid = 0;

      if(NULL==buffer_ptr_pos)
      {
            return -1;
      }

      if( buffer_ptr_pos->readfrom < buffer_ptr_pos->head ||buffer_ptr_pos->readfrom > buffer_ptr_pos->tail )
      {
            return -1;
      }
      if( buffer_ptr_pos->writeto < buffer_ptr_pos->head ||buffer_ptr_pos->writeto > buffer_ptr_pos->tail )
      {
            return -1;
      }	  

      tmp_pos = buffer_ptr_pos->head;
      for(i=0; i<buffer_ptr_pos->total_items; i++)
      {
            if( buffer_ptr_pos->readfrom != tmp_pos )
            {
                  tmp_pos += buffer_ptr_pos->item_size;
	    }
            else
            {
                  rd_valid = 1;
                  break;
	    }
            if(i>=loop_max)
            {
                  return -1;
	    }
      }
	  
      tmp_pos = buffer_ptr_pos->head;
      for(i=0; i<buffer_ptr_pos->total_items; i++)
      {
            if( buffer_ptr_pos->writeto != tmp_pos )
            {
                  tmp_pos += buffer_ptr_pos->item_size;
	    }
            else
            {
                  wr_valid = 1;
                  break;
	    }
            if(i>=loop_max)
            {
                  return -1;
	    }			
      }

      if(rd_valid==1 && wr_valid==1)
      {
            ret = 0;
      }

      return ret;
}
static int BlockBufferIsValid( block_buffer_t *pblock_buffer )
{
       block_buffer_id_t *curr = (block_buffer_id_t *)&block_buffer_head;
       block_buffer_id_t *ptr = curr;
       int ret = 0;

       if ( pblock_buffer==NULL )
	   return 0;

	while( ptr != NULL )
	{
             if ( ptr->used==1 )
             { 
                   if ( ptr->pblock_buffer == pblock_buffer ) //this block buffer is valid
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
void BlockBufferIDNodesDisplay(void)
{
	block_buffer_id_t *curr = (block_buffer_id_t *)&block_buffer_head;
       block_buffer_id_t *ptr = curr;

	while( ptr != NULL )
	{
	      BUFFER_DEBUG(("DisplayBlockBufferIDNodes: %s, %d, curr_bbuff=%x, next_id=%x\n", ptr->block_buffer_name, ptr->used, (int)(ptr->pblock_buffer), (int)(ptr->next)));
	      ptr = ptr->next; //pointer to next
	}

       return ;
}
static void BlockBufferIdReset(block_buffer_t *pblock_buffer)
{
	block_buffer_id_t *curr = (block_buffer_id_t *)&block_buffer_head;
       block_buffer_id_t *ptr = curr;

       if ( NULL==pblock_buffer )
       	{
               return ;
	}
	   
	while( ptr != NULL )
	{
             if ( ptr->used==1 )
             {
                   if ( pblock_buffer==ptr->pblock_buffer ) //block buffer matched
                   {
                         //delete node of ptr which is not head
                         if ( ptr != (block_buffer_id_t *)&block_buffer_head )
                         {
                               curr->next = ptr->next;
                               LocalPortFree(ptr);
                         }
                         else //you try to delete head, but we just let used=0
                         {
                              ptr->used = 0;
			 }
			 break;
		     }				   
	      }
	      curr = ptr; //save current
	      ptr = ptr->next; //pointer to next
	}
}
int BlockBufferOpen( char *buffer_name )
{
       int min_len = 0, ret = -1;
       block_buffer_id_t *curr = (block_buffer_id_t *)&block_buffer_head;
       block_buffer_id_t *ptr = curr;

       if( buffer_name==NULL )
	   return -1;

	while( ptr != NULL )
	{
	     min_len = 0;
             if ( ptr->used==1 )
             { 
                   if ( strlen(buffer_name) >= sizeof(ptr->block_buffer_name) )
                   {
                         min_len = sizeof(ptr->block_buffer_name)-1;
		   }
		   else if ( strlen(buffer_name)==strlen(ptr->block_buffer_name) )
		   {
                          min_len = strlen(ptr->block_buffer_name);
		   }
                   if ( min_len>0 && strncmp(ptr->block_buffer_name, buffer_name, min_len)==0 ) //buffer name matched
                   {
                        ret = (int)(ptr->pblock_buffer);
                        break;
                   }
	      }
	      curr = ptr; //save current
	      ptr = ptr->next; //pointer to next
	}

	return ret;
}
static int BlockBufferPointerSave(block_buffer_t  *px_new_buffer, char *buffer_name)
{
       int min_len = 0, ret = 0;
	unsigned char found = 0;
	block_buffer_id_t *curr = (block_buffer_id_t *)&block_buffer_head;
       block_buffer_id_t *ptr = curr;

	if ( NULL==px_new_buffer || NULL==buffer_name  )
	{
              return errBUFFER_POINTER_ERROR;
	}		   

        //check if new buffer name is same to someone
	while( ptr != NULL )
	{
             if ( ptr->used==1 )
             {
                   if ( strcmp(ptr->block_buffer_name, buffer_name)==0 ) //buffer exist,so don't save this new buffer
                   {
                         BUFFER_DEBUG(("BlockBufferPointerSave: same name buffer exist\n"));
			 return -1;
		   }
	      }
	      curr = ptr; //save old
	      ptr = ptr->next; //pointer to next
	}	   

        //now, buffer name is uniq
	curr = (block_buffer_id_t *)&block_buffer_head;
        ptr = curr;
	while( ptr != NULL )
	{
             if ( !ptr->used )
             {
                   ptr->used = 1;
		   if ( strlen(buffer_name) >= sizeof(ptr->block_buffer_name) )
		   {
                         min_len = sizeof(ptr->block_buffer_name);
		   }
		   else
		   {
                        min_len = strlen(buffer_name)+1;
		   }		   
                   strncpy( ptr->block_buffer_name, buffer_name, min_len-1 );
                   ptr->block_buffer_name[min_len] = 0x00;
		   ptr->pblock_buffer = px_new_buffer;
		   found = 1;
		   break;
	      }
	      curr = ptr; //save old
	      ptr = ptr->next; //pointer to next
	}

	if ( found==0 ) //all nodes are used ,so we need a new node, now, curr is the tail node
	{
	      block_buffer_id_t *tmp_node = NULL;
             tmp_node = (block_buffer_id_t *) LocalPortMalloc( sizeof( block_buffer_id_t ) );
	      if( tmp_node==NULL )
	      	{
                  ret = -1;
		}
		else
		{
		     //save new pointer into new node
                   memset( (void *)tmp_node, 0x00, sizeof(*tmp_node) );
                   tmp_node->used = 1;
		   if ( strlen(buffer_name) >= sizeof(tmp_node->block_buffer_name) )
		   {
                         min_len = sizeof(tmp_node->block_buffer_name);
		   }
		   else
		   {
                        min_len = strlen(buffer_name)+1;
		   }	
                   strncpy( tmp_node->block_buffer_name, buffer_name, min_len-1 );
                   tmp_node->block_buffer_name[min_len] = 0x00;
		     tmp_node->pblock_buffer = px_new_buffer;		     
		     curr->next = tmp_node; //tmp_node as tail node
		}
	}

	return ret;
}
int BlockBufferCreate( unsigned int total_items, unsigned int item_size, char *buffer_name )
{
       block_buffer_t  *pnew_block_buffer;
       unsigned int buffer_size_bytes;

	if ( buffer_name==NULL )
	   return (int)NULL;

	/* Allocate the new queue structure. */
	if( total_items > ( unsigned int ) 0 )
	{
                #ifndef CONFIG_BUFFER_MUTEX_LOCK	
                    if(total_items==1)
                    {
                          total_items = 2;
                    }
                #endif
		pnew_block_buffer = (block_buffer_t *) LocalPortMalloc( sizeof( block_buffer_t ) );
		if( NULL != pnew_block_buffer )
		{
		       buffer_size_bytes = ( unsigned int  ) ( total_items * (item_size+4) ) + ( unsigned int  ) 1;
			pnew_block_buffer->pb_head = ( unsigned char * ) LocalPortMalloc( buffer_size_bytes );
			if( NULL != pnew_block_buffer->pb_head )
			{
				pnew_block_buffer->pb_tail = pnew_block_buffer->pb_head + ( total_items * (item_size+4) );
				pnew_block_buffer->pb_writeto = pnew_block_buffer->pb_head;
				pnew_block_buffer->pb_readfrom = pnew_block_buffer->pb_head + ( ( total_items - 1 ) * (item_size+4) );
			        pnew_block_buffer->items_waiting = 0;
                                pnew_block_buffer->total_items = total_items;
                                pnew_block_buffer->item_size = item_size+4;
				#ifdef CONFIG_BUFFER_MUTEX_LOCK
			        pthread_mutex_init(&pnew_block_buffer->block_rw_lock, NULL);
				#endif

				if ( BlockBufferPointerSave(pnew_block_buffer, buffer_name) == 0 ) //save ok
				{
				     BUFFER_DEBUG(("BlockBufferCreate: ptr=%x,buff_name=%s\n", (unsigned int)(pnew_block_buffer), buffer_name));
				     return  (int)pnew_block_buffer;
				}
				else
				{
				      LocalPortFree(pnew_block_buffer->pb_head);
                                      LocalPortFree( pnew_block_buffer );
				}
			}
			else
			{
				LocalPortFree( pnew_block_buffer );
			}
		}
	}

	/* Will only reach here if we could not allocate enough memory or no memory was required. */
	return (int)NULL;
}
static int prvCopyQueueData( block_buffer_t  *pblock_buffer, const void *pitem_to_buffer, unsigned int pitem_size )
{
       unsigned int min_len = 0;
	min_len = (pitem_size<=pblock_buffer->item_size-4) ? (pitem_size) : (pblock_buffer->item_size-4);
       *((unsigned int *)pblock_buffer->pb_writeto) = min_len; /* every item: first 4 bytes is the length */
	memcpy( (void *)(pblock_buffer->pb_writeto+4), pitem_to_buffer, min_len );
	++( pblock_buffer->items_waiting);
	pblock_buffer->pb_writeto += pblock_buffer->item_size;
	if( pblock_buffer->pb_writeto >= pblock_buffer->pb_tail )
	{
		pblock_buffer->pb_writeto = pblock_buffer->pb_head;
	}

    return min_len;
}
int BlockBufferWrite( int pblock_buffer_v, const void *psrc_buffer, unsigned int src_buffer_size )
{
       int xreturn = 0;
       block_buffer_t  *pblock_buffer = (block_buffer_t  *)pblock_buffer_v;

	if ( BlockBufferIsValid(pblock_buffer) == 0 || NULL==psrc_buffer )
	{
              return errBUFFER_POINTER_ERROR;
	}
	
#ifdef CONFIG_BUFFER_MUTEX_LOCK
       pthread_mutex_lock(&pblock_buffer->block_rw_lock);
        if( 1==pblock_buffer->total_items ) //only one block, so replace
        {
                if(pblock_buffer->items_waiting == 1 )
                {
                       BUFFER_DEBUG(("BlockBufferWrite: cmd replaced\n"));
                }
                xreturn = prvCopyQueueData( pblock_buffer, psrc_buffer, src_buffer_size );
                if(pblock_buffer->items_waiting >= 1)
                {
                     pblock_buffer->items_waiting = 1;
                }
        }
        else if( pblock_buffer->items_waiting < pblock_buffer->total_items )
        {
             /* There is room in the queue, copy the data into the queue. */
             xreturn = prvCopyQueueData( pblock_buffer, psrc_buffer, src_buffer_size );
        }
        else
        {
              BUFFER_DEBUG(("BlockBufferWrite: block buffer full\n"));
	      xreturn = errBUFFER_FULL;
        }
       pthread_mutex_unlock(&pblock_buffer->block_rw_lock);
#else ///< no lock
{
        unsigned int tmp_readfrom = 0, tmp_writeto = 0;
	unsigned int delta = 0, items_waiting = 0;
        block_buffer_ptr_t buffer_ptr_pos;
        int local_ret = 0, i = 0;
        int max_loop = 255; ///< total items < 255
		
	tmp_readfrom = (unsigned int)pblock_buffer->pb_readfrom;
        tmp_writeto = (unsigned int)pblock_buffer->pb_writeto;
        delta = pblock_buffer->item_size;
        buffer_ptr_pos.head = (unsigned int)pblock_buffer->pb_head;
        buffer_ptr_pos.tail = (unsigned int)pblock_buffer->pb_tail;
        buffer_ptr_pos.total_items = (unsigned int)pblock_buffer->total_items;
        buffer_ptr_pos.item_size = (unsigned int)pblock_buffer->item_size;
        buffer_ptr_pos.readfrom = tmp_readfrom;
        buffer_ptr_pos.writeto = tmp_writeto;
        local_ret = BlockBufferPtrIsValid(&buffer_ptr_pos);
        if( local_ret < 0 )
        {
              BUFFER_DEBUG(("BlockBufferWrite:  rd/wr is wrong\n"));
              return errBUFFER_RW_BUSY;
        }		
		
        if(1 != buffer_ptr_pos.total_items)
        {
		for(i=0; i<buffer_ptr_pos.total_items; i++)
		{
		      tmp_readfrom += delta;
                      if( tmp_readfrom >= (unsigned int)buffer_ptr_pos.tail )
                      {
                            tmp_readfrom = (unsigned int)buffer_ptr_pos.head;
                      }
		      if(tmp_readfrom != tmp_writeto)
		      {
                            items_waiting++;
		      }
		      else
		      {
                            break;
		      }
			  
                      if(i>=max_loop)
                      {
                           i = buffer_ptr_pos.total_items; ///< busy
			   break;
                      }
		}
	        if(i==buffer_ptr_pos.total_items)
	        {
                       BUFFER_DEBUG(("BlockBufferWrite:  busy, don't read this time\n"));
		       return errBUFFER_RW_BUSY;
	        }
	}

        if(1==pblock_buffer->total_items) //only one block, so replace
        {
                if(items_waiting == 1)
                {
                       BUFFER_DEBUG(("BlockBufferWrite: cmd replaced\n"));
                }
                xreturn = prvCopyQueueData( pblock_buffer, psrc_buffer, src_buffer_size );
        }
        else if( items_waiting < pblock_buffer->total_items-1 ) ///< when rd=wr,don't write this time
        {
             /* There is room in the queue, copy the data into the queue. */
             xreturn = prvCopyQueueData( pblock_buffer, psrc_buffer, src_buffer_size );
        }
        else
        {
              BUFFER_DEBUG(("BlockBufferWrite: block buffer full\n"));
	      xreturn = errBUFFER_FULL;
        }
}
#endif	   

	return xreturn;
}
int BlockBufferRead( int pblock_buffer_v, void *pdest_buffer, unsigned int pdest_buffer_size )
{
       int xreturn = 0;
       block_buffer_t  *pblock_buffer  = (block_buffer_t  *)pblock_buffer_v; 
	   
	if ( BlockBufferIsValid(pblock_buffer) == 0 || NULL==pdest_buffer )
	{
              return errBUFFER_POINTER_ERROR;
	}	   

#ifdef CONFIG_BUFFER_MUTEX_LOCK
       pthread_mutex_lock(&pblock_buffer->block_rw_lock);
       {
	      unsigned int min_len = 0, real_len = 0;
             if( pblock_buffer->items_waiting > ( unsigned int ) 0 )
             {
                  pblock_buffer->pb_readfrom  += pblock_buffer->item_size; /* firstly, point to next item */
                  if( pblock_buffer->pb_readfrom >= pblock_buffer->pb_tail )
                  {
                      pblock_buffer->pb_readfrom = pblock_buffer->pb_head;
                  }

		  real_len = *((unsigned int *)pblock_buffer->pb_readfrom);
		  min_len = (pdest_buffer_size<=real_len) ? (pdest_buffer_size) : (real_len);
                  memcpy( (void *)pdest_buffer, (void *)(pblock_buffer->pb_readfrom+4), min_len );
                  --( pblock_buffer->items_waiting );
                  xreturn = min_len;
             }
             else
             {
                 xreturn = errBUFFER_EMPTY;
             }
       }
      pthread_mutex_unlock(&pblock_buffer->block_rw_lock);
#else
{
        unsigned int tmp_readfrom = 0, tmp_writeto = 0;
        unsigned int delta = 0, items_waiting = 0;
        unsigned int min_len = 0, real_len = 0;
	int local_ret = 0, i = 0;
	int max_loop = 255; ///< total items < 255
        block_buffer_ptr_t buffer_ptr_pos;
        
        tmp_readfrom = (unsigned int)pblock_buffer->pb_readfrom;
        tmp_writeto = (unsigned int)pblock_buffer->pb_writeto;
        delta = pblock_buffer->item_size;
        buffer_ptr_pos.head = (unsigned int)pblock_buffer->pb_head;
        buffer_ptr_pos.tail = (unsigned int)pblock_buffer->pb_tail;
        buffer_ptr_pos.total_items = (unsigned int)pblock_buffer->total_items;
        buffer_ptr_pos.item_size = (unsigned int)pblock_buffer->item_size;
        buffer_ptr_pos.readfrom = tmp_readfrom;
        buffer_ptr_pos.writeto = tmp_writeto;
        local_ret = BlockBufferPtrIsValid(&buffer_ptr_pos);
        if( local_ret < 0 )
        {
              BUFFER_DEBUG(("BlockBufferRead:  rd/wr is wrong\n"));
              return errBUFFER_RW_BUSY;
        }

        if(1 != buffer_ptr_pos.total_items)
        {
		for(i=0; i<buffer_ptr_pos.total_items; i++)
		{
		      tmp_readfrom += delta;
                      if( tmp_readfrom >= (unsigned int)buffer_ptr_pos.tail )
                      {
                            tmp_readfrom = (unsigned int)buffer_ptr_pos.head;
                      }
		      if(tmp_readfrom != tmp_writeto)
		      {
                            items_waiting++;
		      }
		      else
		      {
                            break;
		      }
                      if(i >= max_loop)
                      {
                            i = buffer_ptr_pos.total_items; ///< busy
			    break;
                      }
		}
	        if(i==buffer_ptr_pos.total_items)
	        {
                       BUFFER_DEBUG(("BlockBufferRead:  busy, don't read this time\n"));
		       return errBUFFER_RW_BUSY;
	        }
	}

        if(items_waiting > ( unsigned int ) 0 )
        {
               pblock_buffer->pb_readfrom  += pblock_buffer->item_size; /* firstly, point to next item */
               if( pblock_buffer->pb_readfrom >= pblock_buffer->pb_tail )
               {
                     pblock_buffer->pb_readfrom = pblock_buffer->pb_head;
               }
               
               real_len = *((unsigned int *)pblock_buffer->pb_readfrom);
               min_len = (pdest_buffer_size<=real_len) ? (pdest_buffer_size) : (real_len);
               memcpy( (void *)pdest_buffer, (void *)(pblock_buffer->pb_readfrom+4), min_len );
               --( pblock_buffer->items_waiting );
               xreturn = min_len;
        }
        else
        {
              xreturn = errBUFFER_EMPTY;
        }
}
#endif

      return xreturn;
}
void BlockBufferClear( int pblock_buffer_v )
{
       block_buffer_t  *pblock_buffer = (block_buffer_t  *)pblock_buffer_v;
	   
       if ( BlockBufferIsValid(pblock_buffer) == 0)
	   return ;	

#ifdef CONFIG_BUFFER_MUTEX_LOCK	   
       pthread_mutex_lock(&pblock_buffer->block_rw_lock);
#endif
	 pblock_buffer->items_waiting = 0;
         pblock_buffer->pb_writeto = pblock_buffer->pb_head;
         pblock_buffer->pb_readfrom = pblock_buffer->pb_head + ( ( pblock_buffer->total_items - 1 ) * pblock_buffer->item_size );
#ifdef CONFIG_BUFFER_MUTEX_LOCK		 
       pthread_mutex_unlock(&pblock_buffer->block_rw_lock);
#endif

       return ;
}
void BlockBufferDelete( int pblock_buffer_v )
{
       block_buffer_t  *pblock_buffer = (block_buffer_t  *)pblock_buffer_v;
	   
       if ( BlockBufferIsValid(pblock_buffer) == 0)
	   return ;

#ifdef CONFIG_BUFFER_MUTEX_LOCK		   
       pthread_mutex_lock(&pblock_buffer->block_rw_lock);
#endif
	BlockBufferIdReset(pblock_buffer);
	LocalPortFree( pblock_buffer->pb_head );
#ifdef CONFIG_BUFFER_MUTEX_LOCK		
       pthread_mutex_unlock(&pblock_buffer->block_rw_lock);
#endif

	LocalPortFree( pblock_buffer );
}
int BlockBufferFull( const  int pblock_buffer_v )
{
      int xreturn;
      block_buffer_t  *pblock_buffer = (block_buffer_t  *)pblock_buffer_v;	  

       if ( BlockBufferIsValid(pblock_buffer) == 0)
	   return -1;
	   
#ifdef CONFIG_BUFFER_MUTEX_LOCK	
       pthread_mutex_lock((pthread_mutex_t *)&pblock_buffer->block_rw_lock);
        xreturn = ( pblock_buffer->items_waiting == pblock_buffer->total_items );
       pthread_mutex_unlock((pthread_mutex_t *)&pblock_buffer->block_rw_lock);
#else
{
        unsigned int tmp_readfrom = 0, tmp_writeto = 0;
        unsigned int delta = 0, items_waiting = 0;	
        
        tmp_readfrom = (unsigned int)pblock_buffer->pb_readfrom;
        tmp_writeto = (unsigned int)pblock_buffer->pb_writeto;
        delta = pblock_buffer->item_size;

        if(1 != pblock_buffer->total_items)
        {
		for(;;)
		{
		      tmp_readfrom += delta;
                      if( tmp_readfrom >= (unsigned int)pblock_buffer->pb_tail )
                      {
                            tmp_readfrom = (unsigned int)pblock_buffer->pb_head;
                      }
		      if(tmp_readfrom != tmp_writeto)
		      {
                            items_waiting++;
		      }
		      else
		      {
                            break;
		      }
		}
	}
        xreturn = (items_waiting == pblock_buffer->total_items);
}
#endif	   

	return xreturn;
}
int BlockBufferCnt( const int pblock_buffer_v )
{
      int xreturn;
      block_buffer_t  *pblock_buffer = (block_buffer_t  *)pblock_buffer_v;	  

       if ( BlockBufferIsValid(pblock_buffer) == 0)
	   return -1;	   
	   
#ifdef CONFIG_BUFFER_MUTEX_LOCK
       pthread_mutex_lock((pthread_mutex_t *)&pblock_buffer->block_rw_lock);
       xreturn = (int)( pblock_buffer->items_waiting );
       pthread_mutex_unlock((pthread_mutex_t *)&pblock_buffer->block_rw_lock);
#else
{
        unsigned int tmp_readfrom = 0, tmp_writeto = 0;
        unsigned int delta = 0, items_waiting = 0;	
        
        tmp_readfrom = (unsigned int)pblock_buffer->pb_readfrom;
        tmp_writeto = (unsigned int)pblock_buffer->pb_writeto;
        delta = pblock_buffer->item_size;

        if(1 != pblock_buffer->total_items)
        {
		for(;;)
		{
		      tmp_readfrom += delta;
                      if( tmp_readfrom >= (unsigned int)pblock_buffer->pb_tail )
                      {
                            tmp_readfrom = (unsigned int)pblock_buffer->pb_head;
                      }
		      if(tmp_readfrom != tmp_writeto)
		      {
                            items_waiting++;
		      }
		      else
		      {
                            break;
		      }
		}
	}
        xreturn = items_waiting;
}
#endif	   

	return xreturn;
}
int BlockBufferEmpty( const int pblock_buffer_v )
{
      int xreturn;
      block_buffer_t  *pblock_buffer = (block_buffer_t  *)pblock_buffer_v;

       if ( BlockBufferIsValid(pblock_buffer) == 0)
	   return -1;	  

#ifdef CONFIG_BUFFER_MUTEX_LOCK
     pthread_mutex_lock((pthread_mutex_t *)&pblock_buffer->block_rw_lock);
      xreturn = ( pblock_buffer->items_waiting == ( unsigned int ) 0 );
     pthread_mutex_unlock((pthread_mutex_t *)&pblock_buffer->block_rw_lock);
#else
{
        unsigned int tmp_readfrom = 0, tmp_writeto = 0;
        unsigned int delta = 0, items_waiting = 0;	
        
        tmp_readfrom = (unsigned int)pblock_buffer->pb_readfrom;
        tmp_writeto = (unsigned int)pblock_buffer->pb_writeto;
        delta = pblock_buffer->item_size;

        if(1 != pblock_buffer->total_items)
        {
		for(;;)
		{
		      tmp_readfrom += delta;
                      if( tmp_readfrom >= (unsigned int)pblock_buffer->pb_tail )
                      {
                            tmp_readfrom = (unsigned int)pblock_buffer->pb_head;
                      }
		      if(tmp_readfrom != tmp_writeto)
		      {
                            items_waiting++;
		      }
		      else
		      {
                            break;
		      }
		}
	}
        xreturn = (items_waiting==0);
}
#endif	 

     return xreturn;
}

