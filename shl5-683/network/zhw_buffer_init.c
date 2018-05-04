/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� zhw_buffer_init.c
 * ���������������
 * ���    ������ ���������������
 *
 * ��������������� V1.0.1
 * ���    ������ zhouwei
 * ���������������
 *
 * ���������������v1.0.0
 * ���������  ���zhouwei
 * ���������������2012-10-08
 *
 */

#include "zhw_buffer_init.h"

static unsigned char local_mutex_init_finish = 0; ///< add, 2013
extern pthread_mutex_t zhw_buffer_register_lock;

buffer_info_t buffer_info_head =
{
		0,
		0,
		0,
		0,
		NULL,
};

buffer_info_t *buffer_info_point ;
buffer_info_t *buffer_info_prt = &buffer_info_head;

//buffer_info_prt = &buffer_info_head;


/*
 *   	@brief	���������������������:���������������������buffer���������������������buffer���������������������������������������������������������
 *  	@param pkg_type ���������������������
 *     @param buffer_type ���������buffer������������1������������buffer���2������������buffer���
 *     @param buffer_id ������������������buffer���
 *     @return 1:���������������������-1���������buffer_id���������������-2������������������������
 */
int ZhwBufferRegister_zhw(int common_type,unsigned int pkg_type,unsigned int buffer_type, int buffer_id)
{
	buffer_info_point = &buffer_info_head;

	while(buffer_info_point != NULL)
	{
		if(buffer_info_point->buffer_id == buffer_id)
		{
//			net_debug_printf(("run out ZhwBufferInit() function the new id already in the list\n"));
			return -1; //buffer_id ������������
		}
		buffer_info_point = buffer_info_point->next;
	}

	buffer_info_prt->next = (buffer_info_t *)malloc(sizeof(buffer_info_t));
	buffer_info_prt = buffer_info_prt->next ;
	if(buffer_info_prt != NULL)
	{
		buffer_info_prt->common_type = common_type;
		buffer_info_prt->pkg_type = pkg_type;
		buffer_info_prt->buffer_type = buffer_type;
		buffer_info_prt->buffer_id = buffer_id;
		buffer_info_prt->next = NULL;
		return 1;
	}

//	net_debug_printf(("run out ZhwBufferInit() function the new id add fail\n"));
   return -2;

}

int ZhwBufferRegister(int common_type,unsigned int pkg_type,unsigned int buffer_type, int buffer_id)
{
       buffer_info_t *tmp_buffer_info_point = &buffer_info_head;	   
       buffer_info_t *new_buffer_info_ptr = NULL;

	while(tmp_buffer_info_point->next != NULL)
	{
		if(tmp_buffer_info_point->buffer_id == buffer_id)
		{
//			net_debug_printf(("run out ZhwBufferInit() function the new id already in the list\n"));
			return -1;
		}
		tmp_buffer_info_point = tmp_buffer_info_point->next;
	}
	new_buffer_info_ptr = (buffer_info_t *)malloc(sizeof(buffer_info_t));
        if(NULL == new_buffer_info_ptr)
        {
              return -1;
        }
        new_buffer_info_ptr->common_type = common_type;
        new_buffer_info_ptr->pkg_type = pkg_type;
        new_buffer_info_ptr->buffer_type = buffer_type;
        new_buffer_info_ptr->buffer_id = buffer_id;
        new_buffer_info_ptr->next = NULL;
		
	pthread_mutex_lock(&zhw_buffer_register_lock);
	tmp_buffer_info_point->next = new_buffer_info_ptr;
	pthread_mutex_unlock(&zhw_buffer_register_lock);		

       return 0;
}

/*
 *   	@brief	������������������������������������������������������������buffer������������������������������������buffer id���
 *  	@param pkg_type ������������������������
 *     @param buffer_type ���������������buffer���������1������������buffer���2������������buffer
 *
 *     @return   int : ������0���������������������������������������������buffer_id
 */
int ZhwBufferInq(int common_type,int pkg_type,int buffer_type)
{
	buffer_info_point = &buffer_info_head;
	while(buffer_info_point != NULL)
	{
		if(common_type == buffer_info_point->common_type && pkg_type == buffer_info_point->pkg_type && buffer_type == buffer_info_point->buffer_type)
		{
			return buffer_info_point->buffer_id;
		}
		else
		{
			buffer_info_point = buffer_info_point->next;
		}
	}

	return -1;
}

