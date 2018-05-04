/*
 * sqlite_middle_layer.c
 *
 *  Created on: 2013-5-20
 *      Author: benty
 */
#include"sqlite_middle_layer.h"

//噪音检测中间层替换
void* TNoiseCheckMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(noise_check_t);
	noise_check_t *p_table = (noise_check_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		p_table->min_volume = (int)atoi(p_result[ret++]);
		p_table->max_volume = (int)atoi(p_result[ret++]);
		p_table->added_value = (int)atoi(p_result[ret++]);
		p_table->gradient = 	(float)atof(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“时间段音量调节”表中间层替换
void* TVolumeControlByTimeMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(volume_control_Bytime_t);
	volume_control_Bytime_t *p_table = (volume_control_Bytime_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;
	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strcpy((char *)(p_table->start_time), p_result[ret++]);
		strcpy((char *)(p_table->end_time), p_result[ret++]);
		p_table->volume = (unsigned char)atoi(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“语言与字体”表中间层替换
void* TLanguageAndFontMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(language_and_font_t);
	language_and_font_t *p_table = (language_and_font_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;
	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strcpy((char *)(p_table->language), p_result[ret++]);
		strcpy((char *)(p_table->font), p_result[ret++]);
		p_table->active_font_or_not = (unsigned char)atoi(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“自动更新（音频文件更新表）”表中间层替换
void* TRefreshAudioPathMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(refresh_audio_path_t);
	refresh_audio_path_t *p_table = (refresh_audio_path_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strncpy(p_table->audio_path, p_result[ret++],MAX_STRING_LEN);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}


//“优先级”表中间层替换
void* TPriorityMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(priority_id_t);
	priority_id_t *p_table = (priority_id_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strcpy(p_table->event_type, p_result[ret++]);
		strcpy(p_table->short_name, p_result[ret++]);
		p_table->priority_id = (unsigned char)atoi(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“路径”表中间层替换
void* TFilePathMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(file_path_t);
	file_path_t *p_table = (file_path_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strncpy((char *)(p_table->even_type), p_result[ret++],20);
		strncpy((char *)(p_table->path_name), p_result[ret++],MAX_STRING_LEN);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}
//“提示音”表中间层替换
void* TPromptAudioMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(prompt_audio_t);
	prompt_audio_t *p_table = (prompt_audio_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;
	for (; i <= (*row); i++)
	{
//		ret = i*(*column) + 1; //roben_ye, a row id is add by the PTU to as a primary key.

		ret = i*(*column) ;
		strcpy((char *)(p_table->even_type), p_result[ret++]);
		strcpy((char *)(p_table->path_name), p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“监听音源”表中间层替换
void* TMonitorMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(monitor_t);
	monitor_t *p_table = (monitor_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;
	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1; //roben_ye, a row id is add by the PTU to as a primary key.

		strcpy(p_table->event_name, p_result[ret++]);

		p_table->monitor_or_not = (unsigned char)atoi(p_result[ret++]);
		p_table->record_or_not = (unsigned char)atoi(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“PTT默认持续时间”表中间层替换
void* TPTTLastTimeMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(PTT_last_time_t);
	PTT_last_time_t *p_table = (PTT_last_time_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;
	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strcpy((char *)(p_table->event_name), p_result[ret++]);
		strcpy((char *)(p_table->short_name), p_result[ret++]);
		p_table->last_time = (unsigned int)atoi(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“设备数量”表中间层替换
void* TDeviceNumberMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(device_number_t);
	device_number_t *p_table = (device_number_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.

		strcpy(p_table->device_name, p_result[ret++]);

//		p_table->device_type = (unsigned char)atoi(p_result[ret++]);

		p_table->device_number_count = atoi(p_result[ret++]);

		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“版本号”表中间层替换
void* TDBVersionMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = sizeof(db_version_t);
	db_version_t *p_table = (db_version_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	if(NULL==p_head)
	{
		sqlite_print(("TDBVersionMiddleLayer:  row=%d, %08x, len \n", (*row), p_head, len));
	    return NULL;
	}

	int i = 1;

	int max_row = 1;
	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.
		ret += 3;//跳转到当前版本号列，只比较当前版本号列
		if (strcmp((char *)(p_table->current_version), p_result[ret]) < 0)
		{
			strcpy((char *)(p_table->current_version), p_result[ret]);
			max_row = i;
		}
	}
	{		
		ret = max_row*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.
		p_table->version_id = (unsigned int)atoi(p_result[ret++]);

		strcpy((char *)(p_table->short_name), p_result[ret++]);

		strcpy((char *)(p_table->time), p_result[ret++]);

		strcpy((char *)(p_table->current_version), p_result[ret++]);

		*row = 1;//add,只返回最大的数据。
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}

//“IP-mac”表中间层替换
void* TIpMacMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(ip_mac_config_t);
	ip_mac_config_t *p_table = (ip_mac_config_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	if(NULL==p_head)
	{
		sqlite_print(("TIpMacMiddleLayer:  row=%d, %08x, len \n", (*row), p_head, len));
	    return NULL;
	}

	int i = 1;//???????

	for (; i <= (*row); i++)
	{
		ret = i*(*column);

		sqlite_print(("--------------------- \n"));

		strcpy(p_table->device_name, p_result[ret++]);
		sqlite_print(("device_name:  %s \n", p_table->device_name));

		p_table->device_no = (unsigned char)atoi(p_result[ret++]);
		sqlite_print(("device_no:  %d \n", p_table->device_no));

		strcpy(p_table->ip_address, p_result[ret++]);
		sqlite_print(("ip_address:  %s \n", p_table->ip_address));

		strcpy(p_table->mac_address, p_result[ret++]);
		sqlite_print(("mac_address:  %s \n", p_table->mac_address));

		p_table->server_whether = (unsigned char)atoi(p_result[ret++]);
		sqlite_print(("server_whether:  %d \n", p_table->server_whether));

		if (ip_mac_old != 0)
		{
			if(strcmp(p_table->device_name,"EAMP") == 0)
			{
				p_table->device_type_no = 7;
			}
			else if(strcmp(p_table->device_name,"PCU")== 0)
			{
				p_table->device_type_no = 1;
			}
			else if(strcmp(p_table->device_name,"BCU") == 0)
			{
				p_table->device_type_no = 3;
			}
			else if(strcmp(p_table->device_name,"CCU") == 0)
			{
				p_table->device_type_no = 5;
			}
			else if(strcmp(p_table->device_name,"CMU") == 0)
			{
				p_table->device_type_no = 0;
			}
			else
			{
				p_table->device_type_no = 8;
				//sqlite_print(("device_name is vmware name:%s \n", p_table->device_name));
			}
		}
		else
		{
			p_table->device_type_no = (unsigned char)atoi(p_result[ret++]);

		}

		sqlite_print(("device_type_no:  %d \n", p_table->device_type_no));
		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}


//“线路号”表中间层替换
void* TlineMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(line_table_t);
	line_table_t *p_table	= 	(line_table_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	if(NULL==p_head)
	{
		sqlite_print(("TlineMiddleLayer:  row=%d, %08x, len \n", (*row), p_head, len));
	        return NULL;
	}	

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.
		p_table->line_no = (unsigned char)atoi(p_result[ret++]);
		p_table->default_line = (unsigned char)atoi(p_result[ret++]);
		strncpy(p_table->short_name, p_result[ret++],50);
		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}



//“模拟设置”表中间层替换
void* TSimulateVariableMiddleLayer(void **p_result,int *row, int *column)
{
	int len = 0;
	int ret =0;

	len = (*row)*sizeof(simulate_variable_info_t);
	simulate_variable_info_t *p_table	= 	(simulate_variable_info_t *)SqlLocalBufferMalloc(len);
	void *p_head = (void *)p_table;

	if(NULL==p_head)
	{
		sqlite_print(("TSimulateVariableMiddleLayer:  row=%d, %08x, len \n", (*row), p_head, len));
	        return NULL;
	}

	int i = 1;

	for (; i <= (*row); i++)
	{
		ret = i*(*column) + 1;//roben_ye, a row id is add by the PTU to as a primary key.
		strncpy(p_table->type, p_result[ret++],50);
	    //p_table->value= (unsigned char)atoi(p_result[ret++]);
		strncpy(p_table->value,p_result[ret++],50);
		strncpy(p_table->explanation, p_result[ret++],100);
		p_table++;
	}

	SqliteAllTableFree((char**)p_result);
	return p_head;
}
//文件名+固定字符串,形成新文件名
//int NewFileNameMake(char *old_file_name, char *new_file_name, char *find_str, char*add_string)
//{
//	char *new_file = new_file_name;
//	char *old_file = old_file_name;
//	char *p_find_str = NULL;
//
//	p_find_str = strstr(old_file, find_str);
//
//	strncpy(new_file, old_file, strlen(old_file)-strlen(p_find_str));
//
//	strcat(new_file, add_string);
//
//	strcat(new_file, p_find_str);
//
//	return 0;
//}

//将主备数据库打开。
int OpenDbfile(sqlite3 **old_db_save, sqlite3 **old_db_backup, char *file_path_save, char *file_path_backup)
{
	int ret_back = 0;
	int ret_save = 0;
	sqlite3 *db_save = NULL;
	sqlite3 *db_backup = NULL;

	ret_back = sqlite3_open(file_path_save, &db_save);//&db, means sqlite3 **
	if(ret_back != SQLITE_OK)
	{
		sqlite_print( ("file_path_save: %s\n", sqlite3_errmsg(db_save)) );
		sqlite3_close(db_save);
	}
	ret_save = sqlite3_open(file_path_backup, &db_backup);//&db, means sqlite3 **
	if(ret_save != SQLITE_OK)
	{
		sqlite_print( ("file_path_backup: %s\n", sqlite3_errmsg(db_backup)) );
		sqlite3_close(db_backup);
	}

	if ((ret_back != SQLITE_OK) && (ret_save != SQLITE_OK))
	{
		return -1;
	}

	*old_db_save = db_save;
	*old_db_backup = db_backup;

	return 0;
}
