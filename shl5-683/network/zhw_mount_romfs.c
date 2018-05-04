/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称： zhw_mount_romfs.c
 * 文件标识：
 * 摘    要： romfs文件系统挂载函数
 *
 * 当前版本： V1.0.0
 * 作    者： zhouwei
 * 完成日期：2012-11-08
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 *
 */
#include "zhw_mount_romfs.h"

int MountRomfs(void)
{
	int err;
	err=umount("/");
	if((err = mount("0x64020000","/","romfs"))<0)
	{
		return -1;
	}
	return 0;
}

int MountYaffs2(void)
{
      int err;
//      umount("/nand");
//      diag_printf("BBB\n");
      if((err = mount("onboard/2","/nand","yaffs"))<0)
      {
    	  diag_printf("err number = %d\n",err);
           perror(" ERROR\n");
           return -1;
      }
//	  diag_printf("AAA\n");
      return 0;
}

int ZhwMkdir(char *newdir,int amode)  //newdir = "/nand/line3"
{
	if(access(newdir,0) == -1)
	{
		if(mkdir(newdir,amode))
		{
			debug_print(("create dir :%s is fail !!!\n",newdir));
			perror("the error code is :\n");
			return -1;
		}
		else
		{
			debug_print(("create new dir : %s is OK !!!\n",newdir));
		}
		return 0 ;
	}
	else
	{
		debug_print(("the dir : %s is exists !!!\n",newdir));
	}
	return 0 ;
}

int DirCreateOfNandflash()
{
	ZhwMkdir("/nand/audio",0777);
	ZhwMkdir("/nand/audio/line3",0777);
	ZhwMkdir("/nand/audio/line4",0777);
	ZhwMkdir("/nand/audio/emerg",0777);
	ZhwMkdir("/nand/audio/prompt",0777);
	///<add by wds 160504
	ZhwMkdir("/nand/audio/line17",0777);
	ZhwMkdir("/nand/audio/line8",0777);
	ZhwMkdir("/nand/audio/line7",0777);
	ZhwMkdir("/nand/audio/line10",0777);

	ZhwMkdir("/nand/dbfile",0777);
	ZhwMkdir("/nand/log",0777);
	ZhwMkdir("/nand/record",0777);
	ZhwMkdir("/nand/volume",0777);
	return 0;
}


