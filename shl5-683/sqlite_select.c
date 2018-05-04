/*
 * sqlite_select.c
 *
 *  Created on: 2013-4-30
 *      Author: benty
 */

#include"sqlite_select.h"

int ip_mac_old = 0;

 sqlite3 *database_db = NULL;//音频数据库打开的handle
 sqlite3 *config_db = NULL;//配置数据库打开的handle

// void CcuTestSqlSetAnnDb(sqlite3 *in_ann_db)
// {
//        if( NULL==in_ann_db )
//        {
//		sqlite_print(("CcuTestSqlSetAnnDb:  null ptr ^^^^ \n"));
//		return ;
//        }
//        database_db = in_ann_db;
//	return ;
// }
// void CcuTestSqlSetConfigDb(sqlite3 *in_config_db)
// {
//        if( NULL==in_config_db )
//        {
//		sqlite_print(("CcuTestSqlSetConfigDb:  null ptr ^^^^ \n"));
//		return ;
//        }
//        config_db = in_config_db;
//	return ;
// }


//返回查询符合条件的行数
//参数都只能有一个。
//参数类型都相通。
int TableRowSelect(sqlite3 *db, char *sql, int line_num)
{
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	int ncols = 0;

	int var = line_num;

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print( ("sqlite3_prepare err in:%s\n", sqlite3_errmsg(db)) );
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt,sqlite3_bind_parameter_index(stmt, ":stationName"),var);

	//执行查询语句。
	sqlite3_step(stmt);

	//从环境中提取查询结果。
	ncols = sqlite3_column_int(stmt, 0);//COUNT查询结果只有一个值，且从0开始。

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return ncols;
}
//********************emergency select ********************//

//将查询数据转换为指定的类型。
int EmergDataConvert(	sqlite3_stmt *stmt, int clom_num, emerg_table_item_t *p_data_place)
{
	int ncolum = 0;
	p_data_place->emerg_ann_id = (unsigned int)sqlite3_column_int(stmt, ncolum++);
	p_data_place->line_no = (unsigned int)sqlite3_column_int(stmt, ncolum++);
	p_data_place->repeat_times = (unsigned int)sqlite3_column_int(stmt, ncolum++);

	strncpy(p_data_place->audio_path, (const char *)sqlite3_column_text(stmt, ncolum++),100);
	strncpy(p_data_place->display_message, (const char *)sqlite3_column_text(stmt, ncolum++),400);
	strncpy(p_data_place->emerg_ann_name, (const char *)sqlite3_column_text(stmt, ncolum++),50);
	return 0;
}

//从数据库查询该线路号的所有紧急广播信息。
//如果结果正确，返回列数。
int EmergSelectFromDB(sqlite3 *db, int line_num, emerg_table_item_t *pToResult)
{
	char *sql = NULL;
	int ret = 0;
	int ncols = 0;
	sqlite3_stmt *stmt = NULL;
	const char * tail = NULL;
	emerg_table_item_t *p_data_buf = pToResult;

	int var = line_num;

	//Sql 语句。
	sql =	"	select  TEmergency.KeyId, lineNO, CycleTime, FilePath, "
			"			   StringContent, EmergShortName	"
			"	from  	 TEmergency , TAudioPath, TTextContent"	//查询紧急广播表
			"	where 	 (	   lineNO = (:stationName) "			//roben_ye,And 优先级高于or，所以这里需要括号。
			"			   		or lineNO = 0 			"
			"			  ) "		//共用广播默认线路号为0
			"			 and TEmergency.AudioID = TAudioPath.KeyId	"
			"			 and TEmergency.TextID =  TTextContent.KeyId ; ";

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print( ("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)) );
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt,sqlite3_bind_parameter_index(stmt, ":stationName"),var);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
	ncols = sqlite3_column_count(stmt);

	while (ret == SQLITE_ROW)
	{
		//填充申请的数据结构。
		EmergDataConvert(stmt, ncols, p_data_buf);
		p_data_buf++;

		ret = sqlite3_step(stmt);
	}

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return ncols;
}

//紧急广播搜索入口参数。
void* EmergenceSelectEntry(int line, int *row, int *column)
{
	int nrow = 0;
	int ncolm = 0;
	int var = line;

	char *sql =	"		select  count(*)	"
					"		from  TEmergency "	//查询紧急广播表

					"		where (  	"			//roben_ye,参数名不能变，函数是共用的。
					"					lineNO = (:stationName) "	//And 优先级高于or，所以这里需要括号。
					"					or lineNO = 0 "
					"				) ; ";		//共用广播都为0
	//当前线路下的所有区间。
	if ( (nrow = TableRowSelect(database_db, sql, var)) <= 0)
	{
		sqlite_print(("the Line Section is nothing--111\n"));
		return NULL;
	}

	//申请空间，存储查询结果。
	emerg_table_item_t *pToResult = NULL;
	unsigned int malloc_len = sizeof(emerg_table_item_t) * nrow + 1;
	pToResult = (emerg_table_item_t*)SqlLocalBufferMalloc(malloc_len);
	if (pToResult == NULL)
	{
		return NULL;
	}

	//查询表信息，得到结果。
	if( (ncolm = EmergSelectFromDB(database_db, var, pToResult)) < 0)
	{
		sqlite_print( ("error on EmergSelectFromDB\n") );
		return NULL;
	}

	*row = nrow;
	*column = ncolm;
	return (void*)pToResult;
}

int AnnReportRowSelect(sqlite3 *db, int line_num, int section_num)
{
//查询符合条件的线路号区间下报站（音频拼接）行数。
	int report_row = 0;//报站的行数
	int pre_row = 0;	//预报在的行数。
	int station_row = 0;//站代码行数
	int max_row = 0;


	//如果预报站查出来是0或者小于0，出错
	if ((pre_row = RowSelectReport(db, line_num, section_num, pre_station)) <= 0)
	{
		sqlite_print( ("the Line Section of pre report is error\n") );
		return -1;
	}

	//已到站的行数为标准。
	report_row = RowSelectReport(db, line_num, section_num, report_station);

//	roben_ye，用当前线路号下面的站数量作为
	//当前线路下的所有站代码的函数。
	char *sql =
			"		select  count(*)	"
			"		from  TStation "
			"		where lineNO = (:stationName) ";//"stationName"必须使用这个名字，TableRowSelect函数是共用的。

	//查询符合条件的行数。
	//2013-12-06
	if ( (station_row = TableRowSelect(db, sql, line_num)) <= 0)
	{
		sqlite_print( ("the Line Section is nothing----3333\n") );
		return -1;
	}
//	sqlite_print( ("report_row:%d\n",report_row) );
//	sqlite_print( ("pre_row:%d\n",pre_row) );
//	sqlite_print( ("station_row:%d\n",station_row) );

	max_row = ( (pre_row > report_row) ? pre_row : report_row);//获取最大行数。
	if (max_row < station_row)
	{
		max_row = station_row;
	}

	return max_row;
}

//报站表（音频拼接表）行数查询
int RowSelectReport(sqlite3 *db, int line_num, int section_num, int report_pre)
{
	char *sql = NULL;
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	int nrows = 0;

	sql =	"		select  count(*)	"

			"		from  TReportStation,TLineSection "	//查询线路号表

			"		where 			TReportStation.LineNumberID  = TLineSection.KeyId "
			"				and 	PreArrive = (:report_para) "
			"				and		TLineSection.LineNO = (:line_para) "
			"				and		TLineSection.LineSectionNO = (:section_para) ;";


	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print( ("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)) );
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt,sqlite3_bind_parameter_index(stmt, ":line_para"), line_num);
	sqlite3_bind_int(stmt,sqlite3_bind_parameter_index(stmt, ":section_para"), section_num);
	sqlite3_bind_int(stmt,sqlite3_bind_parameter_index(stmt, ":report_para"), report_pre);

	//执行查询语句。
	sqlite3_step(stmt);

	//从环境中提取查询结果。
	nrows = sqlite3_column_int(stmt, 0);//COUNT查询结果只有一个值，且从0开始。

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return nrows;
}

//报站表（音频拼接表）填充查询结果到申请空间。
//roben_ye,每次只填冲一条数据
int AnnReportDataConvert(sqlite3 *db, sqlite3_stmt *stmt, int nrow_num, int clom_num, int pre_get, ann_station_report_t *p_buf)
{
	int ncolum = 0;
	int nrow = 0;
	int ret = pre_get ;

	ann_station_report_t *p_struct_t = p_buf;

	unsigned int station_no = (unsigned int)sqlite3_column_int(stmt, ncolum++);

	while(nrow < nrow_num)
	{
		if ((station_no == (p_struct_t+nrow)->station_no ) || (0 == (p_struct_t+nrow)->station_no) )
		{
			p_struct_t = p_struct_t + nrow;

			p_struct_t->station_type[ret].pre_arrive = ret; ///< 到站

			p_struct_t->station_type[ret].station_next_no = (unsigned int)sqlite3_column_int(stmt, ncolum++);

			p_struct_t->station_type[ret].repeat_times = (unsigned int)sqlite3_column_int(stmt, ncolum++);
			p_struct_t->station_type[ret].open_door = (unsigned char)sqlite3_column_int(stmt, ncolum++);

			p_struct_t->station_type[ret].before_after = (unsigned int)sqlite3_column_int(stmt, ncolum++);
			p_struct_t->station_type[ret].triger_distance = (unsigned int)sqlite3_column_int(stmt, ncolum++);
			p_struct_t->station_type[ret].station_distance = (unsigned int)sqlite3_column_int(stmt, ncolum++);
			p_struct_t->station_type[ret].pre_triger_distance = (unsigned int)sqlite3_column_int(stmt, ncolum++);

			int audio_id = sqlite3_column_int(stmt, ncolum++);
//			p_struct_t->station_type[ret].audio = QueryDetailAudio(db, audio_id, &(p_struct_t->station_type[ret].audio_len));
			p_struct_t->station_type[ret].audio = QueryDetailAudio(db, audio_id);

			int text_id = sqlite3_column_int(stmt, ncolum++);
//			p_struct_t->station_type[ret].display = QueryDetailText(db, text_id, &(p_struct_t->station_type[ret].display_len));
			p_struct_t->station_type[ret].display = QueryDetailText(db, text_id);

			if (clom_num != ncolum)
			{
				sqlite_print( ("error on report DataConvert,maybe the some line is empty\n") );
				return -1;
			}

			break;
		}
		else
		{
			nrow++;
			continue;
		}
	}
	return 0;
}


//从数据库查询报站表（音频拼接表）的信息
int AnnReportSelectFromDB(sqlite3 *db, int nrow, int var_line,int var_section, ann_station_report_t *pToResult)
{
	int ncols = 0;//获取列参数。
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	char *sql = NULL;

	ann_station_report_t *p_data_buf = pToResult;

	sql =
			"	select "
			"		( 	select StationNO"
			"			from TStation"
			"			where  TStation.KeyId = TReportStation.StationID"
			"		), "
			"		(	select StationNO "
			"			from TStation "
			"			where  TStation.KeyId = TReportStation.NextStationID "
			"		), "
			"		 CycleTime, OpenDoorSide, Before_After, TrigDistance ,"
			"		StationDistance ,PreTrigDistance, AudioID, TextID "

			"	from 	TReportStation, TLineSection "

			"	where 			TReportStation.LineNumberID  = TLineSection.KeyId	"
			"			and		PreArrive = (:pre_or_arrive)"
			"			and		TLineSection.LineNO = (:line_name)"
			"			and		TLineSection.LineSectionNO = (:section_name)"
			;

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print(("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)));
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":line_name"), var_line);
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":section_name"), var_section);
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":pre_or_arrive"), report_station);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
	ncols = sqlite3_column_count(stmt);

	//roben_ye,有数据才会进入下面这个循环，如果查询的结果为0，不会进循环
	while (ret == SQLITE_ROW)
	{
		//填充报站的查询信息。
		//roben_ye,每次只填冲一条数据
		AnnReportDataConvert(db, stmt, nrow, ncols, report_station, p_data_buf);
//		sqlite_print(("entry the report table\n"));

		//roben_ye_wen,现在不能使用这一句话了，
		//上面将所有的区间信息都整理到
//		p_data_buf++;//指向下一个填充位置。

		ret = sqlite3_step(stmt);
	}

	sqlite3_reset(stmt);//不是sqlite3_clear_bindings
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":pre_or_arrive"), pre_station);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
	ncols = sqlite3_column_count(stmt);

	while (ret == SQLITE_ROW)
	{
//		sqlite_print(("entry the pre table\n"));
		//填充预报站的查询新消息。
		//roben_ye,每次只填冲一条数据
//		PreAnnDataConvert(db, stmt, nrow, ncols, pToResult);//存储位置，每次都查找，所以不用++
		AnnReportDataConvert(db, stmt, nrow, ncols, pre_station, p_data_buf);
		ret = sqlite3_step(stmt);
	}

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return ncols;
}

//报站预报站音频、显示查询的总入口。
void *AnnReportEntry(int line, int section_no, int *row, int *column)
{
	int nrow = 0;
	int nclom  = 0;
	int var_line = line;//线路号可变。
	int var_section = section_no;//区间号可变。

	if ( (nrow = AnnReportRowSelect(database_db, var_line, var_section)) <= 0)
	{
		sqlite_print( ("the Section has no report message\n") );
		return NULL;
	}

	//申请空间。用于保存查询结果
	ann_station_report_t *pToResult = NULL;

	unsigned int malloc_len = sizeof(ann_station_report_t) * nrow + 1;
	pToResult = (ann_station_report_t*)SqlLocalBufferMalloc(malloc_len);
	if (pToResult == NULL)
	{
		return NULL;
	}

	//先将station no 填充好。
	//2013-12-06
	//有问题，填充时还需要细致，0310,空行，乱序
	ReportStationNoFill(pToResult, nrow);
	//stationcodeentry

	//从数据库查询数据。
	if ( (nclom = AnnReportSelectFromDB(database_db, nrow, var_line, var_section, pToResult)) < 0)
	{
		sqlite_print( ("error on the AnnReportSelectFromDB\n") );
		return NULL;
	}

	*column = nclom;
	*row = nrow;
	return (void*)pToResult;
}

//将申请空间的站代码号，有序填充
int ReportStationNoFill(ann_station_report_t *pToResult,int nrow)
{
	int ret = 0;
	while(ret < nrow)
	{
		(pToResult + ret)->station_no = ret +1;
		ret++;
	}

	return 0;
}


//********************ann station report select ********************//



//********************station code select ********************//

//填充站码表查询结果到申请空间。
int StationDataConvert(sqlite3_stmt *stmt, int nrow_num, int clom_num, station_code_of_line_t *p_data_place)
{
	int ncolum = 0;
	int nrow = 0;

	station_code_of_line_t *p_struct_t = p_data_place;

	unsigned int station_no = (unsigned int)sqlite3_column_int(stmt, ncolum++);
//	sqlite_print( ("station_no:%d\n",station_no) );

	while(nrow < nrow_num)
	{
		if ((station_no == (p_struct_t+nrow)->station_no ) || (0 == (p_struct_t+nrow)->station_no) )
		{
			p_struct_t = p_struct_t + nrow;

			p_struct_t->station_no = station_no;

			p_struct_t->fix_across_station = (unsigned char)sqlite3_column_int(stmt, ncolum++);

			p_struct_t->spec_open_door = (unsigned char)sqlite3_column_int(stmt, ncolum++);

			strncpy(p_struct_t->station_audio, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);
			strncpy(p_struct_t->station_display, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);

			strncpy(p_struct_t->station_audio_eng, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);
			strncpy(p_struct_t->station_display_eng, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);
			break;
		}
		else
		{
			nrow++;
			continue;
		}
	}

	return 0;
}

//从数据库查询站代码的信息
int StationSelectFromDB(sqlite3 *db, int nrow, int var, station_code_of_line_t *pToResult)
{
	int ncols = 0;//获取列参数。
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	char *sql = NULL;

	station_code_of_line_t *p_data_buf = pToResult;

	//站序列号、是否固定越战、站名
	sql = "		select  StationNO, FixAcrossStation, SpecOpenDoorSide,"
			"			( select FilePath "				//中文音频
			"				from TAudioPath "
			"				where TStation.StationAudio = TAudioPath.KeyId	) "
			"	, "
			"			(select StringContent "			//中文显示
			"				from TTextContent "
			"				where TStation.StationDisplay = TTextContent.KeyId)"
			"	, "
			"   		( select FilePath "				//英文音频
			"				from TAudioPath "
			"				where TStation.StationAudioEng = TAudioPath.KeyId	)  "
			"	, "
			"			(select StringContent "			//英文显示
			"				from TTextContent "
			"				where TStation.StationDisplayEng = TTextContent.KeyId)"

			"	from TStation "
			"	where LineNO = (:stationName)  ";

	//order by StationNO；查询出来有问题，不是按照序号走的
	//ASC为升序排列,DESC为降序排列


	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print( ("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)) );
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":stationName"), var);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
	ncols = sqlite3_column_count(stmt);

	while (ret == SQLITE_ROW)
	{
		//填充申请的数据结构。
		StationDataConvert(stmt, nrow, ncols, p_data_buf);

		//	//roben_ye_wen,现在不能使用这一句话了，
		//	//上面将所有的区间信息都整理到
		//指向下一个填充数据位置。
//		p_data_buf++;

		//执行查询语句。
		ret = sqlite3_step(stmt);
	}


	//释放准备的查询环境。
	sqlite3_finalize(stmt);

	//返回列的数量。
	return ncols;
}


//线路站代码查询的入口。
void *StationCodeEntry(int line, int *row, int *column)
{
	int nrow = 0;
	int nclom  = 0;
	//线路号可变。
	int var = line;

	//当前线路下的所有站代码的函数。
	char *sql =
			"		select  count(*)	"
			"		from  TStation "
			"		where lineNO = (:stationName) ";//"stationName"必须使用这个名字，TableRowSelect函数是共用的。

	//查询符合条件的行数。
	if ( (nrow = TableRowSelect(database_db, sql, var)) <= 0)
	{
		sqlite_print( ("the Line Section is nothing----3333\n") );
		return NULL;
	}
	sqlite_print( ("TableRowSelect:nrow:%d\n",nrow) );

	//申请空间。用于保存查询结果
	station_code_of_line_t *pToResult = NULL;

	unsigned int malloc_len = sizeof(station_code_of_line_t) * nrow + 1;
	pToResult = (station_code_of_line_t*)SqlLocalBufferMalloc(malloc_len);
	if (pToResult == NULL)
	{
		return NULL;
	}

	//填从站号，并保证站号有序
	StationNoFill(database_db, nrow, var, pToResult);

	//从数据库查询数据。
	if ( (nclom = StationSelectFromDB(database_db,nrow, var, pToResult)) < 0)
	{
		sqlite_print( ("error on the SectionSelectFromDB\n") );
		return NULL;
	}

	*row = nrow;				//返回行信息
	*column = nclom;			//返回列信息
	return (void*)pToResult;	//返回线路站代码的查询结果
}

//保证查询出来的站序号是有序的
int StationNoFill(sqlite3 *db, int nrow, int var, station_code_of_line_t *pToResult)
{
	int ncols = 0;//获取列参数。
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	char *sql = NULL;

	station_code_of_line_t *p_data_buf = pToResult;

	//站序列号、是否固定越战、站名
	sql = "		select  StationNO "
			"	from TStation "
			"	where LineNO = (:stationName) "
			"   order by StationNO asc ; ";//roben_ye_wen,这里必

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print( ("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)) );
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":stationName"), var);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	unsigned int station_no = 0;
	while (ret == SQLITE_ROW)
	{
		//填充申请的数据结构。
		StationNoFill2(stmt, nrow, ncols, p_data_buf);

		//执行查询语句。
		ret = sqlite3_step(stmt);
	}


	//释放准备的查询环境。
	sqlite3_finalize(stmt);

	StationOrderByAsc(nrow, p_data_buf);

	//roben_ye_test;

	return 0;
}

//从小到大的排列。
int StationOrderByAsc(int nrow_num, station_code_of_line_t *p_data_place)
{
	int ret_row1 = 0;
	int ret_row2 = 0;
	int ret_middle = 0;

	for (; ret_row1 < nrow_num; ret_row1++ )
	{
		for(ret_row2 = 0; ret_row2 < nrow_num; ret_row2++)
		{
			if (((p_data_place+ret_row2)->station_no != 0)&&((p_data_place+ret_row1)->station_no != 0))
			{
				if ((p_data_place+ret_row2)->station_no  > (p_data_place+ret_row1)->station_no)
				{
					ret_middle = (p_data_place+ret_row2)->station_no;
					(p_data_place+ret_row2)->station_no = (p_data_place+ret_row1)->station_no;
					(p_data_place+ret_row1)->station_no = ret_middle;
				}
			}

		}
	}

	return 0;
}

//站序号填充。
int StationNoFill2(sqlite3_stmt *stmt, int nrow_num, int clom_num, station_code_of_line_t *p_data_place)
{
	int ncolum = 0;
	int nrow = 0;

	station_code_of_line_t *p_struct_t = p_data_place;

	unsigned int station_no = (unsigned int)sqlite3_column_int(stmt, ncolum++);

	while(nrow < nrow_num)
	{
		if ((station_no == (p_struct_t+nrow)->station_no ) || (0 == (p_struct_t+nrow)->station_no) )
		{
			p_struct_t = p_struct_t + nrow;

			p_struct_t->station_no = station_no;

			break;
		}
		else
		{
			nrow++;
			continue;
		}
	}

	return 0;
}



//********************station code select ********************//



//********************line section select ********************//

//填充区间表查询结果到申请空间。
//必须按照数据结构填充，所以每个查询函数的填充都不一样。
int LineSectionDataConvert(sqlite3_stmt *stmt, int clom_num, line_section_table_t *p_data_place)
{
	int ncolum = 0;

	//sqlite3_column_int将查询结果中的相应列，转换为int型。
	p_data_place->line_no = sqlite3_column_int(stmt, ncolum++);
//	Roben_Print("p_data_place->line_no;%d\n",p_data_place->line_no);

	p_data_place->line_direction = (unsigned char)sqlite3_column_int(stmt, ncolum++);
	p_data_place->spec_cycle = (unsigned char)sqlite3_column_int(stmt, ncolum++);
//	sqlite_print( ("spec_cycle:%d\n",p_data_place->spec_cycle) );
	strncpy(p_data_place->short_name, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);
//	sqlite_print( ("short_name:%s\n",p_data_place->short_name) );
	strncpy(p_data_place->start_time, (const char *)sqlite3_column_text(stmt, ncolum++),10);
	strncpy(p_data_place->end_time, (const char *)sqlite3_column_text(stmt, ncolum++),10);
	p_data_place->default_section = (unsigned char)sqlite3_column_int(stmt, ncolum++);
	p_data_place->section_no = (unsigned int)sqlite3_column_int(stmt, ncolum++);

	p_data_place->start_station= (unsigned int)sqlite3_column_int(stmt, ncolum++);
	p_data_place->end_station=(unsigned int)sqlite3_column_int(stmt, ncolum++);

	//检查填充是否完成。
	if (clom_num != ncolum)
	{
		sqlite_print( ("error on EmergDataConvert\n") );
		return -1;
	}

	return 0;
}

//从数据库查询区间号的信息
int SectionSelectFromDB(sqlite3 *db, int var, line_section_table_t *pToResult)
{
	int ncols = 0;//获取列参数。
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	char *sql = NULL;

	line_section_table_t *p_data_buf = pToResult;

	sql = 	"	select "
			"		LineNO, UPDown, SpecCycle,ShortName, "
			"		StartTime,EndTime, DefaultSection, LineSectionNO "
//			"		,StartStation,EndStation "

			//起始站
			"		,	(select StationNO"
			"				from TStation"
			"				where TStation.KeyId = TLineSection.StartStation),"
			//	终点站
			"	(select StationNO "
			"		from TStation "
			"		where TStation.KeyId = TLineSection.EndStation ) "

			"	from TLineSection "
			"	where TLineSection.LineNO = (:stationName); ";

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print( ("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)) );
		return -1;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt,sqlite3_bind_parameter_index(stmt, ":stationName"),var);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
	ncols = sqlite3_column_count(stmt);

	while (ret == SQLITE_ROW)	//has another row ready，不是查询结果的最后一行。
	{
		//填充申请的数据结构。
		LineSectionDataConvert(stmt, ncols, p_data_buf);
		p_data_buf++;//指向下一个数据填充位置。

		//执行查询语句。
		ret = sqlite3_step(stmt);
	}

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return ncols;		//返回列参数。
}

//线路的区间查询的入口。
void *LineSectionSelectEntry(int line, int *row, int *column)
{
	int nrow = 0;
	int nclom  = 0;
	int var = line;//线路号可变。

	char *sql =
			"		select  count(*)	"

			"		from  TLineSection "	//查询线路号表

			"		where lineNO = (:stationName) ";

	//当前线路下的所有区间。
	if ( (nrow = TableRowSelect(database_db, sql, var)) <= 0)
	{
		sqlite_print( ("the Line Section is nothing---444\n") );
		return NULL;
	}

//	Roben_Print("LineSectionOfEntry---nrow:%d\n",nrow);

	//申请空间。用于保存查询结果
	line_section_table_t *pToResult = NULL;

	unsigned int malloc_len = sizeof(line_section_table_t) * nrow + 1;
	pToResult = (line_section_table_t*)SqlLocalBufferMalloc(malloc_len);
	if (pToResult == NULL)
	{
		return NULL;
	}

	//从数据库查询数据。
	if ( (nclom = SectionSelectFromDB(database_db, var, pToResult)) < 0)
	{
		sqlite_print( ("error on the SectionSelectFromDB\n") );
		return NULL;
	}

	*row = nrow;				//返回行信息
	*column = nclom;			//返回列信息
	return (void*)pToResult;	//返回线路区间的查询结果
}


//释放查询结果所占用空间。为配套使用。
int SqliteAllTableFree(char **SqlResult)
{
	char **p_free_buf = SqlResult;
	if ( 0 == p_free_buf)//可能已经释放，或者参数为空值。
							//roben_ye,注意char**的释放形式。
	{
		;
	}
	else
	{
		//Roben_Print("entry SqliteAllTableFree\n");
		sqlite3_free_table(p_free_buf);	//释放空间。
		p_free_buf = 0;
	}
	return 0;
}
//组合成为真正的sql查询语句。整个表查询才使用
char *SqlStringConnect(char *table_name)
{
	char *sql_sentence = NULL;

	if(strcmp("TIpMac ;",table_name) == 0)//IP-mac表
	{
		sqlite_print(("-----111--------ip_mac_old----%d----\n",ip_mac_old));
		char *ptr_sql= NULL;
		if (ip_mac_old != 0)
		{

			sqlite_print(("-------------the mac is old--------\n"));


			ptr_sql = "	select  DeviceName,DeviceNUM, IP, MAC,ServerNorNot "
						 "	from TIpMac; ";
		}
		else
		{
			ptr_sql= "	select  TIpMac.DeviceName,DeviceNUM, IP, MAC,ServerNorNot,DeviceType "
						 "	from 	TIpMac,TDeviceType"
						 "  where  TIpMac.DeviceName = TDeviceType.DeviceName ; ";
		}
		unsigned int len = strlen(ptr_sql);
		sql_sentence = (char*)SqlLocalBufferMalloc(len);
		if (sql_sentence == NULL)
		{
			sqlite_print(("operate failed: sql_sentence malloc--222\n"));
			return NULL;
		}
		else
		{
			bzero(sql_sentence, len);

			//先将不全的“sql_old”的复制到新申请的空间。
			strcpy(sql_sentence, ptr_sql);

			return sql_sentence;
		}
	}

	if(strcmp("TPromptAudio ;",table_name) != 0)
	{
		const char *sql_old = "SELECT * FROM  ";//表的名称不能作为参数。所以采用下面这种方式。

		//申请足够的空间保存合成的sql语句。
		unsigned int len = strlen(sql_old) + strlen(table_name);
		sql_sentence = (char *)SqlLocalBufferMalloc(len + 1);
		if (sql_sentence == NULL)
		{
			sqlite_print(("operate failed: sql_sentence malloc\n"));
			return NULL;
		}
		else
		{
			bzero(sql_sentence, len);

			//先将不全的“sql_old”的复制到新申请的空间。
			strcat(sql_sentence, sql_old);

			//将数据表的名称接在新申请的空间的尾部，形成新的完整的查询语句。
			strcat(sql_sentence, table_name);
		}

	}
	else	//roben_ye,TPromptAudio表是关联表，但是也是整体查询，所以放在这里
	{
		char *ptr_sql= "	select  EventType,FilePath "
					 "	from 	TAudioPath,TPromptAudio"
					 "  where  TPromptAudio.AudioID = TAudioPath.KeyId ; ";

		unsigned int len = strlen(ptr_sql);
		sql_sentence = (char*)SqlLocalBufferMalloc(len);
		if (sql_sentence == NULL)
		{
			sqlite_print(("operate failed: sql_sentence malloc--222\n"));
			return NULL;
		}
		else
		{
			bzero(sql_sentence, len);

			//先将不全的“sql_old”的复制到新申请的空间。
			strcpy(sql_sentence, ptr_sql);
		}
	}

//	Roben_Print("sql_sentence:%s\n",sql_sentence);

	return sql_sentence;
}

//查询结果，并将行、列、查询结果返回。
char **SqlSelectFromDB(sqlite3 *db, char *table_name, int *row, int *column)
{
	int nrow = 0, ncolumn = 0;
	char **azResult = NULL;
//	int i = 0;
	char *zErrMsg = NULL;

	if (db != NULL)
	{
		char *sql = NULL;
		//查询语句合成。
		sql = SqlStringConnect(table_name);
//		diag_printf("sql = %s\n",sql);
		if (sql == NULL)
		{
			sqlite_print(("operate failed: SqlStringConnect\n"));
			azResult = NULL;
		}

		int ret = 0;
//		strcpy()
		ret = sqlite3_get_table(db, sql, &azResult, &nrow, &ncolumn, &zErrMsg);
		//roben_ye,如果查询没有出错，即使返回行数为0，也是正确的。第0行返回的是表中的列名

		//全表查询。
		if(SQLITE_OK != ret)
		{
			//SQLITE_MISUSE      21   /* 不正确的库使用
			sqlite_print( ("operate failed----111 ret:%d,%s\n",ret, zErrMsg) );

			ip_mac_old = 1;
			azResult = NULL;
		}

		//申请的空间在“SqlStringConnect”中。
		free(sql);
		sql = NULL;
	}

	*row = nrow;			//返回行信息
	*column = ncolumn;	//返回列信息
	return azResult;		//返回查询结果（都是以字符串形式存储的），首行保存的是数据表的列名。
}
//SQL 查询想要的数据表。
//数据库句柄为全局变量。
char **AllTableSelectDB(int event, int *row, int *column)
{
	char **pToresult = NULL;
	char *table_name = NULL;
	switch(event)
	{
		case ip_mac_config:				//设备配置表。
			table_name = "TIpMac ;";		//在字符串结尾处，一定要有分号
			pToresult = SqlSelectFromDB(config_db, table_name, row, column);
			if (NULL == pToresult)
			{
				pToresult = SqlSelectFromDB(config_db, table_name, row, column);
			}

			break;

		case config_DB_version:			//配置数据库版本号表
			table_name = "TConfigDBVersion ;";
			pToresult = SqlSelectFromDB(config_db, table_name, row, column);
			break;

		case device_count:				//设备数量表
			table_name = "TDeviceNumber ;";
			pToresult = SqlSelectFromDB(config_db, table_name, row, column);
			break;

		case DB_version:					//数据库版本号表
			table_name = "TDatabaseVersion ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

			//合并为路径表
			//		case log_path:						//log日志保存路径表
			//		case record_path:					//录音保存路径表
		case file_path:						//路径表
			table_name = "TFilePath ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case prompt_audio:						//提示音表
			table_name = "TPromptAudio ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case monitor:						//监听音源表
			table_name = "TMonitor ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case PTT_last_time:						//PTT默认持续时间
			table_name = "TPTTLastTime ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case volume_control_Bytime:						//时间段音量调节
			table_name = "TVolumeControlByTime ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case language_and_font:						//语言与字体
			table_name = "TLanguageAndFont ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case priority:						//优先级配置表
			table_name = "TPriority ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case audio_refreesh_path:		//音频文件更新统计表
			table_name = "TRefreshAudioPath ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case noise_check:					//噪音检测表
			table_name = "TNoiseCheck ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;

		case line_ID:					//检测列车的所有线路，并判断是否是默认的线路号。
			table_name = "TLine ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
			break;
		case TSimulationTimeInterval ://检查所有模拟设置信息
			table_name = "TSimulationTimeInterval ;";
//			table_name = "TDeviceVersion  ;";
			pToresult = SqlSelectFromDB(database_db, table_name, row, column);
//			diag_printf("444-%d-%d\n",*row,*column);
			break;
		default:
			sqlite_print(("error event case\n"));
			pToresult = NULL;
			break;
	}

	return pToresult;		//错误，返回NULL， 检查。
}

//申请指定长度的空间，并初始化为0
void *SqlLocalBufferMalloc( unsigned int wantedSize )
{
	void *pv_return = NULL;

	pv_return = malloc( wantedSize );

	if ( pv_return == NULL)
	{
		sqlite_print(("error on malloc\n"));
		return NULL;
	}

	bzero(pv_return, wantedSize);

	return pv_return;
}
//释放空间
void SqlLocalBufferFree( void *pv )
{
	if( pv )
	{
       free( pv );
       pv = NULL;
	}
}

//释放查询的空间
void SqlTableBufferFree(void **pv)
{
	if( *pv != NULL )
	{
		//roben_ye,如果pv指向NULL，strlen()函数会导致死机，越界了。
//		Roben_Print("SqlLocalBufferFree-111：%d\n",strlen((char*)pv));
       free( *pv );
       *pv = NULL;
	}
}



//报站音频释放空间
void AnnReportBufferFree(void **pv,int row)
{
	 sqlite_print(("AnnReportBufferFree -entry\n"));

	if( *pv != NULL )
	{
		int ret = 0;
		ann_station_report_t *pTobuf = (ann_station_report_t*)(*pv);

		for (;ret < row; ret++)
		{
			if ((pTobuf+ret)->station_type[0].audio != NULL)
			{
				free((pTobuf+ret)->station_type[0].audio);
				(pTobuf+ret)->station_type[0].audio = NULL;
			}
			if ((pTobuf+ret)->station_type[0].display != NULL)
			{
				free((pTobuf+ret)->station_type[0].display);
				(pTobuf+ret)->station_type[0].display = NULL;
			}
			if ((pTobuf+ret)->station_type[1].audio != NULL)
			{
				free((pTobuf+ret)->station_type[1].audio);
				(pTobuf+ret)->station_type[1].audio = NULL;
			}
			if ((pTobuf+ret)->station_type[1].display != NULL)
			{
				free((pTobuf+ret)->station_type[1].display);
				(pTobuf+ret)->station_type[1].display = NULL;
			}
		}

	   free( *pv );
	   *pv = NULL;
	}
}

//查询具体的、所有的拼接音频文件
audio_display_list * QueryDetailAudio(sqlite3 *db, int coarse_id)
{
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	int file_connect_len = 0;

	char *sql =
			"	select count(*)"
			"	from 	TConnectAudioID "
			"	where 	ConnectAudioID = (:audio_para) ; "
			;
	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print(("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)));
		return NULL;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":audio_para"), coarse_id);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//从环境中提取查询结果。
	//获取行参数。
	file_connect_len = sqlite3_column_int(stmt, 0);//COUNT查询结果只有一个值，且从0开始。;

	//最后添加一行，会被置为空字符串，用于告诉其他地方，拼接文件到末尾了。
	file_connect_len++;
//	Roben_Print("::%d\n", file_connect_len);

	//释放准备的查询环境。
	sqlite3_finalize(stmt);

//	Roben_Print("audio_list_node is %d\n", *list_node);
	//申请audio_display_list;
	audio_display_list *p_data_buf = SqlLocalBufferMalloc( file_connect_len*sizeof(audio_display_list) );
	if (p_data_buf == NULL)
	{
		sqlite_print(("error on malloc\n"));
		return NULL;
	}
	audio_display_list *p_head = p_data_buf;

	sql = " SELECT		 FilePath , Attribute, TConnectAudioID.keyId "
			"FROM	 	 TConnectAudioID, TAudioPath  "
			"WHERE		 ConnectAudioId = (:audio_para) "
			"			AND AudioId = TAudioPath.keyId  "
			"			order by TConnectAudioID.keyId asc ; ";//roben_ye_wen,这里必须要有顺序
																	//ASC为升序排列,DESC为降序排列

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print(("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)));
		return NULL;
	}
	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":audio_para"), coarse_id);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
	//	ncols = sqlite3_column_count(stmt);

//	sqlite_print(("################coarse_id:%d\n",coarse_id));
	while (ret == SQLITE_ROW)
	{
//		//填充音频、显示链表的结构体。
		int ncolum = 0;

		strncpy(p_data_buf->audio_display, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);
		p_data_buf->file_attr = (unsigned int)sqlite3_column_int(stmt, ncolum++);

//		sqlite_print(("QueryDetailAudio:audio_display:%s\n",p_data_buf->audio_display));
//		sqlite_print(("attribute:%d\n",p_data_buf->file_attr));

		p_data_buf++;//指向下一个填充位置。

		ret = sqlite3_step(stmt);
	}

//	sqlite_print(("p_data_buf->audio_display:%s\n",p_data_buf->audio_display));
//	p_data_buf->audio_display = NULL;

	//如果直接这样写代码“NULL == p_data_buf->audio_display”，会出现错误。
	//incompatible types when assigning to type ‘unsigned char[100]’ from type ‘void *’
//	if(0 == p_data_buf->audio_display[0])
//	{
//		sqlite_print(("p_data_buf->audio_display:%s\n",p_data_buf->audio_display));
//	}
//	sqlite_print(("\n"));

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return p_head;
}

//2014-01-23，字幕颜色还没有查询
//2014-01-20,字幕文件属性添加
//查询具体的、所有的拼接字幕文件
audio_display_list * QueryDetailText(sqlite3 *db, int coarse_id)
{
	int ret = 0;
	sqlite3_stmt *stmt = NULL;
	const char *tail = NULL;
	int file_connect_len = 0;

	char *sql =
			"	select count(*)"
			"	from 	TConnectTextID "
			"	where 	ConnectTextID = (:text_para)"
			;
	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print(("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)));
		return NULL;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":text_para"), coarse_id);


	//执行查询语句。
	ret = sqlite3_step(stmt);

	//从环境中提取查询结果。
	//获取行参数。

	file_connect_len = sqlite3_column_int(stmt, 0);//COUNT查询结果只有一个值，且从0开始。;

	//最后添加一行，会被置为空字符串，用于告诉其他地方，拼接文件到末尾了。
	file_connect_len++;


	//释放准备的查询环境。
	sqlite3_finalize(stmt);

//	Roben_Print("*list_node is %d\n", *list_node);
	//申请audio_display_list;
	audio_display_list *p_data_buf = SqlLocalBufferMalloc( file_connect_len*sizeof(audio_display_list) );
	audio_display_list *p_head = p_data_buf;

	//"Attribute",颜色属性，以前的属性标志，所以没有修改
	//“FileAttribute”，文件属性
	sql =  "	select StringContent, Attribute, FileAttribute, TConnectTextID.KeyId"
			"	from 	TConnectTextID,TTextContent "
			"	where 	ConnectTextID = (:text_para) "
			"			and  TextId = TTextContent.KeyId "
			"	order by TConnectTextID.KeyId asc ;"	//roben_ye_wen,这里必须要有顺序
															//ASC为升序排列,DESC为降序排列
			;

	//查询环境准备。
	ret = sqlite3_prepare(db, sql, (int)strlen(sql), &stmt, &tail);
	if (ret != SQLITE_OK)
	{
		sqlite_print(("sqlite3_prepare err in:%s\n",sqlite3_errmsg(db)));
		return NULL;
	}

	//绑定参数。
	//sqlite3_bind_parameter_index,获取bind参数的“位置“
	sqlite3_bind_int(stmt, sqlite3_bind_parameter_index(stmt, ":text_para"), coarse_id);

	//执行查询语句。
	ret = sqlite3_step(stmt);

	//统计列数，固定查询语句之后，列数都相同。没有的sqlite会用NULL补齐。
//	ncols = sqlite3_column_count(stmt);

	while (ret == SQLITE_ROW)
	{
//		//填充音频、显示链表的结构体。
		int ncolum = 0;

		strncpy(p_data_buf->audio_display, (const char *)sqlite3_column_text(stmt, ncolum++),MAX_STRING_LEN);

		//roben_ye,这里不能用sqlite3_column_int和atoi，atol函数
		//这些函数只能转换10以内的数字字符串。
		//PTU中，属性列现在对应于颜色。2014-01-13.
		p_data_buf->color_attr = strtol(sqlite3_column_text(stmt, ncolum++), NULL, 16);

		//PTU功能不完整，不能填写RGB值。
		if (p_data_buf->color_attr != 0xff0000)
		{
			p_data_buf->color_attr = 0x00ff00;//绿色
		}

		p_data_buf->file_attr = strtol(sqlite3_column_text(stmt, ncolum++), NULL, 16);

		p_data_buf++;//指向下一个填充位置。

		ret = sqlite3_step(stmt);
	}

	//释放准备的查询环境。
	sqlite3_finalize(stmt);
	return p_head;
}


