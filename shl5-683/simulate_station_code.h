
#ifndef _SIMULATE_STATION_CODE_H_
#define _SIMULATE_STATION_CODE_H_

#include <stdio.h>
#include <cyg/hal/hal_io.h>
#include <cyg/kernel/kapi.h>
#include <cyg/hal/plf_io.h>
#include <pkgconf/hal.h>
#include <cyg/fileio/fileio.h>


extern unsigned char ddu_simulate_flag;//ddu??????? 1:??  3:??

/** ReadSimulateAudioData function description
 *  ????????
 *
 *  @param   ??id
 *  @param   ???ID
 *  @param   ???ID
 *  @param   ????????
 *  @return  if success return 0
 */
int ReadSimulateAudioData(int audio_data_stop_id, int audio_data_next_id, char *buff_wr, unsigned int buff_wr_len);


/** alarm_func_handle_ddu_simulate function description
 *  DDU???????????
 *
 *  @param   ?????
 *  @param   ???????
 *  @return  null
 */
void *alarm_func_handle_ddu_simulate(cyg_handle_t counter_handle, cyg_addrword_t data);

/** CreateDDUSimulateTimer function description
 *  ??DDU???????
 *
 *  @param   null
 *  @return  null
 */
void CreateDDUSimulateTimer();

/** StartDDUSimulateTimer function description
 *  ??DDU???????
 *
 *  @param   ???????
 *  @return  null
 */
void StartDDUSimulateTimer(int timer_interval);

/** CloseDDUSimulateTimer function description
 *   ??DDU???????
 *
 *  @param   null
 *  @return  null
 */
void CloseDDUSimulateTimer();

#endif /* _SIMULATE_STATION_CODE_H_ */
