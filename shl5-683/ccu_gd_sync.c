/*
 * Copyright (c) 2013, MIT, All right reserved.
 *
 * File Name:	ccu_gd_sync.c
 * File Identification: 
 * Abstract:  
 *
 * Current Version:	V2013_0415_0.0.1
 * Author:	AlvinChang
 * Completion Date:	2013-04-16
 *
 * Last Version:
 * Last Author:
 * Finishing Date:
 */

#include "ccu_gd_sync.h"

//#define CONFIG_CCU_NO_DB_FILE ///< just debug without db file

static int sequeue_waiting_for_response = -1;
static unsigned int sending_counter = 0;
static ann_station_report_t *gb_ann_array_ptr = NULL;
static dev_config_table_item_t *local_ccu_config_info = NULL;
static unsigned char ccu_has_read_ip_table = 0;
static unsigned char dest_dev_no_for_table_sync = 0;

extern int TryCopyPartOfGlobalDevTable(int dev_type, int dev_no, void *dev_state_context);
extern int ReadType(void);      
extern int ReadNumWilson(void);
extern int CcuGdSyncTestDb(void);

#ifdef CONFIG_CCU_NO_DB_FILE
line_table_t  sim_default_line_table[] = {
   {3,0},
   {4,1}
};
station_code_of_line_t  sim_line3_station_code_table[] =
{
     {1, 0, 0, "shanghai", "/line3/1_1.mp3", "yi shan lu"},
     {2, 0, 0, "shilong", "/line3/2_1.mp3", "yi shan lu-2"},
     {3, 0, 0, "longcao", "/line3/3_1.mp3", "yi shan lu-3"}
};
#endif

static dev_type_name_t GbDevName[4] =
{
      	{4, "PCU"}, 
      	{5, "CCU"}, 
      	{6, "BCU"},  
      	{7, "EAMP"}
};

///< test db file, begin, 1126
int display_thread_id = 1; ///< <1>: tms;  <2>: analog;  <3>: tftpd;  <4>: net; <5>: control
unsigned char test_read_db_tms = 255; ///< debug, 1126
unsigned char test_read_db_analog = 255; ///< debug, 1126
unsigned char test_read_db_tftp = 255; ///< debug, 1126
unsigned char test_read_db_net = 255; ///< debug, 1126
unsigned char test_read_db_control = 255; ///< debug, 1126
int CcuGetReadTableDisplaySwitch(void)
{
      return (display_thread_id);
}
void CcuSetReadTableDisplaySwitch(int next_id)
{
      display_thread_id = next_id;
      return ;
}
int CcuTestReadIpMacTable_tms(int *out_row, void **out_table)
{
#if 0
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      ip_mac_config_t *ip_table_item = NULL;

     SqlOfTableTIpMac((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (ip_mac_config_t *)ip_table;
     //CcuGdDisplayTableTIpMac(row, ip_table);
#endif

#if 1
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      db_version_t *ip_table_item = NULL;

     SqlOfTableTDatabaseVersion((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (db_version_t *)ip_table;
#endif

     if( NULL == ip_table_item )
     {
           debug_print(("CcuTestReadIpMacTable_tms:  null ptr \n"));
	   return -1;
     }
     else if( row<=0 )
     {
           debug_print(("CcuTestReadIpMacTable_tms:  row=0 \n"));
	   return -1;
     }

     if( NULL != out_row )
     {
           (*out_row) = row;
           debug_print(("CcuTestReadIpMacTable_tms:  row=%d \n", row));		   
     }
     if( NULL != out_table )
     {
           (*out_table) = ip_table;
           debug_print(("CcuTestReadIpMacTable_tms:  table ptr=%08x \n", ip_table));
     }	 
     //SqlLocalBufferFree((void *)ip_table);

     return 0;
}
int CcuTestReadIpMacTable_analog(int *out_row, void **out_table)
{
#if 0
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      ip_mac_config_t *ip_table_item = NULL;

     SqlOfTableTIpMac((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (ip_mac_config_t *)ip_table;
     //CcuGdDisplayTableTIpMac(row, ip_table);
#endif 

#if 1
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      station_code_of_line_t *ip_table_item = NULL;	  
     StationCodeOfEntry(3, (int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (station_code_of_line_t *)ip_table;
#endif

     if( NULL == ip_table_item )
     {
           debug_print(("CcuTestReadIpMacTable_analog:  null ptr \n"));
	   return -1;
     }
     else if( row<=0 )
     {
           debug_print(("CcuTestReadIpMacTable_analog:  row<=0 \n"));
	   return -1;
     }

     if( NULL != out_row )
     {
           (*out_row) = row;
           debug_print(("CcuTestReadIpMacTable_analog:  row=%d \n", row));		   
     }
     if( NULL != out_table )
     {
           (*out_table) = ip_table;
           debug_print(("CcuTestReadIpMacTable_analog:   table ptr=%08x \n", ip_table));
     }	 
     //SqlLocalBufferFree((void *)ip_table);

     return 0;
}
int CcuTestReadIpMacTable_tftpd(int *out_row, void **out_table)
{
#if 0
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      ip_mac_config_t *ip_table_item = NULL;

     SqlOfTableTIpMac((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (ip_mac_config_t *)ip_table;
     //CcuGdDisplayTableTIpMac(row, ip_table);
#endif

#if 1
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      line_table_t *ip_table_item = NULL;

     SqlOfTableTline((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (line_table_t *)ip_table;
#endif

     if( NULL == ip_table_item )
     {
           debug_print(("CcuTestReadIpMacTable_tftpd:  null ptr \n"));
	   return -1;
     }
     else if( row<=0 )
     {
           debug_print(("CcuTestReadIpMacTable_tftpd:  row<=0 \n"));
	   return -1;
     }

     if( NULL != out_row )
     {
           (*out_row) = row;
           debug_print(("CcuTestReadIpMacTable_tftpd:  row=%d \n", row));		   
     }
     if( NULL != out_table )
     {
           (*out_table) = ip_table;
           debug_print(("CcuTestReadIpMacTable_tftpd:   table ptr=%08x \n", ip_table));
     }	 
     //SqlLocalBufferFree((void *)ip_table);

     return 0;
}
int CcuTestReadIpMacTable_net(int *out_row, void **out_table)
{
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      ip_mac_config_t *ip_table_item = NULL;

     SqlOfTableTIpMac((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (ip_mac_config_t *)ip_table;
     //CcuGdDisplayTableTIpMac(row, ip_table);
     if( NULL == ip_table_item )
     {
           debug_print(("CcuTestReadIpMacTable_net:  null ptr \n"));
	   return -1;
     }
     else if( row<=0 )
     {
           debug_print(("CcuTestReadIpMacTable_net:  row<=0 \n"));
	   return -1;
     }

     if( NULL != out_row )
     {
           (*out_row) = row;
           debug_print(("CcuTestReadIpMacTable_net:  row=%d \n", row));		   
     }
     if( NULL != out_table )
     {
           (*out_table) = ip_table;
           debug_print(("CcuTestReadIpMacTable_net:   table ptr=%08x \n", ip_table));
     }	 
     //SqlLocalBufferFree((void *)ip_table);

     return 0;
}
int CcuTestReadIpMacTable_control(int *out_row, void **out_table)
{
#if 0
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      ip_mac_config_t *ip_table_item = NULL;

     SqlOfTableTIpMac((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (ip_mac_config_t *)ip_table;
     //CcuGdDisplayTableTIpMac(row, ip_table);
#endif

#if 1
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      priority_id_t *ip_table_item = NULL;
      SqlOfTPriority((int *)&row, (int *)&column, (void **)&ip_table);
      ip_table_item = (priority_id_t *)ip_table;
#endif

     if( NULL == ip_table_item )
     {
           debug_print(("CcuTestReadIpMacTable_control:  null ptr \n"));
	   return -1;
     }
     else if( row<=0 )
     {
           debug_print(("CcuTestReadIpMacTable_control:  row<=0 \n"));
	   return -1;
     }

     if( NULL != out_row )
     {
           (*out_row) = row;
           debug_print(("CcuTestReadIpMacTable_control:  row=%d \n", row));		   
     }
     if( NULL != out_table )
     {
           (*out_table) = ip_table;
           debug_print(("CcuTestReadIpMacTable_control:   table ptr=%08x \n", ip_table));
     }
     //SqlLocalBufferFree((void *)ip_table);

     return 0;
}
///< test db file, end

int CcuDisplayRecvSyncNetPackage(common_big_package_t  *precv_common_net_cmd)
{
        gd_sync_cmd_t *pgd_sync_cmd = NULL;
		
        if( NULL==precv_common_net_cmd )
        {
               debug_print(("CcuDisplayRecvSyncNetPackage: null ptr\n"));
	       return -1;
        }
		
    pgd_sync_cmd  = (gd_sync_cmd_t *)&(precv_common_net_cmd->common_big_data_u.pa_common_cmd_info.event_info_common.gd_sync_cmd_info);
	debug_print(("dst: %s-%d\n", precv_common_net_cmd->dst_dev_name, precv_common_net_cmd->dst_dev_number));
	debug_print(("src: %s-%d\n", precv_common_net_cmd->src_dev_name, precv_common_net_cmd->src_dev_number));
	debug_print(("pkg_type = %d\n", precv_common_net_cmd->pkg_type));
	debug_print(("evt_type = %d\n", precv_common_net_cmd->common_big_data_u.pa_common_cmd_info.event_type_common));
	debug_print(("sequence = %d\n", precv_common_net_cmd->common_big_data_u.pa_common_cmd_info.sequence));	
	debug_print(("context_len = %d\n", pgd_sync_cmd->gd_sync_context_len));	
	debug_print(("evt_mask = %d\n", pgd_sync_cmd->gd_sync_mask));
	debug_print(("req=%d, res=%d\n\n", pgd_sync_cmd->gd_sync_req, pgd_sync_cmd->gd_sync_res));	

	return 0;
}

int CcuDisplaySendSyncNetPackage(void)
{
      int sync_package_store = 0;
      common_big_package_t  tmp_comm_pacakge = {"\0", 0};	  

      sync_package_store = BlockBufferOpen("ccu-net-sync-store");

      for(;;)
      {
            if( BlockBufferRead(sync_package_store, (void *)&tmp_comm_pacakge, sizeof(tmp_comm_pacakge)) > 0 )
            {
                   CcuDisplayRecvSyncNetPackage(&tmp_comm_pacakge);
	    }
	    else
	    {
                  break;
	    }
      }

       return 0;
}

int CcuGdDetectServerCcuNetState(int server_flag_from_iptable)
{
       ///< server_flag_from_iptable=2:  this is backup server of CCU
        int ret = -1;
		
/*	if( 0==GetServerCcuNetworkState() && 2==server_flag_from_iptable )
	{
	      ret = 1; ///< CCU2 ---> server CCU
	}
	else if( 1==GetServerCcuNetworkState() && 2==server_flag_from_iptable )
	{
	      ret = 0; ///< keep CCU2 non-server state
	}
	else
	{
                if( 2==ReadNumWilson() )
                {
                       debug_print(("CcuGdDetectServerCcuNetState:  unlikely for CCU2:  %d, %d \n", server_flag_from_iptable, GetServerCcuNetworkState() ));
                }                
	}*/
     
	return ret;        
}
int CcuStrncpy(char *dest, int dest_size, char *src, int src_size)
{
          int min_len = 0, left_len = 0, right_len = 0;

          if( NULL==dest ||NULL== src)
          {
                return -1;
          }
          
          left_len = dest_size;
          right_len = src_size;
          min_len = (left_len>=right_len) ? (right_len) : (left_len);
          strncpy(dest, src, min_len);
          if( min_len>=dest_size )
          {
               min_len = min_len-1;
          }
          dest[min_len] = 0x00;

	 return 0;
}
void CcuGdHasReadIpTable(void)
{
       ccu_has_read_ip_table = 1;
}
int CcuGdIpTableIfHasRead(void)
{
       return (int)(ccu_has_read_ip_table);
}
int CcuGdReadServerIp(char *out_ip, int out_len)
{
       int ret = -1;
       dev_config_table_item_t *tmp_ccu_config_info = NULL;
       int dest_len = 0;
       int src_len = 0;
	   
       tmp_ccu_config_info = local_ccu_config_info;
       dest_len = out_len;

       if( NULL==tmp_ccu_config_info ||NULL==out_ip )
       {
               debug_print(("CcuGdReadServerIp: null ptr\n"));       
               return -1;
       }

        if(1) ///< (1==tmp_ccu_config_info->is_db_server)
        {
              src_len = strlen(tmp_ccu_config_info->dev_ip);
              CcuStrncpy(out_ip, dest_len, tmp_ccu_config_info->dev_ip, src_len);
	      ret = 1;
        }
		
       return ret;
}
void CcuGdSaveConfigInfoPtr(void *gd_param)
{
    local_ccu_config_info = (dev_config_table_item_t *)gd_param;
    return ;
}
int CcuGdSendLocalDbVersion(gd_state_param_t  *ccu_gd_state_param)
{
     int ret = -1;
     common_big_package_t  local_net_cmd = {"\0",0};
     pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(local_net_cmd.common_big_data_u.pa_common_cmd_info);

     if( NULL==ccu_gd_state_param )
     {
           debug_print(("CcuGdSendLocalDbVersion: null ptr\n"));
           return -1;
     }

     memcpy((void *)&local_net_cmd, (void *)(ccu_gd_state_param->precv_common_net_cmd), sizeof(common_big_package_t));
     if( pcomm_cmd_info->event_type_common==EVENT_TYPE_SYS_UPDATE_SYNC && 
	  pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
          pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
     {
            char *tmptr = (char *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
            strcpy(local_net_cmd.dst_dev_name, ccu_gd_state_param->precv_common_net_cmd->src_dev_name);
            local_net_cmd.dst_dev_number = ccu_gd_state_param->precv_common_net_cmd->src_dev_number;
			#if 1 ///< local device
				strcpy(local_net_cmd.src_dev_name, ccu_gd_state_param->plocal_dev_config_info->dev_name);
				local_net_cmd.src_dev_number = ccu_gd_state_param->plocal_dev_config_info->dev_no;
			#else ///< xxx_255 is bad
				strcpy(local_net_cmd.src_dev_name, ccu_gd_state_param->precv_common_net_cmd->dst_dev_name);
				local_net_cmd.src_dev_number = ccu_gd_state_param->precv_common_net_cmd->dst_dev_number;
			#endif
	    pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res = 1;
		
            strcpy(tmptr, ccu_gd_state_param->plocal_dev_config_info->config_db_version_string);
            tmptr += sizeof(ccu_gd_state_param->plocal_dev_config_info->config_db_version_string);
            strcpy(tmptr, ccu_gd_state_param->plocal_dev_config_info->ann_db_version_string);
            tmptr += sizeof(ccu_gd_state_param->plocal_dev_config_info->ann_db_version_string);
	    if(1) ///< ( ccu_gd_state_param->plocal_dev_config_info->is_db_server )
	    {
	           int *tmp_int = NULL;
                   strcpy(tmptr, "SERVER");
                   tmptr += 7;
                   strcpy(tmptr, ccu_gd_state_param->plocal_dev_config_info->dev_name); ///< new server name
                   tmptr += sizeof(ccu_gd_state_param->plocal_dev_config_info->dev_name);
		   tmp_int = (int *)tmptr;
		   (*tmp_int) = ccu_gd_state_param->plocal_dev_config_info->dev_no; ///< new server no
           debug_print(("CcuGdSendLocalDbVersion:  new_db_dev_no=%d \n", (*tmp_int)));
		   tmptr += 4;
        		   tmp_int = (int *)tmptr;
		   (*tmp_int) = (int)(ccu_gd_state_param->plocal_dev_config_info->server_flag_from_iptable); ///< add, 2014-06-23
           debug_print(("CcuGdSendLocalDbVersion:  flag=%d \n", (*tmp_int)));
		   tmptr += 4;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context_len = 
					sizeof(ccu_gd_state_param->plocal_dev_config_info->config_db_version_string) +
					sizeof(ccu_gd_state_param->plocal_dev_config_info->ann_db_version_string) + 
					7 + 
					 sizeof(ccu_gd_state_param->plocal_dev_config_info->dev_name) +
					 4 +
					 4;
          debug_print(("CcuGdSendLocalDbVersion:  receiv package(%d),then response it \n",
                		  pcomm_cmd_info->sequence));
	    }	    		
            else
            {
                   strcpy(tmptr, "000000");
                   tmptr += 7;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context_len = 
					sizeof(ccu_gd_state_param->plocal_dev_config_info->config_db_version_string) +
					sizeof(ccu_gd_state_param->plocal_dev_config_info->ann_db_version_string) + 
					7;
	    }
			
	    ret = BlockBufferWrite(ccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd));
        debug_print(("CcuGdSendLocalDbVersion:  pack_seq=%d, tot_len=%d \n",
			local_net_cmd.common_big_data_u.pa_common_cmd_info.sequence,
			sizeof(local_net_cmd) ));
     }     

     return ret;
}

int CcuGdGetLocalDevIO(int *dev_type, int *dev_no)
{
      int  local_dev_type = -1;
      int local_dev_no = -1;
	  
     local_dev_type = ReadType();
     local_dev_no = ReadNumWilson();

      if( NULL != dev_type)
      {
            *dev_type = local_dev_type;
      }

      if( NULL != dev_no)
      {
            *dev_no = local_dev_no;
      }    

      return 0;      
}

int CcuSqlite3GetDevConfigInfo(dev_config_table_item_t  *pdev_config_info)
{
      int row = 0;
      int column = 0;
      char *ip_table = NULL;
      ip_mac_config_t *ip_table_item = NULL;
	  
      if( NULL==pdev_config_info)
      {
             debug_print(("CcuSqlite3GetDevConfigInfo:  null ptr\n"));
             return -1;
      }

#if 0
      if( 1 != SqliteGetOpenFlag() )
      {
             debug_print(("CcuSqlite3GetDevConfigInfo:  db file not opened or no db \n"));
             return -1;
      }
#endif	  

     #ifdef CONFIG_CCU_READ_DB
     SqlOfTableTIpMac((int *)&row, (int *)&column, (void **)&ip_table);
     ip_table_item = (ip_mac_config_t *)ip_table;
     #ifdef CONFIG_DISPLAY_TABLE_CONTEXT	 
     CcuGdDisplayTableTIpMac(row, ip_table);
     #endif	 
     if( NULL != ip_table_item )
     {
           int i = 0;
	   int len = 0;
	   unsigned char ccu_count = 0;
	   
           for(i=0; i<row; i++)
           {
                if( 2==ccu_count )
                {
                      debug_print(("CcuSqlite3GetDevConfigInfo: There are 2 CCU \n"));                
                      break;
		}
                 //if ( ip_table_item[i].device_type==pdev_config_info->dev_type && 
		 //     ip_table_item[i].device_no==pdev_config_info->dev_no ) //fill local dev info
                 //if ( 0==strcmp(ip_table_item[i].device_name, pdev_config_info->dev_name) && 
                 if ( (ip_table_item[i].device_type_no==pdev_config_info->dev_type) && 				 	
		      ip_table_item[i].device_no==pdev_config_info->dev_no ) //fill local dev info
                 {
                        len = sizeof(pdev_config_info->dev_name);
                        strncpy(pdev_config_info->dev_name,  ip_table_item[i].device_name, len);
                        pdev_config_info->dev_name[len-1] = '\0';
						
                        len = sizeof(pdev_config_info->dev_ip);						
			strncpy(pdev_config_info->dev_ip, ip_table_item[i].ip_address, len);
                        pdev_config_info->dev_ip[len-1] = '\0';	
						
			pdev_config_info->is_db_server =  ip_table_item[i].server_whether;
			if( 1==ip_table_item[i].server_whether )
			{
                               debug_print(("we are server CCU=%s-%d, ip=%s\n", pdev_config_info->dev_name, 
                                            pdev_config_info->dev_no, pdev_config_info->dev_ip));
			}
			else
			{
                               debug_print(("we are non server CCU=%s-%d, ip=%s\n", pdev_config_info->dev_name, 
                                            pdev_config_info->dev_no, pdev_config_info->dev_ip));
			}
                        ccu_count++;
		 }

                 if( 0==strcmp(ip_table_item[i].device_name, pdev_config_info->dev_name) && 
		              ip_table_item[i].device_no != pdev_config_info->dev_no )
                 {
                        ///< put other CCU into <db_server_dev_name>, but we only see the first item
                        pdev_config_info->db_server_dev_no = ip_table_item[i].device_no;
						
                        len = sizeof(pdev_config_info->db_server_dev_name);
                        strncpy(pdev_config_info->db_server_dev_name,  ip_table_item[i].device_name, len);
                        pdev_config_info->dev_name[len-1] = '\0';
						
                        len = sizeof(pdev_config_info->db_server_ip);
			strncpy(pdev_config_info->db_server_ip, ip_table_item[i].ip_address, len);
                        pdev_config_info->db_server_ip[len-1] = '\0';
                        debug_print(("This is another CCU=%s-%d, ip=%s\n", pdev_config_info->db_server_dev_name, 
                                               pdev_config_info->db_server_dev_no, pdev_config_info->db_server_ip));
                        ccu_count++;						
		 }
	   }	              
     }
    else
    {
           debug_print(("CCU,get info from SQLITE, here-03\n"));
    }
    SqlLocalBufferFree((void *)ip_table);

   char *ann_db_version_table = NULL;
   db_version_t *ann_db_version_item = NULL;
   int dest_len = 0;
   int src_len = 0;
    row = 0;
    column = 0;
    debug_print(("CcuSqlite3GetDevConfigInfo:  go-8888888 \n"));		
    SqlOfTableTDatabaseVersion((int *)&row, (int *)&column, (void **)&ann_db_version_table);
    debug_print(("CcuSqlite3GetDevConfigInfo:  go-999999 \n"));
	
    ann_db_version_item = (db_version_t *)ann_db_version_table;
    if( NULL != ann_db_version_item )
    {
          //pdev_config_info->ann_db_version = (unsigned int)(ann_db_version_item[0].current_version);
          //debug_print(("CcuSqlite3GetDevConfigInfo, go-2:  ann_v=%d \n", pdev_config_info->ann_db_version));
          dest_len = sizeof(pdev_config_info->ann_db_version_string);
	  src_len = strlen(ann_db_version_item[0].current_version);
          CcuStrncpy(pdev_config_info->ann_db_version_string, dest_len, ann_db_version_item[0].current_version, src_len);
          debug_print(("CcuSqlite3GetDevConfigInfo, go-2:  ann_v=%s \n", pdev_config_info->ann_db_version_string));
    }
    SqlLocalBufferFree((void *)ann_db_version_table);

    //get local config db version from SQLITE
   char *config_db_version_table = NULL;
   db_version_t *config_db_version_item = NULL;
    row = 0;
    column = 0;
    SqlOfTConfigDBVersion((int *)&row, (int *)&column, (void **)&config_db_version_table);
    config_db_version_item = (db_version_t *)config_db_version_table;
    if( NULL != config_db_version_item )
    {
          //pdev_config_info->config_db_version = (unsigned int)(config_db_version_item[0].current_version);
          //debug_print(("CcuSqlite3GetDevConfigInfo, go-3:  config_v=%d \n", pdev_config_info->config_db_version));
          dest_len = sizeof(pdev_config_info->config_db_version_string);
	  src_len = strlen(config_db_version_item[0].current_version);
          CcuStrncpy(pdev_config_info->config_db_version_string, dest_len, config_db_version_item[0].current_version, src_len);
          debug_print(("CcuSqlite3GetDevConfigInfo, go-3:  config_v=%s \n", pdev_config_info->config_db_version_string));
    }
    SqlLocalBufferFree((void *)config_db_version_table);
    #else
    {
		struct global_info local_dev_info;
		struct global_info back_ccu_dev_info;
		struct global_info main_ccu_dev_info;
		
		local_dev_info = ReadGlobalArray(pdev_config_info->dev_type, NULL, pdev_config_info->dev_no, 1);
		strcpy(pdev_config_info->dev_ip, local_dev_info.dev_ip);
		strcpy(pdev_config_info->dev_name, local_dev_info.dev_name);
	       pdev_config_info->is_db_server = local_dev_info.is_server;
		debug_print(("Ccu Sync thread:  init, local-ip=%s \n", pdev_config_info->dev_ip));
		debug_print(("Ccu Sync thread:  init, local-dev-name=%s \n", pdev_config_info->dev_name));
		back_ccu_dev_info = ResolveUseServerNumber(2); ///< 读次CCU信息		
		main_ccu_dev_info = ResolveUseServerNumber(1); ///< 读主CCU信息
		diag_printf("##### zhw 1111111,%s---%s\n",main_ccu_dev_info.dev_ip,pdev_config_info->dev_ip);
		if( 0==strcmp(main_ccu_dev_info.dev_ip, pdev_config_info->dev_ip) ) ///< we are server-CCU, so other CCU is my server
		{ ///< 保存2个CCU信息
			strcpy(pdev_config_info->db_server_dev_name, back_ccu_dev_info.dev_name);
			strcpy(pdev_config_info->db_server_ip, back_ccu_dev_info.dev_ip);
			pdev_config_info->db_server_dev_no = back_ccu_dev_info.dev_number;
			debug_print(("Ccu Sync thread:  init, backup-CCU=%s_%d, ip=%s \n", pdev_config_info->db_server_dev_name,
			                     pdev_config_info->db_server_dev_no, pdev_config_info->db_server_ip));
		}
		diag_printf("##### zhw 222222\n");
		if( 0==strcmp(back_ccu_dev_info.dev_ip, pdev_config_info->dev_ip) || 0==local_dev_info.is_server ) ///< we are non-server-CCU, so other CCU is my server
		{
			strcpy(pdev_config_info->db_server_dev_name, main_ccu_dev_info.dev_name);
			strcpy(pdev_config_info->db_server_ip, main_ccu_dev_info.dev_ip);
			pdev_config_info->db_server_dev_no = main_ccu_dev_info.dev_number;
			debug_print(("Ccu Sync thread:  init, server-CCU=%s_%d, ip=%s \n", pdev_config_info->db_server_dev_name,
					pdev_config_info->db_server_dev_no, pdev_config_info->db_server_ip));
		}
diag_printf("##### zhw 3333333\n");
              char *ann_db_version = ReadAnnVersion(); 
              char *config_db_version = ReadCfgVersion();
              int src_len = 0;
            		  int 	dest_len = 0;
            		  src_len =dest_len = sizeof(pdev_config_info->config_db_version_string);
		CcuStrncpy(pdev_config_info->ann_db_version_string, dest_len, ann_db_version, src_len);
		debug_print(("CcuSqlite3GetDevConfigInfo:  ann_v=%s from cm \n", pdev_config_info->ann_db_version_string));

		CcuStrncpy(pdev_config_info->config_db_version_string, dest_len, config_db_version, src_len);
		debug_print(("CcuSqlite3GetDevConfigInfo:  config_v=%s from cm \n", pdev_config_info->config_db_version_string));
    }
    #endif
	
    debug_print(("CcuSqlite3GetDevConfigInfo:  exit \n"));		

    return 0;
}

int CcuGdGetDevConfigFromDB(dev_config_table_item_t  *pdev_config_info)
{
     int ret = -1;

      if( NULL == pdev_config_info )
      {
           debug_print(("CcuGdGetDevConfigFromDB: null ptr\n"));
           return -1;
      }
	  
      //read dev configure table,then we will get ourself and remote gb server infomation 
      CcuSqlite3GetDevConfigInfo(pdev_config_info);
diag_printf("@@@@@zhw add 00000\n");
      return  ret;
}

global_device_info_state_t  *CcuGdGetLocalGdTable(void)
{
      global_device_info_state_t  *tmp_ptr = NULL;
      tmp_ptr = GetLocalGlobalDevInfoTable();
      return tmp_ptr;
}

int CcuGdUpdateLocalDevInfo(dev_info_table_update_t *dev_info_update)
{
       int ret = -1;
       unsigned int dev_no = 0;
       global_device_info_state_t  *plocal_gd_dev_info = NULL;

       plocal_gd_dev_info = CcuGdGetLocalGdTable();

      if( NULL != dev_info_update )
      {
             dev_no = dev_info_update->dev_no;
             if( dev_no >= 0 && dev_no <= CONFIG_CCU_NUM )
             {
                    if( NULL != plocal_gd_dev_info )
                    {      
                            //lock
                            if( dev_info_update->gd_mask & GD_SYNC_FIELD_DEV_NAME )
                            {
                                   debug_print(("CcuGdUpdateLocalDevInfo:  ccu,devname: %s --> %s\n", plocal_gd_dev_info->device_ccu[dev_no].devices_name,
                                                         dev_info_update->dev_name));
                                   strcpy(plocal_gd_dev_info->device_ccu[dev_no].devices_name, dev_info_update->dev_name);
                             }
							
                            if( dev_info_update->gd_mask & GD_SYNC_FIELD_DEV_NO )
                            {
                                   debug_print(("CcuGdUpdateLocalDevInfo:  ccu,devno: %d --> %d\n", plocal_gd_dev_info->device_ccu[dev_no].devices_no,
                                                         dev_info_update->dev_no));				      
                                   plocal_gd_dev_info->device_ccu[dev_no].devices_no = dev_info_update->dev_no;
                            }
							
                            if( dev_info_update->gd_mask & GD_SYNC_FIELD_DEV_CURRENT_STATE)
                            {
                                  debug_print(("CcuGdUpdateLocalDevInfo:  ccu_s: %d --> %d\n", plocal_gd_dev_info->device_ccu[dev_no].current_state,
                                                        dev_info_update->dev_stat));				      
                                  plocal_gd_dev_info->device_ccu[dev_no].current_state = dev_info_update->dev_stat;
                            }
							
                            if( dev_info_update->gd_mask & GD_SYNC_FIELD_CONFIG_DB_VERSION )
                            {
                                 debug_print(("CcuGdUpdateLocalDevInfo:  ccu,config_db_v: %d --> %d\n", plocal_gd_dev_info->device_ccu[dev_no].config_db_version,
                                                       dev_info_update->config_db_version));
                                 plocal_gd_dev_info->device_ccu[dev_no].config_db_version = dev_info_update->config_db_version;
                            }

                            if( dev_info_update->gd_mask & GD_SYNC_FIELD_ANN_DB_VERSION )
                            {
                                 debug_print(("CcuGdUpdateLocalDevInfo:  ccu,ann_db_v: %d --> %d\n", plocal_gd_dev_info->device_ccu[dev_no].ann_db_version,
                                                       dev_info_update->ann_db_version));
                                 plocal_gd_dev_info->device_ccu[dev_no].ann_db_version = dev_info_update->ann_db_version;
                            }							

                            if( dev_info_update->gd_mask & GD_SYNC_FIELD_SERVER_FLAG )
                            {
                                 debug_print(("CcuGdUpdateLocalDevInfo:  ccu,is server: %d --> %d\n", plocal_gd_dev_info->device_ccu[dev_no].is_db_server,
                                                       dev_info_update->is_db_server));
                                 plocal_gd_dev_info->device_ccu[dev_no].is_db_server = dev_info_update->is_db_server;
                            }
			     //unlock

                            ret = 0;								
                    }					
             }            
       }     	

	return ret;
}
int CcuGdGetLocalDevName(dev_config_table_item_t  *pdev_config_info)
{
       int ret = -1;
       int i = 0;
	   
       if( NULL == pdev_config_info )
       {
              debug_print(("CcuGdGetLocalDevName: enter, null ptr \n"));
	      return -1;
       }

       for(i=0; i<4; i++)
       {
             if(GbDevName[i].device_type == pdev_config_info->dev_type)
             {
                    strcpy(pdev_config_info->dev_name, GbDevName[i].device_name);
		    debug_print(("CcuGdGetLocalDevName: dev_name=%s \n", pdev_config_info->dev_name));
		    ret = 0;
		    break;
	     }
       }
	   
       return ret;
}
int CcuGdLocalDevConfigInit(dev_config_table_item_t  *pdev_config_info)
{
       dev_info_table_update_t  dev_info_update = {0};

       if( 0 != CcuGdGetLocalDevName(pdev_config_info) )
       {
              strcpy(pdev_config_info->dev_name, "CCU");
       }
	   
       CcuGdGetDevConfigFromDB(pdev_config_info);

       if( NULL != pdev_config_info )
       {
              dev_info_update.dev_type = pdev_config_info->dev_type;
              dev_info_update.dev_no =  pdev_config_info->dev_no;
              dev_info_update.config_db_version =  pdev_config_info->config_db_version;
              dev_info_update.ann_db_version =  pdev_config_info->ann_db_version;
              strcpy(dev_info_update.dev_name,  pdev_config_info->dev_name);
              dev_info_update.is_db_server = pdev_config_info->is_db_server;
              dev_info_update.gd_mask = ( GD_SYNC_FIELD_DEV_NAME |GD_SYNC_FIELD_DEV_TYPE | GD_SYNC_FIELD_DEV_NO |
			  	                             GD_SYNC_FIELD_ANN_DB_VERSION |GD_SYNC_FIELD_SERVER_FLAG );
	      dev_info_update.dev_stat = 0;
              //CcuGdUpdateLocalDevInfo((dev_info_table_update_t *)&dev_info_update);
       }

       return 0;
}

int CcuGdSendDbServerChanged( gd_state_param_t  *ccu_gd_state_param)
{
     int ret = -1;
     common_big_package_t  local_net_cmd = {"\0", 0};
     pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(local_net_cmd.common_big_data_u.pa_common_cmd_info);

     if( NULL != ccu_gd_state_param )
     {
          db_server_update_t *tmptr = NULL;
		  
          strcpy(local_net_cmd.src_dev_name, "CCU");
          local_net_cmd.src_dev_number = ccu_gd_state_param->plocal_dev_config_info->dev_no;
          local_net_cmd.pkg_type =  COMMON_PACKAGE_TYPE_SYNC_GD;
          pcomm_cmd_info->event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
          pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DB_SERVER_CHANGED;
          tmptr = (db_server_update_t *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
          tmptr->server_dev_no = ccu_gd_state_param->plocal_dev_config_info->dev_no;
          strcpy(tmptr->server_dev_name, ccu_gd_state_param->plocal_dev_config_info->dev_name);
          strcpy(tmptr->server_dev_ip, ccu_gd_state_param->plocal_dev_config_info->dev_ip);

          #if 0 ///< don't
          strcpy(local_net_cmd.dst_dev_name, "CCU");
          local_net_cmd.dst_dev_number = ccu_gd_state_param->plocal_dev_config_info->db_server_dev_no; ///< this is origin server ccu number
	  if ( BlockBufferWrite(ccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd)) < 0 )
	  {
                  debug_print(("CcuGdSendDbServerChanged,to ccu:  write buff error\n"));
	  }
          #endif
		  
          strcpy(local_net_cmd.dst_dev_name, "BCU");
          local_net_cmd.dst_dev_number = MUL_SYNC_DST_NO;		  
	  if ( BlockBufferWrite(ccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd)) < 0 )
	  {
                  debug_print(("CcuGdSendDbServerChanged, to bcu:  write buff error\n"));
	  }
          strcpy(local_net_cmd.dst_dev_name, "EAMP");
          local_net_cmd.dst_dev_number = MUL_SYNC_DST_NO;		  
	  if ( BlockBufferWrite(ccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd)) < 0 )
	  {
                  debug_print(("CcuGdSendDbServerChanged,to eamp:  write buff error\n"));
	  }
          strcpy(local_net_cmd.dst_dev_name, "PCU");
          local_net_cmd.dst_dev_number = MUL_SYNC_DST_NO;		  
	  if ( BlockBufferWrite(ccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd)) < 0 )
	  {
                  debug_print(("CcuGdSendDbServerChanged,to pcu:  write buff error\n"));
	  }		  
     }
	 
     return ret;
}
int CcuGdSendSyncPackageFixed(unsigned char flag, gd_state_param_t  *pccu_gd_state_param)
{
     int ret = -1;
     common_big_package_t  local_net_cmd = {"\0", 0};
     pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(local_net_cmd.common_big_data_u.pa_common_cmd_info);
     unsigned int *tmptr = (unsigned int *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);

     //if( NULL != pccu_gd_state_param && !pccu_gd_state_param->plocal_dev_config_info->is_db_server )
     {
            strcpy(local_net_cmd.dst_dev_name, "CCU");
            local_net_cmd.dst_dev_number =  1;
            strcpy(local_net_cmd.src_dev_name, "CCU");
            local_net_cmd.src_dev_number =  1;
            local_net_cmd.pkg_type =  COMMON_PACKAGE_TYPE_SYNC_GD;
            pcomm_cmd_info->event_type_common =  EVENT_TYPE_SYS_UPDATE_SYNC;
            if( 1==flag ) //request package
            {
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req = 1;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res = 0;
	    }
            else if( 2==flag ) //response package
            {
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req = 1;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res = 1;
	    }			
            strcpy((char *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_source_ip), pccu_gd_state_param->plocal_dev_config_info->dev_ip);
            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DB_VERSION_SYNC_CCU;
            *tmptr = pccu_gd_state_param->plocal_dev_config_info->config_db_version;
            tmptr++; //next 4 bytes for ann-db-version
            *tmptr = pccu_gd_state_param->plocal_dev_config_info->ann_db_version;
            if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd)) < 0 )
            {
                 debug_print(("CcuGdSendSyncPackageFixed: write cmd buffer error\n"));
            }
       }

       debug_print(("CcuGdSendSyncPackageFixed: exit\n"));
       return ret;
}
int CcuGdSendSyncPackageToCcu(unsigned char flag, gd_state_param_t  *pccu_gd_state_param)
{
     int ret = -1;
     common_big_package_t  local_net_cmd = {"\0", 0};
     pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(local_net_cmd.common_big_data_u.pa_common_cmd_info);
     char *tmptr = (unsigned int *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
     char *sync_context = tmptr;	 

     if( NULL != pccu_gd_state_param )
     {
            strcpy(local_net_cmd.dst_dev_name, pccu_gd_state_param->plocal_dev_config_info->db_server_dev_name); ///< For serverCCU: db_server_dev_name=other ccu
            local_net_cmd.dst_dev_number =  pccu_gd_state_param->plocal_dev_config_info->db_server_dev_no;           
            strcpy(local_net_cmd.src_dev_name, pccu_gd_state_param->plocal_dev_config_info->dev_name);
            local_net_cmd.src_dev_number =  pccu_gd_state_param->plocal_dev_config_info->dev_no;
            local_net_cmd.pkg_type = COMMON_PACKAGE_TYPE_SYNC_GD;			
            pcomm_cmd_info->event_type_common =  EVENT_TYPE_SYS_UPDATE_SYNC;
            if( 1==flag ) //request package
            {
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req = 1;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res = 0;
	    }
            else if( 2==flag ) //response package
            {
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req = 1;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res = 1;
	    }			
            strcpy((char *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_source_ip), pccu_gd_state_param->plocal_dev_config_info->dev_ip);
            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DB_VERSION_SYNC_CCU;
		
            // *tmptr = pccu_gd_state_param->plocal_dev_config_info->config_db_version;
            // tmptr++; //next 4 bytes for ann-db-version
            // *tmptr = pccu_gd_state_param->plocal_dev_config_info->ann_db_version;
            strcpy(tmptr, pccu_gd_state_param->plocal_dev_config_info->config_db_version_string);
            tmptr += sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string);
            strcpy(tmptr, pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string);
            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context_len = 
				sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) + 
				sizeof(pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string);
            debug_print(("CcuGdSendSyncPackageToCcu:  config_v=%s, ann_v=%s\n", sync_context, sync_context+sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) ));
            debug_print(("CcuGdSendSyncPackageToCcu:  config_v=%s, ann_v=%s\n", sync_context, sync_context+sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) ));
			
            if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &local_net_cmd, sizeof(local_net_cmd)) < 0 )
            {
                 debug_print(("CcuGdSendSyncPackageToCcu: write cmd buffer error\n"));
            }
       }

       return ret;
}

void CcuAlarmSyncWaiting(cyg_handle_t alarm, cyg_addrword_t data)	
{
        AlarmTimeOutProcess( (void *)data );
        //other application process
       return ;
}

void CcuTftpClientAlarmWaiting(cyg_handle_t alarm, cyg_addrword_t data)	
{
        AlarmTimeOutProcess( (void *)data );
        //other application process
       return ;
}

int CcuCheckGlobalDeviceInfoChanged(gd_dev_state_count_t *last_dev_state_count_array, int net_cmd_send_buffer_id)
{
	int i = 0;
        global_device_info_state_t   local_global_device_info;
        common_big_package_t   tmp_net_send_cmd;
        pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(tmp_net_send_cmd.common_big_data_u.pa_common_cmd_info);
        int dev_sync_buffer_id = net_cmd_send_buffer_id;

        if( NULL == last_dev_state_count_array )
	{
		debug_print(("CheckGlobalDeviceInfoChanged:  null ptr\n"));
		return -1;
	}
	if (TryScreenCopyGlobalDeviceInfo(&local_global_device_info) < 0) // ??
	{
		debug_print(("CheckGlobalDeviceInfoChanged:  locked, so can't copy\n"));
		return -1;
	}

        for(i=0; i<CONFIG_BCU_NUM; i++)
        {
              if ( last_dev_state_count_array->bcu_state_count[i] != local_global_device_info.device_bcu[i].state_count ) //changed
              {
                     int *tmptr = (int *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
                     pcomm_cmd_info->event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
		     pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DEV_INFO_SYNC_BEP;
                     *tmptr = i;
                     memcpy((void *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context), 
				    (void *)(&local_global_device_info.device_bcu[i]), sizeof(device_bcu_state_t));
		     BlockBufferWrite(dev_sync_buffer_id, &tmp_net_send_cmd, sizeof(tmp_net_send_cmd));
                     last_dev_state_count_array->bcu_state_count[i] = local_global_device_info.device_bcu[i].state_count;
	      }
	}
		
        for(i=0; i<CONFIG_CCU_NUM; i++)
        {
              if ( last_dev_state_count_array->ccu_state_count[i] != local_global_device_info.device_ccu[i].state_count ) //changed
              {
                     int *tmptr = (int *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
                     pcomm_cmd_info->event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
		     pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DEV_INFO_SYNC_CCU;
                     *tmptr = i;
                     memcpy((void *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context), 
				    (void *)(&local_global_device_info.device_bcu[i]), sizeof(device_bcu_state_t));
		       BlockBufferWrite(dev_sync_buffer_id, &tmp_net_send_cmd, sizeof(tmp_net_send_cmd));
                     last_dev_state_count_array->ccu_state_count[i] = local_global_device_info.device_ccu[i].state_count;			 
	      }
	}
		
        for(i=0; i<CONFIG_EAMP_NUM; i++)
        {
              if ( last_dev_state_count_array->eamp_state_count[i] != local_global_device_info.device_eamp[i].state_count ) //changed
              {
                     int *tmptr = (int *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);              
                     pcomm_cmd_info->event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
		     pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DEV_INFO_SYNC_BEP;
                     *tmptr = i;
                     memcpy((void *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context), 
				    (void *)(&local_global_device_info.device_eamp[i]), sizeof(device_bcu_state_t));
		       BlockBufferWrite(dev_sync_buffer_id, &tmp_net_send_cmd, sizeof(tmp_net_send_cmd));
                     last_dev_state_count_array->eamp_state_count[i] = local_global_device_info.device_eamp[i].state_count;			 
	      }
	}
		
        for(i=0; i<CONFIG_PECU_NUM; i++)
        {
              if ( last_dev_state_count_array->pcu_state_count[i] != local_global_device_info.device_pcu[i].state_count ) //changed
              {
                     int *tmptr = (int *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
                     pcomm_cmd_info->event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
		     pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DEV_INFO_SYNC_BEP;
                     *tmptr = i;
                     memcpy((void *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context+4), 
				    (void *)(&local_global_device_info.device_pcu[i]), sizeof(device_bcu_state_t));
		     BlockBufferWrite(dev_sync_buffer_id, &tmp_net_send_cmd, sizeof(tmp_net_send_cmd));
                     last_dev_state_count_array->pcu_state_count[i] = local_global_device_info.device_pcu[i].state_count;
	      }
	}

         return 0;        
}

int CcuNetFaultStatus(int current_dev_type, int current_dev_no)
{
       int ret = -1;
       device_ccu_state_t   tmp_ccu_dev_state = {"\0", 0};

       TryCopyPartOfGlobalDevTable(current_dev_type, current_dev_no, (void *)&tmp_ccu_dev_state);

       if( tmp_ccu_dev_state.is_db_server ==1 )
       {
             if ( tmp_ccu_dev_state.current_state==EVENT_TYPE_DEV_DEAD )
             {
                   ret = EVENT_TYPE_DEV_DEAD;
             }
       }
	   
       return ret;
}

int CcuGdStartTftpUpdate(char *server_ip, int db_type )
{
      int ret = -1;

      ret = DBfileTftpclientApi(server_ip, db_type);

      return ret;
}
int CcuAnnTableOperation(ann_station_report_t *new_ann_array_ptr, int operate_flag)
{
    int ret = -1;

    switch( operate_flag )
    {
           case 0: //get current pointer
               ret = (int)gb_ann_array_ptr;
	   break;

           case 1: //save new pointer
               if( NULL != new_ann_array_ptr )
               {
                     gb_ann_array_ptr = new_ann_array_ptr;
		     ret = 0;
               }           
	   break;

           default: 
	   break;	   
    }
	
     return ret;
}
int CcuPriorListInit(void) ///< 
{
       int ret = -1;       
       int row = 0, column = 0, i = 0;
       int malloc_bytes = 0, real_len = 0;
       int left_len = 0, right_len = 0;
       void *p_table = NULL;
       priority_id_t *prior_table_item_ptr = NULL;
       event_priority_table_t  *pevt_prior_table = NULL;

       SqlOfTPriority(&row, &column, (void *)&p_table);
       prior_table_item_ptr = (priority_id_t *)p_table;
       debug_print(("CcuPriorListInit:  %08x, %d, %d \n", prior_table_item_ptr, row, column));
	#ifdef CONFIG_DISPLAY_TABLE_CONTEXT	   
       CcuGdDisplayTableTPriority(row, p_table); ///< row is real 
       #endif       
       if( NULL==prior_table_item_ptr || row<=0 )
       {
             debug_print(("CcuPriorListInit:  prior table null\n"));
	     return -1;
       }

       malloc_bytes = (row) * (sizeof(event_priority_table_t));
       pevt_prior_table = (event_priority_table_t  *)malloc( malloc_bytes );
       if( NULL==pevt_prior_table )
       {
             debug_print(("CcuPriorListInit:  malloc evt_table,fail\n"));
	     return -1;             
       }
       left_len = sizeof(pevt_prior_table->event_name);
       right_len = sizeof(prior_table_item_ptr[1].event_type);
       real_len = (left_len<=right_len) ? (left_len) : (right_len);
       for(i=0; i<row; i++)
       {              
              strncpy(pevt_prior_table[i].event_name, prior_table_item_ptr[i].event_type, real_len);
	      pevt_prior_table[i].event_name[real_len-1] = 0;
              pevt_prior_table[i].event_priority = (unsigned int)(prior_table_item_ptr[i].priority_id);
              debug_print(("CcuPriorListInit:  %s-%d\n", prior_table_item_ptr[i].event_type, prior_table_item_ptr[i].priority_id));
       }
       ret = CcuAnnStateListUpdate(pevt_prior_table, (row) );   
       free(pevt_prior_table);

       return ret;
}
int CcuAnnTableInit(void)
{
       int ret = -1;
       int row = 0;
       int column = 0;
       void *p_table =NULL;
       ann_station_report_t *new_ann_array_ptr = NULL;
       int default_line_no =0;
       int default_section_no = 0;

	SqlOfTableTline(&row, &column, &p_table);
	#ifdef CONFIG_DISPLAY_TABLE_CONTEXT	
        CcuGdDisplayTableTline(row, p_table);
	#endif
        default_line_no = JudgeDefaultLine(p_table, row);
	if ( default_line_no < 0 )
	{
		debug_print(("CcuAnnTableInit: error on JudgeDefaultSecion\n"));
		return -1;
	}
        debug_print(("CcuAnnTableInit:  Default line=%d \n", default_line_no));	

	LineSectionOfEntry(default_line_no, &row, &column, &p_table);
	#ifdef CONFIG_DISPLAY_TABLE_CONTEXT	
        CcuGdDisplayTableTLineSection(row, p_table); ///< debug
        #endif
        default_section_no = JudgeDefaultSecion(p_table, row);
	if (default_section_no < 0)
	{
		debug_print(("CcuAnnTableInit:  error on JudgeDefaultSecion\n"));
		return -1;
	}
        debug_print(("CcuAnnTableInit:  Default section=%d \n", default_section_no));
	
       ret = AnnReportOfEntry(default_line_no, default_section_no, &row, &column, &p_table);
       new_ann_array_ptr = (ann_station_report_t *)p_table;
       CcuAnnTableOperation(new_ann_array_ptr, 1);
       debug_print(("CcuAnnTableInit: Ann Table created in the RAM\n"));
       ///< Note: don't release p_table
	   
       return ret;
}
int CcuConfigVersionCompareProcess(char *recv_config_version, gd_state_param_t  *pccu_gd_state_param)
{
        if( NULL==recv_config_version || NULL==pccu_gd_state_param )
        {
              return -1;
	}
		
	if( strcmp(recv_config_version, pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) > 0 )
	{
		debug_print(("CcuConfigVersionCompareProcess:  (ip=%s) \n", pccu_gd_state_param->plocal_dev_config_info->db_server_ip));	                                                     
		for(;;)
		{
			if ( CcuGdStartTftpUpdate(pccu_gd_state_param->plocal_dev_config_info->db_server_ip, DB_Config_type) == 0 )
			{
				strncpy(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string, recv_config_version, CONFIG_CCU_VERSION_STRING_LEN);
				pccu_gd_state_param->plocal_dev_config_info->config_db_version_string[CONFIG_CCU_VERSION_STRING_LEN-1] = 0x00;	                                                                 
				break;
			}
			debug_print(("CcuConfigVersionCompareProcess:  get new db from (%s), fail, try again\n"));
		}
	}

	return 0;
}
int CcuAnnVersionCompareProcess(char *recv_ann_version, gd_state_param_t  *pccu_gd_state_param)
{
        if( NULL==recv_ann_version || NULL==pccu_gd_state_param )
        {
              return -1;
	}

        if( strcmp(recv_ann_version, pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string) > 0 )
        {
		debug_print(("CcuAnnVersionCompareProcess:  (ip=%s) \n", pccu_gd_state_param->plocal_dev_config_info->db_server_ip));	         
	        for(;;)
	        {
		        if ( CcuGdStartTftpUpdate(pccu_gd_state_param->plocal_dev_config_info->db_server_ip, DB_ann_type)  == 0 )
		        {
			        strncpy(pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string, recv_ann_version, CONFIG_CCU_VERSION_STRING_LEN);
			        pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string[CONFIG_CCU_VERSION_STRING_LEN-1] = 0x00;	                                                                 
			        break;
		        }
		        debug_print(("CcuAnnVersionCompareProcess:  get new db from (%s), fail, try again\n"));
	        }
        }
		
	return 0;
}

//globle data sync thread entry
#ifdef CCU_STEP_SYNC_WITH_SM
void  CcuGdSyncIdleEnter(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       debug_print(("CcuGdSyncIdleEnter\n"));

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncIdleEnter: null ptr\n"));
	     return ;
       }
}
void   CcuGdSyncIdleProcess(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;
       common_big_package_t  *pnet_recv_common_cmd = NULL;
       pa_common_information_t  *pcomm_cmd_info = NULL;	  
       unsigned char state_changed = 0;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncIdleProcess: null ptr\n"));
	     return ;
       }

        pnet_recv_common_cmd = pccu_gd_state_param->precv_common_net_cmd;
        if( NULL == pnet_recv_common_cmd )
        {
               debug_print(("CcuGdSyncCcuDbProcess:  null ptr-02\n"));
	       return ;
	}
        pcomm_cmd_info = &(pnet_recv_common_cmd->common_big_data_u.pa_common_cmd_info);	   

       ///< 本阶段接收、解析命令
       if(BlockBufferRead(pccu_gd_state_param->recv_net_cmd_buffer_id, pccu_gd_state_param->precv_common_net_cmd,  
	   	  sizeof(common_big_package_t)) > 0)
       {
              debug_print(("CcuGdSyncIdleProcess: distplay recv net cmd\n"));
              CcuDisplayRecvSyncNetPackage(pccu_gd_state_param->precv_common_net_cmd); ///< 打印接收命令,用于调试
	      #if 0 ///< 不再使用
              if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
                   pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
              {
                     CcuGdSendLocalDbVersion(pccu_gd_state_param);
              }
	      #endif
       }
	
       if( 1==AlarmCheckTimeOut(pccu_gd_state_param->alarm_sync_id) )
       {
             AlarmDisable(pccu_gd_state_param->alarm_sync_id);
	     if( !(pccu_gd_state_param->plocal_dev_config_info->is_db_server) )
	     {
                    if( CcuNetFaultStatus(pccu_gd_state_param->plocal_dev_config_info->dev_type, pccu_gd_state_param->plocal_dev_config_info->dev_no) == EVENT_TYPE_DEV_DEAD )
                    { ///< 本段未用,保留
                         CcuGdSendDbServerChanged( pccu_gd_state_param );
                         pccu_gd_state_param->plocal_dev_config_info->is_db_server = 1;
			 #if 0 ///< don't
			 strcpy(pccu_gd_state_param->plocal_dev_config_info->db_server_dev_name, pccu_gd_state_param->plocal_dev_config_info->dev_name);
                         strcpy(pccu_gd_state_param->plocal_dev_config_info->db_server_ip, pccu_gd_state_param->plocal_dev_config_info->dev_ip);
                         pccu_gd_state_param->plocal_dev_config_info->db_server_dev_no = pccu_gd_state_param->plocal_dev_config_info->dev_no;
			 #endif
                    }
                    else
                    {
                           debug_print(("server CCU online, so we just wait for it's ann\n"));
		    }
                    state_changed = 1;
	    }
            else
            {
                  state_changed = 1;
	    }

      ///< 目前,Idle阶段没有做其他事情,仅仅延迟下,然后自动进入CCU1-CCU2数据库更新阶段
	    if(state_changed)
	    {
	          #if 1 ///< 进入CCU1-CCU2数据库更新阶段
                   CcuGdSyncStateSwitch((gd_state_process_t **)&pccu_gd_state_param->plocal_curr_gd_state, GD_SYNC_STATE_TRIGGER_PU_READY, (void *)pccu_gd_state_param);
                  #else ///< 调试,直接进入BCP,EAMP,PCU数据库更新阶段
                   CcuGdSyncStateSwitchForce((gd_state_process_t **)&pccu_gd_state_param->plocal_curr_gd_state, 
                                                                      (gd_state_process_t *)&ccu_gd_sync_bep_db, 
                                                                      (void *)pccu_gd_state_param); ///< debug
                 #endif
	    }
       }
	   
       return ;
}
void  CcuGdSyncIdleExit(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncIdleExit: null ptr\n"));
	     return ;
       }

       debug_print(("CcuGdSyncIdleExit: exit\n"));

       return ;
}

void  CcuGdSyncCcuDbEnter(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       debug_print(("CcuGdSyncCcuDbEnter: enter\n"));

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncCcuDbEnter: null ptr\n"));
	     return ;
       }
	   
       if( NULL ==  pccu_gd_state_param->plocal_dev_config_info )
       {
             debug_print(("CcuGdSyncCcuDbEnter: null ptr-2\n"));
	     return ;
       }

       if( !pccu_gd_state_param->plocal_dev_config_info->is_db_server )
       {
             debug_print(("CcuGdSyncCcuDbEnter, we are not server:  send request\n"));
             //CcuGdSendSyncPackageToCcu( 1, pccu_gd_state_param );
             AlarmEnableWithNewTrigger( pccu_gd_state_param->alarm_sync_id, 300, 300 );
       }
       else
       {
             debug_print(("CcuGdSyncCcuDbEnter, we are server:  send broadcast for db-SYNC\n"));
             AlarmEnableWithNewTrigger( pccu_gd_state_param->alarm_sync_id, 200, 200 );
       }
	   
       //gd_thread_enter_ccu_db = 1;
       pccu_gd_state_param->soft_count = 0; 
       CcuGdSendSyncPackageToCcu( 1, pccu_gd_state_param ); ///< 向对端CCU发一次请求包
	   
      return ;
}
void  CcuGdSyncCcuDbProcess(void *sync_info)
{
        gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;
	common_big_package_t  *pnet_recv_common_cmd = NULL;
        pa_common_information_t  *pcomm_cmd_info = NULL;	
        dev_config_table_item_t   *plocal_dev_config_info = NULL;
	unsigned char sync_finished = 0;
        char recv_ann_version[CONFIG_CCU_VERSION_STRING_LEN];
	char recv_config_version[CONFIG_CCU_VERSION_STRING_LEN];
	char *db_version_ptr = NULL;

        if( NULL == pccu_gd_state_param )
        {
               debug_print(("CcuGdSyncCcuDbProcess:  null ptr\n"));
	       return ;
	}
		
        pnet_recv_common_cmd = pccu_gd_state_param->precv_common_net_cmd;
        if( NULL == pnet_recv_common_cmd )
        {
               debug_print(("CcuGdSyncCcuDbProcess:  null ptr-02\n"));
	       return ;
	}
        pcomm_cmd_info = &(pnet_recv_common_cmd->common_big_data_u.pa_common_cmd_info);
        db_version_ptr = (char*)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);		
		
        plocal_dev_config_info = pccu_gd_state_param->plocal_dev_config_info;
        if( NULL == plocal_dev_config_info )
        {
               debug_print(("CcuGdSyncCcuDbProcess:  null ptr-03\n"));
	       return ;
	}

       ///< 本阶段接收、解析命令包
       if(BlockBufferRead(pccu_gd_state_param->recv_net_cmd_buffer_id, pccu_gd_state_param->precv_common_net_cmd,  
	   	  sizeof(common_big_package_t)) > 0)
       {
              //CcuDisplayRecvSyncNetPackage(pccu_gd_state_param->precv_common_net_cmd);
			   
               if( pcomm_cmd_info->event_type_common==EVENT_TYPE_SYS_UPDATE_SYNC ) ///< 只处理该类型
               {
                       if( strncmp(pnet_recv_common_cmd->dst_dev_name,"CCU", 3) == 0 )
                       {
	                      if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask == SUBEVENT_TYPE_DB_VERSION_SYNC_CCU )
	                       {
			               if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
				           pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
				       { ///< 如果是请求包,一律回应
			                      CcuGdSendLocalDbVersion(pccu_gd_state_param);
				       }
						   
	                              if( strncmp(pnet_recv_common_cmd->src_dev_name, "CCU", 3) == 0 )
	                              {
	                                       if ( 0==AlarmCheckTimeOut(pccu_gd_state_param->alarm_sync_id) ||
						     1==AlarmCheckTimeOut(pccu_gd_state_param->alarm_sync_id) )
	                                       {
						       AlarmDisable(pccu_gd_state_param->alarm_sync_id);
	                                       }
	                                             ///< 获取版本号
                                               strncpy(recv_config_version, db_version_ptr, CONFIG_CCU_VERSION_STRING_LEN);
                                               recv_config_version[CONFIG_CCU_VERSION_STRING_LEN-1] = 0x00;
                                               strncpy(recv_ann_version, db_version_ptr+CONFIG_CCU_VERSION_STRING_LEN, CONFIG_CCU_VERSION_STRING_LEN);
                                               recv_ann_version[CONFIG_CCU_VERSION_STRING_LEN-1] = 0x00;
	                                       debug_print(("CcuGdSyncCcuDbProcess:  recv, config=%s, ann=%s\n", recv_config_version, recv_ann_version ));
	                                       if ( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
	                                             pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
	                                       {
	                                             debug_print(("CcuGdSyncCcuDbProcess:  this is Req from %s_%d \n", pnet_recv_common_cmd->src_dev_name,
												 	pnet_recv_common_cmd->src_dev_number));
	                                       }
	                                       else if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
	                                                   pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==1 )
	                                       {
	                                             debug_print(("CcuGdSyncCcuDbProcess:  this is Res from %s_%d \n", pnet_recv_common_cmd->src_dev_name,
												 	pnet_recv_common_cmd->src_dev_number));
	                                       }     

                                              if( plocal_dev_config_info->is_db_server )
                                              {
                                                      //strcpy(pccu_gd_state_param->plocal_dev_config_info->db_server_ip,  "192.168.7.14");
                                              }

                                              ///< 判断是否需要config数据库更新,需要则调用tftp接口
                                              CcuConfigVersionCompareProcess(recv_config_version, pccu_gd_state_param);
                                              ///< 判断是否需要ann数据库更新,需要则调用tftp接口
                                              CcuAnnVersionCompareProcess(recv_ann_version, pccu_gd_state_param);
                                              if( strcmp(recv_ann_version, pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string) <= 0 &&
                                                   strcmp(recv_config_version, pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) <= 0 )
                                              {
                                                     sync_finished = 1; ///< 本地标志为1,表示CCU之间更新结束
                                              }
	                              }
	                       }
                       }
              }	      
       }

	if( 1 != sync_finished ) ///< ( plocal_dev_config_info->is_db_server ) ///< 如果CCU之间没有结束
	{
              if( pccu_gd_state_param->soft_count >= 3 && 1==AlarmCheckTimeOut(pccu_gd_state_param->alarm_sync_id) )
              {
              	     ///< 如果超过3次没有响应,直接跳过CCU更新阶段
                     sync_finished = 1;
                     AlarmDisable(pccu_gd_state_param->alarm_sync_id);
                     pccu_gd_state_param->other_ccu_offline = 1;
                     debug_print(("CcuGdSyncCcuDbProcess:  timeout, 3, other-CCU is not online \n"));
              }
              if ( 1==AlarmCheckTimeOut(pccu_gd_state_param->alarm_sync_id) )
              {
                    AlarmDisable(pccu_gd_state_param->alarm_sync_id);
                    AlarmEnable(pccu_gd_state_param->alarm_sync_id); ///< restart this timer
                    pccu_gd_state_param->soft_count++;
		    if( plocal_dev_config_info->is_db_server == 1 )
		    {
	                 debug_print(("CcuGdSyncCcuDbProcess:  server CCU --> other CCU\n"));
	            }
		    else if( plocal_dev_config_info->is_db_server == 0 )
		    {
	                 debug_print(("CcuGdSyncCcuDbProcess:  non-server CCU --> server CCU\n"));
	            }			

                    CcuGdSendSyncPackageToCcu( 1, pccu_gd_state_param ); ///< 重新发送一次请求包
              }
	}

       if( sync_finished ) ///< 更新结束,切换到BCP,EAMP,PCU数据库更新阶段
       {
	    debug_print(("CcuGdSyncCcuDbProcess:  ready to change for BEP \n"));       
            CcuGdSyncStateSwitch( (gd_state_process_t **)&(pccu_gd_state_param->plocal_curr_gd_state), 
				                     GD_SYNC_STATE_TRIGGER_BEP_DB_SYNC, (void *)pccu_gd_state_param);
       }

      return ;
}
void  CcuGdSyncCcuDbExit(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncCcuDbExit: null ptr\n"));
	     return ;
       }       

      return ;       
}

#ifndef CONFIG_BCU_SYNC_FROM_CCU_NET ///< this is normal operation mode,但目前没有使用
void  CcuGdSyncBepDbEnter(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncBepDbEnter: null ptr\n"));
	     return ;
       }

       if( pccu_gd_state_param->plocal_dev_config_info->is_db_server )
       {
            network_send_package_t  temp_net_cmd = {0};       
            unsigned int *tmptr = (unsigned int *)(temp_net_cmd.send_information.event_info_common.gd_sync_cmd_info.gd_sync_context);
		
            //we are db server
	    strcpy(temp_net_cmd.dst_devices_name, "BCU");
            temp_net_cmd.dst_devices_no =  255;
	    strcpy(temp_net_cmd.send_information.src_devices_name, pccu_gd_state_param->plocal_dev_config_info->dev_name);
            temp_net_cmd.send_information.src_devices_no =  pccu_gd_state_param->plocal_dev_config_info->dev_no;
            temp_net_cmd.send_information.event_type_ann =  ANN_IDLE;
            temp_net_cmd.send_information.event_type_intercom =  INTERCOM_IDLE;
            temp_net_cmd.send_information.event_type_common =  EVENT_TYPE_SYS_UPDATE_SYNC;
            temp_net_cmd.send_information.event_info_common.gd_sync_cmd_info.gd_sync_req = 1;
            temp_net_cmd.send_information.event_info_common.gd_sync_cmd_info.gd_sync_res = 0;
            temp_net_cmd.send_information.event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DB_VERSION_SYNC_BEP;
            *tmptr = pccu_gd_state_param->plocal_dev_config_info->config_db_version;
	    tmptr++;
            *tmptr = pccu_gd_state_param->plocal_dev_config_info->ann_db_version;
	    if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &temp_net_cmd, sizeof(temp_net_cmd)) < 0 )
	    {
                   debug_print(("CcuGdSyncBepDbEnter, to all BCU: write cmd buffer error=%d\n"));
            }
	    strcpy(temp_net_cmd.dst_devices_name, "EAMP");
	    if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &temp_net_cmd, sizeof(temp_net_cmd)) < 0 )
	    {
                   debug_print(("CcuGdSyncBepDbEnter, to all EAMP: write cmd buffer error=%d\n"));
            }
	    strcpy(temp_net_cmd.dst_devices_name, "PCU");
	    if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &temp_net_cmd, sizeof(temp_net_cmd)) < 0 )
	    {
                   debug_print(("CcuGdSyncBepDbEnter, to all PCU: write cmd buffer error=%d\n"));
            }
      }	   

      return ;
}
void  CcuGdSyncBepDbProcess(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncBepDbProcess: null ptr\n"));
	     return ;
       }

       if(BlockBufferRead( pccu_gd_state_param->recv_net_cmd_buffer_id, pccu_gd_state_param->precv_net_cmd, 
	   	                           sizeof(network_send_package_t)) > 0)
       {     
               if ( pccu_gd_state_param->precv_net_cmd->send_information.event_type_common==EVENT_TYPE_SYS_UPDATE_SYNC )
               {
                       if( pccu_gd_state_param->precv_net_cmd->send_information.event_info_common.gd_sync_cmd_info.gd_sync_mask == 
					   	SUBEVENT_TYPE_DB_VERSION_SYNC_BEP )
                       {
                                if ( pccu_gd_state_param->precv_net_cmd->send_information.event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
                                      pccu_gd_state_param->precv_net_cmd->send_information.event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
                                {
                                        if ( pccu_gd_state_param->plocal_dev_config_info->is_db_server )
                                        {
                                               //we are serverCCU, we receive one request package
                                               CcuGdSendLocalDbVersion(pccu_gd_state_param);
					}
					else
					{
                                               //we  are non-server CCU, ignore this request package
					}
				}
                                else if ( pccu_gd_state_param->precv_net_cmd->send_information.event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
                                             pccu_gd_state_param->precv_net_cmd->send_information.event_info_common.gd_sync_cmd_info.gd_sync_res==1 )
                                {
                                      
				}
		       }
	       }
       }

       return ;
}
void  CcuGdSyncBepDbExit(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncBepDbExit: null ptr\n"));
	     return ;
       }
}
#else //sync bcu from ccu-net,目前使用这里,BCU从CCU获取数据库
#if 0 //old-cp,该函数暂时不用
int CcuSetupNetPackageToBcu(int net_cmd_tx_buffer, int sync_mask, void *sync_row, int sync_type_size)
{
       network_send_package_t  local_net_package = {0};
       gd_sync_cmd_t *psync_cmd = (gd_sync_cmd_t *)&(local_net_package.send_information.event_info_common.gd_sync_cmd_info);
       int *crc_p = NULL;

       strcpy(local_net_package.dst_devices_name,"BCU");
       local_net_package.dst_devices_no = 255;
       local_net_package.send_information.event_type_ann = ANN_IDLE;
       local_net_package.send_information.event_type_intercom = INTERCOM_IDLE;
       local_net_package.send_information.event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
       psync_cmd->gd_sync_mask = (unsigned char)sync_mask;						   
       psync_cmd->gd_sync_req = 1;
       psync_cmd->gd_sync_res = 0;
       memcpy((void *)(psync_cmd->gd_sync_context), sync_row, sync_type_size );
       crc_p = (int *)psync_cmd->gd_sync_context[sync_type_size];
       (*crc_p) = 0xA5A5A5A5;
       psync_cmd->gd_sync_context_len = sync_type_size+4;
       if( BlockBufferWrite(net_cmd_tx_buffer, &local_net_package, sizeof(local_net_package)) < 0 )
       {
           debug_print(("CcuSetupNetPackageToBcu:  write cmd buff<0\n"));
       }	   

       return 0;	   
}
#endif
void CcuSyncResetRowOption(db_table_row_sync_opt_t *ptable_row_opt, int next_phae)
{
      if(NULL != ptable_row_opt)
      {
            ptable_row_opt->read_db_once = 0;
            ptable_row_opt->read_db2_once = 0;
            ptable_row_opt->curr_row = 1;
            ptable_row_opt->row = 0;
            ptable_row_opt->column = 0;
            ptable_row_opt->repeat_times = 0;
            ptable_row_opt->next_phae = next_phae;	
            #ifdef  CONFIG_CCU_NO_DB_FILE
                 //ptable_row_opt->p_table = NULL;
            #endif
            ptable_row_opt->door_side_array = NULL;	///< med,1229
      }
      sequeue_waiting_for_response = -1; ///< reset

       return ;
}
int CcuSetupNetPackageToBcu(int net_cmd_tx_buffer, int sync_mask, void *sync_row, int sync_type_size, int src_dev_no)
{
       common_big_package_t  local_net_package = {"\0",0};
       pa_common_information_t *pcom_cmd_info = (pa_common_information_t *)&( local_net_package.common_big_data_u.pa_common_cmd_info);	   
       gd_sync_cmd_t *psync_cmd = (gd_sync_cmd_t *)&(pcom_cmd_info->event_info_common.gd_sync_cmd_info);
       int *crc_p = NULL;

       if( NULL==sync_row )
        {
           debug_print(("CcuSetupNetPackageToBcu:  NULL sync_row\n"));
	        return -1;
        }

       strcpy(local_net_package.dst_dev_name,"BCU");
       local_net_package.dst_dev_number = dest_dev_no_for_table_sync; ///< MUL_SYNC_DST_NO;
       strcpy(local_net_package.src_dev_name,"CCU");
       local_net_package.src_dev_number = src_dev_no;
       local_net_package.pkg_type = COMMON_PACKAGE_TYPE_SYNC_GD;
       pcom_cmd_info->event_type_common = EVENT_TYPE_SYS_UPDATE_SYNC;
       pcom_cmd_info->sequence = ++sending_counter; ///<add,0715
       if( pcom_cmd_info->sequence==0 || (~0)==pcom_cmd_info->sequence )
       {
              pcom_cmd_info->sequence = 1;
       }
       psync_cmd->gd_sync_mask = (unsigned char)sync_mask;
       psync_cmd->gd_sync_req = 1;
       psync_cmd->gd_sync_res = 0;
       memcpy((void *)(psync_cmd->gd_sync_context), sync_row, sync_type_size );
       crc_p = (int *)&(psync_cmd->gd_sync_context[sync_type_size]);
       (*crc_p) = 0xA5A5A5A5;
       psync_cmd->gd_sync_context_len = sync_type_size+4;
       #if 0 ///< debug
       {
             void *recv_buffer = NULL;
	     int subevent_type = 0;
	     int recv_len = 0;		 
	     recv_buffer = (void *)(local_net_package.common_big_data_u.pa_common_cmd_info.event_info_common.gd_sync_cmd_info.gd_sync_context);
	     subevent_type = local_net_package.common_big_data_u.pa_common_cmd_info.event_info_common.gd_sync_cmd_info.gd_sync_mask;	
             recv_len =  local_net_package.common_big_data_u.pa_common_cmd_info.event_info_common.gd_sync_cmd_info.gd_sync_context_len;			 
             if( SUBEVENT_TYPE_LINE_SECTION_TABLE_SYNC==subevent_type)
             {
			line_section_row_t  *pcurrent_section_row = (line_section_row_t  *)recv_buffer;		
                        debug_print(("CcuSetupNetPackageToBcu:  test line section: send len = %d \n", recv_len));
                        debug_print(("Bcu, line section:  total=%d, curr=%d \n", pcurrent_section_row->total_row, pcurrent_section_row->current_index));
                        debug_print(("Bcu, line section,row:  %d, %d, ", pcurrent_section_row->current_row_info.line_no, pcurrent_section_row->current_row_info.line_direction));
                        debug_print(("%d, %s, ", pcurrent_section_row->current_row_info.spec_cycle, pcurrent_section_row->current_row_info.short_name));
                        debug_print(("%s, %s, ", pcurrent_section_row->current_row_info.start_time, pcurrent_section_row->current_row_info.end_time));
                        debug_print(("%d, %d, ", pcurrent_section_row->current_row_info.default_section, pcurrent_section_row->current_row_info.section_no));
                        debug_print(("%d, %d \n", pcurrent_section_row->current_row_info.start_station, pcurrent_section_row->current_row_info.end_station));
                        debug_print(("%d, %d \n", pcurrent_section_row->current_row_info.start_station, pcurrent_section_row->current_row_info.end_station));						
	     }
       }
       #endif
       debug_print(("CcuSetupNetPackageToBcu: src=CCU-%d: dest=BCU-%d \n", src_dev_no,dest_dev_no_for_table_sync ));

       int wr_len = 0;
       wr_len = BlockBufferWrite(net_cmd_tx_buffer, &local_net_package, sizeof(local_net_package));
       if( wr_len < 0 )
       {
            debug_print(("CcuSetupNetPackageToBcu:  write cmd buff<0\n"));
	    return -1;
       }
      else
      {
            debug_print(("CcuSetupNetPackageToBcu:  write cmd buff ok,seq=%d, wr_len=%d \n", pcom_cmd_info->sequence, wr_len));
      }

       //update current package sequeue which has been written into Net-Queue
       sequeue_waiting_for_response = (int)pcom_cmd_info->sequence;
	   
       return 0;	   
}
int CcuSendLineStationTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       station_code_of_line_t  *tmp_ptr = NULL;
       line_station_code_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;	   
	   
       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (station_code_of_line_t *)(psync_bcu_param->p_table);
	   
       send_row.line_no = psync_bcu_param->line_no;
       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       send_row.current_row_info.station_no = (tmp_ptr[psync_bcu_param->curr_row-1].station_no);
       send_row.current_row_info.fix_across_station = (tmp_ptr[psync_bcu_param->curr_row-1].fix_across_station);
       send_row.current_row_info.spec_open_door = (tmp_ptr[psync_bcu_param->curr_row-1].spec_open_door);
	   
       dest_size = sizeof(send_row.current_row_info.station_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].station_display);
       CcuStrncpy((char *)send_row.current_row_info.station_name, dest_size, tmp_ptr[psync_bcu_param->curr_row-1].station_display, src_size );

       dest_size = sizeof(send_row.current_row_info.station_name_english);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].station_display_eng);
       CcuStrncpy((char *)send_row.current_row_info.station_name_english, dest_size, tmp_ptr[psync_bcu_param->curr_row-1].station_display_eng, src_size );

#if 0 ///< don't need this
       dest_size = sizeof(send_row.current_row_info.station_audio);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].station_audio);
       CcuStrncpy((char *)send_row.current_row_info.station_audio, dest_size, tmp_ptr[psync_bcu_param->curr_row-1].station_audio, src_size );

       dest_size = sizeof(send_row.current_row_info.station_audio_eng);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].station_audio_eng);
       CcuStrncpy((char *)send_row.current_row_info.station_audio_eng, dest_size, tmp_ptr[psync_bcu_param->curr_row-1].station_audio_eng, src_size );	   
#endif

       debug_print(("ccu send station code:  %d, totaol=%d, curr=%d, ", send_row.line_no, send_row.total_row, send_row.current_index));
       debug_print(("%d, %d, ", send_row.current_row_info.station_no, send_row.current_row_info.fix_across_station));
       //debug_print(("%s\n", send_row.current_row_info.station_name));
       int qq=0;
       for(qq=0; qq<9; qq++)
       {
            debug_print(("%02x ", send_row.current_row_info.station_name[qq]));	   
       }
	   
       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;
       ret = CcuSetupNetPackageToBcu(net_cmd_tx_buffer, SUBEVENT_TYPE_LINE_STATION_TABLE_SYNC, 
	   	                                         (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendLineSectionTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       line_section_table_t *tmp_ptr = NULL;
       line_section_row_t  send_row = {0};
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;
       int dest_len = 0, src_len = 0;

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (line_section_table_t *)psync_bcu_param->p_table;
	   
       send_row.line_no = psync_bcu_param->line_no;
       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;

       //memcpy((void *)&send_row.current_row_info, &(tmp_ptr[psync_bcu_param->curr_row-1]), sizeof(send_row.current_row_info));
       send_row.current_row_info.line_no = tmp_ptr[psync_bcu_param->curr_row-1].line_no;
       send_row.current_row_info.line_direction = tmp_ptr[psync_bcu_param->curr_row-1].line_direction;
       send_row.current_row_info.spec_cycle = tmp_ptr[psync_bcu_param->curr_row-1].spec_cycle;

       dest_len = sizeof(send_row.current_row_info.short_name);
       src_len = strlen(tmp_ptr[psync_bcu_param->curr_row-1].short_name);
       debug_print(("&&&&&&-1:  %d, %d \n", dest_len, src_len));	   
       CcuStrncpy(send_row.current_row_info.short_name, dest_len, tmp_ptr[psync_bcu_param->curr_row-1].short_name, src_len);

       dest_len = sizeof(send_row.current_row_info.start_time);
       src_len = strlen(tmp_ptr[psync_bcu_param->curr_row-1].start_time);
       debug_print(("&&&&&&-2:  %d, %d \n", dest_len, src_len));	   
       CcuStrncpy(send_row.current_row_info.start_time, dest_len, tmp_ptr[psync_bcu_param->curr_row-1].start_time, src_len);

       dest_len = sizeof(send_row.current_row_info.end_time);
       src_len = strlen(tmp_ptr[psync_bcu_param->curr_row-1].end_time);
       debug_print(("&&&&&&-3:  %d, %d \n", dest_len, src_len));
       CcuStrncpy(send_row.current_row_info.end_time, dest_len, tmp_ptr[psync_bcu_param->curr_row-1].end_time, src_len);
	   
       send_row.current_row_info.default_section = tmp_ptr[psync_bcu_param->curr_row-1].default_section;
       send_row.current_row_info.section_no =  tmp_ptr[psync_bcu_param->curr_row-1].section_no;
       send_row.current_row_info.start_station = tmp_ptr[psync_bcu_param->curr_row-1].start_station;
       send_row.current_row_info.end_station = tmp_ptr[psync_bcu_param->curr_row-1].end_station;
	   
       debug_print(("ccu send setion:  %d, %d, %d, ", send_row.line_no, send_row.total_row, send_row.current_index));
       debug_print(("%d, %d, %d, ", send_row.current_row_info.line_no, send_row.current_row_info.line_direction, send_row.current_row_info.spec_cycle));
       debug_print(("%s, %s, %s, ", send_row.current_row_info.short_name, send_row.current_row_info.start_time, send_row.current_row_info.end_time));	   
       debug_print(("%d, %d, ", send_row.current_row_info.default_section,send_row.current_row_info.section_no));
       debug_print(("%d, %d \n", send_row.current_row_info.start_station,send_row.current_row_info.end_station));
	   
       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_LINE_SECTION_TABLE_SYNC, 
	   	                                         (void *)(&send_row), sizeof(send_row), src_dev_no);
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendDefaultLineTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       line_table_t *tmp_ptr = NULL;
       default_line_table_row_t  send_row = {0};
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (line_table_t *)psync_bcu_param->p_table;
	   
       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       //memcpy((void *)&send_row.current_row_info, (void *)&(tmp_ptr[psync_bcu_param->curr_row-1]), sizeof(send_row.current_row_info));
       send_row.current_row_info.default_line = tmp_ptr[psync_bcu_param->curr_row-1].default_line;
       send_row.current_row_info.line_no = tmp_ptr[psync_bcu_param->curr_row-1].line_no;	   

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_DEFAULT_LINE_TABLE_SYNC, 
	   	                                                  (void *)(&send_row), sizeof(send_row), src_dev_no);
       debug_print(("CcuSendDefaultLineTableToBcu:  t=%d, c=%d\n", send_row.total_row, send_row.current_index));
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }
	   
       return ret;
}
int CcuSendEmergTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       emerg_table_item_t *tmp_ptr = NULL;
       emerg_ann_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;	   
       int src_dev_no = -1;

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
        tmp_ptr = (emerg_table_item_t *)psync_bcu_param->p_table;

       //send_row.line_no = 3/4; ///<same emerg files	   
       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       send_row.current_row_info.emerg_ann_id = (tmp_ptr[psync_bcu_param->curr_row-1].emerg_ann_id);
       //send_row.current_row_info.line_no = (tmp_ptr[psync_bcu_param->curr_row-1].line_no);
       send_row.current_row_info.repeat_times = (tmp_ptr[psync_bcu_param->curr_row-1].repeat_times);

       dest_size = sizeof(send_row.current_row_info.audio_path);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].audio_path);
	CcuStrncpy((char *)send_row.current_row_info.audio_path, dest_size, (char *)tmp_ptr[psync_bcu_param->curr_row-1].audio_path, src_size);

       dest_size = sizeof(send_row.current_row_info.display_message);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].display_message);
//       src_size = 150;
	CcuStrncpy((char *)(send_row.current_row_info.display_message), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].display_message), src_size);

       dest_size = sizeof(send_row.current_row_info.emerg_ann_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].emerg_ann_name);
	CcuStrncpy((char *)(send_row.current_row_info.emerg_ann_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].emerg_ann_name), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	   
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_EMERG_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendIpTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       ip_mac_config_t *tmp_ptr = NULL;
       ip_mac_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (ip_mac_config_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       send_row.current_row_info.device_type = (tmp_ptr[psync_bcu_param->curr_row-1].device_type_no);
       send_row.current_row_info.device_no = (tmp_ptr[psync_bcu_param->curr_row-1].device_no);
       send_row.current_row_info.server_whether = (tmp_ptr[psync_bcu_param->curr_row-1].server_whether);

       dest_size = sizeof(send_row.current_row_info.device_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].device_name);
       CcuStrncpy((char *)(send_row.current_row_info.device_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].device_name), src_size);

       dest_size = sizeof(send_row.current_row_info.ip_address);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].ip_address);
       CcuStrncpy((char *)(send_row.current_row_info.ip_address), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].ip_address), src_size);

       dest_size = sizeof(send_row.current_row_info.mac_address);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].mac_address);
       CcuStrncpy((char *)(send_row.current_row_info.mac_address), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].mac_address), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_IP_TABLE_SYNC, 
	   	                                         (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendAnnVersionTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       db_version_t *tmp_ptr = NULL;
       ann_db_version_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;	   
	   
       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (db_version_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       send_row.current_row_info.version_id = (tmp_ptr[psync_bcu_param->curr_row-1].version_id); 

       dest_size = sizeof(send_row.current_row_info.current_version);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].current_version);
       CcuStrncpy((char *)(send_row.current_row_info.current_version), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].current_version), src_size);
	   

       dest_size = sizeof(send_row.current_row_info.short_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].short_name);
       CcuStrncpy((char *)(send_row.current_row_info.short_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].short_name), src_size);

       dest_size = sizeof(send_row.current_row_info.time);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].time);
       CcuStrncpy((char *)(send_row.current_row_info.time), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].time), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_ANN_VERSION_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendConfigVersionTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       db_version_t *tmp_ptr = NULL;
       config_db_version_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;
	   
       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (db_version_t *)psync_bcu_param->p_table;	   

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       send_row.current_row_info.version_id = (tmp_ptr[psync_bcu_param->curr_row-1].version_id);

       dest_size = sizeof(send_row.current_row_info.current_version);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].current_version);
       CcuStrncpy((char *)(send_row.current_row_info.current_version), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].current_version), src_size);
	   	   

       dest_size = sizeof(send_row.current_row_info.short_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].short_name);
       CcuStrncpy((char *)(send_row.current_row_info.short_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].short_name), src_size);

       dest_size = sizeof(send_row.current_row_info.time);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].time);
       CcuStrncpy((char *)(send_row.current_row_info.time), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].time), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_CONFIG_VERSION_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendPriorityTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       priority_id_t *tmp_ptr = NULL;
       priority_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;	   
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;
	   
       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (priority_id_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       send_row.current_row_info.priority_id = (tmp_ptr[psync_bcu_param->curr_row-1].priority_id);

       dest_size = sizeof(send_row.current_row_info.event_type);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].event_type);
       CcuStrncpy((char *)(send_row.current_row_info.event_type), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].event_type), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_PRIORITY_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   

       return 0;
}
int CcuSendDEvNumberTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       device_number_t *tmp_ptr = NULL;
       device_number_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;	   

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (device_number_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
       //send_row.current_row_info.device_type = (tmp_ptr[psync_bcu_param->curr_row-1].device_type);
       send_row.current_row_info.device_number_count = (tmp_ptr[psync_bcu_param->curr_row-1].device_number_count);

       dest_size = sizeof(send_row.current_row_info.device_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].device_name);
       CcuStrncpy((char *)(send_row.current_row_info.device_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].device_name), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_DEV_NUMBER_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   
       return 0;
}
int CcuSendRecPathTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       file_path_t *tmp_ptr = NULL;
       record_path_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (file_path_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;
	   
       dest_size = sizeof(send_row.current_row_info.path_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].path_name);
       CcuStrncpy((char *)(send_row.current_row_info.path_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].path_name), src_size);

       dest_size = sizeof(send_row.current_row_info.even_type);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].even_type);
       CcuStrncpy((char *)(send_row.current_row_info.even_type), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].even_type), src_size);

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_RECORD_PATH_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }
	   
       return 0;
}
int CcuSendLogPathTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       file_path_t *tmp_ptr = NULL;
       log_path_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;	   
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;	   

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (file_path_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;

       dest_size = sizeof(send_row.current_row_info.even_type);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].even_type);
       CcuStrncpy((char *)(send_row.current_row_info.even_type), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].even_type), src_size);	   
	   
       dest_size = sizeof(send_row.current_row_info.path_name);
       src_size = strlen(tmp_ptr[psync_bcu_param->curr_row-1].path_name);
       CcuStrncpy((char *)(send_row.current_row_info.path_name), dest_size, (char *)(tmp_ptr[psync_bcu_param->curr_row-1].path_name), src_size);	 

       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_LOG_PATH_TABLE_SYNC, 
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }	   
       return 0;
}
#if 1 ///< 发送开门侧信息,使用该函数, 2014-09-20注
void *CcuSendNewSectionDoorSideToBcu( int line_no, int section_no, int net_cmd_tx_buffer, int src_dev_no )
{
#if 1  ///< get door-side from ccu_state
       int i = 0;
       int row = 0;
       int column = 0;
       void *p_table = NULL;
       int max_station =  MAX_DOOR_SIDE_ARRAY_ITEMS;
       line_station_door_side_t *pline_door_side_array = NULL;
       extern ccu_state_t ccu_state;
	   
         if( 3==line_no )
         {
              ///< total of line 3, but when we go here, control thread maybe unfinished
              row = ccu_state.ccu_region_info[0].total_stations_number;
         }
         else if( 4==line_no )
         {
              ///< total of line 4, but when we go here, control thread maybe unfinished
              row = ccu_state.ccu_region_info[1].total_stations_number;
         }
	else
	{
	       pline_door_side_array = NULL;
	       return NULL;
	}
	
         pline_door_side_array = (line_station_door_side_t *)malloc(GD_DEV_INFO_MAX_LEN);
         if( NULL==pline_door_side_array )
         { 
               debug_print( ("CcuSendDoorSide, malloc array fail :-( \n ") );
               return NULL;
	 }
		 
         pline_door_side_array->line_no = line_no;
         pline_door_side_array->section_no = section_no; ///< all stations	
         pline_door_side_array->total_bytes = sizeof(line_station_door_side_t) + (row+1)*sizeof(line_station_door_side_array_t);
         if( pline_door_side_array->total_bytes >= MAX_DOOR_SIDE_ARRAY_BYTES )
         {
               pline_door_side_array->total_bytes = MAX_DOOR_SIDE_ARRAY_BYTES;
         }
         debug_print( ("ccu-net-bcu-sync, station door side:  line=%d, stations=%d \n ", line_no, row) );
//         diag_printf ("ccu-net-bcu-sync, station door side:  line=%d, stations=%d \n ", line_no, row) ;
         pline_door_side_array->array_items = (row+1); ///< row = stations
         for(i=1; i<=(row)&&i<MAX_STATION_NUMBER; i++)
         {
               if( 3==line_no )
               {
	               pline_door_side_array->door_side_array[i].station_no = ccu_state.pp_station_info[0][i].station_no;
	               pline_door_side_array->door_side_array[i].door_side[0] = ccu_state.pp_station_info[0][i].open_door_side[0];
	               pline_door_side_array->door_side_array[i].door_side[1] = ccu_state.pp_station_info[0][i].open_door_side[1];
	      }
              else if( 4==line_no )
              {
	               pline_door_side_array->door_side_array[i].station_no = ccu_state.pp_station_info[1][i].station_no;
	               pline_door_side_array->door_side_array[i].door_side[0] = ccu_state.pp_station_info[1][i].open_door_side[0];
	               pline_door_side_array->door_side_array[i].door_side[1] = ccu_state.pp_station_info[1][i].open_door_side[1];
	      }			   
         }
		 
         return (void *)(pline_door_side_array);     
#else ///< read db
  int i = 0;
       int row = 0;
       int column = 0;
       void *p_table = NULL;	   
       int max_station =  MAX_DOOR_SIDE_ARRAY_ITEMS;   
       ann_station_report_t *ann_report_table_item = NULL;
       line_station_door_side_t *pline_door_side_array = NULL;
       ann_station_report_t *tmp_gb_ann_array_ptr = NULL;

        AnnReportOfEntry(line_no, section_no, &row, &column, &p_table);
        debug_print( ("ccu-net-bcu-sync, station door side:  %d, %d, %d, %d, %d\n ", line_no, section_no, row, column, max_station) );
         if( row<=0 ||NULL==p_table )
         { 
               return -1;
	 }
         else if( row > max_station )
         {
               debug_print( ("ccu-net-bcu-sync, station door side,too much station:  %d,%d\n ", row, max_station) );
               row = max_station;
	 }
         ann_report_table_item = (ann_station_report_t *)p_table;
         pline_door_side_array = (line_station_door_side_t *)malloc(GD_DEV_INFO_MAX_LEN);
         if( NULL==pline_door_side_array )
         { 
               debug_print( ("CcuSendDoorSide, malloc array fail :-( \n ") );
               return -1;
	 }         
		 
         pline_door_side_array->line_no = line_no;
         pline_door_side_array->section_no = section_no;
         pline_door_side_array->total_bytes = sizeof(line_station_door_side_t) + row*sizeof(line_station_door_side_array_t);
         if( pline_door_side_array->total_bytes >= MAX_DOOR_SIDE_ARRAY_BYTES )
         {
               pline_door_side_array->total_bytes = MAX_DOOR_SIDE_ARRAY_BYTES;
         }
         debug_print( ("ccu-net-bcu-sync, station door side:  stations=%d \n ", row) );
         pline_door_side_array->array_items = (row); ///< row = stations
         for(i=0; i<(row); i++)
         {
               pline_door_side_array->door_side_array[i].station_no = ann_report_table_item[i].station_no;
               pline_door_side_array->door_side_array[i].door_side = ann_report_table_item[i].station_type[1].open_door;
         }
		 
         #if 0 ///< for future
         tmp_gb_ann_array_ptr = (ann_station_report_t *)CcuAnnTableOperation(NULL, 0);
         if( NULL != tmp_gb_ann_array_ptr )
         {
               SqlTableBufferFree((void *)tmp_gb_ann_array_ptr); ///< release old Ann-Table
         }
         CcuAnnTableOperation(ann_report_table_item, 1); ///< save new Ann-Table
         #endif
		 
         SqlTableBufferFree(&p_table); ///< release SQlite memory

         return (void *)(pline_door_side_array);       
#endif		 
}
#else ///< backup, 0703, 暂时未用
int CcuSendNewSectionDoorSideToBcu( int line_no, int section_no, int net_cmd_tx_buffer, int src_dev_no )
{
       int i = 0;
       int row = 0;
       int column = 0;
       void *p_table = NULL;	   
       int max_station =  MAX_DOOR_SIDE_ARRAY_ITEMS;
       ann_station_report_t *ann_report_table_item = NULL;
       char tmp_array[GD_DEV_INFO_MAX_LEN];
       line_station_door_side_t *pline_door_side_array = (line_station_door_side_t *)tmp_array;
       ann_station_report_t *tmp_gb_ann_array_ptr = NULL;

        AnnReportOfEntry(line_no, section_no, &row, &column, &p_table);
        debug_print( ("ccu-net-bcu-sync, station door side:  %d, %d, %d, %d, %d\n ", line_no, section_no, row, column, max_station) );
         if( row<=0 ||NULL==p_table )
         { 
               return -1;
	 }
         else if( row > max_station )
         {
               debug_print( ("ccu-net-bcu-sync, station door side,too much station:  %d,%d\n ", row, max_station) );
               row = max_station;
	 }
         ann_report_table_item = (ann_station_report_t *)p_table;
		 
         pline_door_side_array->line_no = line_no;
         pline_door_side_array->section_no = section_no;
         pline_door_side_array->total_bytes = sizeof(line_station_door_side_t) + row*sizeof(line_station_door_side_array_t);
         if( pline_door_side_array->total_bytes >= MAX_DOOR_SIDE_ARRAY_BYTES )
         {
               pline_door_side_array->total_bytes = MAX_DOOR_SIDE_ARRAY_BYTES;
         }
         debug_print( ("ccu-net-bcu-sync, station door side:  stations=%d \n ", row) );
         pline_door_side_array->array_items = (row); ///< row = stations
         for(i=0; i<(row); i++)
         {
               pline_door_side_array->door_side_array[i].station_no = ann_report_table_item[i].station_no;
               pline_door_side_array->door_side_array[i].door_side = ann_report_table_item[i].station_type[1].open_door;
         }
		 
         CcuSetupNetPackageToBcu(net_cmd_tx_buffer, SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC, 
		 	                                 (void *)tmp_array, pline_door_side_array->total_bytes, src_dev_no);
         tmp_gb_ann_array_ptr = (ann_station_report_t *)CcuAnnTableOperation(NULL, 0);
         if( NULL != tmp_gb_ann_array_ptr )
         {
               SqlLocalBufferFree((void *)tmp_gb_ann_array_ptr); ///< release old Ann-Table
         }
         CcuAnnTableOperation(ann_report_table_item, 1); ///< save new Ann-Table 

         return 0;
}
#endif
int CcuSendPuTSPasswordTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
    int ret = -1;
     pu_ts_password_table_row_t  send_row = {0};
     pu_ts_passeord_table_item_t *tmp_ptr = NULL;
     int net_cmd_tx_buffer = 0;
     int src_dev_no = -1;

     tmp_ptr = (pu_ts_passeord_table_item_t *)psync_bcu_param->p_table;

     send_row.total_row = psync_bcu_param->total_row;
     send_row.current_index = psync_bcu_param->curr_row;
     strncpy(send_row.current_row_info.ts_password,tmp_ptr->ts_password,6);

     net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
     src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;
     ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_STATION_BCU_TS_PASSWORD,
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
     if( ret==0 ) //package written into net-buffer,so enable alarm
     {
            AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
     }

     return 0;
}
int CcuSendPuVolTableToBcu( net_sync_bcu_param_t *psync_bcu_param )
{
       int ret = -1;
       pu_volume_table_item_t *tmp_ptr = NULL;
       pu_volume_table_row_t  send_row = {0};
       int dest_size = 0, src_size = 0;
       int net_cmd_tx_buffer = 0;
       int src_dev_no = -1;

       if( NULL==psync_bcu_param ||NULL==psync_bcu_param->p_table ||
	    NULL==psync_bcu_param->gd_state_param )
       {
             return -1;
       }
       tmp_ptr = (pu_volume_table_item_t *)psync_bcu_param->p_table;

       send_row.total_row = psync_bcu_param->total_row;
       send_row.current_index = psync_bcu_param->curr_row;

       send_row.current_row_info.d2d_volume = tmp_ptr[psync_bcu_param->curr_row-1].d2d_volume;
       send_row.current_row_info.d2p_volume = tmp_ptr[psync_bcu_param->curr_row-1].d2p_volume;
       send_row.current_row_info.d2eamp_volume = tmp_ptr[psync_bcu_param->curr_row-1].d2eamp_volume;
       send_row.current_row_info.d2listen_volume = tmp_ptr[psync_bcu_param->curr_row-1].d2listen_volume;
       
       net_cmd_tx_buffer = psync_bcu_param->gd_state_param->send_net_cmd_buffer_id;
       src_dev_no = psync_bcu_param->gd_state_param->plocal_dev_config_info->dev_no;	
       ret = CcuSetupNetPackageToBcu( net_cmd_tx_buffer, SUBEVENT_TYPE_STATION_BCU_VOLUME_SYNC,
	   	                                          (void *)(&send_row), sizeof(send_row), src_dev_no );
       if( ret==0 ) //package written into net-buffer,so enable alarm
       {
              AlarmEnableWithNewTrigger(psync_bcu_param->gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
       }
	   
       return 0;
}

static void CcuCreateBcuVolFile(char *fname)
{
        int i = 0;
	FILE *fp = NULL;
		
        for(i=0; i<10; i++)
        {
		fp = fopen(fname, "r");
	        if( NULL==fp )
	        {
                        cyg_thread_delay(20);  
			debug_print(("[%d]: retry to open file(%s) again \n", i, fname));
		}
		else
		{
		      debug_print(("[%d]:  open file(%s) ok \n", i, fname));
                       break;
		}
	}

	if(0) ///< ( NULL != fp ) ///< debug only
	{
	   fclose(fp);
	   fp = NULL;
	}
		
	if( NULL == fp ) ///< create file
	{
	        for(i=0; i<10; i++)
	        {
			fp = fopen(fname, "w+");
		        if( NULL==fp )
		        {
	                        cyg_thread_delay(20);  
				debug_print(("[%d]: create file(%s) again \n", i, fname));
			}
			else
			{
			      debug_print(("[%d]:  create file(%s) ok \n", i, fname));
	                      break;
			}
		}
	}

	if( NULL == fp )
	{
	      debug_print(("create file(%s), too much fails \n", fname));
	}
	else
	{
	       fclose(fp);
	       fp = NULL;
	}

	return ;
}

int CcuSetPowerUpVolumeTable( pu_volume_table_item_t *pvol_table_item)
{
         int ret = -1;
         FILE *fp = NULL;
	 char vol_buf[128];	 
	 char tmp_string[16] = {',', 0x00};
	 int *ptr = NULL;
	 int i = 0, j = 0;

         if( NULL == pvol_table_item )
         {
               return -1;
         }
         memset((void *)vol_buf, 0, sizeof(vol_buf)); ///< must
		 
         fp = fopen(CONFIG_BCU_VOL_FILE, "w+"); ///< clear file, then write new
         if( NULL==fp )
         {
               debug_print(("CcuSetPowerUpVolumeTable: open file(%s) fail \n", CONFIG_BCU_VOL_FILE));
	       ret = -1;
         }
	 else
	 {
		debug_print(("Pu Vol Table: new vol: d2d=%d, d2p=%d, d2e=%d, d2l=%d \n", 
                                       pvol_table_item->d2d_volume, pvol_table_item->d2p_volume, 
                                       pvol_table_item->d2eamp_volume, pvol_table_item->d2listen_volume));
		ptr = (int *)&(pvol_table_item->d2d_volume);
		for(i=0; i<4; i++)
		{                       
                       sprintf((char *)&tmp_string[1], "%d", (*ptr));
		       //debug_print(("new vol: [%d]=%d, (%s) \n", i, (*ptr), tmp_string ));
                       if(0==i)
   		          strcat(vol_buf, (char *)&tmp_string[1]);
                       else
   		          strcat(vol_buf, tmp_string);                          
		       //debug_print(("new vol: %s \n", vol_buf ));			   
		       ptr++;
		}
   		strcat(vol_buf, "\n");
                debug_print(("full new vol: %s \n", vol_buf ));		
		i = strlen(vol_buf);
		fwrite( (const void*)vol_buf, i, 1, fp);
	 }

	 if( NULL != fp )
	 {
		fclose(fp);
	 }
	 
	 return ret;	
}

int CcuGetPowerUpTSPasswordTable(int *row, int *column, int **p_table)
{
         int ret = -1;
         //FILE *fp = NULL;
	 //char vol_buf[128];
         //char *vol_ptr[8];
	 int i = 0, j = 0;
	 pu_ts_passeord_table_item_t *p_ts_pswd_table_item = NULL;

         if( NULL != *p_table )  ///< release last one
         {
              free( *p_table );
              *p_table = NULL;
         }
         p_ts_pswd_table_item = (pu_ts_passeord_table_item_t *)malloc(sizeof(pu_ts_passeord_table_item_t));
		if( NULL==p_ts_pswd_table_item )
		{
                     debug_print(("CcuCreateBcuVolFile: malloc fail \n"));
                     return -1;
		}
		strncpy(p_ts_pswd_table_item->ts_password,ccu_state.ts_password,6);
		 debug_print(("bcu ts password -> BCU-%s \n",p_ts_pswd_table_item->ts_password));
		if( NULL == *p_table ) ///<last is released
		{
			  (*p_table) = (void *)p_ts_pswd_table_item;
			  if( NULL != row )
			  {
								(*row) = 1;
			  }
			  if( NULL != column )
			  {
								(*column) = 1;
			  }
		}
		ret = 0;
	 return ret;
}

int CcuGetPowerUpVolumeTable(int *row, int *column, int **p_table)
{
         int ret = -1;
         //FILE *fp = NULL;
	 //char vol_buf[128];
         //char *vol_ptr[8];
	 int i = 0, j = 0;
	 pu_volume_table_item_t *pvol_table_item = NULL;

         if( NULL != *p_table )  ///< release last one
         {
              free( *p_table );
              *p_table = NULL;
         }	 

#if 0
    	 memset((void *)vol_buf, 0, sizeof(vol_buf));
    	 memset((void *)vol_ptr, 0, sizeof(vol_ptr));
         fp = fopen(CONFIG_BCU_VOL_FILE, "r");
         if( NULL==fp )
         {
               debug_print(("CcuCreateBcuVolFile: open file(%s) fail \n", CONFIG_BCU_VOL_FILE));
	       ret = -1;
         }
	 else
	 {
	        pvol_table_item = (pu_volume_table_item_t *)malloc(sizeof(pu_volume_table_item_t));
		if( NULL==pvol_table_item )
		{
                        debug_print(("CcuCreateBcuVolFile: malloc fail \n"));
                        return -1;
		}
                fgets(vol_buf, sizeof(vol_buf)-1, fp);
		vol_buf[sizeof(vol_buf)-1] = 0x00;
		debug_print(("Pu Vol Table: %s \n", vol_buf)); ///< 123,456,789,258
		vol_ptr[j++] = vol_buf;
		for(i=0; i<sizeof(vol_buf); i++)
		{
                      if( ','==vol_buf[i] )
                      {
                             vol_buf[i] = 0x00;
			     vol_ptr[j++] = vol_buf+i+1;
		      }
                      else if('\n'==vol_buf[i] )
                      {
                             vol_buf[i] = 0x00;
                             break;
		      }					  
		      else if( vol_buf[i]<'0' || vol_buf[i]>'9' )
		      {
		           debug_print(("Pu Vol Table: [%d]=%c <not 0-9> \n", i, vol_buf[i]));
                           j = 0;
			   break;
		      }
		}

                if( j>=4 ) 
                {
	                pvol_table_item->d2d_volume = atoi(vol_ptr[0]);
	                pvol_table_item->d2p_volume = atoi(vol_ptr[1]);
	                pvol_table_item->d2eamp_volume = atoi(vol_ptr[2]);
	                pvol_table_item->d2listen_volume = atoi(vol_ptr[3]);
		        debug_print(("Pu Vol Table: dd=%d, dp=%d, de=%d, dl=%d \n", pvol_table_item->d2d_volume, 
					pvol_table_item->d2p_volume, pvol_table_item->d2eamp_volume, 
                                        pvol_table_item->d2listen_volume)); 
			if( NULL == *p_table ) ///<last is released
			{
                              (*p_table) = (void *)pvol_table_item;
			      if( NULL != row )
			      {
                                    (*row) = 1;
			      }
			      if( NULL != column )
			      {
                                    (*column) = 4;
			      }				  
			}
			ret = 0;
		}
		else
		{
		       free(pvol_table_item);
                       ret = -1;
		}
	 }

         if( NULL !=fp )
         {
              fclose(fp);
	 }
#else
	        pvol_table_item = (pu_volume_table_item_t *)malloc(sizeof(pu_volume_table_item_t));
		if( NULL==pvol_table_item )
		{
                     debug_print(("CcuCreateBcuVolFile: malloc fail \n"));
                     return -1;
		}
		pvol_table_item->d2d_volume = ccu_state.current_d2d_volume;
		pvol_table_item->d2p_volume = ccu_state.current_d2p_volume;
		pvol_table_item->d2eamp_volume = ccu_state.current_pa_volume;
		pvol_table_item->d2listen_volume = ccu_state.current_monitor_volume;
				        debug_print(("Pu Vol Table: dd=%d, dp=%d, de=%d, dl=%d \n", pvol_table_item->d2d_volume,
							pvol_table_item->d2p_volume, pvol_table_item->d2eamp_volume,
		                                        pvol_table_item->d2listen_volume));
					if( NULL == *p_table ) ///<last is released
					{
		                              (*p_table) = (void *)pvol_table_item;
					      if( NULL != row )
					      {
		                                    (*row) = 1;
					      }
					      if( NULL != column )
					      {
		                                    (*column) = 4;
					      }
					}
					ret = 0;
#endif
	 
	 return ret;	
}
int CcuGdSyncCheckResponse(gd_state_param_t  *ccu_gd_state_param)
{
        int ret = -1;
        db_table_row_sync_opt_t *ptable_row_opt;		

	if( NULL == ccu_gd_state_param ||NULL==ccu_gd_state_param->ptable_row_opt)
	{
               debug_print(("CcuGdSyncCheckResponse: null ptr\n"));
	       return -1;
        }
        ptable_row_opt = ccu_gd_state_param->ptable_row_opt;

#ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
    	if( -1 == sequeue_waiting_for_response )
    	{
    		return -1;
    	}
#endif
		
	if( sequeue_waiting_for_response != 0 )
	{
	     if( 0==AlarmCheckTimeOut(ccu_gd_state_param->alarm_sync_id) )
	     {
             debug_print(("CcuGdSyncCheckResponse: keep seq=%d waiting \n", sequeue_waiting_for_response));
	           return 0; ///< keep waiting
	     }
             else ///< time-out
             {
                    AlarmDisable(ccu_gd_state_param->alarm_sync_id);
                    ptable_row_opt->repeat_times++;
                    //AlarmEnable(ccu_gd_state_param->alarm_sync_id);
		    ret = 2;
        	    debug_print(("CcuGdSyncCheckResponse:  timeout=%d \n", ptable_row_opt->repeat_times));					
                    if( ptable_row_opt->repeat_times > 3 )
                    {
                          #if 0 ///< send all the left
                          ptable_row_opt->read_db_once = 0;
                          ptable_row_opt->read_db2_once = 0;
                          ptable_row_opt->row = 0;
                          ptable_row_opt->column = 0;
                          ptable_row_opt->curr_row = 1;
                          ptable_row_opt->next_phae = 0;
			  #endif
                          ptable_row_opt->repeat_times = 0;
                          //AlarmDisable(ccu_gd_state_param->alarm_sync_id);                          
                          ret = 3;
                    }
              }
	}
	else
	{
             ret = 1;
	}

	return ret;
}
/*********************************************************
 ** @brief   CCU发送数据库表格的处理函数
 ** @param   gd_state_param_t  *  状态入口参数指针
 ** @return  是否发送完成
 ** @return  0  没有发送完成,需要继续
 **          -1 发送完成
 ** @see     
 ** @note： 本函数实质是一个switch控制的状态机,每个case对应一个表格.
            每个case处理方式基本相同,详见代码注释
 ********************************************************/
int CcuSyncDbToBcuSMachineEntry(gd_state_param_t  *ccu_gd_state_param) 
{
        net_sync_bcu_param_t  sync_bcu_param = {0};
        db_table_row_sync_opt_t *ptable_row_opt = NULL;
        int ret = 0;

	if( NULL == ccu_gd_state_param ||NULL==ccu_gd_state_param->ptable_row_opt)
	{
               debug_print(("CcuSyncDbToBcuSMachineEntry: null ptr\n"));
	       return -1;
        }
        sync_bcu_param.gd_state_param = ccu_gd_state_param;
        ptable_row_opt = ccu_gd_state_param->ptable_row_opt;	

       diag_printf("CcuSyncDbToBcuSMachineEntry:  enter, state=%d \n", ptable_row_opt->next_phae);
       cyg_thread_delay(20);
	
      switch( ptable_row_opt->next_phae )
      {
             case 0: 
	          ptable_row_opt->next_phae = SUBEVENT_TYPE_DEFAULT_LINE_TABLE_SYNC;
	      break;

	     case SUBEVENT_TYPE_DEFAULT_LINE_TABLE_SYNC: ///< 发送默认线路表
	     {
		    debug_print(("default line table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if ( 0 != SqlOfTableTline(&ptable_row_opt->row, &ptable_row_opt->column, &(ptable_row_opt->p_table)) )
                           {
                                #ifndef  CONFIG_CCU_NO_DB_FILE
                                    //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LINE_STATION_TABLE_SYNC);
                                #else
				     ptable_row_opt->p_table = (void *)&sim_default_line_table;
                                     ptable_row_opt->row = 2;
                                     ptable_row_opt->column = 2;
                                     ptable_row_opt->read_db_once = 1;
                                     debug_print(("default line table, simulate:  c=%d\n", ptable_row_opt->curr_row));
                                #endif
			   }
                           else
                           {
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT                           
                                 CcuGdDisplayTableTline(ptable_row_opt->row, ptable_row_opt->p_table); ///< 打印表格数据,调试
				#endif
                                 ptable_row_opt->read_db_once = 1;
			   }
                           ccu_gd_state_param->default_line_no = JudgeDefaultLine(ptable_row_opt->p_table, ptable_row_opt->row);
		           debug_print( ("send defaultLine to Bcu:  %d, %d, %d\n ", ptable_row_opt->row, ptable_row_opt->column, ccu_gd_state_param->default_line_no) );
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE ///< 每发送表格一行数据,在这里等待响应到来
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0; ///< 返回0,则下次还是发送该表格
			   }
		           else if( 1 == tmp_ret ||3 == tmp_ret )
		           {
		                 if( 3 == tmp_ret ) ///< 3表示超过3次,没有收到响应,则丢弃该包,发送下一行
		                 {
                                       debug_print(("send defaultLine to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));
				 }
				 ptable_row_opt->curr_row++; ///< send the next
                                 if( ptable_row_opt->curr_row > ptable_row_opt->row )
                                 {
                                       ptable_row_opt->read_db_once = 2; ///< 所有行都处理了
                                 }				 
			   }			   
                           #endif
						   
			   if( 1== ptable_row_opt->read_db_once ) ///< read_db_once=1表示已经读出数据库
			   {
			           sync_bcu_param.total_row = ptable_row_opt->row;
			           sync_bcu_param.curr_row = (ptable_row_opt->curr_row);
			           sync_bcu_param.p_table = ptable_row_opt->p_table;
	                           CcuSendDefaultLineTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param ); ///< 打包发送
			           debug_print( ("send defaultLine to Bcu<0701>:  total=%d, curr=%d, %d\n ", sync_bcu_param.total_row, sync_bcu_param.curr_row) );
				   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE ///< 这是没有表格行响应包的发送处理,目前没有使用
                                    ptable_row_opt->curr_row++; ///< send the next
                                    if( ptable_row_opt->curr_row > ptable_row_opt->row )
                                    {
                                          ptable_row_opt->read_db_once = 2;
                                    }
				   #endif
			   }
		    }
                    else
                    {
		            debug_print(("send defaultLine to Bcu: nothing,  %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));                      
                            ptable_row_opt->read_db_once = 2;
		    }			
		    if( ptable_row_opt->read_db_once==2 ) //< read_db_once=2表示该表格处理完毕
		    {
                           CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LINE_STATION_TABLE_SYNC); ///< 进入下一个表格
//		    	 CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_DEFAULT_LINE_TABLE_SYNC);
		    }
	     }
	     break; /* default line table, end */
		  
             case SUBEVENT_TYPE_LINE_STATION_TABLE_SYNC: ///< 发送线路区间表
	     {
		    debug_print(("line station table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if( 0 != StationCodeOfEntry(3, &ptable_row_opt->row, &ptable_row_opt->column, (void **)&ptable_row_opt->p_table) )
                           {
                                #ifndef  CONFIG_CCU_NO_DB_FILE
				    //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LINE_SECTION_TABLE_SYNC);
                                #else
				     ptable_row_opt->p_table = (void *)&sim_line3_station_code_table;
                                     ptable_row_opt->row = 3;
                                     ptable_row_opt->column = 3;
                                     ptable_row_opt->read_db_once = 1;
                                #endif
			   }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1; ///< 表格读取OK
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
				 CcuGdDisplayTableTStation(ptable_row_opt->row, ptable_row_opt->p_table);
				#endif
			   }
		           debug_print(("send L3 station to BCU:  row=%d, column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE ///< 等待表格行的响应包
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                      debug_print(("send L3 station to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));   
				 }
				 ptable_row_opt->curr_row++; ///< send the next
                                 if( ptable_row_opt->curr_row > ptable_row_opt->row )
                                 {
                                       ptable_row_opt->read_db_once = 2;
                                 }				 
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.line_no = 3;
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;			   
	                           CcuSendLineStationTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param ); ///< 打包发送
			           debug_print(("send L3 station to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));
				   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
                                    ptable_row_opt->curr_row++; ///< send the next
                                    if( ptable_row_opt->curr_row > ptable_row_opt->row )
                                    {
                                          ptable_row_opt->read_db_once = 2;
                                    }
				   #endif
			   }
		    }
		    else  ///< L3 has nothing, so read L4
		    {
		           debug_print(("send L3 station to BCU:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
	            }
			
		    if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==0 ) ///< 3号线的区间表处理完成,开始4号线的区间表
		    {
		         ptable_row_opt->read_db2_once = 1;
			 ptable_row_opt->row = 0;
                         ptable_row_opt->column = 0;
                         ptable_row_opt->curr_row = 1;
                         ptable_row_opt->repeat_times = 0;
							#ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
							sequeue_waiting_for_response = -1;
							#endif
                         if(0 != StationCodeOfEntry(4, &ptable_row_opt->row, &ptable_row_opt->column, (void **)&ptable_row_opt->p_table) )
                         {
                                  //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LINE_SECTION_TABLE_SYNC);
			 }
                         else
                         {
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT                         
                                 CcuGdDisplayTableTStation(ptable_row_opt->row, ptable_row_opt->p_table);
                                #endif				
			 }
		         debug_print(("send L4 station to BCU::  row=%d, column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==1 && ptable_row_opt->curr_row <= ptable_row_opt->row)
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                if( 3 == tmp_ret )
		                {
                                     debug_print(("send L4 station to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row)); 
				}  
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                 ptable_row_opt->read_db2_once = 2;
				   }
			   }				   
                           #endif	

			   if( 1==ptable_row_opt->read_db2_once )
			   {
			           sync_bcu_param.line_no = 4;
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;
	                           CcuSendLineStationTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send L4 station to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));
				  #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                 ptable_row_opt->read_db2_once = 2;
				   }
				   #endif
			   }
		    }
                    else if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==1 && 0==ptable_row_opt->row) ///< L4 has nothing, so next
                    {
		            debug_print(("send L4 station to BCU: nothing,  %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
			     ptable_row_opt->read_db2_once = 2;
		    }

		    if(  2==ptable_row_opt->read_db_once && 2==ptable_row_opt->read_db2_once )
		    {
                            CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LINE_SECTION_TABLE_SYNC);
		    }
	     }
	     break; /* line station code, end */

	     case SUBEVENT_TYPE_LINE_SECTION_TABLE_SYNC:
	     {
		    debug_print(("line section table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != LineSectionOfEntry(3, &ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                 //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_EMERG_TABLE_SYNC); ///< don't 
                           }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
				 CcuGdDisplayTableTLineSection(ptable_row_opt->row, ptable_row_opt->p_table);
                                #endif				
                           }
			   if( 3==ccu_gd_state_param->default_line_no )
			   {
                                   ccu_gd_state_param->default_section_no = JudgeDefaultSecion(ptable_row_opt->p_table, ptable_row_opt->row);
			   }
		           debug_print(("send Line3Setionto Bcu:  %d, %d, %d\n ", ptable_row_opt->row, ptable_row_opt->column, ccu_gd_state_param->default_section_no));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                     debug_print(("send Line3Setion to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row)); 
				 }  
				  ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.line_no = 3;
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;
	                           CcuSendLineSectionTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send Line3Setion to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
			           #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else ///< L3 section has nothing, so send L4 section
                    {
		           debug_print(("send Line3Setionto Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }
					
		    if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==0 )
		    {
		         ptable_row_opt->read_db2_once = 1;
			 ptable_row_opt->row = 0;
                         ptable_row_opt->column = 0;
                         ptable_row_opt->curr_row = 1;
                         ptable_row_opt->repeat_times = 0;
							#ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
                            sequeue_waiting_for_response = -1;
							#endif
                         if(0 != LineSectionOfEntry(4, &ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table) )
                         {
                               //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_EMERG_TABLE_SYNC);
			 }
			 else
			 {
                                ptable_row_opt->read_db2_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								
			        CcuGdDisplayTableTLineSection(ptable_row_opt->row, ptable_row_opt->p_table);		
				#endif
			 }
                         if( 4==ccu_gd_state_param->default_line_no )
                         {
                               ccu_gd_state_param->default_section_no = JudgeDefaultSecion(ptable_row_opt->p_table, ptable_row_opt->row);
                         }
		         debug_print(("send Line4Setionto Bcu:  %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1== tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                      debug_print(("send Line4Setion to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));  
				 } 
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
                                          ptable_row_opt->read_db2_once = 2;
				   }				 
			   }				   
                           #endif

			   if( 1==ptable_row_opt->read_db2_once )
			   {
			           sync_bcu_param.line_no = 4;
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendLineSectionTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send Line4Setion to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
                                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                ptable_row_opt->read_db2_once = 2;
				   }	
				   #endif
			   }					   
		    }
		    else if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==1 && 0==ptable_row_opt->row ) ///< L4 section has nothing, so next
		    {
		           debug_print(("send Line4Setionto Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db2_once = 2;
		    }
			
                    if( ptable_row_opt->read_db_once==2 && ptable_row_opt->read_db2_once==2 )
                    {
                           CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_EMERG_TABLE_SYNC);
                    }				
	     }
	     break; /* line section, end */

             case SUBEVENT_TYPE_EMERG_TABLE_SYNC:
	     {
		    debug_print(("emerg table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != EmergencyOfEntry(3, &ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table) )
                           {
                                  //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_IP_TABLE_SYNC);
			   }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
				 CcuGdDisplayTableEmergency(ptable_row_opt->row, ptable_row_opt->p_table);
                                #endif				
                           }
		           debug_print(("send EmergTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
					
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                       debug_print(("send EmergTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));
				 }   
				  ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif

			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendEmergTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send EmergTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
                                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE                                   
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send EmergTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }
					
		    if( ptable_row_opt->read_db_once==2 )
		    {
                          CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_IP_TABLE_SYNC);
		    }
	     }
	     break; /* emerg table, end */

             case SUBEVENT_TYPE_IP_TABLE_SYNC:
	     {
		    debug_print(("ip mac table -> BCU \n"));		 	
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != SqlOfTableTIpMac(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table) )
                           {
                                 //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_ANN_VERSION_TABLE_SYNC);
			   }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
				 CcuGdDisplayTableTIpMac(ptable_row_opt->row, ptable_row_opt->p_table);
                                #endif				
                           }
		           debug_print(("send IpTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                       debug_print(("send IpTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row)); 
				 }  
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif	

			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendIpTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send IpTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
                                    #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE                  
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send IpTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }			
		    if( ptable_row_opt->read_db_once==2 )
		    {
                          CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_ANN_VERSION_TABLE_SYNC);
		    }
	     }
	     break; /* ip table, end */

             case SUBEVENT_TYPE_ANN_VERSION_TABLE_SYNC:
	     {
		    debug_print(("ann version table -> BCU \n"));		 	
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != SqlOfTableTDatabaseVersion(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                 //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_CONFIG_VERSION_TABLE_SYNC);
			   }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
                                 CcuGdDisplayTableTDatabaseVersion(ptable_row_opt->row, ptable_row_opt->p_table);
				#endif
                           }						   
		           debug_print(("send AnnVersionTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                       debug_print(("send AnnVersionTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));   
		                 }
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendAnnVersionTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send AnnVersionTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));
		                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send AnnVersionTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }			
		    if( ptable_row_opt->read_db_once==2 )
		    {
                          CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_CONFIG_VERSION_TABLE_SYNC);
		    }
	     }
	     break; /* ann version table, end */

             case SUBEVENT_TYPE_CONFIG_VERSION_TABLE_SYNC:
	     {
		    debug_print(("config version table -> BCU \n"));		 	
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           ptable_row_opt->read_db_once = 1;
                           if(0 != SqlOfTConfigDBVersion(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table) )
                           {
                                 //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_PRIORITY_TABLE_SYNC);
			   }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
                                 CcuGdDisplayTableTConfigDBVersion(ptable_row_opt->row, ptable_row_opt->p_table);
                                #endif				
                           }						   
		           debug_print(("send ConfigVersionTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                        debug_print(("send ConfigVersionTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));
				 }   
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendConfigVersionTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send ConfigVersionTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));
		                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send ConfigVersionTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }				
		    if( ptable_row_opt->read_db_once==2 )
		    {
                           CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_PRIORITY_TABLE_SYNC);
		    }
	     }
	     break; /* config version table, end */	

             case SUBEVENT_TYPE_PRIORITY_TABLE_SYNC:
	     {
		    debug_print(("priority table -> BCU \n"));		 	
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != SqlOfTPriority(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                 //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_DEV_NUMBER_TABLE_SYNC);                           
                           }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1; 
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
                                 CcuGdDisplayTableTPriority(ptable_row_opt->row, ptable_row_opt->p_table);
				#endif
                           }
		           debug_print(("send PriorityTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                      debug_print(("send PriorityTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row)); 
				 }  
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			           sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendPriorityTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send PriorityTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
		                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send PriorityTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }				
		    if( ptable_row_opt->read_db_once==2 )
		    {
                           CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_DEV_NUMBER_TABLE_SYNC);
		    }
	     }
	     break; /* prioriry table, end */

             case SUBEVENT_TYPE_DEV_NUMBER_TABLE_SYNC:
	     {
		    debug_print(("dev number table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != SqlOfTableTDeviceNumber(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                  //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_RECORD_PATH_TABLE_SYNC);
                           }
                           else
                           {
                                 ptable_row_opt->read_db_once = 1;   
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT								 
                                 CcuGdDisplayTableTDeviceNumber(ptable_row_opt->row, ptable_row_opt->p_table);		
                                #endif				
                           }
		           debug_print(("send DevNumberTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                      debug_print(("send DevNumberTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));   
				 }
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			    	   sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;
	                           CcuSendDEvNumberTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send DevNumberTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
		                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send DevNumberTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }
		    if( ptable_row_opt->read_db_once==2 )
		    {
                           CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_RECORD_PATH_TABLE_SYNC);
		    }
	     }
	     break; /* dev number table, end */

             case SUBEVENT_TYPE_RECORD_PATH_TABLE_SYNC:
	     {
		    debug_print(("record path table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != SqlOfTableTFilePath(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                  //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LOG_PATH_TABLE_SYNC);
                           }
                           else
                           {
                                ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT
                                CcuGdDisplayTableTFilePath(ptable_row_opt->row, ptable_row_opt->p_table);
				#endif
                           }
		           debug_print(("send FilePathTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                      debug_print(("send FilePathTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));  
				 } 
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif	
			   if( 1==ptable_row_opt->read_db_once )
			   {
			    	   sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendRecPathTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send FilePathTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
		                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send FilePathTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }			
		    if( ptable_row_opt->read_db_once==2 )
		    {
                          //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_LOG_PATH_TABLE_SYNC);
                          CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC);
                          //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_STATION_BCU_VOLUME_SYNC);
                          //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_END_SYNC);						  
		    }
	     }
	     break; /* record path table, end */	

             case SUBEVENT_TYPE_LOG_PATH_TABLE_SYNC: ///< don't go here, 2013
	     {
		    debug_print(("log path table -> BCU \n"));
                    if( ptable_row_opt->read_db_once==0 )
                    {
                           if(0 != SqlOfTableTFilePath(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                 //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC);
                           }
                           else
                           {
                                ptable_row_opt->read_db_once = 1;
				#ifdef CONFIG_DISPLAY_TABLE_CONTEXT
                                CcuGdDisplayTableTFilePath(ptable_row_opt->row, ptable_row_opt->p_table);
				#endif
                           }
		           debug_print(("send LogPathTable to Bcu:  row=%d,column=%d\n ", ptable_row_opt->row, ptable_row_opt->column));
		    }
		    if( ptable_row_opt->read_db_once == 1 && ptable_row_opt->curr_row <= ptable_row_opt->row )
		    {
		           #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
		           int tmp_ret = -1;
                           tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
		           if( 0 == tmp_ret )
		           {
                                 return 0;
			   }
		           else if( 3 == tmp_ret ||1==tmp_ret )
		           {
		                 if( 3 == tmp_ret )
		                 {
                                       debug_print(("send LogPathTable to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));
				 }   
				 ptable_row_opt->curr_row++; ///< send the next
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }				 
			   }				   
                           #endif
			   if( 1==ptable_row_opt->read_db_once )
			   {
			    	   sync_bcu_param.total_row = ptable_row_opt->row;	
			           sync_bcu_param.curr_row = ptable_row_opt->curr_row;
			           sync_bcu_param.p_table = ptable_row_opt->p_table;		    
	                           CcuSendLogPathTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
			           debug_print(("send LogPathTable to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row));						   
		                   #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
	                           ptable_row_opt->curr_row++;
				   if( ptable_row_opt->curr_row > ptable_row_opt->row )
				   {
	                                  ptable_row_opt->read_db_once = 2;
				   }
				   #endif
			   }
		    }
                    else 
                    {
		           debug_print(("send LogPathTable to Bcu:  nothing, %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
                           ptable_row_opt->read_db_once = 2;
		    }				
		    if( ptable_row_opt->read_db_once==2 )
		    {
                           CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC);
		    }
	     }
	     break; /* log path table, end */

	      case SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC: ///< 发送开门侧数据.开门侧是封装为一个包,一起发出去
	      {
		    int net_cmd_tx_buffer = ccu_gd_state_param->send_net_cmd_buffer_id;
		    int src_dev_no = ccu_gd_state_param->plocal_dev_config_info->dev_no;
                    line_station_door_side_t *pline_door_side_array = ptable_row_opt->door_side_array; ///< med,1229
		    int retp = -1;

		    debug_print(("station door side -> BCU \n"));
                    
                    #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE ///< 等待响应
                       int tmp_ret = -1;
                       tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
                       if( 0 == tmp_ret )
                       {
                             return 0; ///< waiting
                       }
		       else if( 1==tmp_ret ||3==tmp_ret )
		       {
                                 if( 1==ptable_row_opt->read_db_once )
                                 {
                                        ptable_row_opt->read_db_once = 2;
                                 }
                                 else if( 1==ptable_row_opt->read_db2_once )
                                 {
                                        ptable_row_opt->read_db2_once = 2;
                                 }								 
		       }
		    #else
		        if( 1==ptable_row_opt->read_db_once )
		    	{
                               ptable_row_opt->read_db_once = 2;
		        }
		        if( 1==ptable_row_opt->read_db2_once )
		    	{
                               ptable_row_opt->read_db2_once = 2;
		        }				
                    #endif

		    if( 0==ptable_row_opt->read_db_once ) ///< 还没有读3号线开门侧,则读取
		    {
                          pline_door_side_array = (line_station_door_side_t *)CcuSendNewSectionDoorSideToBcu(3, 255, 
						  	                                           net_cmd_tx_buffer, src_dev_no );
			  ptable_row_opt->read_db_once = 1;
			  if( NULL==pline_door_side_array )
			  {
			        ptable_row_opt->read_db_once = 2;
			  }
			  ptable_row_opt->door_side_array = pline_door_side_array; ///< med, 1229
	            }

		    if( 1==ptable_row_opt->read_db_once ) ///< 已经读出3号线开门侧信息
		    {
		    	   ///< 打包发送
	                    retp = CcuSetupNetPackageToBcu(net_cmd_tx_buffer, SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC, 
			 	                                 (void *)pline_door_side_array, pline_door_side_array->total_bytes, src_dev_no);
			       if( retp==0 ) //package written into net-buffer,so enable alarm
			       {
			       	      ///< 开启定时器,等待响应
			              AlarmEnableWithNewTrigger(ccu_gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
			       }
	            }

		    if( 2==ptable_row_opt->read_db_once && 0==ptable_row_opt->read_db2_once  )
		    { ///< 开门侧发送OK,准备4号线开门侧
		            if( NULL != pline_door_side_array )
		            {
		                  free(pline_door_side_array); ///< release old, then malloc new
		            }
		            ///< 读出4号线开门侧
                            pline_door_side_array = (line_station_door_side_t *)CcuSendNewSectionDoorSideToBcu(4, 255, 
						  	                                           net_cmd_tx_buffer, src_dev_no );		
			   ptable_row_opt->read_db2_once = 1;
			   if( NULL==pline_door_side_array )
			   {
			         ptable_row_opt->read_db2_once = 2;
			   }
				  ptable_row_opt->door_side_array = pline_door_side_array; ///< med, 1229
		    }

		    if(  2==ptable_row_opt->read_db_once && 1==ptable_row_opt->read_db2_once )
		    {
		    	 ///< 发送4号线开门侧
	                    retp = CcuSetupNetPackageToBcu(net_cmd_tx_buffer, SUBEVENT_TYPE_STATION_DOOR_SIDE_SYNC, 
			 	                                 (void *)pline_door_side_array, pline_door_side_array->total_bytes, src_dev_no);
			       if( retp==0 ) //package written into net-buffer,so enable alarm
			       {
			              AlarmEnableWithNewTrigger(ccu_gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
			       }
		    }

		    if(  2==ptable_row_opt->read_db_once && 2==ptable_row_opt->read_db2_once )
		    {
		    	   ///< 3、4号线开门侧,都发送完成,释放数据库内存
		          SqlTableBufferFree((void **)&pline_door_side_array); ///< release door side memory
                          //CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_END_SYNC);
                          CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_STATION_BCU_VOLUME_SYNC);
		    }
	      }
	      break; /* door side, end */

	      case SUBEVENT_TYPE_STATION_BCU_VOLUME_SYNC: ///< 发送上次BCU保存的音量,2014
	      {
		    debug_print(("bcu volume for power up -> BCU \n"));
		    //diag_printf("bcu volume for power up -> BCU \n");
                    
                    #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
                       int tmp_ret = -1;
                       tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
                       if( 0 == tmp_ret )
                       {
                             return 0; ///< waiting
                       }
		       else if( 1==tmp_ret ||3==tmp_ret )
		       {
		                 if( 3 == tmp_ret )
		                 {
                                       debug_print(("send Pu Vol to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));
				 }
				 ptable_row_opt->curr_row++; ///< send the next
                                 if( ptable_row_opt->curr_row > ptable_row_opt->row )
                                 {
                                       ptable_row_opt->read_db_once = 2;
                                 }
		       }
		    #else
		        if( 1==ptable_row_opt->read_db_once )
		    	{
                               ptable_row_opt->read_db_once = 2;
		        }
                    #endif

		    if( 0==ptable_row_opt->read_db_once )
		    {
                           if(0 == CcuGetPowerUpVolumeTable(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                           {
                                 ptable_row_opt->read_db_once = 1;
                           }
                           else
                           {
                                  debug_print(("sync pu vol -> BCU:  read file fail \n"));
                           }
	            }

		    if( 1==ptable_row_opt->read_db_once )
		    {
                              sync_bcu_param.total_row = ptable_row_opt->row;	
                              sync_bcu_param.curr_row = ptable_row_opt->curr_row;
                              sync_bcu_param.p_table = ptable_row_opt->p_table;		    
                              CcuSendPuVolTableToBcu( (net_sync_bcu_param_t *) &sync_bcu_param );
                              //diag_printf("send Pu Vol to BCU<0701>:  total=%d, curr=%d \n ", sync_bcu_param.total_row, sync_bcu_param.curr_row);
                              #ifndef CONFIG_CCU_WAIT_SYNC_RESPONSE
                              ptable_row_opt->curr_row++;
                              if( ptable_row_opt->curr_row > ptable_row_opt->row )
                              {
                                    ptable_row_opt->read_db_once = 2;
                              }
                              #endif
	            }
		    else
                    {
		            debug_print(("send Pu Vol to BCU<0701>: nothing,  %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));                      
                            ptable_row_opt->read_db_once = 2;
		    }			

		    if( 2==ptable_row_opt->read_db_once )
		    {
                           SqlTableBufferFree(&ptable_row_opt->p_table);
                          CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_STATION_BCU_TS_PASSWORD);
		    }
	      }
	      break; /* bcu volume for power up, end */
	      case SUBEVENT_TYPE_STATION_BCU_TS_PASSWORD:
	      {//BCU 触摸屏密码记录
	    	  debug_print(("bcu ts password -> BCU \n"));
	    	  		    //diag_printf("bcu volume for power up -> BCU \n");

			#ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
			 int tmp_ret = -1;
			 tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
			if( 0 == tmp_ret )
			{
				return 0; ///< waiting
			}
			else if( 1==tmp_ret ||3==tmp_ret )
			{
				if( 3 == tmp_ret )
				{
					debug_print(("send Pu Vol to BCU<0701>:  curr=%d lost \n ", ptable_row_opt->curr_row));
				}
				ptable_row_opt->curr_row++; ///< send the next
				if( ptable_row_opt->curr_row > ptable_row_opt->row )
				{
					ptable_row_opt->read_db_once = 2;
				}
			}
			#else
				if( 1==ptable_row_opt->read_db_once )
				{
							   ptable_row_opt->read_db_once = 2;
				}
					#endif
			if( 0==ptable_row_opt->read_db_once )
			{//读数据库
                if(0 == CcuGetPowerUpTSPasswordTable(&ptable_row_opt->row, &ptable_row_opt->column, &ptable_row_opt->p_table))
                {
					ptable_row_opt->read_db_once = 1;
				}
				else
				{
					debug_print(("sync pu pswd -> BCU:  read file fail \n"));
				}
			}

			if( 1==ptable_row_opt->read_db_once )
			{//有多少条发送多少条
				sync_bcu_param.total_row = ptable_row_opt->row;
				sync_bcu_param.curr_row = ptable_row_opt->curr_row;
				sync_bcu_param.p_table = ptable_row_opt->p_table;
				CcuSendPuTSPasswordTableToBcu((net_sync_bcu_param_t *) &sync_bcu_param);
			}
			else
			{
				debug_print(("send Pu pswd to BCU<0701>: nothing,  %d, %d \n ", ptable_row_opt->row, ptable_row_opt->column));
				ptable_row_opt->read_db_once = 2;
			}

				if( 2==ptable_row_opt->read_db_once )
				{//数据已经发送完毕，定位下一个表格
					// SqlTableBufferFree(&ptable_row_opt->p_table);
					CcuSyncResetRowOption(ptable_row_opt, SUBEVENT_TYPE_END_SYNC);
				}
	      }
	      break;

	      case SUBEVENT_TYPE_END_SYNC: ///< 发送结束包
	      {
                    char tmp_array[GD_DEV_INFO_MAX_LEN];
		    int net_cmd_tx_buffer = ccu_gd_state_param->send_net_cmd_buffer_id;
		    int src_dev_no = ccu_gd_state_param->plocal_dev_config_info->dev_no;
		    int retp = -1;

		    debug_print(("sync end -> BCU, once=%d \n", ptable_row_opt->read_db_once));

                    #ifdef CONFIG_CCU_WAIT_SYNC_RESPONSE
                       int tmp_ret = -1;
                       tmp_ret = CcuGdSyncCheckResponse(ccu_gd_state_param);
                       if( 0 == tmp_ret )
                       {
                             return 0; ///< waiting
                       }
		       else if( 1==tmp_ret ||3==tmp_ret )
		       {
                                 if( 1==ptable_row_opt->read_db_once )
                                 {
                                        ptable_row_opt->read_db_once = 2;
                                 }
		       }
		    #else
       		        if( 1==ptable_row_opt->read_db_once )
       		    	{
                                      ptable_row_opt->read_db_once = 2;
       		        }
                    #endif

		    if( 0==ptable_row_opt->read_db_once )
		    {
                            ptable_row_opt->read_db_once = 1;
		    }

		    if( 1==ptable_row_opt->read_db_once )
		    {
	                    retp = CcuSetupNetPackageToBcu(net_cmd_tx_buffer, SUBEVENT_TYPE_END_SYNC,
			 	                                              (void *)tmp_array, sizeof(tmp_array), src_dev_no);
                            if( retp==0 ) //package written into net-buffer,so enable alarm
                            {
                                  AlarmEnableWithNewTrigger(ccu_gd_state_param->alarm_sync_id, 100, 100); ///< wait for 1 second
                            }
		    }
					
		    if( 2==ptable_row_opt->read_db_once )
		    {
                           SqlTableBufferFree(&ptable_row_opt->p_table); ///< release last malloc, 2014
                           CcuSyncResetRowOption(ptable_row_opt, 0);
                           ret = -1; ///< finish this state machine
		    }
	      }
	      break; /* all, end */		  

	     default :
	     break;		 
      }
	  
      return ret;
}
void  CcuGdSyncBepDbEnter(void *sync_info)
{
       int index = 0;
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       debug_print(("CcuGdSyncBepDbEnter: enter\n"));	   

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncBepDbEnter: null ptr\n"));
	     return ;
       }

       if( pccu_gd_state_param->plocal_dev_config_info->is_db_server || ///< we are server-CCU
	     (pccu_gd_state_param->other_ccu_offline==1 && 1 != pccu_gd_state_param->plocal_dev_config_info->is_db_server) ) ///< we are non-server-CCU,but server-CCU offline
       {
            common_big_package_t  temp_net_cmd = {"\0",0};
            pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(temp_net_cmd.common_big_data_u.pa_common_cmd_info);
            char *tmptr = (char *)(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context);
			
	    memset((void *)tmptr, 0, sizeof(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context));
		
	    strcpy(temp_net_cmd.src_dev_name, pccu_gd_state_param->plocal_dev_config_info->dev_name);
            temp_net_cmd.src_dev_number =  pccu_gd_state_param->plocal_dev_config_info->dev_no;
            temp_net_cmd.pkg_type =  COMMON_PACKAGE_TYPE_SYNC_GD;
            pcomm_cmd_info->event_type_common =  EVENT_TYPE_SYS_UPDATE_SYNC;
            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req = 1;
            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res = 0;
            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask = SUBEVENT_TYPE_DB_VERSION_SYNC_BEP;

            strcpy(tmptr, pccu_gd_state_param->plocal_dev_config_info->config_db_version_string);
            tmptr += sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string);
            strcpy(tmptr, pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string);
            tmptr += sizeof(pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string);
	    if( pccu_gd_state_param->plocal_dev_config_info->is_db_server )
	    {
	           int *tmp_int = NULL;
                   strcpy(tmptr, "SERVER");
                   tmptr += 7;
                   strcpy(tmptr, pccu_gd_state_param->plocal_dev_config_info->dev_name); ///< new server name
                   tmptr += sizeof(pccu_gd_state_param->plocal_dev_config_info->dev_name);
		   tmp_int = (int *)tmptr;
		   (*tmp_int) = pccu_gd_state_param->plocal_dev_config_info->dev_no;
                   tmptr += 4;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context_len = 
					sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) +
					sizeof(pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string) + 
					7 + 
					 sizeof(pccu_gd_state_param->plocal_dev_config_info->dev_name) +
					 4;
	    }	    		
            else
            {
                   strcpy(tmptr, "000000");
                   tmptr += 7;
	            pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_context_len = 
					sizeof(pccu_gd_state_param->plocal_dev_config_info->config_db_version_string) +
					sizeof(pccu_gd_state_param->plocal_dev_config_info->ann_db_version_string) + 
					7;
	    }

	    strcpy(pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_source_ip, pccu_gd_state_param->plocal_dev_config_info->dev_ip);

           unsigned char broad_time = 0;
           for(broad_time=0; broad_time<3; broad_time++)
	    {
	           strcpy(temp_net_cmd.dst_dev_name, "EAMP");
	           temp_net_cmd.dst_dev_number =  MUL_SYNC_DST_NO;
		    if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &temp_net_cmd, sizeof(temp_net_cmd)) < 0 )
		    {
	                   debug_print(("CcuGdSyncBepDbEnter, to all EAMP: write cmd buffer error\n"));
	           }
	           else
	           {
	                  debug_print(("CcuGdSyncBepDbEnter, to all EAMP: write cmd buffer ok\n"));
	           }
		    cyg_thread_delay(50); ///< delay 0.5 second or more		   
			
		    strcpy(temp_net_cmd.dst_dev_name, "PCU");
	           temp_net_cmd.dst_dev_number =  MUL_SYNC_DST_NO;		
		    if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &temp_net_cmd, sizeof(temp_net_cmd)) < 0 )
		    {
	                   debug_print(("CcuGdSyncBepDbEnter, to all PCU: write cmd buffer error\n"));
	            }
	           else
	           {
	                  debug_print(("CcuGdSyncBepDbEnter, to all PCU: write cmd buffer ok\n"));
	           }
		    cyg_thread_delay(50); ///< delay 0.5 second or more			   

		    strcpy(temp_net_cmd.dst_dev_name, "BCU");
	           temp_net_cmd.dst_dev_number =  MUL_SYNC_DST_NO;
		    if ( BlockBufferWrite(pccu_gd_state_param->send_net_cmd_buffer_id, &temp_net_cmd, sizeof(temp_net_cmd)) < 0 )
		    {
	                   debug_print(("CcuGdSyncBepDbEnter, to all BCU: write cmd buffer error\n"));
	           }
	           else
	           {
	                  debug_print(("CcuGdSyncBepDbEnter, to all BCU: write cmd buffer ok\n"));
	           }
		    cyg_thread_delay(50); ///< delay 0.5 second or more			   
		}
		   
           //pccu_gd_state_param->enable_sync_bcu = 1; ///< debug
      }	   

      for(index=1; index<sizeof(pccu_gd_state_param->bcu); index++)
      {
             pccu_gd_state_param->bcu[index] = 0; ///< reset
      }	  
      dest_dev_no_for_table_sync = 0; ///< reset  
      pccu_gd_state_param->soft_count = 0;
      AlarmDisable(pccu_gd_state_param->alarm_sync_id); ///< reset
      pccu_gd_state_param->plocal_dev_config_info->is_db_server = 0; ///< from now on, all CCUs are not server

      return ;
}
int CcuGdCheckAllBcuUpdateStatus3(unsigned char bcu[], int bcu_nums)
{
           int i = 0, ret = 0;
		   
	    if( NULL ==bcu )
		return -1;
		
	    for(i=0; i<bcu_nums; i++)
	    {
                if( bcu[i] >= 3 ) 
		  {		   
		       ret = 1;
			break;
                }
	    }

	    return ret;
}
int CcuGdCheckAllBcuUpdateStatus2(unsigned char bcu[], int bcu_nums)
{
           int i = 0, ret = 0;
		   
	    if( NULL ==bcu )
		return -1;
		
	    for(i=0; i<bcu_nums; i++)
	    {
                if ( bcu[i] >= 1 && bcu[i] < 3 )
		  {
		       ret = 2;
			break;
		  }
	    }

	    return ret;
}
int CcuGdResetBcuUpdateStatus(unsigned char bcu[], int bcu_nums)
{
           int i = 0, ret = 0;
		   
	    if( NULL ==bcu )
		return -1;
		
	    for(i=0; i<bcu_nums; i++)
	    {
	         if ( 1<= bcu[i] && bcu[i]<3 )
	         {
                     bcu[i] = 0;
	         }
	    }

	    return ret;
}
void  CcuGdSyncBepDbProcess(void *sync_info) ///< BCU,EAMP,PCU数据库更新阶段,2014-09-20正在使用
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;
       pa_common_information_t *pcomm_cmd_info = (pa_common_information_t *)&(pccu_gd_state_param->precv_common_net_cmd->common_big_data_u.pa_common_cmd_info);	   
       unsigned char sync_finish = 0;
       char index = 0;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncBepDbProcess: null ptr\n"));
	     return ;
       } 

       ///< 本阶段接收、解析命令
       if(BlockBufferRead( pccu_gd_state_param->recv_net_cmd_buffer_id, pccu_gd_state_param->precv_common_net_cmd, 
	   	                           sizeof(common_big_package_t)) > 0)
       {
               //CcuDisplayRecvSyncNetPackage(pccu_gd_state_param->precv_common_net_cmd);
	       //if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask == SUBEVENT_TYPE_DB_VERSION_SYNC_CCU )
               if ( pcomm_cmd_info->event_type_common==EVENT_TYPE_SYS_UPDATE_SYNC )
	       { ///< response all Req package
	               if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
		           pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
		       { ///< 响应所有请求包
	                      CcuGdSendLocalDbVersion(pccu_gd_state_param);
		       }
	       }
			   
               if ( pcomm_cmd_info->event_type_common==EVENT_TYPE_SYS_UPDATE_SYNC )
               {
                       if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask == SUBEVENT_TYPE_DB_VERSION_SYNC_BEP )
                       { ///< 本段已经没有实际效果,仅仅保留而已(因为前面已经应答所有请求包)
                                if ( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
                                      pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
                                {
                                        if ( pccu_gd_state_param->plocal_dev_config_info->is_db_server )
                                        {
                                               debug_print(("CcuGdSyncBepDbProcess:  from %s-%d\n", 
                                                                       pccu_gd_state_param->precv_common_net_cmd->src_dev_name, 
                                                                       pccu_gd_state_param->precv_common_net_cmd->src_dev_number));
					       #if 0 ///< 2014-06-23,不再使用这里
					       if( 0 != strcmp(pccu_gd_state_param->precv_common_net_cmd->src_dev_name, "BCU") )
                                               {
                                                      CcuGdSendLocalDbVersion(pccu_gd_state_param);
					       }
					       else
					       {
                                                       //pccu_gd_state_param->enable_sync_bcu = 1;
						       debug_print(("CcuSyncBepProcess: ready to send tables to BCU\n"));
                                                       CcuGdSendLocalDbVersion(pccu_gd_state_param);
					       }
					       #endif
					}
					else
					{
					}
				}                                
		       }
		       else if( (pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask >= SUBEVENT_TYPE_LINE_STATION_TABLE_SYNC &&
			   	    pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask <= SUBEVENT_TYPE_STATION_BCU_TS_PASSWORD)  ||
			   	    pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask == SUBEVENT_TYPE_END_SYNC )
		       { ///< 子类型为表格包或结束包
                               if ( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
                                     pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==1 )
                                { ///< 如果是响应包
                                       if( pccu_gd_state_param->plocal_dev_config_info->is_db_server && 1==pccu_gd_state_param->enable_sync_bcu )
                                       { ///< 主服务器正在更新BCU
	                                       if( pcomm_cmd_info->sequence == sequeue_waiting_for_response )
	                                       { ///< 接收的响应包,是表格数据的确认
	                                              if( 0==AlarmCheckTimeOut(pccu_gd_state_param->alarm_sync_id) )
	                                              { ///< 在规定时间内,收到该响应包,不再重发该表格数据
	                                                    AlarmDisable(pccu_gd_state_param->alarm_sync_id);
	                                                    sequeue_waiting_for_response = 0;
	                                                    debug_print(("CcuSyncBepProcess:  response from CCU in time: seq=%d \n", pcomm_cmd_info->sequence));	
	                                              }
					       }
				       }
				}
		       }
		       else if( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_mask == 
			   	     SUBEVENT_TYPE_READY_SYNC_REQ ) ///< 2014-06-23: ready-sync-req is p2p
		       { ///< 接收的是点对点的READY_SYNC请求包
                                if ( pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_req==1 && 
                                      pcomm_cmd_info->event_info_common.gd_sync_cmd_info.gd_sync_res==0 )
                                {
                                      //if( pccu_gd_state_param->plocal_dev_config_info->is_db_server )
                                      {
                                          debug_print(("CcuSyncBepProcess, %s_%d: receive %s_%d Ready Req \n",
									                         pccu_gd_state_param->plocal_dev_config_info->dev_name,
									                         pccu_gd_state_param->plocal_dev_config_info->dev_no,
                                                          pccu_gd_state_param->precv_common_net_cmd->src_dev_name,
                                                          pccu_gd_state_param->precv_common_net_cmd->src_dev_number ));
                                                if( 0==strcmp(pccu_gd_state_param->precv_common_net_cmd->src_dev_name,"BCU") )
                                                { 
                                                           #if 0
                                                           if( pccu_gd_state_param->precv_common_net_cmd->src_dev_number==1)
                                                           {
                                                                  pccu_gd_state_param->bcu[1] += 1; ///< 统计BCU1的请求次数
                                                           }
                                                           else if( pccu_gd_state_param->precv_common_net_cmd->src_dev_number==2)
                                                           {
                                                                  pccu_gd_state_param->bcu[2] += 1; ///< 统计BCU2的请求次数
                                                           }
								   #endif
                                                           if( pccu_gd_state_param->precv_common_net_cmd->src_dev_number >0 && 
									pccu_gd_state_param->precv_common_net_cmd->src_dev_number < sizeof(pccu_gd_state_param->bcu) )
                                                           {
                                                                  pccu_gd_state_param->bcu[pccu_gd_state_param->precv_common_net_cmd->src_dev_number] += 1;
                                                           }
                                                }
				      }                                      
			        }
		       }		       	
	       }
       }

	if( 1==CcuGdCheckAllBcuUpdateStatus3(pccu_gd_state_param->bcu, sizeof(pccu_gd_state_param->bcu))  )
	{ 
		///< 请求次数超过3次,设置本地CCU为主服务器.
		///< 注意: 由于BCU发READY_SYNC请求包,是一个点对点的包,因此不存在2个CCU同时是主服务器情况
		if( 0==pccu_gd_state_param->enable_sync_bcu )
		{ ///< current CCU is server
		       pccu_gd_state_param->plocal_dev_config_info->is_db_server = 1;
		}
	}
	else if(  2==CcuGdCheckAllBcuUpdateStatus2(pccu_gd_state_param->bcu, sizeof(pccu_gd_state_param->bcu))  )
	{ 
		           ///< 请求次数不足3次,但因为网络环境等因素,始终收不到BCU请求包,则CCU必复位,重新统计,避免累积造成误发送表格
               ///< 循环等待5000次(相当于软延时),自动清零BCU请求次数,此时BCU无法及时等到表格数据,将会超时,重新发送请求
               ///< 这种处理方式,虽然耗时,但可以保证有限的网络条件下,BCU、CCU总会达到步调一致,保持有效的更新
               if( pccu_gd_state_param->soft_count++ >= 5000 ) 
               {
//	              diag_printf("Ccu, Gd thread:  too short Ready recved, [1]=%d,[2]=%d \n",
//				  	pccu_gd_state_param->bcu[1],
//				  	pccu_gd_state_param->bcu[2]);
                      CcuGdResetBcuUpdateStatus(pccu_gd_state_param->bcu, sizeof(pccu_gd_state_param->bcu));			 
                      pccu_gd_state_param->soft_count = 0;
	       }
	}
       //if( pccu_gd_state_param->enable_sync_bcu==0 )
       if( pccu_gd_state_param->enable_sync_bcu==0 && pccu_gd_state_param->plocal_dev_config_info->is_db_server )
       { ///< 如果还没有开始更新BCU,但已经确认了主服务器表示
             for(index=1; index<sizeof(pccu_gd_state_param->bcu); index++)
             {
                   if( pccu_gd_state_param->bcu[index] >= 3 ) ///< BCU-index
                   {
			 dest_dev_no_for_table_sync = index; ///< 确定目标BCU号,点对点发包
                         pccu_gd_state_param->enable_sync_bcu = 1; ///< <1>表示开始更新BCU
			 AlarmDisable(pccu_gd_state_param->alarm_sync_id); ///< 关闭定时器,当每次发送一个表格数据时,再开启它
                         CcuSyncResetRowOption(pccu_gd_state_param->ptable_row_opt, 0); ///< 复位表格行信息
			 sequeue_waiting_for_response = 0; //< 复位发送包号,用来记录每次发送包的包号
			 debug_print(("Ccu, Gd thread:  begin to send table --> BCU-%d \n", dest_dev_no_for_table_sync));
			 break;
		   }
	     }
       }

       if( pccu_gd_state_param->enable_sync_bcu && pccu_gd_state_param->plocal_dev_config_info->is_db_server )
       {
            ///< 调用更新主入口,本质是switch控制的状态
            if ( 0 != CcuSyncDbToBcuSMachineEntry(pccu_gd_state_param) ) 
            {
            	     ///< 至此,更新完一个BCU,复位参数
                   pccu_gd_state_param->enable_sync_bcu = 0; ///< 禁止更新BCU
                   pccu_gd_state_param->bcu[dest_dev_no_for_table_sync] = 0; ///< BCU请求记录清零
                   pccu_gd_state_param->plocal_dev_config_info->is_db_server = 0; ///< 本地CCU不再是主服务器
		   dest_dev_no_for_table_sync = 0; ///< 清零目标BCU号
                   //sync_finish = 1;
            }
       }

       if( sync_finish )
       {
       	  ///< sync_finish=1表示BCU,EAMP,PCU都更新完成,可以进入下一个阶段,但目前始终保持在BEP阶段
                   debug_print(("CcuSyncBepProcess: sync finished, go to next step \n"));
                   CcuGdSyncStateSwitch( (gd_state_process_t **)&(pccu_gd_state_param->plocal_curr_gd_state), 
                           GD_SYNC_STATE_TRIGGER_CCU_DEV_SYNC, (void *)pccu_gd_state_param);
       }
	   
       return ;
}
void  CcuGdSyncBepDbExit(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncBepDbExit: null ptr\n"));
	     return ;
       }

       debug_print(("CcuGdSyncBepDbExit: exit\n"));

       CcuDisplaySendSyncNetPackage();      

       return ;
}
#endif

void  CcuGdSyncCcuDevEnter(void *sync_info) ///< 设备信息更新阶段,目前暂时没有使用,保留
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncCcuDevEnter: null ptr\n"));
	     return ;
       }

       debug_print(("CcuGdSyncCcuDevEnter\n"));

       return ;
}
void  CcuGdSyncCcuDevProcess(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncCcuDevProcess: null ptr\n"));
	     return ;
       }

       return ;
}
void  CcuGdSyncCcuDevExit(void *sync_info)
{
       gd_state_param_t  *pccu_gd_state_param = (gd_state_param_t  *)sync_info;

       if( NULL == pccu_gd_state_param )
       {
             debug_print(("CcuGdSyncCcuDevExit: null ptr\n"));
	     return ;
       }

       return ;
}

///< CCU数据库更新,状态结构体, 2014-09-20注
static gd_state_process_t  ccu_gd_sync_idle = {
      CcuGdSyncIdleEnter,
      CcuGdSyncIdleProcess,
      CcuGdSyncIdleExit,
      GD_SYNC_STATE_IDLE
};
static gd_state_process_t  ccu_gd_sync_ccu_db = {
      CcuGdSyncCcuDbEnter,
      CcuGdSyncCcuDbProcess,
      CcuGdSyncCcuDbExit,
      GD_SYNC_STATE_CCU_DB_SYNC
};
static gd_state_process_t  ccu_gd_sync_bep_db = {
      CcuGdSyncBepDbEnter,
      CcuGdSyncBepDbProcess,
      CcuGdSyncBepDbExit,
      GD_SYNC_STATE_BEP_DB_SYNC
};
static gd_state_process_t  ccu_gd_sync_ccu_dev = {
      CcuGdSyncCcuDevEnter,
      CcuGdSyncCcuDevProcess,
      CcuGdSyncCcuDevExit,
      GD_SYNC_STATE_CCU_DEV_SYNC
};

static gd_state_rule_table_t  ccu_gd_rule_table[] = {
{&ccu_gd_sync_idle, GD_SYNC_STATE_TRIGGER_PU_READY, &ccu_gd_sync_ccu_db},
{&ccu_gd_sync_ccu_db, GD_SYNC_STATE_TRIGGER_BEP_DB_SYNC, &ccu_gd_sync_bep_db},
{&ccu_gd_sync_bep_db, GD_SYNC_STATE_TRIGGER_CCU_DEV_SYNC, &ccu_gd_sync_ccu_dev},
{&ccu_gd_sync_ccu_dev, GD_SYNC_STATE_TRIGGER_IDLE, &ccu_gd_sync_idle}
};
static unsigned int ccu_gd_rule_table_items = sizeof(ccu_gd_rule_table)/sizeof(ccu_gd_rule_table[0]);
//static global_device_info_state_t  *ccu_gd_table =NULL;

int CcuGdSyncStateSwitchForce(gd_state_process_t **gd_src_state_proc, gd_state_process_t *gd_dest_state_proc, void *sync_info)
{
        int active_state_id = 0;
        int force_state_id = 0;	  
        gd_state_process_t *in_active_state = NULL;	
        gd_state_process_t *in_dest_state = NULL;
        if( NULL==gd_src_state_proc || NULL==(*gd_src_state_proc) )
        {
               debug_print(("CcuGdSyncStateSwitchForce:  src, null ptr\n"));
               return -1;
        }
        if( NULL==gd_dest_state_proc )
        {
               debug_print(("CcuGdSyncStateSwitchForce:  dest, null ptr\n"));
               return -1;
        }	  	
        in_active_state = (*gd_src_state_proc);
        in_dest_state = (gd_dest_state_proc);
	active_state_id =  in_active_state->state_id;
	force_state_id = in_dest_state->state_id;
	if (active_state_id != force_state_id )
	{
		in_active_state->exit((void *)sync_info);
		in_dest_state->enter((void *)sync_info);
		(*gd_src_state_proc) = in_dest_state; //update active pointer
		debug_print(("CcuGdSyncStateSwitchForce:  force state %d ---> state %d\n", active_state_id, force_state_id ));
	}	  

      return 0;
}

int CcuGdSyncStateSwitch(gd_state_process_t **gd_src_state_proc, int trigger_signal, void *sync_info)
{
      int i = 0;
//      gd_state_process_t *current_gd_state_proc = NULL;

      if( NULL==gd_src_state_proc || NULL==(*gd_src_state_proc) )
      {
             debug_print(("CcuGdSyncStateSwitch:  null ptr\n"));
             return -1;
      }
	  
      debug_print(("CcuGdSyncStateSwitch:  rules=%d\n", ccu_gd_rule_table_items));
      
      for(i=0; i<ccu_gd_rule_table_items; i++)
      {
             if( (*gd_src_state_proc)->state_id==ccu_gd_rule_table[i].psrc_gd_state->state_id &&
		  trigger_signal==ccu_gd_rule_table[i].trigger_event )
             {
		     if( NULL != ccu_gd_rule_table[i].pdest_gd_state )
		     {
                           debug_print(("CcuGdSyncStateSwitch:  rule found,%d, %d-->%d\n", trigger_signal, (*gd_src_state_proc)->state_id, 
      					 	 ccu_gd_rule_table[i].pdest_gd_state->state_id));
		           (*gd_src_state_proc)->exit(sync_info);
			   (*gd_src_state_proc) = ccu_gd_rule_table[i].pdest_gd_state;
		           (*gd_src_state_proc)->enter(sync_info);
		     }
                    else
                    {
                           debug_print(("CcuGdSyncStateSwitch:  rule found but dest has null pointer\n"));
		    }
		    break;
	     }
      }

      if( i >= ccu_gd_rule_table_items )
      {
            debug_print(("CcuGdSyncStateSwitch:  no rule\n"));
	    return -1;
      }

      return 0;
}

///< CCU数据库更新线程入口,2014-09-15注
void *CcuSyncThreadEntry(void *arg)
{
        dev_config_table_item_t  ccu_config_info = {0}; ///< 保存本地设备信息
        //network_send_package_t net_recv_cmd = {"\0",0};
        common_big_package_t common_net_recv_cmd = {"\0",0}; ///< 保存接收的命令包
	db_table_row_sync_opt_t  table_row_opt = {0,1}; ///< 保存表格数据行号等信息
        gd_state_param_t ccu_gd_state_param = {0}; ///< 传递给gd状态机的入口参数
	char *dev_info = NULL;
        int gd_net_cmd_rx_buffer_id = 0; ///< 接收命令缓冲区ID
        int gd_net_cmd_tx_buffer_id = 0; ///< 发送命令缓冲区ID
        int alarm_id = 0;
        int loop_time = 0;
        int alarm_id_tftp_client = 0;	
        unsigned char server_flag_from_iptable = 0;

        memset((void *)&ccu_config_info, 0, sizeof(ccu_config_info));	
        gd_net_cmd_rx_buffer_id = BlockBufferOpen("ccu-gd-net-cmd-Rx");
        ZhwBufferRegister(BUFFER_TYPE_COMMON_BIG, COMMON_PACKAGE_TYPE_SYNC_GD, BUFFER_TYPE_RECV_FROM_NET, gd_net_cmd_rx_buffer_id);
        debug_print(("Ccu Sync thread:  comm Rx buff=%d\n",gd_net_cmd_rx_buffer_id));

         //gd_net_cmd_tx_buffer_id = BlockBufferCreate(5, sizeof(common_big_package_t), "ccu-gd-net-cmd-Tx");
         //ZhwBufferInit(2, 2, sizeof(common_big_package_t), "ccu-gd-net-cmd-Tx");
         gd_net_cmd_tx_buffer_id = BlockBufferOpen( "network-udp-common-big-send" );

        CcuGdGetLocalDevIO((int *)&ccu_config_info.dev_type, (int *)&ccu_config_info.dev_no);
	 if( ccu_config_info.dev_no == 0)
	 {
             ccu_config_info.dev_no = 2;
	 }
        CcuGdLocalDevConfigInit((void *)&ccu_config_info); ///< 默认先读数据库
        server_flag_from_iptable = zhw_dev_weather_server(20); ///< add, 1226
        ccu_config_info.server_flag_from_iptable = server_flag_from_iptable; ///< 保存主次CCU标志

        debug_print(("Ccu Sync thread:  before for(), local-ip=%s \n", ccu_config_info.dev_ip));
        if( '\0'==ccu_config_info.dev_ip[0] ) ///< 如果获取IP失败,读本地IP数组
        {
                struct global_info local_dev_info;
                struct global_info back_ccu_dev_info;
                struct global_info main_ccu_dev_info;
		unsigned char loop_v =0 ;
		unsigned char loop_done = 0;
                local_dev_info = ReadGlobalArray(ccu_config_info.dev_type, NULL, ccu_config_info.dev_no, 1);
		strcpy(ccu_config_info.dev_ip, local_dev_info.dev_ip);
		strcpy(ccu_config_info.dev_name, local_dev_info.dev_name);		
                debug_print(("Ccu Sync thread:  reset local-ip=%s from ip-array \n", ccu_config_info.dev_ip));
                debug_print(("Ccu Sync thread:  reset local-dev-name=%s from ip-array \n", ccu_config_info.dev_name));
		memset((void *)&local_dev_info, 0, sizeof(local_dev_info));
                for(loop_v=0; loop_v<255; loop_v++)
                {
                      local_dev_info = ReadGlobalArray(ccu_config_info.dev_type, NULL, ccu_config_info.dev_no, 1);
		      if( 1==local_dev_info.is_server )
		      {
                            main_ccu_dev_info = local_dev_info;
			    back_ccu_dev_info = ResolveUseServerNumber(2); ///< 读次CCU信息
			    loop_done++;
                            break;
		      }
		      else if( 2==local_dev_info.is_server )
		      {
                            back_ccu_dev_info = local_dev_info;
			    main_ccu_dev_info = ResolveUseServerNumber(1); ///< 读主CCU信息
			    loop_done++;
                            break;
		      }
		}
	        if( 0==strcmp(main_ccu_dev_info.dev_ip, ccu_config_info.dev_ip) ) ///< we are server-CCU, so other CCU is my server
	        { ///< 保存2个CCU信息
                      strcpy(ccu_config_info.db_server_dev_name, back_ccu_dev_info.dev_name);
                      strcpy(ccu_config_info.db_server_ip, back_ccu_dev_info.dev_ip);
                      ccu_config_info.db_server_dev_no = back_ccu_dev_info.dev_number;
                      debug_print(("Ccu Sync thread:  other-CCU=%s_%d, ip=%s from ip-array \n", ccu_config_info.db_server_dev_name,
					  	ccu_config_info.db_server_dev_no, ccu_config_info.db_server_ip));	
	        }
	        if( 0==strcmp(back_ccu_dev_info.dev_ip, ccu_config_info.dev_ip) ) ///< we are non-server-CCU, so other CCU is my server
	        {
                      strcpy(ccu_config_info.db_server_dev_name, main_ccu_dev_info.dev_name);
                      strcpy(ccu_config_info.db_server_ip, main_ccu_dev_info.dev_ip);
                      ccu_config_info.db_server_dev_no = main_ccu_dev_info.dev_number;
                      debug_print(("Ccu Sync thread:  other-CCU=%s_%d, ip=%s from ip-array \n", ccu_config_info.db_server_dev_name,
					  	ccu_config_info.db_server_dev_no, ccu_config_info.db_server_ip));	
	        }
	}

	if( 1==server_flag_from_iptable )
	{
               ccu_config_info.is_db_server = 1;
               ccu_config_info.is_main_ccu = 1;
	}
	else if( 2==server_flag_from_iptable )
	{
               ccu_config_info.is_db_server = 0;
               ccu_config_info.is_main_ccu = 0;
	}
	else
	{
               ccu_config_info.is_db_server = 1;
               ccu_config_info.is_main_ccu = 1;
               debug_print(("Ccu Sync thread:  before for(), here's wrong \n"));
	}

        #if 0	 ///< just debug
        if(ccu_config_info.is_db_server)
        {
	      strcpy(ccu_config_info.db_server_dev_name, "CCU"); ///< now, db_server_dev_name is non-server CCU
              ccu_config_info.db_server_dev_no = 2;
              strcpy(ccu_config_info.db_server_ip, "192.168.7.14");
	      strcpy(ccu_config_info.dev_name, "CCU");
              ccu_config_info.dev_no = 1;
              strcpy(ccu_config_info.dev_ip, "192.168.7.13");
        }
        else
        {
	      strcpy(ccu_config_info.db_server_dev_name, "CCU");
              ccu_config_info.db_server_dev_no = 1;
              strcpy(ccu_config_info.db_server_ip, "192.168.7.13");
	      strcpy(ccu_config_info.dev_name, "CCU");
              ccu_config_info.dev_no = 2;
              strcpy(ccu_config_info.dev_ip, "192.168.7.14");
        }   		
	#endif
	
        //CcuAnnTableInit(); ///<default line, default section, ann-table	
        //CcuTestDbReadTable();
        ccu_gd_state_param.plocal_curr_gd_state = &ccu_gd_sync_idle; ///< 填充状态入口参数
        ccu_gd_state_param.plocal_dev_config_info = (dev_config_table_item_t *)&ccu_config_info;
        //ccu_gd_state_param.precv_net_cmd = (network_send_package_t *)&net_recv_cmd;
        ccu_gd_state_param.precv_common_net_cmd = (common_big_package_t *)&common_net_recv_cmd;
	ccu_gd_state_param.ptable_row_opt = (db_table_row_sync_opt_t *)&table_row_opt;
        ccu_gd_state_param.recv_net_cmd_buffer_id = gd_net_cmd_rx_buffer_id;
        ccu_gd_state_param.send_net_cmd_buffer_id = gd_net_cmd_tx_buffer_id;	
        CcuGdSaveConfigInfoPtr((void *)&ccu_config_info);
        diag_printf("CcuGd<2016>: server=%s_%d_%s, other=%s_%d_%s\n", 
			ccu_config_info.db_server_dev_name, ccu_config_info.db_server_dev_no, 
			ccu_config_info.db_server_ip, 
			ccu_config_info.dev_name, ccu_config_info.dev_no, ccu_config_info.dev_ip);
        if(ccu_config_info.is_db_server)
        {
	      alarm_id = AlarmCreate("ccu-sync-wait-alarm", 300, 300, CcuAlarmSyncWaiting );
        }
        else
        {
	      alarm_id = AlarmCreate("ccu-sync-wait-alarm", 300, 300, CcuAlarmSyncWaiting );
        }

        ccu_gd_state_param.alarm_sync_id  = alarm_id;
	AlarmEnable(alarm_id);
	//AlarmDisable(alarm_id);	
	
        //alarm_id_tftp_client = AlarmCreate("ccu-tftp-client-alarm", 9000, 9000, CcuTftpClientAlarmWaiting );
	//AlarmEnable(alarm_id_tftp_client);
        //debug_print(("Ccu Sync thread: 12, %d, %d, %d\n", (unsigned int)(ccu_gd_state_param.plocal_curr_gd_state->process), (unsigned int)(*(ccu_gd_state_param.plocal_curr_gd_state->process)), (unsigned int)CcuGdSyncIdleProcess));	

	unsigned char read_ip_table = 0;

//        CcuCreateBcuVolFile(CONFIG_BCU_VOL_FILE);
		
        for(;;)
        {

		if(loop_time++ >= 1000)
		{
			//debug_print(("Ccu Sync thread:  running\n" ));
			debug_print(("<0701>Ccu Sync thread:  running, ann_v=%s, config_v=%s, %d \n",
					ccu_gd_state_param.plocal_dev_config_info->ann_db_version_string,
			        ccu_gd_state_param.plocal_dev_config_info->config_db_version_string,
			        sizeof(ccu_gd_state_param.bcu) ));
			loop_time = 0;
		}

		if(0==read_ip_table)
		{
                       //CcuTestDbReadTable();
                       read_ip_table = 1;
		       ///< tell Tftpd thread
		       CcuGdHasReadIpTable(); ///< 通知Tftpd线程,启动Tftp服务器
		}
//		diag_printf("GetServerCcuNetworkState = %d\n",GetServerCcuNetworkState());

#if  0 ///< test tftp client
              if( 1==AlarmCheckTimeOut(alarm_id_tftp_client) ) 
              {
                    debug_print(("Ccu Sync thread,tftp client:  timeout again\n"));
	            AlarmDisable(alarm_id_tftp_client);
		    DownloadFileFromTFTP_test("/a", "192.168.7.13", "/b");
                    //CcuGdSendSyncPackageFixed(1, &ccu_gd_state_param);
	      }
#endif	

#if  0 ///< debug, ccu ---> bcu, 2013
              if( 1==AlarmCheckTimeOut(alarm_id) )
              {
	            AlarmDisable(alarm_id);              
                    debug_print(("Ccu Sync thread,:  ready to send table to BCU \n"));
                    ccu_gd_state_param.enable_sync_bcu = 1;
	      }
#endif

//              diag_printf("server_flag_from_iptable = %d",server_flag_from_iptable);
//              diag_printf(" %d,%d,%d\n",CcuGdDetectServerCcuNetState(server_flag_from_iptable),
//            		  ccu_config_info.is_db_server,
//            		  ccu_gd_state_param.enable_sync_bcu);
#if 0 ///< just for OCC,不再使用
        if( 1==CcuGdDetectServerCcuNetState(server_flag_from_iptable) )
        {
                if( ccu_config_info.is_main_ccu==0 && server_flag_from_iptable==2 )
                {
                      ccu_config_info.is_main_ccu = 1;
		      CcuUpdateMainCcuFlag(1);
		}
	}
        else if( 0==CcuGdDetectServerCcuNetState(server_flag_from_iptable) )
        {
                if( ccu_config_info.is_main_ccu==1 && server_flag_from_iptable==2 )
                {
                      ccu_config_info.is_main_ccu = 0;                
		      CcuUpdateMainCcuFlag(0);
		}
	}
#endif

              if( NULL != ccu_gd_state_param.plocal_curr_gd_state )
              {
                    ccu_gd_state_param.plocal_curr_gd_state->process( (void *)&ccu_gd_state_param); ///< 当前状态process
	      }
			  
	      sched_yield();
	}

	return NULL;
}
#endif



