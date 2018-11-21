/*
 * _led.h
 *
 *  Created on: 12 но€б. 2018 г.
 *      Author: Alexander
 */

#ifndef LED_H_
#define LED_H_

//#include "F28x_Project.h"
#include "F28x_Project.h"
#include <stdint.h>

static inline void
LED_RED_on(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
}
static inline void
LED_RED_off(void)
{
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;
}
static inline void
LED_RED_toggle(void)
{
    GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
}
static inline void
LED_RED_write(uint32_t outVal)
{
    GpioDataRegs.GPBDAT.bit.GPIO34 = !outVal;
}

static inline void
LED_BLUE_on(void)
{
    GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
}
static inline void
LED_BLUE_off(void)
{
    GpioDataRegs.GPASET.bit.GPIO31 = 1;
}
static inline void
LED_BLUE_toggle(void)
{
    GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
}
static inline void
LED_BLUE_write(uint32_t outVal)
{
    GpioDataRegs.GPADAT.bit.GPIO31 = !outVal;
}

void InitGpioLED(void);

#endif /* LED_H_ */
