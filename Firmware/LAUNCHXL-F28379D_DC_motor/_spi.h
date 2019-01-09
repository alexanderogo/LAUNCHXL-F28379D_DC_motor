/*
 * _spi.h
 *
 *  Created on: 3 џэт. 2019 у.
 *      Author: Alexander
 */

#ifndef SPI_H_
#define SPI_H_

#include "F28x_Project.h"
#include <stdint.h>

__interrupt void spia_tx_int_isr(void);
__interrupt void spia_rx_int_isr(void);

void InitSPIA(void);
void InitSPIB(void);

static inline void
CS1_SPIA_on(void)
{
    GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;
}
static inline void
CS2_SPIA_on(void)
{
    GpioDataRegs.GPDCLEAR.bit.GPIO123 = 1;
}
static inline void
CS3_SPIA_on(void)
{
    GpioDataRegs.GPDCLEAR.bit.GPIO124 = 1;
}
static inline void
CS4_SPIA_on(void)
{
    GpioDataRegs.GPDCLEAR.bit.GPIO125 = 1;
}
static inline void
CS1_SPIA_off(void)
{
    GpioDataRegs.GPBSET.bit.GPIO61 = 1;
}
static inline void
CS2_SPIA_off(void)
{
    GpioDataRegs.GPDSET.bit.GPIO123 = 1;
}
static inline void
CS3_SPIA_off(void)
{
    GpioDataRegs.GPDSET.bit.GPIO124 = 1;
}
static inline void
CS4_SPIA_off(void)
{
    GpioDataRegs.GPDSET.bit.GPIO125 = 1;
}
static inline void
CS_all_SPIA_off(void)
{
    GpioDataRegs.GPBSET.bit.GPIO61 = 1;
    GpioDataRegs.GPDSET.bit.GPIO123 = 1;
    GpioDataRegs.GPDSET.bit.GPIO124 = 1;
    GpioDataRegs.GPDSET.bit.GPIO125 = 1;
}

#endif /* SPI_H_ */
