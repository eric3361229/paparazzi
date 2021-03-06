/*
    ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
                 2011,2012 Giovanni Di Sirio.

    This file is part of ChibiOS/RT.

    ChibiOS/RT is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/RT is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    ---

    A special exception to the GPL can be applied should you wish to distribute
    a combined work that includes ChibiOS/RT, without being obliged to provide
    the source code for any proprietary components. See the file exception.txt
    for full details of how and when the exception can be applied.
    */

/**
 * @file arch/chibios/mcu_periph/sdio_arch.c
 *
 * SDIO interface using ChibiOS API for Paparazzi
 *
 */

#include "std.h"
#include <string.h>
#include <ch.h>
#include <hal.h>
#include "mcu_periph/sdio.h"
#include <stdarg.h>


static enum {STOP, CONNECT} cnxState = STOP;


bool sdio_connect(void)
{
  if (!sdc_lld_is_card_inserted (NULL)) {
    return FALSE;
  }

  if (cnxState == CONNECT) {
    return TRUE;
  }

  /*
   * Initializes the SDIO drivers.
   *
   * FIXME hardcoded for Apogee board ?
   */
  const uint32_t mode = PAL_MODE_ALTERNATE(12) | PAL_STM32_OTYPE_PUSHPULL |
    PAL_STM32_OSPEED_HIGHEST | PAL_STM32_PUPDR_FLOATING | PAL_STM32_MODE_ALTERNATE;

  palSetPadMode (GPIOC, GPIOC_SDIO_D0, mode | PAL_STM32_PUPDR_PULLUP);
  palSetPadMode (GPIOC, GPIOC_SDIO_D1, mode | PAL_STM32_PUPDR_PULLUP);
  palSetPadMode (GPIOC, GPIOC_SDIO_D2, mode | PAL_STM32_PUPDR_PULLUP);
  palSetPadMode (GPIOC, GPIOC_SDIO_D3, mode | PAL_STM32_PUPDR_PULLUP);
  palSetPadMode (GPIOC, GPIOC_SDIO_CK, mode);
  palSetPadMode (GPIOD, GPIOD_SDIO_CMD, mode | PAL_STM32_PUPDR_PULLUP);
  // palSetPadMode (GPIOD, GPIOD_SDIO_CMD, mode);

  chThdSleepMilliseconds(100);


  sdcStart(&SDCD1, NULL);
  while (sdcConnect(&SDCD1) != HAL_SUCCESS) {
    chThdSleepMilliseconds(100);
  }

  cnxState = CONNECT;
  return TRUE;
}


bool sdio_disconnect(void)
{
  if (cnxState == STOP)
    return TRUE;
  if (sdcDisconnect(&SDCD1)) {
    return FALSE;
  }
  sdcStop (&SDCD1);
  cnxState = STOP;
  return TRUE;
}

bool is_card_inserted(void)
{
  return sdc_lld_is_card_inserted (NULL);
}

