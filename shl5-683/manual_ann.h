#ifndef MANUAL_ANN_H_
#define MANUAL_ANN_H_

#include "ccu_function.h"
#include "struct_type.h"


extern unsigned char ddu_start_stataion_id;

extern unsigned char ddu_current_stataion_id;

extern unsigned char ddu_next_stataion_id;

extern unsigned char ddu_end_stataion_id;


int CreateEAMPToSyncTimer(unsigned int msecond);

void SendManualSYNCPackageToEamp();


int SetCmd(void *param_table );

void SetTSCmd(unsigned char event_type,
                    char *src_devices_name,	unsigned char src_devices_no,
                    unsigned char state_request,unsigned char request_or_over,
                    unsigned char line_number,unsigned char region_number,
                    unsigned int broadcast_sequence,unsigned char broadcast_times,
                    unsigned char arrived_station,unsigned char current_station_code,unsigned char station_effective,unsigned char whether_pre_read,
                    unsigned char dst_device_number);

void SetTSManualCMDPackage(int param_stop_or_leave,int param_open_door_flag);

 void SetUpOrDownStream(int up_down_flag);


#endif
