/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： buffer_read_write_fun.h
 * 文件标识：
 * 摘    要：1. 字节缓冲区接口：创建、读、写、销毁等函数
 * 			 2. 块缓冲区接口：创建、读、写、销毁等函数; 注意: 不能在中断中调用块缓冲区接口
 *
 *
 *
 * 当前版本： V1.0.1
 * 作    者： AlvinChang
 * 完成日期：2012-09-28
 *
 * 取代版本： V1.0.0
 * 原作者  ： AlvinChang
 * 完成日期：2012-08-20
 */

#ifndef BUFFER_READ_WRITE_FUN_H_
#define BUFFER_READ_WRITE_FUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>


#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef errBUFFER_FULL
#define errBUFFER_FULL -1
#endif

#ifndef errBUFFER_EMPTY
#define errBUFFER_EMPTY -2
#endif

#ifndef errBUFFER_POINTER_ERROR
#define errBUFFER_POINTER_ERROR -3
#endif

#ifndef errBUFFER_RW_BUSY
#define errBUFFER_RW_BUSY -4
#endif

#define  CONFIG_CHAR_BUFFER_CLEAR_FLAG  //add, 2013

#define BUFFER_NAME_LEN    64  // 理解为文件名长度,字节

//#define AUDIO_BUFFER_SIZE                                    (1024)
#define DEFAULT_AUDIO_BUFFER_SIZE                 (1024*8)
#define DEFAULT_BUFFER_TOTAL_BLOCKS         (16)

//#ifndef CONFIG_BUFFER_MUTEX_LOCK
//#define CONFIG_BUFFER_MUTEX_LOCK
//#endif

//字节缓冲区结构体
typedef struct CharBufferDefinition
{
	unsigned char *pb_head; ///<存放数据的首地址
	unsigned char *pb_tail; ///<存放数据的尾地址
	unsigned char *pb_writeto; ///<可以写数据的当前位置
	unsigned char *pb_readfrom; ///<可以读数据的当前位置
	unsigned int  buffer_size; ///<可以存放数据的总字节数
	#ifdef CONFIG_CHAR_BUFFER_CLEAR_FLAG
	unsigned char clear_count; ///<add, 2013
        #endif	
}char_buffer_t;

//块缓冲区结构体
typedef struct BlockBufferDefinition
{
	unsigned char *pb_head; ///<存放数据的首地址
	unsigned char *pb_tail; ///<存放数据的尾地址
	unsigned char *pb_writeto; ///<可以写数据的当前块首地址
	unsigned char *pb_readfrom; ///<上次读过数据的那个块的首位置
	unsigned int    items_waiting; ///<缓冲区内当前已经保存下来的块总数
	unsigned int    total_items; ///<缓冲区内额定的块总数
	unsigned int    item_size;  ///<每块能容纳的字节数(其中头4个字节表示本块后面实际字节数)
        #ifdef CONFIG_BUFFER_MUTEX_LOCK
        pthread_mutex_t block_rw_lock; ///<本缓冲区的读写同步锁
        #endif
} block_buffer_t;

//使用ID号,保存多个字节缓冲区指针
struct CharBufferIdType
{
   char buffer_name[BUFFER_NAME_LEN];
   char_buffer_t *pdata_buffer;
   unsigned char used;
   struct CharBufferIdType *next;   
};
typedef struct CharBufferIdType char_buffer_id_t;

//使用ID号,保存多个块缓冲区指针
struct BlockBufferIdType
{
   char block_buffer_name[BUFFER_NAME_LEN];
   block_buffer_t *pblock_buffer;
   unsigned char used;
   struct BlockBufferIdType *next;   
};
typedef struct BlockBufferIdType block_buffer_id_t;

typedef struct BlockBufferPtrType
{
	unsigned int head; ///<
	unsigned int tail; ///<
	unsigned int writeto; ///<
	unsigned int readfrom; ///<
	unsigned int total_items; ///<
	unsigned int item_size;  ///<        
}block_buffer_ptr_t;


/*********************************************************
 ** @brief  打印字节缓冲区ID链表中所有节点信息，本函数是调试函数
 ** @param  
 ** @return
 ** @see
 ** @note： 
 ********************************************************/
void CharBufferIDNodesDisplay(void);

/*********************************************************
 ** @brief   打开一个字节缓冲区
 ** @param   buffer_name  缓冲区名称(可理解为文件名)
 ** @return  字节缓冲区ID
 ** @return  -1  打开时出错
 ** @see     用法类似open()
 ** @note： 
 ********************************************************/
int CharBufferOpen( char *buffer_name );

/*********************************************************
 ** @brief   创建一个字节缓冲区
 ** @param   buffer_size  存放字节数据的大小(字节数)
 ** @param   buffer_name  缓冲区名称(可理解为文件名)
 ** @return  字节缓冲区ID
 ** @return  -1  创建时失败
 ** @see
 ** @note： 
 ********************************************************/
int CharBufferCreate( unsigned int buffer_size, char *buffer_name );

/*********************************************************
 ** @brief  将用户数据送入字节缓冲区
 ** @param  char_buffer_id  缓冲区ID
 ** @param  psrc_buffer  用户源数据首地址
 ** @param  data_real_size  用户源数据实际长度
 ** @return   >0  实际写入缓冲区的长度(字节数)
 ** @return   -3  表示参数中有空指针
 ** @return   -1: 表示缓冲区已满
 ** @see
 ** @note               
 ********************************************************/
int CharBufferWrite( int char_buffer_id, const void *psrc_buffer, unsigned int data_real_size );

/*********************************************************
 ** @brief  从字节缓冲区，读出数据，放入用户区
 ** @param  char_buffer_id   缓冲区ID
 ** @param  pdest_buffer  用户区首地址
 ** @param  dest_buffer_size  用户区最大长度,通常指数组长度等
 ** @return   >0  实际读出来的数据长度(字节数)
 ** @return   -3  表示参数中有空指针
 ** @return   -2  表示缓冲区已空
 ** @see
 ** @note          
 ********************************************************/
int CharBufferRead( int char_buffer_id, void *pdest_buffer, unsigned int dest_buffer_size );

/*********************************************************
 ** @brief  清空字节缓冲区
 ** @param  char_buffer_id   字节缓冲区ID
 ** @return
 ** @see 
 ** @note  
 ********************************************************/
void CharBufferClear( int char_buffer_id );

/*********************************************************
 ** @brief  清空字节缓冲区
 ** @param  char_buffer_id   字节缓冲区ID
 ** @return
 ** @see
 ** @note
 ********************************************************/
void CharBufferBrush( int pbuffer_v );

/*********************************************************
 ** @brief  释放字节缓冲区占用的内存空间
 ** @param  char_buffer_id  字节缓冲区ID
 ** @return   
 ** @see
 ** @note
 ********************************************************/
void CharBufferDelete( int char_buffer_id );

/*********************************************************
 ** @brief   check if this buffer had been cleared last time
 ** @param  char_buffer_id  
 ** @return   
 ** @see
 ** @note: add, 2013
 ********************************************************/
int CharBufferCheckClearFlag( int pbuffer_v );

/*********************************************************
 ** @brief     This function is called by MIC/SoundCard  
 ** @param   pbuffer_v    This is a char buffer ID
 ** @param   pcurrent_total_bytes    This is a pointer to user's variable. when quit this function,user will get current total bytes in the buffer
 ** @return    return current readable position
 ** @see
 ** @note
 ********************************************************/
 //void *CharBufferCurrentReadPointer( int pbuffer_v, unsigned int *pcurrent_total_bytes );
 void *CharBufferCurrentReadPointer( int pbuffer_v, unsigned int *pcurrent_total_bytes,unsigned int *pcurrent_right_delta);
void *CharBufferCurrentReadPointer_temp( int pbuffer_v, unsigned int *pcurrent_total_bytes,unsigned int *pcurrent_right_delta); 
/*********************************************************
 ** @brief     This function is called by MIC/SoundCard  
 ** @param   pbuffer_v     It is a char buffer ID
 ** @param   origin_len     It is length before called
 ** @param   left_len         It is left length
 ** @return   
 ** @see
 ** @note
 ********************************************************/
void CharBufferUpdateCurrentReadPointer( int pbuffer_v, unsigned int origin_len, unsigned int left_len );
void CharBufferUpdateCurrentReadPointer2( int pbuffer_v, unsigned int origin_len, unsigned int left_len, unsigned char last_clear_flag );


/*********************************************************
 ** @brief 
 ** @param  pbuffer_v
 ** @return   >0:   caller can write some bytes into buffer
 ** @return   <=0:   caller can,t write bytes into buffer
 ** @see 
 ** @note  
 ********************************************************/
int CharBufferIsFull( int pbuffer_v );

/*********************************************************
 *  SingleThreadCharBufferBackWrite
 *
 *  @param :  pbuffer_v
 *  @return:  back_len
 *  @
 *  @note:   This function is just used in the single thread after CharBufferRead called and before CharBufferWrite
 *  @note:    This funciton is just called when device is busy.
 ********************************************************/
int SingleThreadCharBufferBackWrite( int pbuffer_v, int back_len );


//block buffer is for BCU response

/*********************************************************
 ** @brief  打印当前块缓冲区链表中的所有节点信息，本函数是调试函数
 ** @param 
 ** @return
 ** @see 
 ** @note 
 ********************************************************/
void BlockBufferIDNodesDisplay(void);

/*********************************************************
 ** @brief   打开一个块缓冲区
 ** @param   buffer_name  缓冲区名称
 ** @return  块缓冲区ID
 ** @see 
 ** @note 
 ********************************************************/
int BlockBufferOpen( char *buffer_name );

/*********************************************************
 ** @brief   创建一个块缓冲区
 ** @param   total_items  缓冲区含有总块数
 ** @param   item_size    每个块能容纳的用户数据字节数
 ** @param   buffer_name  缓冲区名称
 ** @return  块缓冲区ID
 ** @see 
 ** @note 
 ********************************************************/
int BlockBufferCreate( unsigned int total_items, unsigned int item_size, char *buffer_name );

/*********************************************************
 ** @brief  将用户数据，加入到缓冲区中的某一个块中
 ** @param  block_buffer_id   缓冲区结构体指针
 ** @param  psrc_buffer  用户数据首地址
 ** @param  data_real_size  用户数据实际长度
 ** @return  >0  实际写入的长度(字节数)
 ** @return  -3  表示参数中有空指针
 ** @return  -1  表示缓冲区已满
 ** @see 
 ** @note 
 ********************************************************/
int BlockBufferWrite( int block_buffer_id, const void *psrc_buffer, unsigned int data_real_size );

/*********************************************************
 ** @brief  从缓冲区的某一个块中，读出数据，送入用户区
 ** @param  block_buffer_id   缓冲区结构体指针
 ** @param  pdest_buffer   用户区首地址
 ** @param  dest_buffer_size  用户区最大长度,通常指数组长度等
 ** @return  >0  实际读出的长度(字节数)
 ** @return  -3  表示参数中有空指针
 ** @return  -2  表示缓冲区已空
 ** @see 
 ** @note                
 ********************************************************/
int BlockBufferRead( int block_buffer_id, void *pdest_buffer, unsigned int pdest_buffer_size );

/*********************************************************
 ** @brief  释放块缓冲区的内存空间
 ** @param  block_buffer_id   缓冲区结构体指针        
 ** @return  
 ** @see 
 ** @note              
 ********************************************************/
void BlockBufferClear( int block_buffer_id );

/*********************************************************
 ** @brief  释放块缓冲区的内存空间
 ** @param  block_buffer_id   缓冲区结构体指针        
 ** @return  
 ** @see 
 ** @note              
 ********************************************************/
void BlockBufferDelete( int block_buffer_id );

/*********************************************************
 ** @brief  判断块缓冲区是否已经满
 ** @param  block_buffer_id  缓冲区结构体指针
 ** @return  0  缓冲区未满，还可以写入
 ** @return  1  缓冲区已满，不能再写入
 ** @see 
 ** @note                
 ********************************************************/
int BlockBufferFull( const int block_buffer_id );

/*********************************************************
 ** @brief  判断块缓冲区保存了几个可读的块 
 ** @param  block_buffer_id  缓冲区结构体指针
 ** @return  >0 可读的块数
 ** @return  0  当前没有可读的块,即缓冲区已空
 ** @see 
 ** @note               
 ********************************************************/
int BlockBufferCnt( const int block_buffer_id );

/*********************************************************
 ** @brief  判断块缓冲区是否已空 
 ** @param  block_buffer_id  缓冲区ID
 ** @return  0   缓冲区未空，可读
 ** @return  1   缓冲区已空
 ** @see 
 ** @note                  
 ********************************************************/
int BlockBufferEmpty( const int block_buffer_id );

#endif /* BUFFER_READ_WRITE_FUN_H_ */
