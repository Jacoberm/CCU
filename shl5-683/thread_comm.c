/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：thread_comm.c
 * 文件标识：
 * 摘    要：控制信息结构体
 *
 * 当前版本：V1.0.0
 * 作    者：wilson
 * 完成日期：2012-10-09
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "thread_comm.h"

/*The declaration of mutex*/
pthread_mutex_t mutex_of_control_to_network_comm;/*the mutex between control thread and netwoek thread*/
pthread_mutex_t mutex_of_global_device_info;/*the mutex of global info struct*/

/*The declaration of semaphore*/
sem_t  sem_wakeup_bcu_mic_sample;

//控制线程与网络线程之间的通信结构体:主控线程-->网络线程
volatile threads_communication_type_t control_to_network_communication;

/*All device information*/
volatile global_device_info_state_t global_device_info_state =
{
		InitGlobalInfo,
		UpdateGlobalInfo,
		{{"0"}},
		{{"0"}},
		{{"0"}},
		{{"0"}}
};

/*Initial the mutex*/
void InitMutex()
{
	pthread_mutex_init(&mutex_of_control_to_network_comm,NULL);
	pthread_mutex_init(&mutex_of_global_device_info,NULL);
}

/*Initialize the semaphore*/
void InitSemaphore(sem_t *param_sem)
{
	sem_init(param_sem, 0, 0);
}

/*Initialize the destination of audio data*/
void InitAudioDataDestination(void)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	strcpy((char *)control_to_network_communication.device_name,"");
	control_to_network_communication.device_no = 0;
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Clear the destination of audio data*/
void ClearAudioDataDestination()
{
	InitAudioDataDestination();
}

/*Set the destination of audio data*/
void SetAudioDataDestination(char *device_name, unsigned char device_no)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	strcpy((char *)control_to_network_communication.device_name,device_name);
	control_to_network_communication.device_no = device_no;
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Initial all device information*/
void InitAllDeviceInfo()
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	global_device_info_state.init();
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Update  device information*/
void UpdataDeviceInformation(send_infomation_t recv_package)
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	global_device_info_state.setglobalinfo(recv_package);
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Initial the global device information struct*/
void InitGlobalInfo()
{
	int i = 0;
	for(i = 1;i < CONFIG_BCU_NUM + 1;i ++)//bcu
	{
		global_device_info_state.device_bcu[i].devices_no = i;
		strcpy((char *)global_device_info_state.device_bcu[i].devices_name,"BCU");
		global_device_info_state.device_bcu[i].current_state_ann = ANN_IDLE;
		global_device_info_state.device_bcu[i].current_state_intercom = INTERCOM_IDLE;
	}
	for(i = 1;i < CONFIG_CCU_NUM + 1;i ++)//ccu
	{
		global_device_info_state.device_ccu[i].devices_no = i;
		strcpy((char *)global_device_info_state.device_ccu[i].devices_name,"CCU");
	}
	for(i = 1;i < CONFIG_EAMP_NUM + 1;i ++)//eamp
	{
		global_device_info_state.device_eamp[i].devices_no = i;
		strcpy((char *)global_device_info_state.device_eamp[i].devices_name,"EAMP");
		global_device_info_state.device_eamp[i].current_state = ANN_IDLE;
	}
	for(i = 1;i < CONFIG_PECU_NUM + 1;i ++)//pcu
	{
		global_device_info_state.device_pcu[i].devices_no = i;
		strcpy((char *)global_device_info_state.device_pcu[i].devices_name,"PCU");
		global_device_info_state.device_pcu[i].current_state_intercom = INTERCOM_IDLE;
	}
}

/*Update  global device information struct*/
void UpdateGlobalInfo(send_infomation_t receive_package)
{
	//int current_device_type = GetDeviceType(receive_package.src_devices_name);
	int current_device_type = TYPE_OF_CCU;
	int current_device_number = receive_package.src_devices_no ;

	debug_print(("current_device_type = %d,TYPE_OF_PECU = %d\n",current_device_type,TYPE_OF_PECU));

	switch(current_device_type)
	{
		case TYPE_OF_BCU:
			if(receive_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active == 1)
			{
				//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = receive_package.event_type_intercom;
				memcpy((void *)&global_device_info_state.device_bcu[current_device_number].event_infomation_intercom,
						(void *)&receive_package.event_info_intercom,sizeof(receive_package.event_info_intercom));
			}
			if(receive_package.event_info_intercom.d2d_intercomm.d2d_intercomm_active == 0)
			{
				//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = INTERCOM_IDLE;
				memset((void *)&global_device_info_state.device_bcu[current_device_number].event_infomation_intercom,
						0,sizeof(receive_package.event_info_intercom));
			}
			break;
		case TYPE_OF_CCU:
			break;
		case TYPE_OF_EAMP:
			if(receive_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over == 0)
			{
				//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_ann = receive_package.event_type_ann;
				global_device_info_state.device_eamp[current_device_number].current_state = receive_package.event_type_ann;
				memcpy((void *)&global_device_info_state.device_bcu[current_device_number].event_infomation_ann,
						(void *)&receive_package.event_info_ann,sizeof(receive_package.event_info_ann));
			}
			if(receive_package.event_info_intercom.d2d_intercomm.d2d_intercomm_request_or_over == 1)
			{
				//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_ann = ANN_IDLE;
				memset((void *)&global_device_info_state.device_bcu[current_device_number].current_state_ann,
						0,sizeof(receive_package.event_info_ann));
			}
			break;
		case TYPE_OF_PECU:
#if 0
			/*need to judge whether this package is send to this bcu*/
			if(receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_bcu_device_no != bcu_state.bcu_info.devices_no)
			{
				/*This package is send to other bcu,we shoule update relate information*/
				if(receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1)
				{
					//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = receive_package.event_type_intercom;
					global_device_info_state.device_pcu[current_device_number].current_state_intercom = receive_package.event_type_intercom;
					memcpy((void *)&global_device_info_state.device_pcu[current_device_number].event_infomation_intercom,
							(void *)&receive_package.event_info_intercom,sizeof(receive_package.event_info_intercom));
				}
			}
			else
			{
				if(receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 1)
				{
					//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = receive_package.event_type_intercom;
					global_device_info_state.device_pcu[current_device_number].current_state_intercom = receive_package.event_type_intercom;
					memcpy((void *)&global_device_info_state.device_pcu[current_device_number].event_infomation_intercom,
							(void *)&receive_package.event_info_intercom,sizeof(receive_package.event_info_intercom));
				}
				if(receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_active == 0)
				{
					debug_print(("d2p:%d intercomm_active:%d\n",current_device_number,receive_package.event_info_intercom.d2p_intercomm.d2p_intercomm_active));
					//global_device_info_state.device_bcu[bcu_state.bcu_info.devices_no].current_state_intercom = ANN_IDLE;
					memcpy((void *)&global_device_info_state.device_pcu[current_device_number].event_infomation_intercom,
							(void *)&receive_package.event_info_intercom,sizeof(receive_package.event_info_intercom));
				}
			}
#endif
			break;
			
		default:
			break;
	}
}

/*Set the destination device.which is offer to network thread*/
void SetDSTDeviceNameAndNumber(char *device_name,int device_number)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	control_to_network_communication.device_no = device_number;
	strcpy((char *)&control_to_network_communication.device_name,(char *)device_name);
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Get the destination device's name and number*/
void DSTDeviceNameAndNumber(char *dst_device_name,int *dst_device_number)
{
	pthread_mutex_lock(&mutex_of_control_to_network_comm);
	*dst_device_number = control_to_network_communication.device_no;
	strcpy((char *)dst_device_name,(char *)control_to_network_communication.device_name);
	pthread_mutex_unlock(&mutex_of_control_to_network_comm);
}

/*Update global device information*/
void UpdataGlobalDeviceInfo(send_infomation_t received_information_backup)
{
	pthread_mutex_lock(&mutex_of_global_device_info);

	global_device_info_state.setglobalinfo(received_information_backup);

	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Show pcu request*/
void ShowPCURequest()
{
	int i = 1;
	for(i = 1; i < 6; i ++)
	{
		debug_print(("PCU%d = %d\n",i,global_device_info_state.device_pcu[i].event_infomation_intercom.d2p_intercomm.d2p_intercomm_active));
	}
}

/*Copy global struct*/
void ScreenCopyGlobalDeviceInfo(global_device_info_state_t *global_device_info_state_backup)
{
	pthread_mutex_lock(&mutex_of_global_device_info);
	memcpy((void *)global_device_info_state_backup,(void *)&global_device_info_state,sizeof(global_device_info_state));
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

/*Try Copy global struct*/
int TryScreenCopyGlobalDeviceInfo(global_device_info_state_t *global_device_info_state_backup)
{
	if ( pthread_mutex_trylock(&mutex_of_global_device_info) != 0 )
	{
	       return -1;
	}

	if( NULL != global_device_info_state_backup )
	{
		  memcpy((void *)global_device_info_state_backup, (void *)&global_device_info_state, sizeof(global_device_info_state));
	}
	pthread_mutex_unlock(&mutex_of_global_device_info);

	return 0;
}

/*BCU update PCU state*/
void BCUUpdataPCUDeviceInfo(send_infomation_t received_information_backup)
{
	int pcu_no;
	pthread_mutex_lock(&mutex_of_global_device_info);
	pcu_no = received_information_backup.event_info_intercom.d2p_intercomm.d2p_intercomm_pcu_device_no;
	global_device_info_state.device_pcu[pcu_no].event_infomation_intercom.d2p_intercomm.d2p_intercomm_active = 0;
	pthread_mutex_unlock(&mutex_of_global_device_info);
}

global_device_info_state_t  *GetLocalGlobalDevInfoTable(void)
{
      return (global_device_info_state_t *)&global_device_info_state;
}

void GlobalDevTableMutexLock(void)
{
        pthread_mutex_lock(&mutex_of_global_device_info);
	return ;
}

void GlobalDevTableMutexUnlock(void)
{
	pthread_mutex_unlock(&mutex_of_global_device_info);
	return ;
}

int TryCopyPartOfGlobalDevTable(int dev_type, int dev_no, void *dev_state_context)
{ 
      int ret = -1;
      global_device_info_state_t  *plocal_global_dev_info_table = GetLocalGlobalDevInfoTable();
	  
      GlobalDevTableMutexLock();
      switch( dev_type )
      {
             case 1:   //eamp
		    if( dev_no>=1 && dev_no<=CONFIG_EAMP_NUM  )
		    {
                          device_eamp_state_t  *ptmp_device_eamp_array = plocal_global_dev_info_table->device_eamp;
                          memcpy(dev_state_context, (void *)&(ptmp_device_eamp_array[dev_no]), sizeof(device_eamp_state_t));
		    }
		break;
				
             case 2:   //pcu 
		    if( dev_no>=1 && dev_no<=CONFIG_PECU_NUM  )
		    {
                          device_pcu_state_t  *ptmp_device_pcu_array = plocal_global_dev_info_table->device_pcu;
                          memcpy(dev_state_context, (void *)&(ptmp_device_pcu_array[dev_no]), sizeof(device_pcu_state_t));
		    }
		break;
		
             case 3:   //bcu          
		    if( dev_no>=1 && dev_no<=CONFIG_BCU_NUM  )
		    {
                          device_bcu_state_t  *ptmp_device_pcu_array = plocal_global_dev_info_table->device_bcu;
                          memcpy(dev_state_context, (void *)&(ptmp_device_pcu_array[dev_no]), sizeof(device_bcu_state_t));
		    }
		break;
		
             case 4:   //ccu
		    if( dev_no>=1 && dev_no<=CONFIG_CCU_NUM  )
		    {
                          device_ccu_state_t  *ptmp_device_pcu_array = plocal_global_dev_info_table->device_ccu;
                          memcpy(dev_state_context, (void *)&(ptmp_device_pcu_array[dev_no]), sizeof(device_ccu_state_t));
		    }	
		break;
		
             default:
		break;		
      }
      GlobalDevTableMutexUnlock();      
      
      return ret;
}
