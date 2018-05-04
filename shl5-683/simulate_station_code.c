#include "simulate_station_code.h"
#include "struct_type.h"
char temp_audio_data[] = {
0xff,0xf2,0x49,0xc,0x67,0x25,0x0,0x6,0xc9,0x29,0x2c
};

static unsigned int temp_audio_data_len = sizeof(temp_audio_data);
static unsigned int temp_audio_data_curr_postion = 0;
static unsigned char simu_audio_end = 0;

/*DDU?????*/
cyg_handle_t  counter_handle_ddu_simulate;
cyg_handle_t alarm_handle_ddu_simulate;
cyg_alarm alarm_object_ddu_simulate;
unsigned char ddu_simulate_flag = 0;//ddu??????? 1:??  3:??
int GetSimulateAudioDataStatus(void)
{
	return (int)simu_audio_end;
}
int ResetSimulateAudioDataStatus(void)
{
        temp_audio_data_curr_postion = 0;
	simu_audio_end = 0;
	return 0;
}
int ReadSimulateAudioData(int audio_data_stop_id, int audio_data_next_id, char *buff_wr, unsigned int buff_wr_len)
{
	int a = 0, i = 0, b = 0;
        int number_of_read_local_file = 0;

        if( NULL==buff_wr ) 		
        {	           
              return -1;
        }

        if( temp_audio_data_curr_postion >= temp_audio_data_len )
        {
              simu_audio_end = 1;              
              return -1;
        }		

       for(i=0; i<buff_wr_len; i++)
       {
             buff_wr[i] = temp_audio_data[temp_audio_data_curr_postion++];
             number_of_read_local_file++;			 
	     if( temp_audio_data_curr_postion >= temp_audio_data_len )
	     {	           
                    break;
	     }
       }
		
	a = CharBufferWrite(audio_data_stop_id, (void *)buff_wr, number_of_read_local_file);
	b = CharBufferWrite(audio_data_next_id, (void *)buff_wr, number_of_read_local_file);
	debug_print(("array_curr_index=%d, total=%d, write_buffer_bytes=%d, %d\n", temp_audio_data_curr_postion, temp_audio_data_len, a, b));

	return a;
}

void *alarm_func_handle_ddu_simulate(cyg_handle_t counter_handle, cyg_addrword_t data)
{///DDU???????????
	if(ddu_simulate_flag == 0)
	{
		ddu_simulate_flag = 1;
	}
	else if(ddu_simulate_flag == 2)
	{
		ddu_simulate_flag = 3;
	}
}

void CreateDDUSimulateTimer()
{///<??DDU???????
	cyg_clock_to_counter(cyg_real_time_clock(),&counter_handle_ddu_simulate);
	cyg_alarm_create(counter_handle_ddu_simulate,alarm_func_handle_ddu_simulate,0,&alarm_handle_ddu_simulate,&alarm_object_ddu_simulate);
	cyg_alarm_initialize(alarm_handle_ddu_simulate,cyg_current_time()+1000,1000);
	cyg_alarm_disable(alarm_handle_ddu_simulate);
}
void StartDDUSimulateTimer(int timer_interval)
{///??DDU???????
	cyg_alarm_initialize(alarm_handle_ddu_simulate,cyg_current_time()+(timer_interval/1),(timer_interval/1));
	cyg_alarm_enable(alarm_handle_ddu_simulate);
}

void CloseDDUSimulateTimer()
{///??DDU???????
	cyg_alarm_disable(alarm_handle_ddu_simulate);
}
