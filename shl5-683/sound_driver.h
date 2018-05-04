/*
 * Copyright (c) 2012，MIT
 * All right reserved.
 *
 * 文件名称：sound_driver.h
 * 文件标识：
 * 摘    要：The header of sound drivers
 *
 * 当前版本：V1.0.0
 * 作    者：benty
 * 完成日期：
 *
 * 取代版本：
 * 原作者  ：
 * 完成日期：
 * */


#ifndef SOUND_DRIVER_H_
#define SOUND_DRIVER_H_

#include <stdio.h>
#include <string.h>
#include <pkgconf/hal.h>
#include <pkgconf/io_audio.h>
#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/io/io.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <errno.h>
#include <cyg/io/audio/audio.h>
#include <cyg/io/audio/audio_dev.h>
#include <cyg/hal/hal_if.h>
#include <pkgconf/devs_snd_vs1063a.h>
#include "buffer_read_write_fun.h"
#include "log_record.h"

extern cyg_io_handle_t	ccu_analog_audio_handle;

/** MicAdPcmWavheader function description
 *  Add sound header
 *
 *  @param   buffer id
 *  @return  if success,return 0
 */
int MicAdPcmWavheader(int buffer_id);

/** MicZeroAfterWaveHeader function description
 *  Add zero to sound
 *
 *  @param   buffer id
 *  @return  if success,return 0
 */
int MicZeroAfterWaveHeader(int buffer_id);

/** LookupSndCard function description
 *  This function is used to lookup the sound card
 *
 *  @param   the point of sound card handle
 *  @return  the state of lookup sound card
 */
int LookupSndCard(cyg_io_handle_t *param_audio_handle) ;

/** OpenSndCard function description
 *  This function is used to open the sound card
 *
 *  @param   the point of sound card handle
 *  @param   the mode of open sound card
 *  @return  the state of open sound card
 */
int OpenSndCard(cyg_io_handle_t *param_audio_handle,char *snd_mode) ;

/** CloseSndCard function description
 *  This function is used to close the sound card
 *
 *  @param   the handle of sound card
 *  @param   the mode of sound card

 *  @param   the type of close sound card
 *  @return  the state of close sound card
 */
int CloseSndCard(cyg_io_handle_t *param_audio_handle,char *snd_mode, char *close_type);
#endif /* SOUND_DRIVER_H_ */

