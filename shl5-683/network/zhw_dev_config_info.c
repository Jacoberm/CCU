/*
 * Copyright (c) 2012���MIT
 * All right reserved.
 *
 * ��������������� zhw_dev_config_info.c
 * ���������������
 * ���    ������ ���������������������������
 *
 * ��������������� V1.0.0
 * ���    ������ ������
 * ���������������2013-03-26
 *
 * ���������������
 * ���������  ���
 * ���������������
 *
 */
#include "zhw_dev_config_info.h"

#define BCU_DEV

#ifdef BCU_DEV
int update_start = 0 ;
pthread_mutex_t mutex_update_ip_array;
#endif

#define DEV_NUMBER_CHANGE(source,source_old_num,source_new_num) \
	case (source_old_num): \
	(source) = (source_new_num); \
	break;


//struct dev_type_and_no local_dev;
global_array_t *global_array_ptr = NULL;
global_array_t *global_array_ptr_head = NULL;
pthread_mutex_t zhw_buffer_register_lock;
//static global_array_t global_array_head =
//{
//		NULL,
//		NULL
//};

int row;
int column;

int eamp_number = 0;
int bcu_number = 0;
int ccu_number = 0;
int pcu_number = 0;
int cmu_number = 0;

int db_err_status = 0 ;

int ZhwUpdateBCUIpArray(ip_mac_table_row_t *pip_mac_table_row)
{
	char *save_point_start = NULL ;
	char *save_point_middle = NULL ;
	char *save_point_end = NULL ;
	char a = 0;
	char b = 0;
	int length = 0 ;
	int i ;

#ifdef BCU_DEV
	pthread_mutex_lock(&mutex_update_ip_array);
#endif
	global_array_ptr = global_array_ptr_head;

	while(global_array_ptr != NULL)
	{
		if((global_array_ptr->global_config_info.dev_type == pip_mac_table_row->current_row_info.device_type)
				&&(global_array_ptr->global_config_info.dev_number == pip_mac_table_row->current_row_info.device_no))
		{
			global_array_ptr->global_config_info.is_server = pip_mac_table_row->current_row_info.server_whether;
			strcpy((char *)global_array_ptr->global_config_info.dev_name,(char *)pip_mac_table_row->current_row_info.device_name);
			strcpy((char *)global_array_ptr->global_config_info.dev_ip,(char *)pip_mac_table_row->current_row_info.ip_address);
//			strcpy((char *)global_array_ptr->global_config_info.dev_mac,(char *)pip_mac_table_row->current_row_info.mac_address);

			{
				save_point_start = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.mac_address));
				bzero(save_point_start,sizeof(pip_mac_table_row->current_row_info.mac_address));
				memcpy(save_point_start,pip_mac_table_row->current_row_info.mac_address,sizeof(pip_mac_table_row->current_row_info.mac_address));
				save_point_middle = save_point_start;
				save_point_end = save_point_start;
				while(save_point_end != NULL)
				{
					save_point_end = strstr(save_point_middle,",");

					length = (int)(strlen(save_point_middle) - strlen(save_point_end));

					if( i < 6 )
					{
						if(length  == 2)
						{
							a = *(save_point_middle);
							save_point_middle++;
							b = *(save_point_middle);
							global_array_ptr->global_config_info.dev_mac[i] = (16 * (a-48)) + (b-48) ;
						}
						else if ( (length > 0) && (length < 2) )
						{
							memcpy(&b,save_point_middle,sizeof(b));
							global_array_ptr->global_config_info.dev_mac[i] = atoi(&b) ;
						}

						i++ ;
					}
					save_point_middle = (save_point_end + 1) ;
				}
//				diag_printf("zhw-11111111-info:%s\n",save_point_start);
				free(save_point_start); // add by zhw-2014-12-29
				save_point_start = NULL ;
//				diag_printf("zhw-33333333\n");
			}

			global_array_ptr = global_array_ptr_head;
#ifdef BCU_DEV
			pthread_mutex_unlock(&mutex_update_ip_array);
#endif
			return 1;
		}
		else
		{
			if(global_array_ptr->next != NULL)
			{
				global_array_ptr = global_array_ptr->next;
			}
			else
			{
				break;
			}
		}
	}
	global_array_ptr->next = (global_array_t *)malloc(sizeof(global_array_t));
	global_array_ptr = global_array_ptr->next;
	if(global_array_ptr != NULL)
	{
		bzero(global_array_ptr,sizeof(global_array_t));
		global_array_ptr->global_config_info.dev_number = pip_mac_table_row->current_row_info.device_no;
		global_array_ptr->global_config_info.dev_type = pip_mac_table_row->current_row_info.device_type;
		global_array_ptr->global_config_info.is_server = pip_mac_table_row->current_row_info.server_whether;

		global_array_ptr->global_config_info.dev_name = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.device_name));
		bzero(global_array_ptr->global_config_info.dev_name,sizeof(pip_mac_table_row->current_row_info.device_name));
		strcpy((char *)global_array_ptr->global_config_info.dev_name,(char *)pip_mac_table_row->current_row_info.device_name);

		global_array_ptr->global_config_info.dev_ip = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.ip_address));
		bzero(global_array_ptr->global_config_info.dev_ip,sizeof(pip_mac_table_row->current_row_info.ip_address));
		strcpy((char *)global_array_ptr->global_config_info.dev_ip,(char *)pip_mac_table_row->current_row_info.ip_address);

//		strcpy((char *)global_array_ptr->global_config_info.dev_mac,(char *)pip_mac_table_row->current_row_info.mac_address);
		{
			save_point_start = (char *)malloc(sizeof(pip_mac_table_row->current_row_info.mac_address));
			bzero(save_point_start,sizeof(pip_mac_table_row->current_row_info.mac_address));
			memcpy(save_point_start,pip_mac_table_row->current_row_info.mac_address,sizeof(pip_mac_table_row->current_row_info.mac_address));
			save_point_end = save_point_start;
			save_point_middle = save_point_start;
			while(save_point_end != NULL)
			{
				save_point_end = strstr(save_point_middle,",");

				length = (int)(strlen(save_point_middle) - strlen(save_point_end));

				if( i < 6 )
				{
					if(length  == 2)
					{
						a = *(save_point_middle);
						save_point_middle++;
						b = *(save_point_middle);
						global_array_ptr->global_config_info.dev_mac[i] = (16 * (a-48)) + (b-48) ;
					}
					else if ( (length > 0) && (length < 2) )
					{
						memcpy(&b,save_point_middle,sizeof(b));
						global_array_ptr->global_config_info.dev_mac[i] = atoi(&b) ;
					}

					i++ ;
				}
				save_point_middle = (save_point_end + 1) ;
			}
//			diag_printf("zhw-11111111-info:%s\n",save_point_start);
			free(save_point_start); // add by zhw-2014-12-29
			save_point_start = NULL ;
//			diag_printf("zhw-33333333\n");
		}

		global_array_ptr->next = NULL;
		global_array_ptr = global_array_ptr_head;
#ifdef BCU_DEV
		pthread_mutex_unlock(&mutex_update_ip_array);
#endif
		return 2;
	}
	else
	{
		net_debug_printf(("malloc filed for the new device\n"));
	}
#ifdef BCU_DEV
	pthread_mutex_unlock(&mutex_update_ip_array);
#endif
	return 0;
}

#if LOW_TEST
int ReadType(void)
{
	int type = 5; ///< 1:PCU; 5:CCU; 3:BCU; 7:EAMP
	return type;
}

int ReadNum(void)
{
	int number = 1; ///< 1:192.168.10.113  2:192.168.10.114
	return number;
}
#else
int ReadType(void)
{
	int type ;
//	READ_DEVICE_TYPE(type);
	int con= 9;
   READ_DEVICE_TYPE(con);
   type = (con & 0x07 );
	return type ;
}

int ReadNum(void)
{
	int num ;
//	READ_DEVICE_ADDR(num) ;
	int con= 9;
    READ_DEVICE_TYPE(con);
    num = (con & 0xf8 );
	return num ;
}
#endif

int CountDevNumber(char *dev_name,int dev_number)
{
	if((strcmp(dev_name,"EAMP") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		eamp_number++;
		return 1;
	}
	if((strcmp(dev_name,"BCU") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		bcu_number++;
		return 1;
	}
	if((strcmp(dev_name,"CCU") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		ccu_number++;
		return 1;
	}
	if((strcmp(dev_name,"PCU") == 0) && ( 0 < dev_number ) && ( dev_number < 200 ) )
	{
		pcu_number++;
		return 1;
	}
	return 0;
}

int ZhwGetDevNumber(char *dev_name)
{
//	net_debug_printf(("I am success entry ZhwGetDevNumber() function to inqu the number of dev %s!\n",dev_name));
	int inqu_i;
	int dev_number = 0;

	global_array_t *inqu_ptr = global_array_ptr_head;

	for(inqu_i = 0;inqu_i<row;inqu_i++)
	{
		if( (strcmp(dev_name,inqu_ptr->global_config_info.dev_name) == 0) && (0 < inqu_ptr->global_config_info.dev_number) && ( inqu_ptr->global_config_info.dev_number < 200) )
		{
			(dev_number) = (dev_number + 1) ;
		}
		inqu_ptr =inqu_ptr->next;
	}
	if(dev_number > 0)
	{
		return dev_number;
	}

//	net_debug_printf(("the %s number in ZhwGetDevNumber() function inqu is %d\n",dev_name,dev_number));
	return -1;
}

/*
 *   	@brief	���������������������������������������I/O���������������������������������
 *   			���������������
 *     @param void ���������������
 *     @return   dev_type_and_no  ������������������������������������������
 */
struct dev_type_and_no ReadFromIO(void)
{
	struct dev_type_and_no local_dev;

#if LOW_TEST
	local_dev.dev_type = ReadType();	//������������������
	local_dev.dev_number = ReadNum();	//���������������

#else
	int local_type = 0;
	int local_num = 0;
	local_type = ReadType();
	local_num = ReadNum();
//	diag_printf("type from local is :%d,number from local is:%d\n",local_type,local_num);
	switch(local_type)
	{
	//add for CMU ,2014-11-17-zhw
	case 0:
	{
		switch(local_num)
		{
		DEV_NUMBER_CHANGE(local_num,0,1);
		DEV_NUMBER_CHANGE(local_num,128,2);
		default:
			local_num = 0 ;
			break;
		}
	}
		break;
	case 1:
	{
		switch(local_num)
		{
		DEV_NUMBER_CHANGE(local_num,248,1);
		DEV_NUMBER_CHANGE(local_num,232,2);
		DEV_NUMBER_CHANGE(local_num,216,3);
		DEV_NUMBER_CHANGE(local_num,200,4);
		DEV_NUMBER_CHANGE(local_num,184,5);
		DEV_NUMBER_CHANGE(local_num,168,6);
		DEV_NUMBER_CHANGE(local_num,152,7);
		DEV_NUMBER_CHANGE(local_num,136,8);
		DEV_NUMBER_CHANGE(local_num,120,9);
		DEV_NUMBER_CHANGE(local_num,104,10);
		DEV_NUMBER_CHANGE(local_num,88,11);
		DEV_NUMBER_CHANGE(local_num,72,12);
		DEV_NUMBER_CHANGE(local_num,56,13);
		DEV_NUMBER_CHANGE(local_num,40,14);
		DEV_NUMBER_CHANGE(local_num,24,15);
		DEV_NUMBER_CHANGE(local_num,8,16);
		default:
			local_num = 0 ;
			break;
		}
	}
		break;
	case 3:
	{
		switch(local_num)
		{
		DEV_NUMBER_CHANGE(local_num,0,1);
		DEV_NUMBER_CHANGE(local_num,8,2);
		default:
			local_num = 0 ;
			break;
		}
	}
		break;
	case 5:
	{
		switch(local_num)
		{
		DEV_NUMBER_CHANGE(local_num,0,1);
		DEV_NUMBER_CHANGE(local_num,128,2);
		default:
			local_num = 0 ;
			break;
		}
	}
		break;
	case 7:
	{
		switch(local_num)
		{
		DEV_NUMBER_CHANGE(local_num,240,1);
		DEV_NUMBER_CHANGE(local_num,232,2);
		DEV_NUMBER_CHANGE(local_num,216,3);
		DEV_NUMBER_CHANGE(local_num,184,4);
		DEV_NUMBER_CHANGE(local_num,120,5);
		DEV_NUMBER_CHANGE(local_num,112,6);
		default:
			local_num = 0 ;
			break;
		}
	}
		break;
	default:
		local_type = 0 ;
		break;
	}
	local_dev.dev_type = local_type;	//������������������
	local_dev.dev_number = local_num;	//���������������

#endif

	return local_dev;
}

/*
 *   	@brief	���������������������������������������������������������������������������������������
 *     @param device_name ������������������������
 *     @param device_number 	������������������������
 *     @return   device_ip  ���������������IP������
 */
void InitGlobalArray(void)
{
	int row_mark;
	int read_db_ret = 1;
	global_array_ptr = (global_array_t *)malloc(sizeof(global_array_t));
	bzero(global_array_ptr,sizeof(global_array_t));
	global_array_ptr_head = global_array_ptr;

	char *save_point_start = NULL ;
	char *save_point_middle = NULL ;
	char *save_point_end = NULL ;
	char a = 0;
	char b = 0;
	int length = 0 ;

	ip_mac_table_item_t *point = NULL;
	int i ;

	pthread_mutex_init(&zhw_buffer_register_lock, NULL); ///< add, 2013
#ifndef BCU_DEV
	read_db_ret = AllConfigDBState();
	if(0 == read_db_ret)
	{
		net_debug_printf(("get message from DB!!!\n"));
		//read from DataBase
		if(SqlOfTableTIpMac(&row, &column, (void **)&point) < 0)
		{
			net_debug_printf(("get message from DB is err,DB is wrong,get from local file now!!!\n"));
			db_err_status = 1 ;
			if(ZhwReadFromTestDB(&row, &column,(void **)&point) < 0)
			{
				net_debug_printf(("get message from local file is err!!!\n"));
			}
		}
		else
		{
			net_debug_printf(("get message from DB is OK!!!\n"));
		}
	}
	else
#else
		//add for update ip array zhw-2014-11-11
		pthread_mutex_init(&mutex_update_ip_array,NULL);
#endif
	{
		net_debug_printf(("not have DB,get message from local file !!!\n"));
		if(ZhwReadFromTestDB(&row, &column,(void **)&point) < 0)
		{
			net_debug_printf(("get message from local file is err!!!\n"));
		}
	}

	diag_printf("while init row = %d\n",row);

	for(row_mark = 0;row_mark<row;row_mark++,point++)
	{
		i = 0 ;
		global_array_ptr->next = (global_array_t *)malloc(sizeof(global_array_t));
		global_array_ptr = global_array_ptr->next;
		bzero(global_array_ptr,sizeof(global_array_t));

		global_array_ptr->global_config_info.dev_type = point->device_type;
//		net_debug_printf(("device type  = %d--",point->device_type));

		global_array_ptr->global_config_info.dev_name = (char *)malloc(sizeof(point->device_name));
		bzero(global_array_ptr->global_config_info.dev_name,sizeof(point->device_name));
		memcpy(global_array_ptr->global_config_info.dev_name,point->device_name,sizeof(point->device_name));
		net_debug_printf(("device name  = %s--",point->device_name));

		global_array_ptr->global_config_info.dev_number = point->device_no ;
		net_debug_printf(("device number  = %d--",point->device_no));

		global_array_ptr->global_config_info.dev_ip = (char *)malloc(sizeof(point->ip_address));
		bzero(global_array_ptr->global_config_info.dev_ip,sizeof(point->ip_address));
		memcpy(global_array_ptr->global_config_info.dev_ip,point->ip_address,sizeof(point->ip_address));
		net_debug_printf(("IP address = %s--",point->ip_address));

		if( (0 == read_db_ret) && (0 == db_err_status) )
		{
			save_point_start = (char *)malloc(sizeof(point->mac_address));
			bzero(save_point_start,sizeof(point->mac_address));
			memcpy(save_point_start,point->mac_address,sizeof(point->mac_address));
			save_point_middle = save_point_start;
			save_point_end = save_point_start;
			while(save_point_end != NULL)
			{
				save_point_end = strstr(save_point_middle,",");

				length = (int)(strlen(save_point_middle) - strlen(save_point_end));

				if( i < 6 )
				{
					if(length  == 2)
					{
						a = *(save_point_middle);
						save_point_middle++;
						b = *(save_point_middle);
						global_array_ptr->global_config_info.dev_mac[i] = (16 * (a-48)) + (b-48) ;
					}
					else if ( (length > 0) && (length < 2) )
					{
						memcpy(&b,save_point_middle,sizeof(b));
						global_array_ptr->global_config_info.dev_mac[i] = atoi(&b) ;
					}

					i++ ;
				}
				save_point_middle = (save_point_end + 1) ;
			}
			free(save_point_start); // add by zhw-2014-12-29
			save_point_start = NULL ;
//			diag_printf("zhw-33333333\n");
		}
		else
		{
			for(i = 0 ;i < 6 ;i++)
			{
				global_array_ptr->global_config_info.dev_mac[i] = point->mac_address[i];
			}
		}

		net_debug_printf(( "Mac addr %02x:%02x:%02x:%02x:%02x:%02x\n",
	    		global_array_ptr->global_config_info.dev_mac[0],
	    		global_array_ptr->global_config_info.dev_mac[1],
	    		global_array_ptr->global_config_info.dev_mac[2],
	    		global_array_ptr->global_config_info.dev_mac[3],
	    		global_array_ptr->global_config_info.dev_mac[4],
	    		global_array_ptr->global_config_info.dev_mac[5] ));

		global_array_ptr->global_config_info.is_server = point->server_whether ;

//		if(CountDevNumber(global_array_ptr->global_config_info.dev_name,global_array_ptr->global_config_info.dev_number) != 1)
//		{
//			net_debug_printf(("get dev number is error !!!\n"));
//		}
//		global_array_ptr->next = (global_array_t *)malloc(sizeof(global_array_t));
//		global_array_ptr = global_array_ptr->next;
//		bzero(global_array_ptr,sizeof(global_array_t));
	}

	global_array_ptr->next = NULL;

//	free(point);
	point = NULL;


	ZhwSetNet();
}

/*
 *   	@brief	���������������������������������������������������������������������������������������������������������������������0���NULL���������������������������������������IP������
 *   			���������������������������������������������������������
 *  	@param device_type ���IO������������������������������������������������������������������������������������
 *  			111���������7���:������EAMP���
 *  			011���������3���:������BCU���
 *  			001���������1���:������PCU���
 *  			101 (������5)���������CCU;
 *     @param device_name ������������������������
 *     @param device_number 	������������������������
 *     @param resolve_type ���������������������������������������������������������������
 *     				1���������������������������������������������������������������
 *     				2������������������������������������������������������������
 *     				3 :������������ip���������������.
 *
 *     @return   device_info  ������������������������
 */
struct global_info ReadGlobalArray(int device_type,char *device_name_or_ip,int device_number,int resolve_type)
{
	struct global_info ret_dev_info;
#ifdef BCU_DEV
	if(1 == update_start)
	{
		pthread_mutex_lock(&mutex_update_ip_array);
	}
#endif
	switch(resolve_type)
	{
	case 1:
//		if(device_type == 0)
		if(device_type < 0) //changed by zhw-2014-11-17 for CMU
		{
			net_debug_printf(("the number of device_type is err!!!\n"));
			break;
		}
		ret_dev_info = ResolveUseType(device_type,device_number);
		break;
	case 2:
		if(device_name_or_ip == NULL)
		{
			net_debug_printf(("the number of device_name is err!!!\n"));
			break;
		}
		ret_dev_info = ResolveUseName(device_name_or_ip,device_number);
		break;
	case 3:
		if(device_name_or_ip == NULL)
		{
			net_debug_printf(("the number of device_name is err!!!\n"));
			break;
		}
		ret_dev_info = ResolveUseIp(device_name_or_ip);
		break;
	default :
		net_debug_printf(("the number of resolve_type is err!!!\n"));
		break;
	}
#ifdef BCU_DEV
	if(1 == update_start)
	{
		pthread_mutex_unlock(&mutex_update_ip_array);
	}
#endif
	return ret_dev_info;
}

/*
 *   	@brief	���������������������������������������������������������������������������������������IP���������
 *     @param device_type ������������������������������������
 *     @param device_number 	������������������������
 *     @return  local_device_ip  ���������������IP������
 *     				-1:���������������������������������������������������������������������
 */
struct global_info ResolveUseType(int device_type,int device_number)
{
	int inqu_i;
	int i;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	for(inqu_i = 0;inqu_i<(row+1);inqu_i++)
	{
		if((device_type == inqu_ptr->global_config_info.dev_type)&&(device_number == inqu_ptr->global_config_info.dev_number))
		{
			ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
			ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
			ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
			for(i = 0;i < 6; i++)
			{
				ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
			}
			ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
			ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
			break;
		}
		else
		{
			inqu_ptr =inqu_ptr->next;
		}
	}
	if(ret_dev_info.dev_ip == NULL)
	{
		net_debug_printf(("local inqu is err,something is wrong!!!\n"));
	}
	return ret_dev_info;
}

/*
 *   	@brief	������������������������������������������������������������������������������������IP���������
 *     @param device_name ������������������������
 *     @param device_number 	������������������������
 *     @return  global_device_ip  ������������������������������IP������
 *     				-1:���������������������������������������������������������������������
 */
struct global_info ResolveUseName(char *device_name,int device_number)
{
	int inqu_i;
	int i;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;

//	diag_printf("while resolve row = %d,the resolve device info =%s-%d\n",row,device_name,device_number);

	for(inqu_i = 0;inqu_i<(row+1);inqu_i++)
	{
//		diag_printf("inqu-ptr info = %s-%d\n",inqu_ptr->global_config_info.dev_name,inqu_ptr->global_config_info.dev_number);
		if((strcmp(device_name,inqu_ptr->global_config_info.dev_name) == 0)&&(device_number == inqu_ptr->global_config_info.dev_number))
		{
			ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
			ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
			ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
			for(i = 0;i < 6;i++)
			{
				ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
			}
			ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
			ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
			break;
		}
		else
		{
			inqu_ptr =inqu_ptr->next;
		}
	}
//	if(inqu_i==row)
//	{
//		net_debug_printf(("ip not found,something is wrong!!!\n"));
//	}
	if(ret_dev_info.dev_ip == NULL)
	{
		net_debug_printf(("global inqu is err,something is wrong!!!\n"));
	}

	return ret_dev_info;
}


/*
 *   	@brief	������������������������������������������IP���������������������
 *     @param device_ip ���������������������ip
 *     @return  ret_dev_info  ���������������������������������
 *     				-1:���������������������������������������������������������������������
 */
struct global_info ResolveUseIp(char *device_ip)
{
	int inqu_i;
	int i;
	int strcmp_ret = 1;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	for(inqu_i = 0;inqu_i<(row+1);inqu_i++)
	{
		if(((strcmp_ret = strcmp(device_ip,inqu_ptr->global_config_info.dev_ip)) == 0))
		{
			ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
			ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
			ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
			for(i = 0; i < 6; i++)
			{
				ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
			}
			ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
			ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
			break;
		}
		else
		{
			inqu_ptr =inqu_ptr->next;
		}
	}

	if(strcmp_ret != 0)
	{
		net_debug_printf(("global inqu is err,something is wrong!!!\n"));
	}

	return ret_dev_info;
}

//������������������������������
struct global_info ResolveUseServerNumber(int a)
{
	int inqu_i;
	int i;
	int strcmp_ret = 1;
	struct global_info ret_dev_info;
	global_array_t *inqu_ptr = global_array_ptr_head;
	
	for(inqu_i = 0;inqu_i<row;inqu_i++)
	{
	if(inqu_ptr->global_config_info.is_server == a )
	{
	ret_dev_info.dev_type = inqu_ptr->global_config_info.dev_type;
	ret_dev_info.dev_name = inqu_ptr->global_config_info.dev_name;
	ret_dev_info.dev_ip = inqu_ptr->global_config_info.dev_ip;
	for(i = 0; i < 6; i++)
	{
	ret_dev_info.dev_mac[i] = inqu_ptr->global_config_info.dev_mac[i];
	}
	ret_dev_info.dev_number = inqu_ptr->global_config_info.dev_number;
	ret_dev_info.is_server = inqu_ptr->global_config_info.is_server;
	strcmp_ret = 0 ;
	break;
	}
	else
	{
	inqu_ptr =inqu_ptr->next;
	}
	}
	 
	if(strcmp_ret != 0)
	{
	net_debug_printf(("global inqu is err,something is wrong!!!\n"));
	}
	 
	return ret_dev_info;
}

