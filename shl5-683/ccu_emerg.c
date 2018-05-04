/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： ccu_emerg.c
 * 文件标识：
 * 摘    要： 紧急广播操作
 *
 * 当前版本： V1.0.0
 * 作    者： wilson
 * 完成日期：2013-11-18
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 */
#include "ccu_emerg.h"

emerg_table_items_t *p_ccu_emerg_info = NULL;
ccu_emerg_info_t ccu_emerg_info;

void CreateCCUEmergSpace(int param_emerg_total_items)
{//创建CCU有关紧急广播空间
	if(p_ccu_emerg_info != NULL)
	{
		free(p_ccu_emerg_info);
	}
	p_ccu_emerg_info = (emerg_table_items_t *)malloc(param_emerg_total_items * sizeof(emerg_table_items_t));
	ccu_state.p_ccu_emerg_info = &ccu_emerg_info;
	ccu_emerg_info.emerg_total_items = EMERG_DEBUG_FORTS_TOTAL_ITEMS;
	ccu_emerg_info.emerg_table_items = p_ccu_emerg_info;
}

void InitCCUEmergInfo()
{//初始化紧急广播信息
	int i = 0;
	for(i = 0;i < ccu_state.p_ccu_emerg_info->emerg_total_items;i++)
	{
		(ccu_state.p_ccu_emerg_info->emerg_table_items + i)->emerg_ann_id = i + 20;
	}
	strcpy(ccu_state.p_ccu_emerg_info->emerg_table_items->display_message,"全体乘客请注意,由于故障,本次列车将退出运营,请乘客们尽快下车,给您的出行带来不便,我们深表歉意,敬请谅解.");
	strcpy((ccu_state.p_ccu_emerg_info->emerg_table_items +1)->display_message,"全体乘客请注意,现发生紧急情况,请不要惊慌,听从工作人员指挥,有序撤离。");
	strcpy((ccu_state.p_ccu_emerg_info->emerg_table_items+2)->display_message,"乘客们,现在客流较大,列车比较拥挤,下车乘客请及早准备,抓紧下车,上车乘客请尽量往里走,谢谢配合.");
	strcpy((ccu_state.p_ccu_emerg_info->emerg_table_items+3)->display_message,"乘客们,本次列车因故将稍作停留，请您耐心等待，谢谢配合");
}

char *GetEmergDisplayMessage(unsigned char param_emerg_sequence,unsigned short *len)
{//获取紧急广播显示内容信息
	int i = 0;
	char *p = NULL;
	for(i = 0; i < ccu_state.p_ccu_emerg_info->emerg_total_items;i ++)
	{
		if((ccu_state.p_ccu_emerg_info->emerg_table_items + i)->emerg_ann_id == param_emerg_sequence)
		{
			p = (ccu_state.p_ccu_emerg_info->emerg_table_items + i)->display_message;
			*len = strlen(p);
			break;
		}
	}
	return p;
}
