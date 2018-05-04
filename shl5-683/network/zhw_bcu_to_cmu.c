/*
 * zhw_bcu_to_cmu.c
 *
 *  Created on: 2014-4-4
 *      Author: benty
 */
#include "zhw_bcu_to_cmu.h"
cyg_flag_t bcu_cmd_to_cmu;
network_send_package_t send_buffer;
struct global_info bcu_local_info;
struct dev_type_and_no bcu_device_io;

void ZhwSendCmdInfo(int local_send_buffer,int type)
{
	strcpy(send_buffer.dst_devices_name, "CMU");
	send_buffer.dst_devices_no = 230 ;
	strcpy(send_buffer.send_information.src_devices_name, bcu_local_info.dev_name);
	send_buffer.send_information.src_devices_no = bcu_local_info.dev_number ;
	send_buffer.send_information.line_status.cmd_type = type ;
	send_buffer.send_information.line_status.pkg_number = 0;
	send_buffer.send_information.line_status.pkg_type = 1;
	send_buffer.send_information.event_type_ann = ANN_IDLE;
	send_buffer.send_information.event_type_intercom = INTERCOM_IDLE;
	if(BlockBufferWrite(local_send_buffer,&send_buffer,sizeof(send_buffer))<0)
	{
		cmu_debug_printf(("cmu send cmd = %d to network error!!!\n",type));
	}
	else
	{
		cmu_debug_printf(("cmu send cmd = %d to network ok!!!\n",type));
	}
}

void ZhwBcuToCmuEntry(int cmd_send_buffer)
{
	cmu_debug_printf(("run into ZhwBcuToCmuEntry function!!!\n"));
	int ret = 0 ;

	bcu_device_io = ReadFromIO();
	bcu_local_info = ReadGlobalArray(bcu_device_io.dev_type,NULL,bcu_device_io.dev_number,1);


	while(1)
	{
		ret = cyg_flag_wait(&bcu_cmd_to_cmu,0x3f,CYG_FLAG_WAITMODE_OR);
		cmu_debug_printf(("the cyg_flag_wait ret = %d\n",ret));

		if((ret & 0x01) == 0x01)
		{
			cmu_debug_printf(("flag bit 1 is ok!!!\n"));
			cyg_flag_maskbits(&bcu_cmd_to_cmu,~0x1);
			ZhwSendCmdInfo(cmd_send_buffer,1);
		}
		if((ret & 0x02) == 0x02)
		{
			cmu_debug_printf(("flag bit 2 is ok!!!\n"));
			cyg_flag_maskbits(&bcu_cmd_to_cmu,~0x2);
			ZhwSendCmdInfo(cmd_send_buffer,2);
		}
		if((ret & 0x04) == 0x04)
		{
			cmu_debug_printf(("flag bit 3 is ok!!!\n"));
			cyg_flag_maskbits(&bcu_cmd_to_cmu,~0x4);
			ZhwSendCmdInfo(cmd_send_buffer,3);
		}
		if((ret & 0x08) == 0x08)
		{
			cmu_debug_printf(("flag bit 4 is ok!!!\n"));
			cyg_flag_maskbits(&bcu_cmd_to_cmu,~0x8);
			ZhwSendCmdInfo(cmd_send_buffer,4);
		}
		if((ret & 0x10) == 0x10)
		{
			cmu_debug_printf(("flag bit 5 is ok!!!\n"));
			cyg_flag_maskbits(&bcu_cmd_to_cmu,~0x10);
			ZhwSendCmdInfo(cmd_send_buffer,5);
		}
		if((ret & 0x20) == 0x20)
		{
			cmu_debug_printf(("flag bit 6 is ok!!!\n"));
			cyg_flag_maskbits(&bcu_cmd_to_cmu,~0x20);
			ZhwSendCmdInfo(cmd_send_buffer,6);
		}
	}
}
