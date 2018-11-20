/*
 * _led.c
 *
 *  Created on: 12 но€б. 2018 г.
 *      Author: Alexander
 */

#include "_led.h"

void InitGpioLED(void)
{
    EALLOW;
    //
    // Initialization GPIOs as outputs
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO31 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPASET.bit.GPIO31 = 1;     // Load output latch
    GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPBPUD.bit.GPIO34 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPBSET.bit.GPIO34 = 1;     // Load output latch
    GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;     // GPIOXX is output
    EDIS;
}

