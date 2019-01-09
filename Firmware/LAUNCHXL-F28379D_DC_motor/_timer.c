/*
 * _timer.c
 *
 *  Created on: 20 дек. 2018 г.
 *      Author: Alexander
 */
#include "_timer.h"
#include "_led.h"
#include "_spi.h"

__interrupt void cpu_timer0_isr(void)
{
    CpuTimer0.InterruptCount++;
//    LED_RED_toggle();
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

void initTIMER0(void)
{
    InitCpuTimers();
    ConfigCpuTimer(&CpuTimer0, 200, 1000);
    CpuTimer0Regs.TCR.all = 0x4001;
}
