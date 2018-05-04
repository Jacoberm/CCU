/*
 * ZhwNetworkToLCD.h
 *
 *  Created on: 2015-11-29
 *      Author: benty
 */

#ifndef ZHWNETWORKTOLCD_H_
#define ZHWNETWORKTOLCD_H_

#include <stdio.h>
#include "data_struct.h"

//#include "../eamp_certificate/eamp_function.h"

typedef struct {
	unsigned char first;
	unsigned char second;
	unsigned char target_device_type;//0x07:dynamic map;0xff:all device
	unsigned char target_device_number;//oxff:all device number
	unsigned char persist_1;
	unsigned char inspect_ctl;//0x01:start;other:stop
	unsigned char inspect_device_info;//0`3:LCD number,0x01`0x0a:signle device;0x0d:all device;0x0e:device on left;0x0f:device on right;4`7:car number.
	unsigned char persist_2[9];
}message_head_t;

typedef struct {
	unsigned char main_flag;//1:message valid;other:invalid
	//bit7:upstream,bit6:downstream,bit5:left door open,
	//bit4:right door open,bit3:left door close,bit2:right door close,
	//bit7=1 && bit6=0:upstream
	//bit7=0 && bit6=1:downstream
	struct {
		unsigned char :2;
		unsigned char right_door_close:1;
		unsigned char left_door_close:1;
		unsigned char right_door_open:1;
		unsigned char left_door_open:1;
		unsigned char downstream:1;
		unsigned char upstream:1;
	}direct_and_dooropen;
	//bit7:leave,car run to the next station;
	//bit6:arrive,car arrive at current station;
	struct {
		unsigned char :6;
		unsigned char car_arrive:1;
		unsigned char car_leave:1;
	}arrive_or_leave;
	unsigned char persist_1;
	unsigned char start_station;
	unsigned char end_station;
	unsigned char current_station;
	unsigned char next_station;
	//1:updata the over station info;
	//2:recover all of the over station;
	//0 or other:do not updata the over station info.
	unsigned char over_station_ctl;
	//1:over this station;
	//0: not over this station.
	struct {
			unsigned char state_8:1;
			unsigned char state_7:1;
			unsigned char state_6:1;
			unsigned char state_5:1;
			unsigned char state_4:1;
			unsigned char state_3:1;
			unsigned char state_2:1;
			unsigned char state_1:1;

			unsigned char state_16:1;
			unsigned char state_15:1;
			unsigned char state_14:1;
			unsigned char state_13:1;
			unsigned char state_12:1;
			unsigned char state_11:1;
			unsigned char state_10:1;
			unsigned char state_9:1;

			unsigned char state_24:1;
			unsigned char state_23:1;
			unsigned char state_22:1;
			unsigned char state_21:1;
			unsigned char state_20:1;
			unsigned char state_19:1;
			unsigned char state_18:1;
			unsigned char state_17:1;

			unsigned char state_32:1;
			unsigned char state_31:1;
			unsigned char state_30:1;
			unsigned char state_29:1;
			unsigned char state_28:1;
			unsigned char state_27:1;
			unsigned char state_26:1;
			unsigned char state_25:1;

			unsigned char state_40:1;
			unsigned char state_39:1;
			unsigned char state_38:1;
			unsigned char state_37:1;
			unsigned char state_36:1;
			unsigned char state_35:1;
			unsigned char state_34:1;
			unsigned char state_33:1;

			unsigned char state_48:1;
			unsigned char state_47:1;
			unsigned char state_46:1;
			unsigned char state_45:1;
			unsigned char state_44:1;
			unsigned char state_43:1;
			unsigned char state_42:1;
			unsigned char state_41:1;
	}over_station_info;
	unsigned char persist_2[4];
}data_info_t;

typedef struct {
	unsigned char frame_head;
	message_head_t message_head;
	unsigned char ctl_code[2];
	unsigned char data_length[2];
	data_info_t data;
//	unsigned char check_sum;//changed by zhw 2015-12-9 will be recover in the future
}eamp_to_LCD_t;

int checksystem(void);
void To_LCD_info_init(void);
void ZhwSendCmdToDynamicMap(send_infomation_t message);
#endif /* ZHWNETWORKTOLCD_H_ */
