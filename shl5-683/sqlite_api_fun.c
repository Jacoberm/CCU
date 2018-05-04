/*
 * sqlite_api_fun.c
 *
 *  Created on: 2013-5-21
 *      Author: benty
 *
 * modify:2014-05-27(新增数据库状态查询接口)
 *v7,2014-08-26
 *v8,2014-09-02
 *v9,2014-09-08
 */

#include "sqlite_api_fun.h"

//音频数据库
static int all_data_db_state = 0;//0,正常（至少有一个db文件可以运行）
								//非0，两个db文件都读不出数据
								
//配置数据库
static int all_config_db_state = 0;//0,正常（至少有一个db文件可以运行）
								//非0，两个db文件都读不出数据



//用来查询音频数据库的状态，主要用于都为空或者数据库文件都被损坏的情况
//返回0，数据库正常运行，可以查询数据。
//返回非0，数据库运行有误，不可以查询数据。
int AllDataDBState(void)
{
	return all_data_db_state;
}
//配置数据库
int AllConfigDBState(void)
{
	return all_config_db_state;
}

#define VersionLen 30
char ann_version_from_db[VersionLen];	//保存音频数据库版本
char cfg_version_from_db[VersionLen];	//保存配置数据库版本号。

//读取音频数据库版本号
const char *ReadAnnVersion(void)
{
	return ann_version_from_db;//3.00.001
}

//读取配置数据库版本号
const char *ReadCfgVersion(void)
{
	return cfg_version_from_db;
}

//设置音频数据库版本号
int SetAnnVersion(const char *ann_version_string)
{
	if ((ann_version_string == NULL) || (0 == ann_version_string[0]))
	{
		sqlite_print(( "the input para is error	"));
		return -1;
	}

	if (strncmp(ann_version_string, ann_version_from_db, VersionLen) != 0)//不相等
	{
		bzero(ann_version_from_db, VersionLen);
		strncpy(ann_version_from_db, ann_version_string, VersionLen-1);
	}
	return 0;
}


//设置配置数据库版本号
int SetCfgVersion(const char *cfg_version_string)
{
	if ((cfg_version_string == NULL) || (0 == cfg_version_string[0]))
	{
		sqlite_print(( "the input para is error	"));
		return -1;
	}

	if (strncmp(cfg_version_string, cfg_version_from_db,VersionLen) != 0)//不相等
	{
		bzero(cfg_version_from_db, VersionLen);
		strncpy(cfg_version_from_db, cfg_version_string, VersionLen-1);
	}
	return 0;
}


#ifdef CONFIG_DISPLAY_TABLE_CONTEXT
sqlite3 *ann_test_db = NULL;
sqlite3 *config_test_db = NULL;
extern  void CcuTestSqlSetSameDb(sqlite3 *in_db);
int CcuTestDbReadTable(void)
{
       int nrow = 0, ncolumn = 0;
       char **azResult;
       char *zErrMsg = 0;

       //char *sql = "select * from TPriority";
       //char *sql = "select * from TIpMac";
       char *sql = "select * from TLineSection";
       sqlite3_get_table(ann_test_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

       int i = 0;
       sqlite_print(("CcuTestDbReadTable:  row:%d column=%d \n" , nrow , ncolumn));
       sqlite_print(("\nCcuTestDbReadTable:  The result of querying is : \n"));

       for(i=0; i<( nrow + 1 ) * ncolumn; i++)
       {
             if( i%ncolumn == 0 && i != 0 )
             {
                  sqlite_print(("\n"));
             }
             sqlite_print(( "%s	", azResult[i] ));
       }
       sqlite_print(("\n\n"));

       sqlite3_free_table( azResult );
       sqlite_print(("CcuTestDbReadTable:  err msg = %s \n", zErrMsg));

       return 0;
}

///< display the result of SqlOfTableTIpMac
int CcuGdDisplayTableTIpMac(int nrow, void *table_ptr)
{
		int ii = 0;
                ip_mac_config_t *p_table = (ip_mac_config_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTIpMac:  null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTIpMac:  info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              //sqlite_print(("%d ", p_table[ii].device_type));
	              sqlite_print(("%s ", p_table[ii].device_name));
	              sqlite_print(("%d ", p_table[ii].device_no));
	              sqlite_print(("%s ", p_table[ii].ip_address));
	              sqlite_print(("%s ", p_table[ii].mac_address));
	              sqlite_print(("%d ", p_table[ii].server_whether));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTableTDatabaseVersion
int CcuGdDisplayTableTDatabaseVersion(int nrow, void *table_ptr)
{
		int ii = 0;
                db_version_t *p_table = (db_version_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTDatabaseVersion, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTDatabaseVersion: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", p_table[ii].version_id));
	              sqlite_print(("%s ", p_table[ii].short_name));
	              sqlite_print(("%s ", p_table[ii].time));
	              sqlite_print(("%s ", p_table[ii].current_version));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTConfigDBVersion    db_version_t
int CcuGdDisplayTableTConfigDBVersion(int nrow, void *table_ptr)
{
		int ii = 0;
                db_version_t *p_table = (db_version_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTConfigDBVersion, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTConfigDBVersion: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", p_table[ii].version_id));
	              sqlite_print(("%s ", p_table[ii].short_name));
	              sqlite_print(("%s ", p_table[ii].time));
	              sqlite_print(("%s ", p_table[ii].current_version));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTableTDeviceNumber    device_number_t
int CcuGdDisplayTableTDeviceNumber(int nrow, void *table_ptr)
{
		int ii = 0;
                device_number_t *p_table = (device_number_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTDeviceNumber, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTDeviceNumber: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              //sqlite_print(("%d ", p_table[ii].device_type));
	              sqlite_print(("%d ", p_table[ii].device_number_count));
	              sqlite_print(("%s ", p_table[ii].device_name));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTableTFilePath  file_path_t
int CcuGdDisplayTableTFilePath(int nrow, void *table_ptr)
{
		int ii = 0;
                file_path_t *p_table = (file_path_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTFilePath, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTFilePath: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%s ", p_table[ii].even_type));
	              sqlite_print(("%s ", p_table[ii].path_name));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTPriority  priority_id_t
int CcuGdDisplayTableTPriority(int nrow, void *table_ptr)
{
		int ii = 0;
                priority_id_t *p_table = (priority_id_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTPriority, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTPriority: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", p_table[ii].priority_id));
	              sqlite_print(("%s ", p_table[ii].event_type));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTRefreshAudioPath   refresh_audio_path_t
int CcuGdDisplayTableTRefreshAudioPath(int nrow, void *table_ptr)
{
		int ii = 0;
                refresh_audio_path_t *p_table = (refresh_audio_path_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTNoiseCheck, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTNoiseCheck: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              //sqlite_print(("%d ", p_table[ii].add_del));
	              sqlite_print(("%s ", p_table[ii].audio_path));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of SqlOfTNoiseCheck   noise_check_t
int CcuGdDisplayTableTNoiseCheck(int nrow, void *table_ptr)
{
		int ii = 0;
                noise_check_t *p_table = (noise_check_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTNoiseCheck, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTNoiseCheck: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", p_table[ii].min_volume));
	              sqlite_print(("%d ", p_table[ii].max_volume));
	              sqlite_print(("%d ", p_table[ii].gradient));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< displya the result of SqlOfTableTline, line_table_t
int CcuGdDisplayTableTline(int nrow, void *table_ptr)
{
		int ii = 0;
                line_table_t *p_table = (line_table_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTline, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTline: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", p_table[ii].line_no));
	              sqlite_print(("%d ", p_table[ii].default_line));
	              sqlite_print(("\n"));
		}

		return 0;
}
///<  displya the result of StationCodeOfEntry, station_code_of_line_t
int CcuGdDisplayTableTStation(int nrow, void *table_ptr)
{
		int ii = 0;
                station_code_of_line_t *p_table = (station_code_of_line_t *)table_ptr;

		if(NULL==p_table)
		{
	               sqlite_print(("CcuGdDisplayTableTStation, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTStation: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", p_table[ii].station_no));
	              sqlite_print(("%d ", p_table[ii].fix_across_station));
	              sqlite_print(("%d ", p_table[ii].spec_open_door));
	              sqlite_print(("%s ", p_table[ii].station_name));
	              sqlite_print(("%s ", p_table[ii].station_audio));
	              sqlite_print(("%s ", p_table[ii].station_display));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of LineSectionOfEntry  line_section_table_t
int CcuGdDisplayTableTLineSection (int nrow, void *table_ptr)
{
		int ii = 0;
                line_section_table_t *pToResult = (line_section_table_t *)table_ptr;

		if(NULL==pToResult)
		{
	               sqlite_print(("CcuGdDisplayTableTLineSection, enter, null ptr \n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableTLineSection: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", pToResult[ii].line_no));
	              sqlite_print(("%d ", pToResult[ii].line_direction));
	              //sqlite_print(("%d ", pToResult[ii].last_cycle));
	              sqlite_print(("%d ", pToResult[ii].default_section));
	              sqlite_print(("%d ", pToResult[ii].section_no));
	              sqlite_print(("%d ", pToResult[ii].start_station));
	              sqlite_print(("%d ", pToResult[ii].end_station));
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of AnnReportOfEntry  ann_station_report_t
int CcuGdDisplayTableAnnReport(int nrow, void *table_ptr)
{
		int ii = 0;
	        int jj = 0;
                ann_station_report_t *pToResult = (ann_station_report_t *)table_ptr;
                audio_display_list *tmp_disp_list = NULL;

		if(NULL==pToResult )
		{
	               sqlite_print(("CcuGdDisplayTableAnnReport, enter, null ptr\n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableAnnReport: info \n"));
		for(ii=0; ii<(nrow); ii++)
		{
	              sqlite_print(("%d, ", pToResult[ii].station_no));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].pre_arrive));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].station_next_no));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].repeat_times));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].open_door));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].before_after));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].triger_distance));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].station_distance));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].pre_triger_distance));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].audio_len));
                      tmp_disp_list = pToResult[ii].station_type[0].audio; ///< start pos
		      for(jj=0; jj<pToResult[ii].station_type[0].audio_len; jj++)
		      {
	                  sqlite_print(("-%s_", tmp_disp_list[jj].audio_display));
	                  sqlite_print(("%d", tmp_disp_list[jj].attribute));
		      }
	              sqlite_print((", "));
	              sqlite_print(("%d, ", pToResult[ii].station_type[0].display_len));
                      tmp_disp_list = pToResult[ii].station_type[0].display; ///< start pos
		      for(jj=0; jj<pToResult[ii].station_type[0].display_len; jj++)
		      {
	                  sqlite_print(("-%s_", tmp_disp_list[jj].audio_display));
	                  sqlite_print(("%d", tmp_disp_list[jj].attribute));
		      }
	              sqlite_print(("\n"));
	              sqlite_print(("%d, ", pToResult[ii].station_no));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].pre_arrive));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].station_next_no));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].repeat_times));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].open_door));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].before_after));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].triger_distance));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].station_distance));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].pre_triger_distance));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].audio_len));
                      tmp_disp_list = pToResult[ii].station_type[1].audio; ///< start pos
		      for(jj=0; jj<pToResult[ii].station_type[1].audio_len; jj++)
		      {
	                  sqlite_print(("-%s_", tmp_disp_list[jj].audio_display));
	                  sqlite_print(("%d", tmp_disp_list[jj].attribute));
		      }
	              sqlite_print((", "));
	              sqlite_print(("%d, ", pToResult[ii].station_type[1].display_len));
                      tmp_disp_list = pToResult[ii].station_type[1].display; ///< start pos
		      for(jj=0; jj<pToResult[ii].station_type[1].display_len; jj++)
		      {
	                  sqlite_print(("-%s_", tmp_disp_list[jj].audio_display));
	                  sqlite_print(("%d", tmp_disp_list[jj].attribute));
		      }
	              sqlite_print(("\n"));
		}

		return 0;
}
///< display the result of EmergencyOfEntry  emerg_table_item_t
int CcuGdDisplayTableEmergency(int nrow, void *table_ptr)
{
		int ii = 0;
                emerg_table_item_t *pToResult = (emerg_table_item_t *)table_ptr;

		if(NULL==pToResult)
		{
	               sqlite_print(("CcuGdDisplayTableEmergency, enter, null ptr\n"));
		       return -1;
		}

	        sqlite_print(("CcuGdDisplayTableEmergency: info \n"));
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", pToResult[ii].emerg_ann_id));
	              sqlite_print(("%d ", pToResult[ii].line_no));
	              sqlite_print(("%d ", pToResult[ii].repeat_times));
	              sqlite_print(("%s ", pToResult[ii].audio_path));
	              sqlite_print(("%s ", pToResult[ii].display_message));
	              sqlite_print(("%s ", pToResult[ii].emerg_ann_name));
	              sqlite_print(("\n"));
		}

#if 0
	        sqlite_print(("CcuGdDisplayTableEmergency: message \n"));
		for(ii=0; ii<nrow; ii++)
		{
		     int jj = 0;
		     int mess_bytes = 0;
		     mess_bytes = strlen(pToResult[ii].display_message);
	             sqlite_print(("message-%d:  bytes= \n", mess_bytes));
		     for(jj=0; jj<mess_bytes; jj++)
		     {
	                  sqlite_print(("%02x, ", pToResult[ii].display_message[jj]));
			  if( jj%3==0 && jj !=0 )
			  {
	                        sqlite_print(("\n"));
			  }
		     }
	             sqlite_print(("\n"));
		}
#endif

		return 0;
}

int CcuGdSyncTestDb(void)
{
	     int nrow = 0;
	     int nclom = 0;
	     int var = 4;
	     char *sql = "select  count(*) from  TLineSection where lineNO = (:stationName)";

	     nrow = TableRowSelect(database_db, sql, var);
	     sqlite_print(("select  count(*)=%d \n", nrow));
	     if( nrow<= 0 )
	     {
	           sqlite_print(("select  count(*) <=0, return \n"));
		   return 0;
	     }

		line_section_table_t *pToResult = NULL;
		int malloc_len = sizeof(line_section_table_t) * nrow + 1;
		pToResult = (line_section_table_t*)malloc(malloc_len);
		if (pToResult == NULL)
		{
		        sqlite_print(("malloc(sizeof(line_section_table_t)) = null \n"));
			return 0;
		}

		if ( (nclom = SectionSelectFromDB(database_db, var, pToResult)) < 0)
		{
			sqlite_print(("error on the SectionSelectFromDB\n"));
			return 0;
		}

	        sqlite_print(("Line-%d section info: row=%d, col=%d \n", var, nrow, nclom ));
                CcuGdDisplayTableTline(nrow, pToResult);

		return 0;
}

int CcuTestDbPowerOn( int opt )
{
      char *zErrMsg = 0;
      int rc;

      //rc = sqlite3_open("/nand/dbfile/sqlit3_pa_13.db", &test_db);
      rc = sqlite3_open("/nand/dbfile/pa_ann_sqlit3.db", &ann_test_db);
      if( rc )
      {
           sqlite_print((stderr, "Can't open database: %s\n", sqlite3_errmsg(ann_test_db)));
           sqlite3_close(ann_test_db);
            sqlite_print(("You haven't opened a sqlite3 database, %s --- \n",sqlite3_errmsg(ann_test_db)));
           return -1;
      }
      else
      {
            sqlite_print(("You have opened a ANN sqlite3 database successfully --- \n"));
            CcuTestSqlSetAnnDb(ann_test_db);
      }

      rc = 0;
      rc = sqlite3_open("/nand/dbfile/pa_config_sqlit3.db", &config_test_db);
      if( rc )
      {
           sqlite_print((stderr, "Can't open database: %s\n", sqlite3_errmsg(config_test_db)));
           sqlite3_close(config_test_db);
            sqlite_print(("You haven't opened a sqlite3 database, %s --- \n",sqlite3_errmsg(config_test_db)));
           return -1;
      }
      else
      {
            sqlite_print(("You have opened a Config sqlite3 database successfully --- \n"));
            CcuTestSqlSetConfigDb(config_test_db);
      }

      if(1==opt)
      {
        #if 0
           int nrow = 0, ncolumn = 0;
           char **azResult;
           //char *sql = "select * from TPriority";
           //char *sql = "select * from TIpMac";
           char *sql = "select * from TLineSection";
           sqlite3_get_table(test_db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);

           int i = 0;
           sqlite_print(("row:%d column=%d \n" , nrow , ncolumn));
           sqlite_print(("\nThe result of querying is : \n"));

           for(i=0; i<( nrow + 1 ) * ncolumn; i++)
           {
                if( i%ncolumn == 0 && i != 0 )
                {
                     sqlite_print(("\n"));
                }
                sqlite_print(( "%s	", azResult[i] ));
           }
           sqlite_print(("\n\n"));

           sqlite3_free_table( azResult );
           sqlite_print(("sqlite3_free_table:  err msg = %s \n", zErrMsg));
	 #endif

	 #if 1
	{
	     int nrow = 0;
	     int nclom = 0;
	     int var = 3;
	     char *sql = "select  count(*) from  TLineSection where LineNO = (:stationName)";

	     nrow = TableRowSelect(ann_test_db, sql, var);
	     sqlite_print(("select  count(*)=%d \n", nrow));
	     if( nrow<= 0 )
	     {
	           sqlite_print(("select  count(*) <=0, return \n"));
		   return 0;
	     }

		line_section_table_t *pToResult = NULL;
		int malloc_len = sizeof(line_section_table_t) * nrow + 1;
		pToResult = (line_section_table_t*)malloc(malloc_len);
		if (pToResult == NULL)
		{
		        sqlite_print(("malloc(sizeof(line_section_table_t)) = null \n"));
			return 0;
		}

		if ( (nclom = SectionSelectFromDB(ann_test_db, var, pToResult)) < 0)
		{
			sqlite_print(("error on the SectionSelectFromDB\n"));
			return 0;
		}

	        sqlite_print(("Line-%d section info: row=%d, col=%d \n", var, nrow, nclom ));
		int ii = 0;
		for(ii=0; ii<nrow; ii++)
		{
	              sqlite_print(("%d ", pToResult[ii].line_no));
	              sqlite_print(("%d ", pToResult[ii].line_direction));
	              //sqlite_print(("%d ", pToResult[ii].last_cycle));
	              sqlite_print(("%d ", pToResult[ii].default_section));
	              sqlite_print(("%d ", pToResult[ii].section_no));
	              sqlite_print(("%d ", pToResult[ii].start_station));
	              sqlite_print(("%d ", pToResult[ii].end_station));
	              sqlite_print(("\n"));
		}

	}
	 #endif

           sqlite3_close(ann_test_db);
      }
      else
      {
            return 1;
      }

      return 0;
}
#endif


#ifdef MySqlTest
int SqlTestMain(void)
{
	int row = 0;
	int column = 0;
	void *p_station_code = NULL;
	void *p_line_section = NULL;
	void *p_line_table = NULL;
	void *p_ann_report = NULL;

	int ret = 0;

	//打开音频数据库和配置文件数据库。
	if (SqliteOpen() < 0)
	{
		Roben_Print("open fail \n");
//		return -1;
	}

	//音频数据版本号表的查询接口
//	if (SqlOfTableTDatabaseVersion(&row, &column, &p_line_table) < 0)
//	{
////		return -1;
//	}
//	Roben_Print("SqlOfTableTDatabaseVersion\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->current_version);
//
//	//配置数据版本号表的查询接口
//	SqlOfTConfigDBVersion(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTConfigDBVersion\n");
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->current_version);


	//1，两个文件都不存在
	//2，仅一个文件存在
	//3，两个文件都存在，但是没有数据
	//4，两个文件正常

	int ret_db_test = 0;
	sqlite_print((">>>>>>>>>>>>>>>>>>>> \n"));
	//打开音频数据库测试是否失败。
	if ((ret_db_test = AllDataDBState()) != 0)
	{
		sqlite_print(("open data db fail:%d \n",ret_db_test));
//		return -1;
	}
	else
	{
		sqlite_print(("******open data OK:%d***** \n",ret_db_test));
	}
	//打开配置文件数据库测试是否失败。
	if ((ret_db_test = AllConfigDBState()) != 0)
	{
		sqlite_print(("open config db fail:%d \n",ret_db_test));
//		return -1;
	}
	else
	{
		sqlite_print(("******open config OK:%d***** \n",ret_db_test));
	}

	sqlite_print(("<<<<<<<<<<<<<<<<<<< \n"));


	//IP、MAC地址等配置数据表的查询接口
//	SqlOfTableTIpMac(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTableTIpMac--end\n");
//
//	Roben_Print("row:%d,column:%d\n",row,column);
//	char* p_test_str = p_line_table;
//	ret = 0;
//	while (ret < row)
//	{
//
//		Roben_Print("%s\n",((ip_mac_config_t*)p_test_str+ret)->device_name);
//		Roben_Print("device_type_no:%d\n",((ip_mac_config_t*)p_test_str+ ret)->device_type_no);
//		Roben_Print("%d\n",((ip_mac_config_t*)p_test_str+ ret)->device_no);
//		Roben_Print("%s\n",((ip_mac_config_t*)p_test_str+ ret)->ip_address);
//		Roben_Print("%s\n",((ip_mac_config_t*)p_test_str+ ret)->mac_address);
//		Roben_Print("%d\n",((ip_mac_config_t*)p_test_str+ ret)->server_whether);
//		ret++;
//	}
//
//
//	Roben_Print("SqlOfTableTIpMac---end\n");

	//音频数据版本号表的查询接口
//	if (SqlOfTableTDatabaseVersion(&row, &column, &p_line_table) < 0)
//	{
//		return -1;
//	}
//	Roben_Print("SqlOfTableTDatabaseVersion\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%d\n",((db_version_t*)p_line_table)->version_id);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->short_name);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->time);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->current_version);

//	//设备数量表的查询接口
//	SqlOfTableTDeviceNumber(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTableTDeviceNumber\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//
//
//	 p_test_str = p_line_table;
//	ret = 0;
//	while (ret < row)
//	{
//		Roben_Print(":%s\n",((device_number_t*)p_line_table + ret)->device_name);
//		Roben_Print(":%d\n",((device_number_t*)p_line_table + ret)->device_number_count);
//		ret++;
//	}

//	//配置数据版本号表的查询接口
//	SqlOfTConfigDBVersion(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTConfigDBVersion\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%d\n",((db_version_t*)p_line_table)->version_id);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->short_name);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->time);
//	Roben_Print("%s\n",((db_version_t*)p_line_table)->current_version);

	//路径表的查询接口
	//现在已经合并为路径表了
	//	//Log日志保存路径表的查询接口
	//	SqlOfTableTLogSavePath(&row, &column, &p_line_table);
	//
	//	//录音文件保存路径表的查询接口
	//	SqlOfTRecordSavePath(&row, &column, &p_line_table);
//	SqlOfTableTFilePath(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTableTFilePath\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((file_path_t*)p_line_table)->even_type);
//	Roben_Print("%s\n",((file_path_t*)p_line_table)->path_name);
//	Roben_Print("%s\n",((file_path_t*)p_line_table+1)->even_type);
//	Roben_Print("%s\n",((file_path_t*)p_line_table+1)->path_name);

	//提示音表的查询接口
//	SqlOfTPromptAudio(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTPromptAudio\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((prompt_audio_t*)p_line_table)->even_type);
//	Roben_Print("%s\n",((prompt_audio_t*)p_line_table)->path_name);

	//监听音源表的查询接口
//	SqlOfTMonitor(&row, &column, &p_line_table);
//	Roben_Print("monitor_t\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((monitor_t*)p_line_table+8)->event_name);
//	Roben_Print("%d\n",((monitor_t*)p_line_table+8)->monitor_or_not);
//	Roben_Print("%d\n",((monitor_t*)p_line_table)->record_or_not);

	//PTT默认持续时间表的查询接口
//	SqlOfTPTTLastTime(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTPTTLastTime\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((PTT_last_time_t*)p_line_table)->event_name);
//	Roben_Print("%s\n",((PTT_last_time_t*)p_line_table)->short_name);
//	Roben_Print("%d\n",((PTT_last_time_t*)p_line_table)->last_time);

	//时间段音量调节表的查询接口
//	SqlOfTVolumeControlByTime(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTVolumeControlByTime\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((volume_control_Bytime_t*)p_line_table)->start_time);
//	Roben_Print("%s\n",((volume_control_Bytime_t*)p_line_table)->end_time);
//	Roben_Print("%d\n",((volume_control_Bytime_t*)p_line_table)->volume);

	//语言与字体表的查询接口
//	SqlOfTLanguageAndFont(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTLanguageAndFont\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((language_and_font_t*)p_line_table)->language);
//	Roben_Print("%s\n",((language_and_font_t*)p_line_table)->font);
//	Roben_Print("%d\n",((language_and_font_t*)p_line_table)->active_font_or_not);

	//事件优先级表的查询接口
//	SqlOfTPriority(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTPriority\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((priority_id_t*)p_line_table+3)->event_type);
//	Roben_Print("%s\n",((priority_id_t*)p_line_table+3)->short_name);
//	Roben_Print("%d\n",((priority_id_t*)p_line_table+3)->priority_id);

	//tftp更新文件路径统计表的查询接口
	//roben_ye,数据量太大，容易司机
//	Roben_Print("SqlOfTRefreshAudioPath----entry\n");
//	SqlOfTRefreshAudioPath(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTRefreshAudioPath\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%s\n",((refresh_audio_path_t*)p_line_table)->audio_path);

	//噪检信息表的查询接口
//	SqlOfTNoiseCheck(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTNoiseCheck--end\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%d\n",((noise_check_t*)p_line_table)->min_volume);
//	Roben_Print("%d\n",((noise_check_t*)p_line_table)->max_volume);
//	Roben_Print("%d\n",((noise_check_t*)p_line_table)->added_value);
//	Roben_Print("%f\n",((noise_check_t*)p_line_table)->gradient);

	//寻找默认线路
//	int line = 0;

	//线路号表的查询接口
//	SqlOfTableTline(&row, &column, &p_line_table);
//	Roben_Print("SqlOfTableTline\n");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%d\n",((line_table_t*)p_line_table)->line_no);
//	Roben_Print("%d\n",((line_table_t*)p_line_table)->default_line);
//	Roben_Print("%s\n",((line_table_t*)p_line_table)->short_name);

//	//通过标志位判断默认的线路号
//	if ((line = JudgeDefaultLine(p_line_table, row)) < 0)
//	{
//		return -1;
//	}
//	Roben_Print("the default line:%d\n",line);
//
//	//站代码扫描。
//	line = 3;
//	Roben_Print("StationCodeOfEntry\n");

	//线路号下的站代码表的查询接口
//	StationCodeOfEntry(4, &row, &column, &p_station_code);
////	Roben_Print("StationCodeOfEntry");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("****StationCodeOfEntry******\n");

//	ret = 0;
//	while (ret < row)
//	{
//		Roben_Print("%d\n",((station_code_of_line_t*)p_station_code +ret)->station_no);
//		Roben_Print("%d\n",((station_code_of_line_t*)p_station_code+ret)->fix_across_station);
//		Roben_Print("%d\n",((station_code_of_line_t*)p_station_code+ret)->spec_open_door);
//		Roben_Print("station_name:%s\n",((station_code_of_line_t*)p_station_code+ret)->station_name);
//		Roben_Print("station_name_english:%s\n",((station_code_of_line_t*)p_station_code+ret)->station_name_english);
//		ret++;
//	}

//	Roben_Print("%s\n",((station_code_of_line_t*)p_station_code)->station_audio);
//	Roben_Print("%s\n",((station_code_of_line_t*)p_station_code)->station_display);

	//寻找默认线路下的默认区间。
//	int section = 0;
//
//	line = 3;
//	Roben_Print("LineSectionOfEntry\n");
//
//	//线路号下的区间表的查询接口
//	LineSectionOfEntry(line, &row, &column, &p_line_section);
//	Roben_Print("SqlOfTableTline");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->line_no);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->line_direction);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->spec_cycle);
//	Roben_Print("%s\n",((line_section_table_t*)p_line_section+1)->short_name);
//	Roben_Print("%s\n",((line_section_table_t*)p_line_section)->start_time);
//	Roben_Print("%s\n",((line_section_table_t*)p_line_section+1)->end_time);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->default_section);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->section_no);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->start_station);
//	Roben_Print("%d\n",((line_section_table_t*)p_line_section+1)->end_station);
//
//	//通过标志位判断默认线路号下的默认区间
//	if ((section = JudgeDefaultSecion(p_line_section, row)) < 0)
//	{
//		perror("error on JudgeDefaultSecion\n");
//		return -1;
//	}
//	Roben_Print("the line :%d,default section:%d\n",line,section);


//	false_loop_att = 0,	//不是环线，播放终点站
//	true_loop_att = 1,//环线,不播放终点站
//	int ret_loop = 0;
//	//判断默认区间是否是环线
//	ret_loop = JudgeSectionIsLoop(line, section, p_line_section, row);
//	sqlite_print(("******ret_loop:%d\n", ret_loop));


//	//线路号下对应区间的报站表的查询接口(拼接)
//	Roben_Print("AnnReportOfEntry \n");
//	row = 0;
//	column = 0;

	if (AnnReportOfEntry(4, 3, &row, &column, &p_line_section) < 0)
	{
		Roben_Print("####### the section is error    ########\n");
	}

	Roben_Print("##########row:%d,column:%d\n",row,column);

	//【1】,外圈下行（Outer Down），
	//【0】，内圈上行（Inner Up）
//	int up_down = 1;

	//开门侧
	//【2】,两侧开门
	//【1】,右侧开门
	//【0】，左侧开门
//	int open_door = 0;

	//	false_loop_att = 0,	//不是环线，播放终点站
	//	true_loop_att = 1,//环线,不播放终点站
//	ret_loop = 0;

	//拼接属性初始化。现在定为用于每一站报站之前，
	//填充属性：内外圈、终点站替换 and 下一站、是否显示播放环线、开门侧
	//1
//	FileAttributeInit(line, up_down, "海伦路", "海伦路.mp3" , 3, ret_loop, open_door, p_line_section);

//	//2
//	ret_loop = 1;
//	up_down = 0;
//	open_door = 1;
//	FileAttributeInit(line, up_down, "海伦路", "海伦路.mp3" , 1, ret_loop, open_door, p_line_section);

//	ret_loop = 0;
//	up_down = 1;
//	open_door = 0;
//	FileAttributeInit(line, up_down, "海伦路", "海伦路.mp3" , 1, ret_loop, open_door, p_line_section);

	ret = 0;
	ann_station_report_t *prt_test = (ann_station_report_t*)p_line_section;

//	while(ret < row)
//	{
//		Roben_Print("res:%d,",ret);
////		Roben_Print("name:%s\n",(prt_test->station_type[report_station]. )->audio_text);
//		Roben_Print("audio_text:%s\n",(prt_test->station_type[report_station].audio )->audio_text);
//		Roben_Print("audio_text:%s\n",(prt_test->station_type[pre_station].audio )->audio_text);
//
//		prt_test++;
//		ret++;
//	}
//	AnnReportBufferFree(&p_line_section,row);

//	AnnReportOfEntry(4, 2, &row, &column, &p_line_section);
//
//	Roben_Print("row:%d,column:%d\n",row,column);
//
//	EndStationFill(row,p_line_section,"YiShanLu");
//
//	EndStationFill(row,p_line_section,"JiangYangRoad");
//
//	ret = 0;
//	ann_station_report_t *prt_test2 = (ann_station_report_t*)p_line_section;

//	while(ret < row)
//	{
//		Roben_Print("res:%d,",ret);
//		Roben_Print("name:%s\n",(prt_test->station_type[report_station]. )->audio_text);
//		Roben_Print("audio_text:%s\n",(prt_test2->station_type[report_station].audio )->audio_text);
//		Roben_Print("audio_text:%s\n",(prt_test2->station_type[pre_station].audio )->audio_text);
//
//		prt_test2++;
//		ret++;
//	}
//	AnnReportBufferFree(&p_line_section,row);

//	while(ret < row)
//	{
//		int res =0;
//		while(res < prt_test->station_type[0].display_len)
//		{
//			Roben_Print("res:%d,",res);
//			Roben_Print("audio_text:%s\n",(prt_test->station_type[0].display + res)->audio_text);
//			res++;
//		}
//
//		prt_test++;
//		ret++;
//
//	}

		while(ret < row)
		{

			Roben_Print("line 4 station_no:%d\n",prt_test->station_no);

			Roben_Print("before_after:%d\n",prt_test->station_type[pre_station].before_after);
			Roben_Print("triger_distance:%d\n",prt_test->station_type[pre_station].triger_distance);
			Roben_Print("station_distance:%d\n",prt_test->station_type[pre_station].station_distance);
			Roben_Print("pre_triger_distance:%d\n",prt_test->station_type[pre_station].pre_triger_distance);

			Roben_Print("open_door:%d\n",prt_test->station_type[pre_station].open_door);

			Roben_Print("\n");

			prt_test++;
			ret++;
		}
		AnnReportBufferFree(&p_line_section,row);



//	SqlLocalBufferFree(p_line_section);//不能这样释放了，会有问题

	Roben_Print("################################\n");
	Roben_Print("AnnReportBufferFree------enrty\n");


	AnnReportOfEntry(3, 1, &row, &column, &p_line_section);

	Roben_Print("###############row:%d,column:%d\n",row,column);

	ret = 0;
	prt_test = (ann_station_report_t*)p_line_section;

		while(ret < row)
		{

			Roben_Print("line 3 station_no:%d\n",prt_test->station_no);

			Roben_Print("before_after:%d\n",prt_test->station_type[pre_station].before_after);
			Roben_Print("triger_distance:%d\n",prt_test->station_type[pre_station].triger_distance);
			Roben_Print("station_distance:%d\n",prt_test->station_type[pre_station].station_distance);
			Roben_Print("pre_triger_distance:%d\n",prt_test->station_type[pre_station].pre_triger_distance);

			Roben_Print("open_door:%d\n",prt_test->station_type[pre_station].open_door);

			Roben_Print("\n");

			prt_test++;
			ret++;
		}
		AnnReportBufferFree(&p_line_section,row);

	//线路号下的紧急广播表的查询接口
//	Roben_Print("EmergencyOfEntry \n");
//	EmergencyOfEntry(line, &row, &column, &p_station_code);
//	Roben_Print("EmergencyOfEntry ");
//	Roben_Print("row:%d,column:%d\n",row,column);
//	Roben_Print("%d\n",((emerg_table_item_t*)p_station_code)->emerg_ann_id);
//	Roben_Print("%d\n",((emerg_table_item_t*)p_station_code)->line_no);
//	Roben_Print("%d\n",((emerg_table_item_t*)p_station_code)->repeat_times);
//	Roben_Print("%s\n",((emerg_table_item_t*)p_station_code)->audio_path);
//	Roben_Print("%s\n",((emerg_table_item_t*)p_station_code)->display_message);
//	Roben_Print("%s\n",((emerg_table_item_t*)p_station_code)->emerg_ann_name);

	SqlTableBufferFree(&p_line_table);
	SqlTableBufferFree(&p_station_code);

	SqlTableBufferFree(&p_ann_report);

	//关闭以前打开过的数据库（音频数据库，配置数据库）
	SqliteClose();
	return 0;
}
#endif


//打开音频数据库和配置文件数据库。
//roben_ye_wen打开错误直接调用tftp更新，不能添加
//roben_ye_wen,这个没有实际的用途，因为两个数据库都坏了，那么server IP就不知道了。
//roben_ye_wen,同时，因为网络的设置都没有进行。
int SqliteOpen(void)
{
//	Roben_Print("********sqlite test********\n");
	//打开配置数据库
	if (OpenConfigDB() < 0)
	{
		sqlite_print( ("error on OpenConfigDB\n") );
//		return -1;
	}
	//打开音频数据库
	if (OpenDataDB() < 0)
	{
		sqlite_print( ("error on OpenDataDB\n") );
//		return -1;
	}

	//模糊数据（即报站表模糊字段的填从源）
	ConnectInit();

	//获取版本号。如果数据库获取出错，使用默认值“0.00.000”
	int row_num = 0;
	int col_num = 0;
	char *P_Version = NULL;

	if (SqlOfTConfigDBVersion(&row_num, &col_num, &P_Version) < 0)
	{
		SetCfgVersion(NullVersionValue);
		sqlite_print(("the cfg db file read error\n"));
	}
	else
	{
		SetCfgVersion(((db_version_t *)P_Version)->current_version);
		//strncpy(cfg_version_from_db, , VersionLen-1);
	}

	if (SqlOfTableTDatabaseVersion(&row_num, &col_num, &P_Version) < 0)
	{
		SetAnnVersion(NullVersionValue);
		//strncpy(ann_version_from_db, NullVersionValue, VersionLen-1);
		sqlite_print(("the ann db file read error\n"));
	}
	else
	{
		SetAnnVersion(((db_version_t *)P_Version)->current_version);
		//strncpy(ann_version_from_db, ((db_version_t *)P_Version)->current_version, VersionLen-1);
	}

	//roben_ye,如果查询没有出错，即使返回行数为0，也是正确的。第0行返回的是表中的列名
	return 0;
}


//关闭以前打开过的数据库（音频数据库，配置数据库）
//这个一般在退出整个系统时使用或者后面代码都不使用数据库。
int SqliteClose(void)
{
	if (database_db != NULL)
	{
		sqlite3_close(database_db);
		database_db = NULL;
		all_data_db_state = -1;
	}

	if (config_db != NULL)
	{
		sqlite3_close(config_db);
		config_db = NULL;
		all_config_db_state = -1;
	}

	return 0;
}
//关闭以前打开过的数据库（音频数据库）
int AnnDbClose(void)
{
	sqlite_print(("AnnDbClose---entry\n"));
	if (database_db != NULL)
	{
		//中途调用，其他线程可能在使用，所以必须检测是否关闭
		if (sqlite3_close(database_db) == SQLITE_BUSY )
		{
			sqlite3_close_v2(database_db);
		}
		database_db = NULL;
	}

	return 0;
}

//打开音频数据库的接口。
int AnnDbopen(char *save_file_name)
{
	tftp_print( ("AnnDbopen: %s\n",save_file_name) );
	if (sqlite3_open(save_file_name, &database_db) < 0)//&db, means sqlite3 **
	{
		tftp_print( ("file_path_one: %s\n",sqlite3_errmsg(database_db)) );
		sqlite3_close(database_db);
		return -1;
	}
	else
	{
		all_data_db_state = 0;//修改标志位，表示数据可以查询。
	}
	return 0;
}

//关闭以前打开过的数据库（音频数据库，配置数据库）
int ConfigDbClose(void)
{
	//中途调用，其他线程可能在使用，所以必须检测是否关闭
	if (config_db != NULL)
	{
		if (sqlite3_close(config_db) == SQLITE_BUSY)
		{
			sqlite3_close_v2(config_db);
		}
		config_db = NULL;
	}

	return 0;
}


//噪检信息表的查询接口
int SqlOfTNoiseCheck(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	//初始化
	 *row = 0;
	 *column = 0;
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);


	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		sqlite_print(("the audio db is not good \n"));
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(noise_check, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TNoiseCheckMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//配置数据版本号表的查询接口
int SqlOfTConfigDBVersion(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);


	//打开配置文件数据库测试是否失败。
	if (AllConfigDBState() != 0)
	{
		SetCfgVersion(NullVersionValue);
		sqlite_print(("the config db is not good \n"));
		return -1;
	}

//	sqlite_print( ("SqlOfTConfigDBVersion --1111\n") );
	//查询
	p_result = (void**)AllTableSelectDB(config_DB_version, row, column);
	if (0 == p_result)
	{
		SetCfgVersion(NullVersionValue);
		return -1;
	}

//	sqlite_print( ("SqlOfTConfigDBVersion --222\n") );
	if (*row > 0)//必须判断，否者申请长度为0的buffer，有问题。
	{
		//中间层转换
		p_data_place = TDBVersionMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

//	sqlite_print( ("SqlOfTConfigDBVersion --33333\n") );
	return 0;
}

//音频更新文件路径统计表的查询接口
int SqlOfTRefreshAudioPath(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);
//	*p_data = NULL;

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		sqlite_print(("the audio db is not good \n"));
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(audio_refreesh_path, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TRefreshAudioPathMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//事件优先级表的查询接口
int SqlOfTPriority(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;


	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		sqlite_print(("the audio db is not good \n"));
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(priority, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TPriorityMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//录音,log保存路径表的查询接口
int SqlOfTableTFilePath(int *row, int *column,void **p_data)
{
	sqlite_print(("----------SqlOfTableTFilePath---------------- \n"));

	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		sqlite_print(("the audio db is not good \n"));
		SetAnnVersion(NullVersionValue);
		return -1;
	}

//	Roben_Print("SqlOfTableTFilePath--222:%d\n",strlen(*p_data));
	//查询
	p_result = (void**)AllTableSelectDB(file_path, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TFilePathMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}
//提示音表的查询接口
int SqlOfTPromptAudio(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}


//	Roben_Print("SqlOfTPromptAudio--222:%d\n",strlen(*p_data));
	//查询
	p_result = (void**)AllTableSelectDB(prompt_audio, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TPromptAudioMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}
//监听音源表的查询接口
int SqlOfTMonitor(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(monitor, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TMonitorMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//PTT默认持续时间表的查询接口
int SqlOfTPTTLastTime(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;


	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(PTT_last_time, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TPTTLastTimeMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}
//时间段音量调节表的查询接口
int SqlOfTVolumeControlByTime(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;


	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(volume_control_Bytime, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TVolumeControlByTimeMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}
//语言与字体表的查询接口
int SqlOfTLanguageAndFont(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(language_and_font, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TLanguageAndFontMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//设备数量统计表的查询接口
int SqlOfTableTDeviceNumber(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开配置文件数据库测试是否失败。
	if (AllConfigDBState() != 0)
	{
		SetCfgVersion(NullVersionValue);
		sqlite_print(("the config db is not good \n"));
		return -1;
	}


	//roben_ye,如果查询没有出错，即使返回行数为0，也是正确的。第0行返回的是表中的列名
	//查询
	p_result = (void**)AllTableSelectDB(device_count, row, column);
	if (0 == p_result)
	{
		SetCfgVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TDeviceNumberMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//音频数据版本号表的查询接口
int SqlOfTableTDatabaseVersion(int *row, int *column,void **p_data)
{

	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;

	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);


	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(DB_version, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TDBVersionMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

//	//中间层转换
//	p_data_place = TDBVersionMiddleLayer(p_result, row, column);
//	*p_data = p_data_place;

	return 0;
}



//IP、MAC地址等配置数据表的查询接口
int SqlOfTableTIpMac(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);


	//打开配置文件数据库测试是否失败。
	if (AllConfigDBState() != 0)
	{
		SetCfgVersion(NullVersionValue);
		sqlite_print(("the config db is not good \n"));
		return -1;
	}

//	Roben_Print("return AllTableSelectDB\n");
	//查询
	p_result = (void**)AllTableSelectDB(ip_mac_config, row, column);
	if (0 == p_result)
	{
		SetCfgVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TIpMacMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

//	//中间层转换
//	p_data_place = TIpMacMiddleLayer(p_result, row, column);
//	*p_data = p_data_place;

	return 0;
}

//线路号表的查询接口
int SqlOfTableTline(int *row, int *column,void **p_data)
{

	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}


	//查询
	p_result = (void**)AllTableSelectDB(line_ID, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	if(*row > 0)
	{
		//中间层转换
		p_data_place = TlineMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//模拟设置信息获取
int SimulateVariableOfEntry(int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;
	void **p_result = 0;

	 *row = 0;
	 *column = 0;
	//初始化

	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_result = (void**)AllTableSelectDB(TSimulationTimeInterval, row, column);
	if (0 == p_result)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}
	if(*row > 0)
	{
		//中间层转换
		p_data_place = TSimulateVariableMiddleLayer(p_result, row, column);
		*p_data = p_data_place;
	}

	return 0;
}

//线路号下的紧急广播表的查询接口
int EmergencyOfEntry(int line, int *row, int *column,void **p_data)
{

	void *p_data_place  = NULL;

	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}


	//查询
	p_data_place = EmergenceSelectEntry(line, row, column);
	if (NULL == p_data_place)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//中间层转换
	//转换函数暂时不需要，结构体一样。
	*p_data = p_data_place;

	return 0;
}

//音频、字幕拼接初始化，将公用部分的字段、音频的内容、路径保存到全局结构体中
int ConnectInit(void)
{
	sqlite_print(("******ConnectInit()******\n"));
	int row = 0;
	int column = 0;
	int ret = 0;
	ann_station_report_t *p_to_sql = NULL;

	//把模糊数据查出来
	if (AnnReportOfEntry(0, 0, &row, &column, (void**)&p_to_sql) < 0)
	{
		sqlite_print(("the AnnReportOfEntry:0,0 error\n"));
		return -1;
	}

	//查询结果有3条数据，（排序是按照 车站信息表中 的 站序号 来排列）
	//1，左侧-外圈-下一站终点站
	//2，右侧-内圈-下一站
	//3，两侧开门，现在没有为空

	//把模糊数据分解到结构体中。
	if (DataConvertStruct(p_to_sql) < 0)
	{
		return -1;
	}

	//把查询占用的空间释放。
	AnnReportBufferFree((void **)&p_to_sql,row);

	return 0;
}

//判断区间是否是环线
int JudgeSectionIsLoop(int line, int section, void *p_data, int row)
{
	if (NULL == p_data)
	{
		sqlite_print(("has no section num\n"));
		return -1;
	}

	int i = 0;
	int ret = 0;
	line_section_table_t *p_to_table = (line_section_table_t*)p_data;

	for (; i < row; i++)
	{
//		sqlite_print(("JudgeSectionIsLoop--111:%d\n",row));
		if ((section == p_to_table->section_no) && (line == p_to_table->line_no))
		{
//			sqlite_print(("JudgeSectionIsLoop--222\n"));
			if(p_to_table->end_station == p_to_table->start_station)
			{
//				sqlite_print(("JudgeSectionIsLoop--333\n"));
				ret = 1;
				break;
			}
		}

		p_to_table++;
	}

	return ret;
}


//线路号下对应区间的报站表的查询接口
int AnnReportOfEntry(int line, int section, int *row, int *column, void **p_data)
{
	void *p_data_place  = NULL;

	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_data_place = AnnReportEntry(line, section, row, column);
	if (NULL == p_data_place)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//中间层转换
	//转换函数暂时不需要，结构体一样。
	*p_data = p_data_place;

	return 0;
}

//线路号下的区间表的查询接口
int LineSectionOfEntry(int line, int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;

	 *row = 0;
	 *column = 0;
	//初始化
//	SqlLocalBufferFree(*p_data);
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}
//	Roben_Print("LineSectionOfEntry---line:%d\n",line);
	//查询
	p_data_place = LineSectionSelectEntry(line, row, column);
	if (NULL == p_data_place)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//中间层转换
	//转换函数暂时不需要，结构体一样。
	*p_data = p_data_place;

	return 0;
}

//线路号下的站代码表的查询接口
int StationCodeOfEntry(int line, int *row, int *column,void **p_data)
{
	void *p_data_place  = NULL;

	 *row = 0;
	 *column = 0;
	//初始化
	SqlTableBufferFree(p_data);

	//打开音频数据库测试是否失败。
	if (AllDataDBState() != 0)
	{
		SetAnnVersion(NullVersionValue);
		sqlite_print(("the audio db is not good \n"));
		return -1;
	}

	//查询
	p_data_place = StationCodeEntry(line, row, column);
	if (NULL == p_data_place)
	{
		SetAnnVersion(NullVersionValue);
		return -1;
	}

	//中间层转换
	//转换函数暂时不需要，结构体一样。
	*p_data = p_data_place;

	return 0;
}

//通过标志位判断默认线路号下的默认区间
int JudgeDefaultSecion(void *p_data, int row )
{
	if (NULL == p_data)
	{
		sqlite_print(("has no section num\n"));
		return -1;
	}

	int i = 0;
	int ret = -1;
	line_section_table_t *p_to_table = (line_section_table_t*)p_data;

	for (; i < row; i++)
	{
		if (1 == p_to_table->default_section)//是该标志表示区间是默认标志
		{
			ret = p_to_table->section_no;
		}
		p_to_table++;
	}

	return ret;
}

//通过标志位判断默认的线路号
int JudgeDefaultLine(void *p_to_table, int row)
{
	if (NULL == p_to_table)
	{
		sqlite_print(("has no line num\n"));
		return -1;
	}

	int i = 0;
	int ret = -1;
	line_table_t *p_data = (line_table_t*)p_to_table;
	for (; i < row; i++)
	{
		if (1 == p_data->default_line)//是该标志表示线路是默认标志
		{
			ret = p_data->line_no;
		}
		p_data++;
	}

	return ret;
}

//打开音频数据库
int OpenDataDB(void)
{
//	Roben_Print("********OpenDataDB********\n");
	sqlite3 * db_save = NULL;
	sqlite3 * db_backup = NULL;
	all_data_db_state = 0;

	//版本号字符串应该控制在最大30
	char *file_path_save = SqlLocalBufferMalloc(MAX_STRING_LEN);
	char *file_path_backup = SqlLocalBufferMalloc(MAX_STRING_LEN);

	//路径复制到申请的buffer中，其实是用局部变量数组也可以。
	strcpy(file_path_save, SAVE_TFTP_ANN_DB_FILE);
	strcpy(file_path_backup, BACKUP_ANN_DB_FILE);

	//roben_yeTestFileCRC
	//通过比较文件生成校验和与保存的校验和，查看文件是否被改动。
//	if(TestFileCRC(file_path_backup, CRC_ANN_BACK_FILE, file_path_save) < 0)
//	{
//		sqlite_print( ("the config file crc is error\n" ) );
//	}

	//通过比较文件生成校验和与保存的校验和，查看文件是否被改动。
	int ret_value = TestFileCRC(file_path_backup, CRC_ANN_BACK_FILE);
	if(ret_value > 0)
	{
		sqlite_print(("ann-----the main db is crc error\n"));
		//添加关于tftp db 的CRC 检测。
		int ret_tftp = TestFileCRC(file_path_save, CRC_ANN_TFTP_FILE);
		if (ret_tftp > 0)//主、备数据库的CRC 均不正确
		{
			sqlite_print(("ann-----the backup db is crc error\n"));
			//roben_ye,添加状态标志。
			all_data_db_state = -1;
			return -1;
		}
		else if (ret_tftp == 0)//主数据库CRC不正确，备份的正确。
		{
			sqlite_print(("ann-----the backup db is good \n"));
			 //校验和不对，文件应该被覆盖。
			 if (CopyFile(file_path_save, file_path_backup) < 0)
			 {
				 sqlite_print(("ann-----the crc file covery error\n"));
			 }
			 else
			 {
				 //并添加新的校验和
				 if (GenerateFileCRC(file_path_backup,CRC_ANN_BACK_FILE) < 0)
				 {
					 sqlite_print(("ann-----the new crc Generate error\n"));
				 }
			 }
		}
	}
	else if (ret_value < 0)
	{
		sqlite_print( ("ann--222---the main db file crc is error\n" ) );
	}

	//将音频的主备数据库打开。
	if (OpenDbfile(&db_save, &db_backup, file_path_save, file_path_backup) < 0)
	{
		//roben_ye,添加状态标志。
		all_data_db_state = -1;
		return -1;
	}

  //比较主备文件的版本号，并根据不同的比较值，做出不同操作。
	if (CompareDBVersion(DB_version, &database_db, db_save, db_backup, file_path_save, file_path_backup) < 0)
	{
		//roben_ye,添加状态标志。
		all_data_db_state = -1;
		return -1;
	}

//	SqlLocalBufferFree(file_path_save);
	SqlTableBufferFree((void**)&file_path_save);
//	SqlLocalBufferFree(file_path_backup);
	SqlTableBufferFree((void**)&file_path_backup);

	return 0;
}

int OpenConfigDB(void)
{
	sqlite3 * db_save = NULL;
	sqlite3 * db_backup = NULL;
	all_config_db_state = 0;

	//版本号字符串应该控制在最大30
	char *file_path_save = SqlLocalBufferMalloc(MAX_STRING_LEN);
	char *file_path_backup = SqlLocalBufferMalloc(MAX_STRING_LEN);

	strcpy(file_path_save, SAVE_TFTP_CONFIG_DB_FILE);
	strcpy(file_path_backup, BACKUP_CONFIG_DB_FILE);

	//roben_yeTestFileCRC
	//通过比较文件生成校验和与保存的校验和，查看文件是否被改动。
	int ret_value = TestFileCRC(file_path_backup, CRC_CFG_BACK_FILE);
	if(ret_value > 0)
	{
		sqlite_print( ("cfg-----the main db file crc is error\n" ) );
		int ret_tftp = TestFileCRC(file_path_save, CRC_CFG_TFTP_FILE);
		if (ret_tftp > 0)//主、备数据库的CRC 均不正确
		{
			sqlite_print( ("cfg-----the back db file crc is error\n" ) );
			//roben_ye,添加状态标志。
			all_config_db_state = -1;
			return -1;
		}
		else if (ret_tftp == 0)//主数据库CRC不正确，备份的正确。
		{
			sqlite_print( ("cfg-----the back db file crc good\n" ) );
			 //校验和不对，文件应该被覆盖。
			 if (CopyFile(file_path_save, file_path_backup) < 0)
			 {
				 sqlite_print(("cfg-----the crc file covery error\n"));
			 }
			 else
			 {
				 //并添加新的校验和
				 if (GenerateFileCRC(file_path_backup,CRC_CFG_BACK_FILE) < 0)
				 {
					 sqlite_print(("cfg-----the new crc Generate error\n"));
				 }
			 }
		}
	}
	else if (ret_value < 0)
	{
		sqlite_print( ("cfg---222--the main file crc is error\n" ) );
	}
	//roben_yeTestFileCRC

	//将配置的主备数据库打开。
	if (OpenDbfile(&db_save, &db_backup, file_path_save, file_path_backup) < 0)
	{
		sqlite_print( ("OpenDbfile----error\n" ) );
		//roben_ye,添加状态标志。
		all_config_db_state = -1;
		config_db = NULL;
		return -1;
	}
	sqlite_print( ("OpenDbfile----end\n" ) );
	//通过比较版本号来，确定是否有，备份数据库和正常使用数据库。
	 //比较主备文件的版本号，并根据不同的比较值，做出不同操作。
	if (CompareDBVersion(config_DB_version, &config_db, db_save, db_backup, file_path_save, file_path_backup) < 0)
	{
		//roben_ye,添加状态标志。
		all_config_db_state = -1;
		return -1;
	}
	sqlite_print( ("CompareDBVersion----end\n" ) );

//	SqlLocalBufferFree(file_path_save);
	SqlTableBufferFree((void**)&file_path_save);
//	SqlLocalBufferFree(file_path_backup);
	SqlTableBufferFree((void**)&file_path_backup);

	return 0;
}


 //比较主备文件的版本号，并根据不同的比较值，做出不同操作。
//比较数据库版本号，完成上次可能没有完成保护操作。
//并给数据库handle指针赋予有效值（高版本数据库）。
//roben_ye,备份文件和使用文件的版本号相同,都是最高的，同时备份就是文件复制。
int CompareDBVersion(int db_type, sqlite3 **database_test, sqlite3 *db_save,
						sqlite3 *db_backup, char *file_path_save, char *file_path_backup)
{
	int row = 0;
	int row_tow = 0;
	int column = 0;
	void *p_db_save = NULL;
	void *p_db_backup = NULL;

	int *p_db_state = NULL;

	char *p_crc_file = NULL;

	//音频数据库
	if (DB_version == db_type)
	{
		p_crc_file = CRC_ANN_BACK_FILE;
		*database_test = db_save;
		SqlOfTableTDatabaseVersion(&row, &column, &p_db_save);

		*database_test = db_backup;
		SqlOfTableTDatabaseVersion(&row_tow, &column, &p_db_backup);

		p_db_state = &all_data_db_state;
		if ((row == 0) && (row_tow == 0))
		{
			//add,2014-06-06
			*p_db_state = -1;
			sqlite3_close(db_save);
			sqlite3_close(db_backup);
			*database_test = NULL;
			return -1;
		}
	}
	//配置数据库
	else if (config_DB_version == db_type)
	{
		p_crc_file = CRC_CFG_BACK_FILE;
//		sqlite_print( ("config_DB_version --11\n") );
		*database_test = db_save;
		SqlOfTConfigDBVersion(&row, &column, &p_db_save);
		sqlite_print( ("config_DB_version:%d\n",row) );

		*database_test = db_backup;
		SqlOfTConfigDBVersion(&row_tow, &column, &p_db_backup);

		sqlite_print( ("config_DB_version row_tow:%d\n",row_tow) );
		p_db_state = &all_config_db_state;
		if ((row == 0) && (row_tow == 0))
		{
			*p_db_state = -1;
			sqlite3_close(db_save);
			sqlite3_close(db_backup);
			*database_test = NULL;
			return -1;
		}
	}
	else
	{
		sqlite_print( ("the db type is error\n") );
		return -1;
	}

	//版本号比较
	//roben_ye,前提假设无数据查出来的row为0
	//sqlite3_get_table,如果只有列名，没有数据返回的row = 0；
	//					如果只有一条数据， row = 1；

	//先将两个数据库都关闭。最后在打开。、
	//作用：防止有些情况下的数据库在打开的状态；有些却需要都关闭，最后带开。
	sqlite3_close(db_save);
	sqlite3_close(db_backup);

	diag_printf( "p_db_save:%s\n",(((db_version_t*)p_db_save)->current_version) );
	diag_printf( "p_db_backup:%s\n",(((db_version_t*)p_db_backup)->current_version));

	//	sqlite_print( ("config_DB_version --3333\n") );
	//行数不为0，就表示会有数据
	//否者查询有误或者数据库中表为0
	if ((0 != row) && (0 != row_tow))
	{
		//roben_ye,不更新文件、数据库没坏 的正常的情况
		int ret = strcmp( (((db_version_t*)p_db_save)->current_version), (((db_version_t*)p_db_backup)->current_version));
		if (ret < 0)//backup 版本号高
		{
			//现在不是改名字，直接复制,将两个file都换成最高版本号的数据库
			if (CopyFile(file_path_backup,file_path_save) < 0)
			{
				sqlite_print( ("CopyFile is error --11\n") );
				return -1;
			}
		}
		else if(ret > 0)//save版本号高
		{
			sqlite_print( ("CompareDBVersion----222\n") );
//			//roben_ye,数据库才打开，还是可以用的。
//			//不能用db_save，因为这个文件可能在被跟新，那么查询就会没有数据了
////			*database_test = db_save;
////			sqlite3_close(db_backup);
//			sqlite3_close(db_backup);
//			sqlite3_close(db_save);

			//现在不是改名字，直接复制,将两个file都换成最高版本号的数据库
			if (CopyFile(file_path_save, file_path_backup) < 0)
			{
				sqlite_print( ("CopyFile is error --11\n") );
				return -1;
			}
			//对指定文件做crc 32 位校验和，并存储到指定文件。
			if(GenerateFileCRC(file_path_backup, p_crc_file) < 0)
			{
				return -1;
			}
		}
		else
		{
			sqlite_print( ("CompareDBVersi-----the version is same\n") );
		}
	}
	else if ((0 == row) && (0 != row_tow))//file file_path_save bad or deleted.
	{
		sqlite_print( ("CompareDBVersion----4444\n") );
		//现在不是改名字，直接复制,将两个file都换成最高版本号的数据库
		if (CopyFile(file_path_backup,file_path_save) < 0)
		{
			sqlite_print( ("CopyFile is error --333\n") );
			return -1;
		}
	}
	else if ((0 != row) && (0 == row_tow))//file file_path_backup bad or deleted.
	{
		sqlite_print( ("CompareDBVersion----5555\n") );
//		*database_test = db_save;
//		sqlite3_close(db_backup);

		//现在不是改名字，直接复制,将两个file都换成最高版本号的数据库
		if (CopyFile(file_path_save, file_path_backup) < 0)
		{
			sqlite_print( ("CopyFile is error --222\n") );
			return -1;
		}

		//对指定文件做crc 32 位校验和，并存储到指定文件。
		if(GenerateFileCRC(file_path_backup, p_crc_file) < 0)
		{
			return -1;
		}
	}
	else//两个文件都不存在
	{
		sqlite_print( ("CompareDBVersion----6666\n") );
		sqlite_print( ("db file is bad\n") );
		//设置本地全局变量，标明两个数据库带开失败。
		//函数读写其位置。

		*p_db_state = -1;//设置标志，如果是“1”，表示两个文件都不存在，其他函数检测到这种情况，
							//可能直接就需要调用 db文件更新函数了。
							//roben_ye_wen,不能更新，网络部分没有设置。
		*database_test = NULL;
		return -1;
	}

	//database_test本来就是sqlite3 **，所以不需要取地址
	int	ret = sqlite3_open(file_path_backup, database_test);
	if(ret)
	{
		sqlite_print( ("file_path_save: %s\n", sqlite3_errmsg(*database_test)) );
		sqlite3_close(*database_test);
		return -1;
	}

	SqlTableBufferFree((void**)&p_db_save);
	SqlTableBufferFree((void**)&p_db_backup);
	return 0;
}
