/*
 * audio_sample.h
 *
 *  Created on: 2013-11-21
 *      Author: benty
 */

#ifndef AUDIO_SAMPLE_H_
#define AUDIO_SAMPLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include <semaphore.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>

#include CYGHWR_MEMORY_LAYOUT_H
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>
#include <cyg/io/io.h>
#include <cyg/io/serialio.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serialio.h>
#include <cyg/io/spi.h>
//#include <cyg/io/spi_stm32.h>
#include <cyg/io/audio/audio.h>
#include <cyg/io/audio/audio_dev.h>

#include <cyg/kernel/kapi.h>       /* All the kernel specific stuff */
#include <cyg/kernel/kapi.h>

#include <cyg/hal/hal_arch.h>      /* CYGNUM_HAL_STACK_SIZE_TYPICAL */
#include <cyg/hal/hal_io.h>
#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>

#include <pkgconf/hal.h>
#include <pkgconf/io_audio.h>
#include <pkgconf/io.h>
#include <pkgconf/system.h>
//#include <pkgconf/devs_spi_cortexm_stm32.h>
#include <pkgconf/devs_snd_vs1063a.h>
#include "buffer_read_write_fun.h"

/** PlayAudioTwice function description
 *
 *  This function is used to broadcast audio data
 *  @param   the handle of sound card
 *  @param   source buffer id
 *  @return  if havd finished broadcast audio data,will be return -1
 *					 if there have other audio data want to broadcast,will be return 0
 */
int PlayAudioTwice(cyg_io_handle_t audio_handle, int audio_source_buffer_id, int pending_audio_buffer_id);

#endif /* AUDIO_SAMPLE_H_ */
