/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：ccu_database_update.c
 * 文件标识：
 * 摘    要：CCU database update function define
 *
 * 当前版本：V1.0.0
 * 作    者：roben
 * 完成日期：
 *
 *v7,2014-08-26
 *v9,2014-09-08
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */
#include "tftp_client_apply.h"



//int main(void)
//{
//	char *tftp_server_ip = "192.168.7.21";
//	tftp_print( ("strlen server ip:%d\n",strlen(tftp_server_ip)) );
//
//	//想要下载的文件名。
//	char *down_file_name = "/benty/roben/tftp_test.txt";
//
//	//配置文件的路径，根据配置文件自动选择存储路径
//	char *config_file_name = "/benty/roben/tftp_config.txt";
//
//	char *save_file_name = "hello.txt";
//	tftp_print( ("entry the main\n") );
//
//	DBfileTftpclientApi(tftp_server_ip, DB_ann_type);
//	DBfileTftpclientApi(tftp_server_ip, DB_Config_type);
//	AudiofileTftpclientApi(tftp_server_ip);
//
//    return 0;
//}
static char update_flage = 0;//0，表示没有正在tftp更新
						//非0，表示正在tftp更新

//查询tftp更新标志
char QueryUpdateFlage(void )
{
	return update_flage;
}

//设置tftp更新标志
void SetUpdateFlage(char set_value )
{
	if (set_value == 0)
	{
		update_flage = 0;
	}
	else
	{
		update_flage = 1;
	}
}


//音频配置使用同一个重启查询接口。即，更新完一种数据库后，需要重启一下。
static char tftp_restart_flag = 0;//0，无完整tftp更新，不需要重启
									//非0，tftp更新完整，需要重启

//设置tftp更新完成标志
void SetTftpRestartFlage(char set_value )
{
	if (set_value == 0)
	{
		tftp_restart_flag = 0;
	}
	else
	{
		tftp_restart_flag = 1;
	}
}

//查询tftp更新完成标志
char QueryTftpRestartFlage(void )
{
	return tftp_restart_flag;
}


//套接字创建
//static int LocalSocketCreate(struct addrinfo * addrinfo)
//{
//	int local_sock_send = 0;
//	if ( (local_sock_send = socket(addrinfo->ai_family, addrinfo->ai_socktype, 0)) < 0)
//	{
//		tftp_print( ("AF_INET:%d,SOCK_DGRAM:%d,protocol:%d\n",addrinfo->ai_family,
//				addrinfo->ai_socktype, addrinfo->ai_protocol) );
//		tftp_print( ("error on the socket create\n") );
//	}
//
//	//add,添加套接字设定。
//	if ( SocketSet(local_sock_send) < 0)
//	{
//		tftp_print( ("error on SocketSet\n") );
//		return -1;
//	}
//
//	return local_sock_send;
//}

//tftp循环尝试接口
int TftpClientFun(char *tftp_server_ip, char *down_file_name, char *save_file_name)
{
	tftp_print( ("tftp_server_ip:%s\n",tftp_server_ip) );
	tftp_print( ("down_file_name:%s\n",down_file_name) );
	tftp_print( ("save_file_name:%s\n",save_file_name) );

	int cycle_time = 0;

	while(cycle_time < 10)
	{
		tftp_print( ("-------tftp--time-------%d\n",cycle_time) );

		cycle_time++;

		//tftp下载、并存储文件。
		if ( DownloadFileFromTFTP(down_file_name, tftp_server_ip, save_file_name) < 0)
		{
			tftp_print( ("error on DownloadFileFromTFTP\n") );
			//暂停一段时间再链接，否则可能会使服务器死机。
			cyg_thread_delay(100);
			continue;
		}
		else
		{
			break;
		}
	}

	if (cycle_time < 10)
	{
		return 0;
	}
	else
	{
		tftp_print( ("more than 10 times,error\n") );
		return -1;
	}
}

//音频文件的整个更新路径和保护机制。
//音频文件更新的entry。
int AudiofileTftpclientApi(char *tftp_server_ip)
{

	tftp_print( ("entry ----AudiofileTftpclientApi\n") );

	int ret = 0;
	char *down_file_name = NULL;
	char *save_file_name = NULL;
	refresh_audio_path_t *p_resulte_table = NULL;
	char *ptr_free = NULL;

	int row_num = 0;
	int colum_num = 0;

	while(1)
	{
		//读取配置文件的标志位
			ret = AudioConfigStateSelect();

			tftp_print( ("Audio config state:%d\n", ret) );

			//“active”状态需要更新
			if (active == ret)
			{
				tftp_print( ("the audio config :active\n") );
				//读取数据库，调用更新
				SqlOfTRefreshAudioPath(&row_num, &colum_num, (void**)&p_resulte_table);
				ptr_free = p_resulte_table;

				ret = 0;
				while(ret < row_num)
				{
					//提取音频文件路径
					down_file_name = p_resulte_table->audio_path;

					//tftp下载和保存的文件路径相同
					save_file_name = down_file_name;
					tftp_print( ("the save_file_name:%s\n", save_file_name) );

					//音频文件的更新
					if (TftpClientFun(tftp_server_ip, down_file_name, save_file_name) < 0)
					{
						continue;
					}

					ret++;
					p_resulte_table += 1;

				}

				//修改标志为音频数据已经更新

				if (AudioConfigModifyToNegative() < 0)
				{
					tftp_print( ("AudioConfigModifyToNegative ---error\n") );
				}

				//释放保存数据库的查询结果
				SqlLocalBufferFree((void *)ptr_free);
				p_resulte_table = NULL;
				ptr_free = NULL;
		//		SqlTableBufferFree((void **)&p_resulte_table);
				break;
			}
			else if(negative == ret)//“negative”状态，不需要更新
			{
				tftp_print( ("the audio config :negative\n") );
				break;
				//没有音频数据需要更新
				;
			}
			else//配置文件错误，必须重新下载
			{
				tftp_print( ("the audio config file is bad\n") );
				//roben,2014-09-03下载配置文件。
				//音频文件的更新
				if (TftpClientFun(tftp_server_ip, AUDIO_CONFIGURE_FILE, AUDIO_CONFIGURE_FILE) < 0)
				{
					tftp_print( ("down the config file is error\n") );
					return -1;
				}
			}
	}

	return 0;
}


//先删除save_file_name的文件，再将new_save_name改为save_file_name。
//int ChangToOldName(char *save_file_name, char *new_save_name)
//{
//	struct stat buffer;
//	int ret = 0;
//
//	//获取旧文件的文件状态。
//	stat( save_file_name, &buffer);
//	ret = buffer.st_nlink;
//
//	while(ret--)
//	{
//		//删除旧的文件。
//		if(unlink(save_file_name)<0)
//		{
//			tftp_print( ("unlink error !\n") );
//		}
//	}
//	//新文件改名为旧的文件。
//	rename(new_save_name, save_file_name);
//
//	return 0;
//}

//数据库更新文件和配置文件和报复机制，整个流程。
//数据库更新entry。
int DBfileTftpclientApi(char *tftp_server_ip, int db_type)
{
	//查看设备数据库是否已经更新了（音频数据库）。更新好了需要重启一次
	if(QueryTftpRestartFlage() != 0)
	{
		tftp_print( ("we need restart the device\n") );
		return 0;
	}

	if (DB_ann_type == db_type)//ann db 更新
	{
		if (DataDbFileUpdate(tftp_server_ip) < 0)
		{
			//首先将更新标志位设为0，表示停止更新
			SetUpdateFlage(0);
			return -1;
		}
		tftp_print( ("DataDbFileUpdate --ann-end\n") );
	}
	else if (DB_Config_type == db_type)//config db 更新
	{
		if (ConfigDbFileUpdate(tftp_server_ip) < 0)
		{
			//首先将更新标志位设为0，表示停止更新
			SetUpdateFlage(0);
			return -1;
		}
	}
	else
	{
		tftp_print( ("error db file update\n") );
		return -1;
	}

	//首先将更新标志位设为0，表示停止更新
	SetUpdateFlage(0);

	return 0;
}

//配置数据库更新
int ConfigDbFileUpdate(char *tftp_server_ip)
{
	//首先将更新标志位设为1，表示正在更新
	SetUpdateFlage(1);

	char *down_file_name = BACKUP_CONFIG_DB_FILE;
	char *save_file_name = SAVE_TFTP_CONFIG_DB_FILE;
	char *backup_file_name = BACKUP_CONFIG_DB_FILE;

	//下载对应的CRC文件。
	if (TftpClientFun(tftp_server_ip, CRC_CFG_BACK_FILE, CRC_CFG_TFTP_FILE) < 0)
	{
		tftp_print( ("the tftp is not finished---111\n") );
		return -1;
	}

	//tftp下载、保存文件接口
	if (TftpClientFun(tftp_server_ip, down_file_name, save_file_name) < 0)
	{
		tftp_print( ("the tftp is not finished\n") );
		return -1;
	}
	else//保护机制处理
	{
		//校验和不正确。
		if (TestFileCRC(SAVE_TFTP_CONFIG_DB_FILE, CRC_CFG_TFTP_FILE) != 0)
		{
			tftp_print( ("the db file's crc is error\n") );
			return -1;
		}

		//关掉正在用的，更加安全
		sqlite3_close(config_db);
		config_db = NULL;

		//更新好的数据库文件备份
		if (ProtectDbBackup(save_file_name, backup_file_name) < 0)
		{
			return -1;
		}

		//更新好的文件产生新的crc校验码，并保存到文件中。
		if(GenerateFileCRC(backup_file_name, CRC_CFG_BACK_FILE) < 0)
		{
			return -1;
		}
	}

	//<<roben_ye_wen,重启函数
	//<<restart,fun
	tftp_print( ("config db update, we will restart\n") );
//	SetTftpRestartFlage(1 );//配置数据库更新后暂不重启，PECU的GD函数特殊设置。

	//首先将更新标志位设为0，表示停止更新
	SetUpdateFlage(0);

	//roben_ye,最好重启重新打开。
	//OpenConfigDB();
	
	return 0;
}

//音频数据库更新
int DataDbFileUpdate(char *tftp_server_ip)
{
	//首先将更新标志位设为1，表示正在更新
	SetUpdateFlage(1);


	//要求，PC机上传之后，CCU必须重新调用打开数据库的函数的接口。
	//CCU 端版本号最高的db文件。

	//roben_ye,PTU 上传为"pa_ann_sqlit3.db"名字，但是CCU启动会有一个版本号比较
	//CCU 这里是两个文件之间的直接复制.CCU 实际使用的是“pa_ann_sqlit3_cp.db”
	char *down_file_name =   BACKUP_ANN_DB_FILE;
	char *save_file_name = SAVE_TFTP_ANN_DB_FILE;
	char *backup_file_name = BACKUP_ANN_DB_FILE;

	tftp_print( ("TftpClientFun-ann--start\n") );

	//下载对应的CRC文件。
	if (TftpClientFun(tftp_server_ip, CRC_ANN_BACK_FILE, CRC_ANN_TFTP_FILE) < 0)
	{
		tftp_print( ("the tftp is not finished---111\n") );
		return -1;
	}

   //tftp下载、保存文件接口
	if (TftpClientFun(tftp_server_ip, down_file_name, save_file_name) < 0)
	{
		tftp_print( ("the tftp is not finished---2\n") );
		return -1;
	}
	else//保护机制处理
	{
		//校验和不正确。
		if (TestFileCRC(SAVE_TFTP_ANN_DB_FILE, CRC_ANN_TFTP_FILE) != 0)
		{
			tftp_print( ("the db file's crc is error\n") );
			return -1;
		}

		tftp_print( ("ProtectDbBackup-entry\n") );
		//暂时关闭音频数据库
		AnnDbClose();

		//最新数据库save_file_name，改为backup_file_name，
		//即函数成功调用后，backup_file_name文件为最新的file文件。
		//更新好的数据库文件备份
		if (ProtectDbBackup(save_file_name, backup_file_name) < 0)
		{
			tftp_print( ("ann--ProtectDbBackup-error\n") );
			return -1;
		}

		//更新好的文件产生新的crc校验码，并保存到文件中。
		if(GenerateFileCRC(backup_file_name, CRC_ANN_BACK_FILE) < 0)
		{
			return -1;
		}

		//打开最新的数据库，其实为了查看是否有音频文件需要更新。
		if (AnnDbopen(backup_file_name) < 0)
		{
			tftp_print( ("open ann db error, ProtectDbBackup\n") );
			return -1;
		}

		//判断是否有音频文件需要更新，如果需要更新，进行音频文件更新
		if (AudioFileUpdate(tftp_server_ip,save_file_name) < 0)
		{
			tftp_print( ("AudioFileUpdate--error\n") );
			return -1;
		}

		//roben_ye_wen,音频数据库更新完成，可以直接重启。
		//<<restart,fun
		tftp_print( ("ann db update, we will restart\n") );
		SetTftpRestartFlage(1);
	}

	//首先将更新标志位设为0，表示停止更新
	SetUpdateFlage(0);



	return 0;
}

//通过查询最新的数据库的音频文件更新表，看是否有音频文件更新了。
//最新的数据库，save_file_name
//程序正在使用的数据库，backup_file_name
int AudioFileUpdate(char *tftp_server_ip, char *save_file_name)
{
//	//关掉正在用的，更加安全
//	sqlite3_close(database_db);
//	database_db = NULL;

	int row_num = 0;
	int column_num = 0;
	refresh_audio_path_t *p_sql_result = NULL;

	//查询是否有音频文件需要更新
	SqlOfTRefreshAudioPath(&row_num, &column_num, (void **)(&p_sql_result));
	//释放查询结果的空间，这里用不到，而且后面可能忘了，所以释放。
	//AudiofileTftpclientApi 里面有查询语句
//	SqlLocalBufferFree(p_sql_result);
	SqlTableBufferFree((void**)&p_sql_result);

	tftp_print( ("SqlOfTRefreshAudioPath:%d\n",row_num) );

	// =0 ,表示没有数据
	if (0 != row_num)
	{
		//首先将更新标志位设为1，表示正在更新
		SetUpdateFlage(1);
		tftp_print( ("SqlOfTRefreshAudioPath  > 0\n") );
		//修改配置文件表示有音频数据要跟新。
		if (AudioConfigModifyToActive() < 0)
		{
			tftp_print( ("AudioConfigModifyToActive config file -error\n") );

			//roben,2014-09-03下载配置文件。
			//音频文件的更新，tftp下载、保存文件接口
			if (TftpClientFun(tftp_server_ip, AUDIO_CONFIGURE_FILE, AUDIO_CONFIGURE_FILE) < 0)
			{
				tftp_print( ("down the config file is error\n") );
				return -1;
			}

			if (AudioConfigModifyToActive() < 0)
			{
				tftp_print( ("down the config file is error-222\n") );
				return -1;
			}

		}
		//roben_ye_wen,调用数据库更新
		//该函数有更新表的查询
		if (AudiofileTftpclientApi(tftp_server_ip) < 0)
		{
			tftp_print( ("AudiofileTftpclientApi\n") );
			return -1;
		}

//		cyg_thread_delay(100*5);
		tftp_print( ("AudioFileUpdate---1111\n") );

		//修改配置文件表示音频数据更新 好了
		if (AudioConfigModifyToNegative() < 0)
		{
			tftp_print( ("AudioConfigModifyToNegative\n") );
			return -1;
		}

		//首先将更新标志位设为0，表示停止更新
		SetUpdateFlage(0);
	}

	tftp_print( ("AudioFileUpdate-----end\n") );

	return 0;
}


//将tftp save file覆盖backup file，tftp save file作备份
//将backup file改为 tftp save file，backup file版本号更高，断电重启后使用这个db文件
int ProtectDbBackup(char* save_file_name,char *backup_file_name)
{
	tftp_print( ("CopyFile file fun\n") );
	//copy文件。
	if (CopyFile(save_file_name, backup_file_name) < 0)
	{
		tftp_print( ("CopyFile is error, ProtectDbBackup\n") );
		return -1;
	}

	return 0;
}

//配置文件标志改为激活：有音频数据需要更新
int AudioConfigModifyToActive(void)
{
	tftp_print( ("entry ---the AudioConfigModifyToActive\n") );

	char *config_file_name = AUDIO_CONFIGURE_FILE;

	//借用以前的配置文件，但是这里只用到一个标志位，不存在多个file。
	if (ModifyConfigfileState(config_file_name, file_one, active) < 0)
	{
		return -1;
	}

	return 0;
}

//配置文件标志改为非激活：没有音频数据需要更新，或者音频数据已经更新完了
int AudioConfigModifyToNegative(void)
{
	char *config_file_name = AUDIO_CONFIGURE_FILE;

	//借用以前的配置文件，但是这里只用到一个标志位，不存在多个file。
	if (ModifyConfigfileState(config_file_name, file_one, negative) < 0)
	{
		return -1;
	}

	return 0;
}

//查询是有MP3文件没有更新完，每次上电时查看，防止每次上电都更新
int AudioConfigStateSelect(void)
{
	char *config_file_name = AUDIO_CONFIGURE_FILE;
	int mp3_file_update_state = 0;

	//借用以前的配置文件，但是这里只用到一个标志位，不存在多个file。
	mp3_file_update_state = ReadConfigFileState(config_file_name, file_one);
	if (mp3_file_update_state < 0)
	{
		return -1;
	}

	return mp3_file_update_state;
}


